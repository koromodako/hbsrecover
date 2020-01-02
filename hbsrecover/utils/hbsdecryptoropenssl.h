#ifndef HBSDECRYPTOROPENSSL_H
#define HBSDECRYPTOROPENSSL_H

#include "utils/hbsdecryptor.h"

class HBSDecryptorOpenSSL : public HBSDecryptor
{
public:
    virtual ~HBSDecryptorOpenSSL(){};
    HBSDecryptorOpenSSL(const QString &){};

    bool recover(const QString &inpath, const QString &outpath);
};

#endif // HBSDECRYPTOROPENSSL_H
