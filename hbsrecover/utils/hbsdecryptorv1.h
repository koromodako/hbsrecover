#ifndef HBSDECRYPTORV1_H
#define HBSDECRYPTORV1_H

#include "utils/hbsdecryptor.h"

class HBSDecryptorV1 : public HBSDecryptor
{
public:
    virtual ~HBSDecryptorV1(){};
    HBSDecryptorV1(const QString &){};

    bool recover(const QString &inpath, const QString &outpath);
};

#endif // HBSDECRYPTORV1_H
