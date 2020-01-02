#include "hbsfileidentifier.h"

#include <QFile>

static const uint SIGN_MAX_SIZE=64;
static const QByteArray HBS_SIGN_V1="__QCS__";
static const QByteArray HBS_SIGN_V2="\x4b\xca\x94\x72\x5e\x83\x1c\x31\x01";
static const QByteArray HBS_SIGN_OPENSSL="Salted__";

HBSFileIdentifier::HBSFileVersion HBSFileIdentifier::identify(const QString &path) const
{
    QFile file(path);
    QByteArray buf(SIGN_MAX_SIZE, 0);
    file.open(QIODevice::ReadOnly);
    file.read(buf.data(), SIGN_MAX_SIZE);
    if(buf.startsWith(HBS_SIGN_V1)){
        return HBSFileVersion::Version1;
    }else if(buf.startsWith(HBS_SIGN_V2)){
        return HBSFileVersion::Version2;
    }else if (buf.startsWith(HBS_SIGN_OPENSSL)){
        return HBSFileVersion::OpenSSL;
    }
    return HBSFileVersion::Unknown;
}
