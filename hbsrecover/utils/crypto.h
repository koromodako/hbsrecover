#ifndef CRYPTO_H
#define CRYPTO_H

#include <QString>
#include <botan/secmem.h>
#include <botan/block_cipher.h>

#include "utils/hbsdecryptor.h"
#include "utils/hbsfileidentifier.h"

namespace Crypto {

typedef Botan::secure_vector<quint8> Buffer;
typedef std::unique_ptr<Botan::BlockCipher> BlockCipherPtr;

QString lib_description();

QByteArray cb2ba(const Buffer &cb);

Buffer ba2cb(const QByteArray &ba);

HBSDecryptorPtr decryptor(HBSFileIdentifier::HBSFileInfo info, const QString &pswd);

}


#endif // CRYPTO_H
