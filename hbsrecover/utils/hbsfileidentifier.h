#ifndef HBSFILEIDENTIFIER_H
#define HBSFILEIDENTIFIER_H

#include "utils/hbsdecryptor.h"

class HBSFileIdentifier
{
public:
    enum HBSFileVersion {
        Unknown = 0,
        Version1 = 1,
        Version2 = 2,
        OpenSSL = 3,
    };

    virtual ~HBSFileIdentifier(){};
    HBSFileIdentifier(){};

    HBSFileVersion identify(const QString &path) const;
};

#endif // HBSFILEIDENTIFIER_H
