#ifndef HBSDECRYPTOROPENSSL_H
#define HBSDECRYPTOROPENSSL_H

#include "utils/hbsdecryptor.h"

class HBSDecryptorOpenSSL : public HBSDecryptor
{
public:
    virtual ~HBSDecryptorOpenSSL(){};
    HBSDecryptorOpenSSL(const QString &){};

    bool recover(const QString &inpath, const QString &outpath);
    void setCompression(bool compressed) {m_compressed=compressed;};

private:
    bool m_compressed;
};

#endif // HBSDECRYPTOROPENSSL_H
