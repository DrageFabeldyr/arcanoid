#-------------------------------------------------
#
# Project created by QtCreator 2016-10-04T15:18:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = game
TEMPLATE = app


SOURCES += main.cpp\
        game.cpp

HEADERS  += game.h

FORMS    += game.ui

# чтобы не варнингало всякие int и float
CONFIG += c++11
