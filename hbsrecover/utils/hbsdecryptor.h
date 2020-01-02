#ifndef DECRYPTOR_H
#define DECRYPTOR_H

#include <QString>
#include <QSharedPointer>

class HBSDecryptor
{
public:
    virtual bool recover(const QString &inpath, const QString &outpath)=0;
};

typedef QSharedPointer<HBSDecryptor> HBSDecryptorPtr;

#endif // DECRYPTOR_H
