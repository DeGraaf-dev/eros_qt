#-------------------------------------------------
#
# Project created by QtCreator 2015-10-30T19:03:29
#
#-------------------------------------------------

QT       += core gui help

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eros_qt
TEMPLATE = app


SOURCES += main.cpp\
        erosmain.cpp \
    catalog.cpp \
    dereader.cpp \
    convertcoor.cpp \
    time.cpp \
    calculation.cpp

HEADERS  += erosmain.h \
    catalog.h \
    dereader.h \
    convertcoor.h \
    time.h \
    calculation.h

FORMS    += erosmain.ui
