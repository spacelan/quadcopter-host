//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#include "widget.h"
#include <QApplication>
#include <QTextCodec>
#include <QtGlobal>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

    Q_INIT_RESOURCE(image);
    QApplication::setOrganizationName("Spacelan");
    QApplication::setApplicationName("quadcopter-host");
    Widget *w = new Widget;
    w->setAttribute(Qt::WA_DeleteOnClose,true);
    w->show();
    
    return a.exec();
}
