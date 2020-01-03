#include "hbsdecryptoropenssl.h"

#include "utils/macro.h"

bool HBSDecryptorOpenSSL::recover(const QString &, const QString &)
{
    LOG_IN("inpath="<<inpath<<", outpath="<<outpath);
    LOG_WARNING("HBSDecryptorOpenSSL::recover(): is not supported yet!");
    LOG_BOOL_RETURN(false);
}
