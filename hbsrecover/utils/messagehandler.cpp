#include "messagehandler.h"

#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QMutexLocker>

static const QString COLOR_DBG="\x1b[32;1m";
static const QString COLOR_INF="\x1b[34;1m";
static const QString COLOR_WRN="\x1b[33;1m";
static const QString COLOR_ERR="\x1b[31;1m";
static const QString COLOR_FAT="\x1b[35;1m";
static const QString COLOR_END="\x1b[0m";

static const QString PREFIX_DBG="[DBG] - ";
static const QString PREFIX_INF="[INF] - ";
static const QString PREFIX_WRN="[WRN] - ";
static const QString PREFIX_ERR="[ERR] - ";
static const QString PREFIX_FAT="[FAT] - ";

static QFile LOG(QDir::temp().absoluteFilePath("hbsrecover.log"));

static QMutex LOG_MTX;

#ifdef QT_DEBUG
void message_handler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
#else
void message_handler(QtMsgType type, const QMessageLogContext &, const QString &msg)
#endif
{
    if(!LOG.isOpen()){
        LOG.open(QIODevice::ReadWrite|QIODevice::Append);
    }

    static QTextStream err(stderr);
    static QTextStream log(&LOG);

    QString prefix, color;
    QString timestamp=QString("(%0)").arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
#ifdef QT_DEBUG
    QString location=QString(" (%0:%1)").arg(QDir(ctx.file).dirName(), QString::number(ctx.line));
#else
    /* location is not provided if QT_DEBUG is not defined */
    QString location="";
#endif
    switch (type){
    case QtDebugMsg:
        color=COLOR_DBG;
        prefix=PREFIX_DBG;
        break;
    case QtInfoMsg:
        color=COLOR_INF;
        prefix=PREFIX_INF;
        break;
    case QtWarningMsg:
        color=COLOR_WRN;
        prefix=PREFIX_WRN;
        break;
    case QtCriticalMsg:
        color=COLOR_ERR;
        prefix=PREFIX_ERR;
        break;
    case QtFatalMsg:
        color=COLOR_FAT;
        prefix=PREFIX_FAT;
        break;
    }
    QMutexLocker locker(&LOG_MTX);
    /* critical section starts here */
#ifdef COLORIZE
    err<<color<<timestamp<<prefix<<msg<<location<<COLOR_END<<endl;
#else
    err<<timestamp<<prefix<<msg<<location<<endl;
#endif
    log<<timestamp<<prefix<<msg<<location<<endl;
    /* critical section ends here */
}
