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
    void displayQuat(float w,float x,float y,float z);
    void displayEuler(float pitch,float roll,float yaw);
    void displayTextBrowser(float w,float x,float y,float z,DATA_TYPE type = DATA_TYPE_NONE);
    void displayTextBrowser(float x,float y,float z,DATA_TYPE type = DATA_TYPE_NONE);
    void displayHelp();

    Ui::Widget *ui;
    Communication *myCom;
    NeHeWidget *openGLWidget;
    QTimer *refreshTimer;
    QSettings *mySettings;
    unsigned char CMD;
    unsigned char isTextBrowserDisplay;
    bool isFirst;
    bool isRun;


private slots:
    void getData(DATA_TYPE type = DATA_TYPE_ALL);
    void getQuatData();
    void getAccelData();
    void getGyroData();

    void on_openclosebtn_clicked();
    void on_sendbtn_clicked();
    void on_clearbtn_clicked();
    void on_runpauseBtn_clicked();
    void on_restartBtn_clicked();
    void on_displayBTN_clicked();

    void anchorClickedSlot(const QUrl&);

signals:

};

#endif // WIDGET_H
