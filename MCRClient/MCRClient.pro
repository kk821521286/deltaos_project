#-------------------------------------------------
#
# Project created by QtCreator 2015-08-13T13:40:11
#
#-------------------------------------------------
QT       +=core
QT       -= gui

TARGET = MCRClient
TEMPLATE = lib

CONFIG   += console
CONFIG   -= app_bundle

DEFINES += MCRCLIENT_LIBRARY

SOURCES += \
    soapC.cpp \
    soapClient.cpp \
    stdsoap2.cpp \
    cJSON.c \
    mcrclientimpl.cpp \
    mcrclient.cpp

HEADERS +=\
        mcrclient_global.h \
    cJSON.h \
    soapH.h \
    soapStub.h \
    stdsoap2.h \
    soapPackBinding.nsmap \
    mcrclientimpl.h \
    mcrclient.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32 {
    LIBS += -lws2_32
}

