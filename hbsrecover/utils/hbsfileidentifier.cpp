#include "hbsfileidentifier.h"

#include <QFile>

static const uint SIGN_MAX_SIZE=64;
static const QByteArray HBS_SIGN_V1="__QCS__";
static const QByteArray HBS_SIGN_V2="\x4b\xca\x94\x72\x5e\x83\x1c\x31\x01";
static const QByteArray HBS_SIGN_OPENSSL="Salted__";

HBSFileIdentifier::HBSFileInfo HBSFileIdentifier::info(const QString &path) const
{
    QFile file(path);
    HBSFileInfo info={
        HBSFileVersion::Unknown,
        HBSFileCompression::None
    };
    QByteArray buf(SIGN_MAX_SIZE, 0);
    file.open(QIODevice::ReadOnly);
    file.read(buf.data(), SIGN_MAX_SIZE);
    if(buf.startsWith(HBS_SIGN_V1)){
        info.version=HBSFileVersion::Version1;
    }else if(buf.startsWith(HBS_SIGN_V2)){
        info.version=HBSFileVersion::Version2;
        if(buf.at(HBS_SIGN_V2.size())=='\x01'){
            info.compression=HBSFileCompression::Compressed;
        }
    }else if (buf.startsWith(HBS_SIGN_OPENSSL)){
        info.version=HBSFileVersion::OpenSSL;
    }
    return info;
}
