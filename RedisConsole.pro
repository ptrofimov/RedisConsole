#-------------------------------------------------
#
# Project created by QtCreator 2011-06-17T22:19:05
#
#-------------------------------------------------

QT       += core gui network

TARGET = RedisConsole
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    client.cpp \
    console.cpp \
    connectdlg.cpp \
    contexthelp.cpp

HEADERS  += mainwindow.h \
    client.h \
    console.h \
    connectdlg.h \
    contexthelp.h

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    todo.txt \
    commands.txt \
    about.txt
