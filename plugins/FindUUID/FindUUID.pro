#-------------------------------------------------
#
# Project created by QtCreator 2018-03-18T08:36:24
#
#-------------------------------------------------

QT       += gui widgets

TARGET = FindUUIDDef
TEMPLATE = lib
CONFIG +=plugin

DESTDIR = $$PWD/../../bin_$$QT_VERSION_$$MAKEFILE_GENERATOR/plugins

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        finduuiddef.cpp \
    ../../common/progresssignaler.cpp \
    finduuiddetector.cpp \
    ../../common/signaturedetectorbase.cpp \
    ../../common/qprefixedsettings.cpp

HEADERS += \
        finduuiddef.h \
    ../../interfaces/signaturedefinterface.h \
    ../../common/progresssignaler.h \
    finduuiddetector.h \
    ../../common/signaturedetectorbase.h \
    ../../common/qprefixedsettings.h

INCLUDEPATH +=../../interfaces ../../common

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    finduuiddef.json
