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
#include "picsou.h"
#include "ui/mainwindow.h"
#include "app/picsouuiservice.h"
#include "app/picsouapplication.h"
#include "app/picsoucommandlineparser.h"
#include "utils/macro.h"
#include "utils/picsoumessagehandler.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    /* configure logging handler before everything else */
    qInstallMessageHandler(picsou_message_handler);
    /* start main function */
    LOG_IN("argc="<<argc<<",argv="<<argv)
    /* configure application */
    QScopedPointer<QApplication> app(new QApplication(argc, argv));
    app->setStyle(QStyleFactory::create("Fusion"));
    app->setApplicationName(PICSOU_NAME);
    app->setApplicationVersion(PICSOU_VERSION);
    app->setApplicationDisplayName(PICSOU_NAME);
    /* install Qt library translator */
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app->installTranslator(&qtTranslator);
    /* install Picsou app translator */
    QTranslator appTranslator;
    appTranslator.load("picsou_" + QLocale::system().name(), ":/translation");
    app->installTranslator(&appTranslator);
    /* parse command line arguments */
    PicsouCommandLineParser parser;
    parser.process(app->arguments());
    const QStringList args=parser.positionalArguments();
    /* construct Picsou application */
    PicsouApplication papp(app.data());
    /* connect termination signal */
    QObject::connect(app.data(), &QCoreApplication::aboutToQuit, &papp, &PicsouApplication::terminate);
    /* initialize Picsou application */
    if(!papp.initialize()) {
        LOG_CRITICAL("failed to initialize application.")
        return 1;
    }
    papp.ui_svc()->show_mainwindow();
    if(args.length()>0) {
        papp.ui_svc()->db_open_file(args.at(0));
    }
    int rcode=app->exec();
    LOG_DEBUG("-> rcode="<<rcode)
    return rcode;
}
