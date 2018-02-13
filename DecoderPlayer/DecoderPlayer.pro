#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T09:38:05
#
#-------------------------------------------------

QT       += core

QT       -=gui
TARGET = DecoderPlayer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    myffplayer.cpp \
    workerthread.cpp \
    soapC.cpp \
    soapServer.cpp \
    stdsoap2.cpp \
    dataStruct.c \
    TMError.c \
    methods.c \
    cJSON.c

HEADERS += \
    workerthread.h \
    myffplayer.h \
    stdsoap2.h \
    soapH.h \
    soapStub.h \
    debug.h \
    soapPackBinding.nsmap \
    dataStruct.h \
    methods.h \
    TMError.h \
    cJSON.h


INCLUDEPATH += C:/msys32/tmp/ffmpeg/include
LIBS += -LC:/msys32/tmp/ffmpeg/lib -lavformat -lavcodec -lavutil -lswscale
