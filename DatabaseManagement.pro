#-------------------------------------------------
#
# Project created by QtCreator 2014-12-23T17:33:07
#
#-------------------------------------------------

QT       += core gui

QT      += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DatabaseManagement
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectiondialog.cpp

HEADERS  += mainwindow.h \
    connectiondialog.h

FORMS    += mainwindow.ui \
    connectiondialog.ui
