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

    enum HBSFileCompression {
        None = 0,
        Compressed = 1
    };

    struct HBSFileInfo {
        HBSFileVersion version;
        HBSFileCompression compression;
    };

    virtual ~HBSFileIdentifier(){};
    HBSFileIdentifier(){};

    HBSFileInfo info(const QString &path) const;
};

#endif // HBSFILEIDENTIFIER_H
