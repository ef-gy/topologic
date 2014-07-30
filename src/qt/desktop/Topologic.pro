#-------------------------------------------------
#
# Project created by QtCreator 2014-07-30T22:26:32
#
#-------------------------------------------------

QT          += core gui opengl
INCLUDEPATH += ../../../include
CONFIG      += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Topologic
TEMPLATE = app


SOURCES += main.cpp\
        topologic.cpp \
    topologicopenglwidget.cpp

HEADERS  += topologic.h \
    topologicopenglwidget.h

FORMS    += topologic.ui

OTHER_FILES += \
    Topologic.config
