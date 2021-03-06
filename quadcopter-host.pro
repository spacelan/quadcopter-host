#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T14:27:50
#
#-------------------------------------------------

CONFIG   += serialport
QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = quadcopter-host
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    nehewidget/nehewidget.cpp \
    quaternion/quaternion.cpp \
    communication/communication.cpp

HEADERS  += widget.h \
    nehewidget/nehewidget.h \
    quaternion/quaternion.h \
    communication/communication.h

FORMS    += widget.ui \
    nehewidget/nehewidget.ui

RESOURCES += \
    nehewidget/image.qrc \
    widget.qrc
