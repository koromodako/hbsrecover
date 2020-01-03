#ifndef HBSDECRYPTORV1_H
#define HBSDECRYPTORV1_H

#include "utils/hbsdecryptor.h"

class HBSDecryptorV1 : public HBSDecryptor
{
public:
    virtual ~HBSDecryptorV1(){};
    HBSDecryptorV1(const QString &){};

    bool recover(const QString &inpath, const QString &outpath);
    void setCompression(bool compressed) {m_compressed=compressed;};

private:
    bool m_compressed;
};

#endif // HBSDECRYPTORV1_H
