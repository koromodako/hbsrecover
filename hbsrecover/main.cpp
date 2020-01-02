/*
 *  Picsou | Keep track of your expenses !
 *  Copyright (C) 2018  koromodako
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "hbsrecover.h"
#include "ui/mainwindow.h"
#include "utils/macro.h"
#include "utils/messagehandler.h"

#include <QTranslator>
#include <QApplication>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    /* configure logging handler before everything else */
    qInstallMessageHandler(message_handler);
    /* start main function */
    LOG_IN("argc="<<argc<<",argv="<<argv)
    /* configure application */
    QScopedPointer<QApplication> app(new QApplication(argc, argv));
    app->setStyle(QStyleFactory::create("Fusion"));
    app->setApplicationName(HBSRECOVER_NAME);
    app->setApplicationVersion(HBSRECOVER_VERSION);
    app->setApplicationDisplayName(HBSRECOVER_NAME);
    LOG_INFO(HBSRECOVER_NAME.toUtf8().data()<<" "<<
             HBSRECOVER_VERSION.toUtf8().data()<<" ("<<
             HBSRECOVER_COMMIT.toUtf8().data()<<")")
    /* install Qt library translator */
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app->installTranslator(&qtTranslator);
    /* install Picsou app translator */
    QTranslator appTranslator;
    appTranslator.load("picsou_" + QLocale::system().name(), ":/translation");
    app->installTranslator(&appTranslator);
    /* construct HBSrecover main window */
    MainWindow mw;
    mw.show();
    int rcode=app->exec();
    LOG_DEBUG("-> rcode="<<rcode)
    return rcode;
}
