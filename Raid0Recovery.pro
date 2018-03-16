#-------------------------------------------------
#
# Project created by QtCreator 2018-03-09T09:32:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS+=-std=gnu++17

QMAKE_CXX=g++-7

TARGET = Raid0Recovery
TEMPLATE = app

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
        main.cpp \
        recoverwindow.cpp \
    imageslist.cpp \
    signaturelist.cpp \
    signaturedef.cpp \
    progresssignaler.cpp \
    progressdrawer.cpp

HEADERS += \
        recoverwindow.h \
    imageslist.h \
    signaturelist.h \
    signaturedef.h \
    progresssignaler.h \
    progressdrawer.h

FORMS += \
        recoverwindow.ui
