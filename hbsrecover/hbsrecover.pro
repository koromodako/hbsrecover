#-------------------------------------------------
#
# Project created by QtCreator 2020-01-02T12:51:00
#
#-------------------------------------------------
TARGET = hbsrecover
TEMPLATE = app
CONFIG += c++14 lrelease
QT += core svg widgets concurrent
#
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#
DEFINES += QT_DEPRECATED_WARNINGS
#
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#
# Platform dependent configuration
#
win32 {
    #
    # MinGW TARGETS
    #
    message("building for windows target")
    system("python.exe prebuild")
    RC_ICONS = hbsrecover.ico
    QMAKE_LFLAGS += -fstack-protector
    QMAKE_CXXFLAGS += -fstack-protector
} else {
    #
    # LINUX TARGETS
    #
    message("building for linux target")
    system("./prebuild")
    DEFINES += COLORIZE
}
#
# Common configuration
#
QMAKE_CXXFLAGS += -Wall -Wextra -Wfatal-errors -pedantic-errors
LIBS += $$PWD/third-party/build/lib/libbotan-2.a
INCLUDEPATH += $$PWD/third-party/build/include/botan-2
#
# Build-type dependent configuration
#
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../dist/debug
    win32:QMAKE_POST_LINK += "windeployqt.exe $$PWD/../dist/debug/hbsrecover.exe"
} else {
    DESTDIR = $$PWD/../dist/release
    win32:QMAKE_POST_LINK += "windeployqt.exe $$PWD/../dist/release/hbsrecover.exe"
}
message("link arguments:        $$LIBS")
message("include arguments:     $$INCLUDEPATH")
message("destination directory: $$DESTDIR")
message("post link action: $$QMAKE_POST_LINK")
#
# Files
#
SOURCES += \
    main.cpp \
    ui/mainwindow.cpp

HEADERS += \
    hbsrecover.h \
    hbsrecover.h.dist \
    ui/mainwindow.h

FORMS += \
    ui/mainwindow.ui

RESOURCES += \
    hbsrecover.qrc

DISTFILES += \
    prebuild

TRANSLATIONS += \
    translation/hbsrecover_fr_FR.ts
