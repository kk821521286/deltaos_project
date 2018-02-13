#-------------------------------------------------
#
# Project created by QtCreator 2015-08-20T14:55:15
#
#-------------------------------------------------

QT       += core gui network



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MCRClientAppUI
TEMPLATE = app

UI_DIR = ./UI
SOURCES += main.cpp\
        dialog.cpp \
    moduleitemdelegate.cpp \
    fileitemdelegate.cpp \
    download.cpp \
    channeldialog.cpp \
    workerthread.cpp \
    moduledialog.cpp \
    addchanneldialog.cpp \
    addmoduledialog.cpp \
    combodelegate.cpp \
    info.cpp \
    updatedialog.cpp \
    updateipdelegate.cpp \
    shiftlogin.cpp \
    myffplayer.cpp \
    shiftmessagebox.cpp


HEADERS  += dialog.h \
    moduleitemdelegate.h \
    mcrclientqt.h \
    fileitemdelegate.h \
    download.h \
    channeldialog.h \
    workerthread.h \
    moduledialog.h \
    addchanneldialog.h \
    addmoduledialog.h \
    combodelegate.h \
    info.h \
    updatedialog.h \
    updateipdelegate.h \
    debug.h \
    usrSysReboot.h \
    shiftlogin.h \
    myffplayer.h \
    shiftmessagebox.h


FORMS    += dialog.ui \
    channeldialog.ui \
    moduledialog.ui \
    addchanneldialog.ui \
    addmoduledialog.ui \
    info.ui \
    updatedialog.ui \
    shiftlogin.ui \
    shiftmessagebox.ui

RESOURCES += \
    default.qrc \
    pic.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build/release/ -lMCRClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build/debug/ -lMCRClient

INCLUDEPATH += $$PWD/../MCRClient
DEPENDPATH += $$PWD/../MCRClient



#INCLUDEPATH += $$PWD/../vlc/include
#LIBS += -L$$PWD/../vlc/lib -lvlc -lvlccore



RC_FILE = MCRClientAppUI.rc

DISTFILES += \
    image/loading.jpg


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build/release/ -lMCRClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build/debug/ -lMCRClient

INCLUDEPATH += $$PWD/../build/release
DEPENDPATH += $$PWD/../build/release


unix:!macx: LIBS += -L$$PWD/../MCRClient -lMCRClient




INCLUDEPATH += C:/msys32/tmp/ffmpeg/include
LIBS += -LC:/msys32/tmp/ffmpeg/lib -lavformat -lavcodec -lavutil -lswscale

unix:!macx: LIBS += -L$$PWD/ -lusrSysReboot

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
