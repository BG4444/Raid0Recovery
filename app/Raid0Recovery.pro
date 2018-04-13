#-------------------------------------------------
#
# Project created by QtCreator 2018-03-09T09:32:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix:{
QMAKE_CXXFLAGS+=-std=gnu++17
QMAKE_CXX=g++-7
}


TARGET = Raid0Recovery
TEMPLATE = app

INCLUDEPATH +=../interfaces ../common

DESTDIR = $$PWD/../bin_$$QT_VERSION_$$MAKEFILE_GENERATOR


win32:{
QMAKE_LIBS_OPENGL_ES2 -=-lEGL -lGLESv2
QMAKE_POST_LINK += windeployqt $$DESTDIR
}

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
    signaturelist.cpp \
    progressdrawer.cpp \
    imageslist.cpp \
    storagedetector.cpp \
    processing.cpp \
    imageinfo.cpp \
    vdetectors.cpp \
    fileglue.cpp \
    raid0recoveryapp.cpp \
    ../common/progresssignaler.cpp

HEADERS += \
        recoverwindow.h \
    imageslist.h \
    signaturelist.h \
    progressdrawer.h \
    storagedetector.h \
    processing.h \
    imageinfo.h \
    ../interfaces/signaturedefinterface.h \
    vdetectors.h \
    fileglue.h \
    raid0recoveryapp.h \
    ../common/progresssignaler.h

FORMS += \
        recoverwindow.ui
