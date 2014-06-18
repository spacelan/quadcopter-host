//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#include "widget.h"
#include "ui_widget.h"
#include "quaternion/quaternion.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    mySettings = new QSettings("settings.ini",QSettings::IniFormat);
    myCom = NULL;
    openGLWidget = NULL;
    refreshTimer = NULL;
    isRun = false;
    isDisplayText = true;
    isTextBrowserDisplay = DATA_TYPE_QUAT;
    CMD = COMMAND_TYPE_SEND_QUAT;
    QString date = QLocale(QLocale::C).toDate(QString(__DATE__).replace("  "," "),"MMM d yyyy").toString("yyyy-MM-dd");
    QString time = QString(__TIME__).left(5);
    connect(ui->recieveTextBrowser, SIGNAL(anchorClicked(const QUrl&)),this, SLOT(anchorClickedSlot(const QUrl&)));

    setWindowTitle(tr("编译于 ")+date+" "+time);
    ui->openclosebtn->setText(tr("打开串口"));
    ui->sendbtn->setText(tr("发送"));
    ui->sendbtn->setEnabled(false);
    ui->runpauseBtn->setEnabled(false);
    ui->restartBtn->setEnabled(false);
    ui->recieveTextBrowser->setOpenLinks(false);
    displayHelp();
    ui->recieveTextBrowser->moveCursor(QTextCursor::Start);

    ui->portnameComboBox->setCurrentIndex(mySettings->value("SERIAL_PORT_NAME",0).toInt());
    ui->baudRateComboBox->setCurrentIndex(mySettings->value("SERIAL_BAUD_RATE",0).toInt());
    ui->dataBitsComboBox->setCurrentIndex(mySettings->value("SERIAL_DATA_BITS",0).toInt());
    ui->parityComboBox->setCurrentIndex(mySettings->value("SERIAL_PARITY",0).toInt());
    ui->stopbitsComboBox->setCurrentIndex(mySettings->value("SERIAL_STOP_BITS",0).toInt());
}

Widget::~Widget()
{
    mySettings->setValue("SERIAL_PORT_NAME",ui->portnameComboBox->currentIndex());
    mySettings->setValue("SERIAL_BAUD_RATE",ui->baudRateComboBox->currentIndex());
    mySettings->setValue("SERIAL_DATA_BITS",ui->dataBitsComboBox->currentIndex());
    mySettings->setValue("SERIAL_PARITY",ui->parityComboBox->currentIndex());
    mySettings->setValue("SERIAL_STOP_BITS",ui->stopbitsComboBox->currentIndex());

    delete refreshTimer;
    delete openGLWidget;
    delete myCom;
    delete mySettings;
    delete ui;
}

void Widget::setComboxEnabled(bool b)
{
    ui->portnameComboBox->setEnabled(b);
    ui->baudRateComboBox->setEnabled(b);
    ui->dataBitsComboBox->setEnabled(b);
    ui->parityComboBox->setEnabled(b);
    ui->stopbitsComboBox->setEnabled(b);
}

//void Widget::closeEvent(QCloseEvent *e)
//{
//    mySettings->setValue("SERIAL_PORT_NAME",ui->portnameComboBox->currentIndex());
//    mySettings->setValue("SERIAL_BAUD_RATE",ui->baudRateComboBox->currentIndex());
//    mySettings->setValue("SERIAL_DATA_BITS",ui->dataBitsComboBox->currentIndex());
//    mySettings->setValue("SERIAL_PARITY",ui->parityComboBox->currentIndex());
//    mySettings->setValue("SERIAL_STOP_BITS",ui->stopbitsComboBox->currentIndex());
//    e->accept();
//}

void Widget::displayQuat(float w, float x, float y, float z)
{
    QString str;
    str.sprintf("% f",w);
    ui->label_w_display->setText(str);
    str.sprintf("% f",x);
    ui->label_x_display->setText(str);
    str.sprintf("% f",y);
    ui->label_y_display->setText(str);
    str.sprintf("% f",z);
    ui->label_z_display->setText(str);
}

void Widget::displayEuler(float pitch, float roll, float yaw)
{
    QString str;
    str.sprintf("% f",pitch);
    ui->label_pitch_display->setText(str);
    str.sprintf("% f",roll);
    ui->label_roll_display->setText(str);
    str.sprintf("% f",yaw);
    ui->label_yaw_display->setText(str);
}

void Widget::displayTextBrowser(float w, float x, float y, float z, DATA_TYPE type)
{
    if(!isDisplayText) return;
    QString str;
    switch(type)
    {
    case DATA_TYPE_QUAT:
        if((isTextBrowserDisplay & (unsigned char)DATA_TYPE_QUAT) == 0) return;
        str = "Quat: ";
        break;
    case DATA_TYPE_THROTTLE:
        if((isTextBrowserDisplay & (unsigned char)DATA_TYPE_THROTTLE) == 0) return;
        str = "Throttle: ";
        break;
    default:
        break;
    }
    ui->recieveTextBrowser->setTextColor(Qt::black);
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertPlainText(str);

    str.sprintf("% f % f % f % f\n",w,x,y,z);
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertPlainText(str);
}

void Widget::displayTextBrowser(float x, float y, float z, DATA_TYPE type)
{
    if(!isDisplayText) return;
    QString str;
    switch(type)
    {
    case DATA_TYPE_ACCEL:
        if((isTextBrowserDisplay & (unsigned char)DATA_TYPE_ACCEL) == 0) return;
        str = "Accel: ";
        break;
    case DATA_TYPE_GYRO:
        if((isTextBrowserDisplay & (unsigned char)DATA_TYPE_GYRO) == 0) return;
        str = "Gyro: ";
        break;
    case DATA_TYPE_EULER:
        if((isTextBrowserDisplay & (unsigned char)DATA_TYPE_EULER) == 0) return;
        str = "Euler:";
        break;
    default:
        break;
    }
    ui->recieveTextBrowser->setTextColor(Qt::black);
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertPlainText(str);

    str.sprintf("% f % f % f\n",x,y,z);
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertPlainText(str);
}

void Widget::displayHelp()
{
    QFile myFile(":/myWidget/readme.txt");
    if(myFile.open(QIODevice::ReadOnly))
    {
        QTextStream textIn(&myFile);
        QString str = textIn.readAll();
        ui->recieveTextBrowser->moveCursor(QTextCursor::End);
        ui->recieveTextBrowser->insertHtml(str);
        ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    }
    myFile.close();
    myFile.setFileName(":/myWidget/help.txt");
    if(myFile.open(QIODevice::ReadOnly))
    {
        QTextStream textIn(&myFile);
        QString str = textIn.readAll();
        ui->recieveTextBrowser->moveCursor(QTextCursor::End);
        ui->recieveTextBrowser->insertPlainText(str);
        ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    }
    myFile.close();
}

void Widget::getData(DATA_TYPE type)
{
    if(type & DATA_TYPE_QUAT)
        getQuatData();
    if(type & DATA_TYPE_ACCEL)
        getAccelData();
    if(type & DATA_TYPE_GYRO)
        getGyroData();
    if(type & DATA_TYPE_THROTTLE)
        getThrottleData();
}
/*
void Widget::getQuatData()
{
    long quat[4];
//    const float q30 = 1073741824.0f;
    float w,x,y,z;

    if(myCom->getQuat(quat) == 0) return;
    qDebug()<<"getQuatData";

    w = (float)quat[0] ;// q30;
    x = (float)quat[1] ;// q30;
    y = (float)quat[2] ;// q30;
    z = (float)quat[3] ;// q30;

    float norm = math_rsqrt(w*w + x*x + y*y + z*z);
    w *= norm;
    x *= norm;
    y *= norm;
    z *= norm;

    float pitch,roll,yaw;
    pitch = asin(2*w*y - 2*z*x) * 57.3;
    roll = atan2(2*w*x + 2*y*z,1 - 2*x*x - 2*y*y) * 57.3;
    yaw = atan2(2*w*z + 2*x*y,1 - 2*y*y - 2*z*z) * 57.3;

    if(openGLWidget) openGLWidget->quatToMatrix(w,x,y,z);

//    QString str;
//    str.sprintf("%f %f %f %f\n",
//                w,x,y,z);
//    ui->recieveTextBrowser->setTextColor(Qt::black);
//    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
//    ui->recieveTextBrowser->insertPlainText(str);

    displayTextBrowser(w,x,y,z,DATA_TYPE_QUAT);

//    str.sprintf("%f",w);
//    ui->label_w_display->setText(str);
//    str.sprintf("%f",x);
//    ui->label_x_display->setText(str);
//    str.sprintf("%f",y);
//    ui->label_y_display->setText(str);
//    str.sprintf("%f",z);
//    ui->label_z_display->setText(str);
//    str.sprintf("%f",pitch);
//    ui->label_pitch_display->setText(str);
//    str.sprintf("%f",roll);
//    ui->label_roll_display->setText(str);
//    str.sprintf("%f",yaw);
//    ui->label_yaw_display->setText(str);

    displayQuat(w,x,y,z);
    displayEuler(pitch,roll,yaw);
}
*/
void Widget::getQuatData()
{
    long quat[4];
    if(myCom->getQuat(quat) == 0) return;
    qDebug()<<"getQuatData";

    Quaternion q((float)quat[0],(float)quat[1],(float)quat[2],(float)quat[3]);
    q.Normalize();

    float pitch,roll,yaw;
    q.ToEuler(pitch,roll,yaw);

    if(openGLWidget) q.ToMatrix4(openGLWidget->Matrix4);

    displayQuat(q.w,q.x,q.y,q.z);
    displayEuler(pitch,roll,yaw);
    displayTextBrowser(q.w,q.x,q.y,q.z,DATA_TYPE_QUAT);
}

void Widget::getAccelData()
{
    short accel[3];
    const float scaleFactor= 16384.0;
    float ax,ay,az;

    if(myCom->getAccel(accel) == 0) return;

    ax = (float)accel[0] / scaleFactor;
    ay = (float)accel[1] / scaleFactor;
    az = (float)accel[2] / scaleFactor;

    displayTextBrowser(ax,ay,az,DATA_TYPE_ACCEL);
}

void Widget::getGyroData()
{
    short gyro[3];
    const float scaleFactor= 16.4;
    float gx,gy,gz;

    if(myCom->getGyro(gyro) == 0) return;

    gx = (float)gyro[0] / scaleFactor;
    gy = (float)gyro[1] / scaleFactor;
    gz = (float)gyro[2] / scaleFactor;

    displayTextBrowser(gx,gy,gz,DATA_TYPE_GYRO);
}

void Widget::getThrottleData()
{
    uint8_t throttle[4];
    if(myCom->getThrottle(throttle) == 0) return;
    displayTextBrowser(throttle[0],throttle[1],throttle[2],throttle[3],DATA_TYPE_THROTTLE);
}

void Widget::on_openclosebtn_clicked()
{
    //如果已经开启，关闭串口
    if(myCom)
    {
        delete myCom;
        myCom =NULL;
        delete refreshTimer;
        refreshTimer = NULL;

        ui->appName->setText(tr("未打开..."));
        ui->openclosebtn->setText(tr("打开串口"));
        ui->sendbtn->setEnabled(false);
        ui->runpauseBtn->setEnabled(false);
        ui->restartBtn->setEnabled(false);
        setComboxEnabled(true);

        return;
    }

    //打开串口准备
    //配置串口
    PortSettings settings;
    settings.PortName = ui->portnameComboBox->currentText();
    settings.BaudRate = (QSerialPort::BaudRate)ui->baudRateComboBox->currentText().toInt();
    switch(ui->dataBitsComboBox->currentIndex())
    {
    case 0:
        settings.DataBits = (QSerialPort::Data7);
        break;
    case 1:
        settings.DataBits = (QSerialPort::Data8);
        break;
    default:
        settings.DataBits = (QSerialPort::Data8);
    }
    switch(ui->parityComboBox->currentIndex())
    {
    case 0:
        settings.Parity = (QSerialPort::NoParity);
        break;
    case 1:
        settings.Parity = (QSerialPort::OddParity);
        break;
    case 2:
        settings.Parity = (QSerialPort::EvenParity);
        break;
    default:
        settings.Parity = (QSerialPort::NoParity);
    }
    switch(ui->stopbitsComboBox->currentIndex())
    {
    case 0:
        settings.StopBits = (QSerialPort::OneStop);
        break;
    case 1:
        settings.StopBits = (QSerialPort::OneAndHalfStop);
        break;
    case 2:
        settings.StopBits = (QSerialPort::TwoStop);
        break;
    default:
        settings.StopBits = (QSerialPort::OneStop);
    }
    settings.FlowControl = (QSerialPort::NoFlowControl);
    myCom = new Communication();
    //打开串口
    if(myCom->openSerialPort(settings))
    {
        refreshTimer = new QTimer;
        refreshTimer->setInterval(40);
        connect(refreshTimer,SIGNAL(timeout()),this,SLOT(getData()));
        //界面控制
        ui->appName->setText(tr("已打开..."));
        ui->openclosebtn->setText(tr("关闭串口"));
        ui->sendLineEdit->setFocus();
        ui->sendbtn->setEnabled(true);
        ui->runpauseBtn->setEnabled(true);
        ui->restartBtn->setEnabled(true);
        setComboxEnabled(false);
    }
    else
    {
        delete myCom;
        myCom = NULL;
        QMessageBox::critical(this, tr("打开失败"), tr("未能打开串口 ") + ui->portnameComboBox->currentText() + tr("\n该串口设备不存在或已被占用"), QMessageBox::Ok);
        return;
    }
}

void Widget::on_sendbtn_clicked()
{
    if(ui->sendLineEdit->text().isEmpty())
    {
        QMessageBox::information(this, tr("提示消息"), tr("没有需要发送的数据"), QMessageBox::Ok);
        return;
    }
    ui->sendLineEdit->setFocus();
    QByteArray buf;
    buf = ui->sendLineEdit->text().toAscii();
    if(buf == "-d display") isDisplayText = true;
    else if(buf == "-d ~display") isDisplayText = false;
    else if(buf == "-d quat") isTextBrowserDisplay |= (unsigned char)DATA_TYPE_QUAT;
    else if(buf == "-d ~quat") isTextBrowserDisplay &= ~((unsigned char)DATA_TYPE_QUAT);
    else if(buf == "-d accel") isTextBrowserDisplay |= (unsigned char)DATA_TYPE_ACCEL;
    else if(buf == "-d ~accel") isTextBrowserDisplay &= ~((unsigned char)DATA_TYPE_ACCEL);
    else if(buf == "-d gyro") isTextBrowserDisplay |= (unsigned char)DATA_TYPE_GYRO;
    else if(buf == "-d ~gyro") isTextBrowserDisplay &= ~((unsigned char)DATA_TYPE_GYRO);
    else if(buf == "-d throttle") isTextBrowserDisplay |= (unsigned char)DATA_TYPE_THROTTLE;
    else if(buf == "-d ~throttle") isTextBrowserDisplay &= ~((unsigned char)DATA_TYPE_THROTTLE);
    else if(buf == "-d help") {isTextBrowserDisplay &= 0b01111111; displayHelp();}
    else if(buf == "-r quat") CMD |= (unsigned char)COMMAND_TYPE_SEND_QUAT;
    else if(buf == "-r ~quat") CMD &= ~((unsigned char)COMMAND_TYPE_SEND_QUAT);
    else if(buf == "-r accel") CMD |= (unsigned char)COMMAND_TYPE_SEND_ACCEL;
    else if(buf == "-r ~accel") CMD &= ~((unsigned char)COMMAND_TYPE_SEND_ACCEL);
    else if(buf == "-r gyro") CMD |= (unsigned char)COMMAND_TYPE_SEND_GYRO;
    else if(buf == "-r ~gyro") CMD &= ~((unsigned char)COMMAND_TYPE_SEND_GYRO);
    else if(buf == "-r throttle") CMD |= (unsigned char)COMMAND_TYPE_SEND_THROTTLE;
    else if(buf == "-r ~throttle") CMD &= ~((unsigned char)COMMAND_TYPE_SEND_THROTTLE);
    else {ui->recieveTextBrowser->insertPlainText("w~r~o~n~g!!!\n"); ui->recieveTextBrowser->moveCursor(QTextCursor::End); return;}

    myCom->sendData(&CMD,DATA_TYPE_COMMAND);
    ui->recieveTextBrowser->setTextColor(Qt::lightGray);
}

void Widget::anchorClickedSlot(const QUrl &url)
{
    ShellExecuteA(NULL, "open", url.toString().toStdString().c_str(), "", "", SW_SHOW);
}

void Widget::on_clearbtn_clicked()
{
    ui->recieveTextBrowser->clear();
}

void Widget::on_restartBtn_clicked()
{
    isRun = true;
    on_runpauseBtn_clicked();
    char buf = COMMAND_TYPE_RESTART;
    myCom->sendData(&buf,DATA_TYPE_COMMAND);
}

void Widget::on_runpauseBtn_clicked()
{
    if(isRun)
    {
        CMD &= ~((unsigned char)COMMAND_TYPE_RUN);
        myCom->sendData(&CMD,DATA_TYPE_COMMAND);
        ui->runpauseBtn->setText("run");
        refreshTimer->stop();
        isRun = false;
    }
    else
    {
        CMD |= COMMAND_TYPE_RUN;
        myCom->sendData(&CMD,DATA_TYPE_COMMAND);
        ui->runpauseBtn->setText("pause");
        refreshTimer->start();
        isRun = true;
    }
}

void Widget::on_displayBTN_clicked()
{
    if(openGLWidget)
    {
        delete openGLWidget;
        openGLWidget = NULL;
        ui->displayBTN->setText(tr("打开动画"));
    }
    else
    {
        openGLWidget = new NeHeWidget;
        openGLWidget->show();
        connect(openGLWidget,SIGNAL(closeWidget()),this,SLOT(on_displayBTN_clicked()));
        ui->displayBTN->setText(tr("关闭动画"));
    }
}

void Widget::on_sendLineEdit_returnPressed()
{
    on_sendbtn_clicked();
}

