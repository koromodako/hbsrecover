#include "crypto.h"

#include <botan/version.h>

#include "utils/macro.h"
#include "utils/hbsdecryptorv1.h"
#include "utils/hbsdecryptorv2.h"
#include "utils/hbsdecryptoropenssl.h"

QString Crypto::lib_description()
{
    /* Expected to be "LIBNAME LIBVERSION" */
    return QString("Botan %0.%1.%2").arg(QString::number(Botan::version_major()),
                                         QString::number(Botan::version_minor()),
                                         QString::number(Botan::version_patch()));
}

QByteArray Crypto::cb2ba(const Crypto::Buffer &cb)
{
    return QByteArray(reinterpret_cast<const char*>(cb.data()), static_cast<int>(cb.size()));
}

Crypto::Buffer Crypto::ba2cb(const QByteArray &ba)
{
    return Crypto::Buffer(ba.begin(), ba.end());
}

HBSDecryptorPtr Crypto::decryptor(HBSFileIdentifier::HBSFileVersion version, const QString &pswd)
{
    HBSDecryptorPtr dec;
    switch (version){
    case HBSFileIdentifier::HBSFileVersion::Unknown:
        break;
    case HBSFileIdentifier::HBSFileVersion::Version1:
        dec=HBSDecryptorPtr(new HBSDecryptorV1(pswd));
        break;
    case HBSFileIdentifier::HBSFileVersion::Version2:
        dec=HBSDecryptorPtr(new HBSDecryptorV2(pswd));
        break;
    case HBSFileIdentifier::HBSFileVersion::OpenSSL:
        dec=HBSDecryptorPtr(new HBSDecryptorOpenSSL(pswd));
        break;
    default:
        break;
    }
    return dec;
}
