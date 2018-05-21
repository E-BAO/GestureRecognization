#-------------------------------------------------
#
# Project created by QtCreator 2017-09-25T10:26:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GestureRecognization
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    renderarea.cpp \
    framework.cpp \
    pickpoint.cpp \
    renderwidget.cpp \
    floodfillarea.cpp \
    contoursarea.cpp \
    detectededgearea.cpp \
    glwidget.cpp \
    logo.cpp \
    handframe.cpp \
    handstructure.cpp \
    training.cpp

HEADERS  += mainwindow.h \
    renderarea.h \
    framework.h \
    pickpoint.h \
    renderwidget.h \
    floodfillarea.h \
    contoursarea.h \
    detectededgearea.h \
    glwidget.h \
    logo.h \
    handframe.h \
    handstructure.h \
    training.h

LIBS += -L/usr/local/Cellar/opencv3/3.2.0/lib

QT_CONFIG -= no-pkg-config
CONFIG  += link_pkgconfig
PKGCONFIG += opencv
