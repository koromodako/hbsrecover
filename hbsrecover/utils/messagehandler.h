#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QDebug>

void message_handler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg);

#endif // MESSAGEHANDLER_H
