//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtCore>
#include <QMessageBox>
#include <QTimer>
#include <windows.h>
#include <QCloseEvent>
#include <QSettings>
#include "nehewidget/nehewidget.h"
#include "communication.h"


//串口读写延时
#define TIME_OUT 10

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    

    void setComboxEnabled(bool);
    void closeEvent(QCloseEvent *e);

private:
    Ui::Widget *ui;
    Communication *myCom;
    NeHeWidget *openGLWidget;
    QTimer *refreshTimer;
    QSettings *mySettings;
    bool isFirst;
    bool isRun;

private slots:
    void getQuatData();
    void getAccelData();
    void anchorClickedSlot(const QUrl&);

    void on_openclosebtn_clicked();
    void on_sendbtn_clicked();
    void on_clearbtn_clicked();
    void on_runpauseBtn_clicked();
    void on_restartBtn_clicked();
    void on_displayBTN_clicked();

signals:

};

#endif // WIDGET_H
