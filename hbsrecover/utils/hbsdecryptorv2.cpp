/*
 *  Picsou | Keep track of your expenses !
 *  Copyright (C) 2018  koromodako
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "hbsdecryptorv2.h"
#include "utils/macro.h"
#include <botan/pipe.h>
#include <botan/filters.h>
#include <botan/exceptn.h>
#include <botan/data_src.h>
#include <botan/data_snk.h>
#include <botan/block_cipher.h>

/*
 * Warning:
 *      Picsou attempts to reduce presence of secrets in memory (and disk through swap/hibernation) yet
 *      it does not guarantees that secrets cannot be retrieved through memory dumping, hibernation file carving, etc.
 */
static const uint SIGN_SZ=16;
static const uint MAGIC_SZ=8;
static const uint MK_SZ=32;
static const uint IV_SZ=16;
static const uint DATA_SZ=8;
static const uint HEADER_SZ=MAGIC_SZ+MK_SZ+IV_SZ+DATA_SZ; /* 64 */
static const std::string HEADER_ENCRYPTION_SPEC="AES-256"; /* implicit ECB mode without padding */
static const std::string CONTENT_ENCRYPTION_SPEC="AES-256/CBC/PKCS7";

HBSDecryptorV2::HBSDecryptorV2(const QString &pswd)
{
    QByteArray wkey=pswd.toUtf8();
    while(wkey.size()<32){
        wkey.append(pswd.toUtf8());
    }
    wkey.truncate(32);
    m_wk=Crypto::ba2cb(wkey);
}

bool HBSDecryptorV2::recover(const QString &inpath, const QString &outpath)
{
    LOG_IN("inpath="<<inpath<<", oupath="<<outpath);
    bool status=true;
    QFileInfo f(inpath);
    if(f.size()>SIGN_SZ+HEADER_SZ){
        Botan::DataSource_Stream pipe_in(inpath.toStdString(), true);
        /* process HBS v2 file header */
        Crypto::Buffer buf(HEADER_SZ);
        /* skip signature /!\ ensure that SIGN_SZ is smaller than HEADER_SZ /!\ */
        pipe_in.read(buf.data(), SIGN_SZ);
        /* take header */
        pipe_in.read(buf.data(), HEADER_SZ);
        /* decrypt header */
        Crypto::BlockCipherPtr cipher=Botan::BlockCipher::create(HEADER_ENCRYPTION_SPEC);
        ulong offset=0;
        if(cipher){
            cipher->set_key(m_wk);
            while(offset<buf.size()){
                cipher->decrypt(buf.data()+offset);
                offset+=cipher->block_size();
            }
            Crypto::Buffer mk=Crypto::Buffer(buf.begin()+MAGIC_SZ, buf.begin()+MAGIC_SZ+MK_SZ);
            Crypto::Buffer iv=Crypto::Buffer(buf.begin()+MAGIC_SZ+MK_SZ, buf.begin()+MAGIC_SZ+MK_SZ+IV_SZ);
            qint64 size=0;
            for(int b=0; b<8; b++){
                size|=buf.at(MAGIC_SZ+MK_SZ+IV_SZ+b);
                if(b<7){
                    size<<=8;
                }
            }
            if(size>=0&&f.size()>size){
                /* decrypt HBS v2 content */
                Botan::Keyed_Filter *aes=Botan::get_cipher(CONTENT_ENCRYPTION_SPEC, mk, iv, Botan::DECRYPTION);
                Botan::DataSink_Stream *pipe_out=new Botan::DataSink_Stream(outpath.toStdString(), true);
                if(aes){
                    Botan::Pipe pipe(aes, pipe_out);
                    pipe.process_msg(pipe_in);
                    LOG_INFO("file recovered.");
                }else{
                    LOG_CRITICAL("AES keyed filter creation failed.")
                    status=false;
                }
            }else{
                LOG_WARNING("header descryption failed (bad password) or file is corrupted.")
                status=false;
            }
        }else{
            LOG_CRITICAL("cipher creation failed.")
            status=false;
        }
    }else{
        LOG_WARNING("file too small to be a valid HBS encrypted file.")
        status=false;
    }
    LOG_BOOL_RETURN(status)
}
