#-------------------------------------------------
#
# Project created by QtCreator 2015-01-22T21:22:34
#
#-------------------------------------------------

QT       += core opengl gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qinowars
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
    glwidget.cpp \
    bubble.cpp \
    dino.cpp \
    movable.cpp \
    rock.cpp

HEADERS  += \
    glwidget.h \
    bubble.h \
    dino.h \
    movable.h \
    utils.h \
    rock.h

RESOURCES += \
    data.qrc
