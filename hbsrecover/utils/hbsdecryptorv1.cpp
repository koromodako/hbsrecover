#include "hbsdecryptorv1.h"

#include "utils/macro.h"

bool HBSDecryptorV1::recover(const QString &, const QString &)
{
    LOG_IN("inpath="<<inpath<<", outpath="<<outpath);
    LOG_WARNING("HBSDecryptorV1::recover(): is not supported yet!");
    LOG_BOOL_RETURN(false);
}
