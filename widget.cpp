//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#include "widget.h"
#include "ui_widget.h"

float math_rsqrt(float number);

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    myCom = NULL;
    openGLWidget = NULL;
    refreshTimer = NULL;
    isFirst = true;
    isRun = false;
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
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->setText(tr("欢迎使用Spacelan's 串口调试助手！\n\nSpacelan's 串口调试助手是Spacelan写的第一个也是目前唯一一个有那么一丢丢实用价值的QT程序\n\nSpacelan's 串口调试助手是某个项目的基础程序\n\nSpacelan's 串口调试助手的代码参考了开源程序QCom"));
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertHtml(tr("<a href=\"http://www.qter.org/?page_id=203\">参考链接</a><br><br>"));
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertPlainText("GitHub ");
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertHtml("<a href=\"https://github.com/spacelan/MyCom.git\">https://github.com/spacelan/MyCom.git</a><br><br>");
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertPlainText("Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.");
    ui->recieveTextBrowser->moveCursor(QTextCursor::Start);
    ui->portnameComboBox->setCurrentIndex(3);
    ui->baudRateComboBox->setCurrentIndex(1);
}

Widget::~Widget()
{
    delete myCom;
    myCom = NULL;
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

void Widget::closeEvent(QCloseEvent *e)
{
    delete openGLWidget;
    openGLWidget = NULL;
    e->accept();
}

void Widget::getQuatData()
{
    long quat[4];
    const float q30 = 1073741824.0f;
    float w,x,y,z;

    if(myCom->getQuat(quat) == 0) return;

    w = (float)quat[0] / q30;
    x = (float)quat[1] / q30;
    y = (float)quat[2] / q30;
    z = (float)quat[3] / q30;

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

    QString str;
    str.sprintf("%f %f %f %f %f %f %f\n",
                w,x,y,z,pitch,roll,yaw);
    ui->recieveTextBrowser->setTextColor(Qt::black);
    ui->recieveTextBrowser->moveCursor(QTextCursor::End);
    ui->recieveTextBrowser->insertPlainText(str);

    str.sprintf("%f",w);
    ui->label_w_display->setText(str);
    str.sprintf("%f",x);
    ui->label_x_display->setText(str);
    str.sprintf("%f",y);
    ui->label_y_display->setText(str);
    str.sprintf("%f",z);
    ui->label_z_display->setText(str);
    str.sprintf("%f",pitch);
    ui->label_pitch_display->setText(str);
    str.sprintf("%f",roll);
    ui->label_roll_display->setText(str);
    str.sprintf("%f",yaw);
    ui->label_yaw_display->setText(str);
}

void Widget::on_openclosebtn_clicked()
{
    //启动后起一次打开，清除屏幕
    if(isFirst)
    {
        ui->recieveTextBrowser->clear();
        isFirst = false;
    }

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
    QString portName = ui->portnameComboBox->currentText();
    PortSettings settings;
    //配置串口
    settings.BaudRate = (BaudRateType)ui->baudRateComboBox->currentText().toInt();
    switch(ui->dataBitsComboBox->currentIndex())
    {
    case 0:
        settings.DataBits = (DATA_8);
        break;
    case 1:
        settings.DataBits = (DATA_7);
        break;
    default:
        settings.DataBits = (DATA_8);
    }
    switch(ui->parityComboBox->currentIndex())
    {
    case 0:
        settings.Parity = (PAR_NONE);
        break;
    case 1:
        settings.Parity = (PAR_ODD);
        break;
    case 2:
        settings.Parity = (PAR_EVEN);
        break;
    default:
        settings.Parity = (PAR_NONE);
    }
    switch(ui->stopbitsComboBox->currentIndex())
    {
    case 0:
        settings.StopBits = (STOP_1);
        break;
    case 1:
        settings.StopBits = (STOP_1_5);
        break;
    case 2:
        settings.StopBits = (STOP_2);
        break;
    default:
        settings.StopBits = (STOP_1);
    }
    settings.FlowControl = (FLOW_OFF);
    settings.Timeout_Millisec = (TIME_OUT);
    myCom = new Communication(portName,settings);
    //打开串口
    if(myCom->mySerialPort->open(QIODevice::ReadWrite))
    {
        refreshTimer = new QTimer;
        refreshTimer->setInterval(30);
        connect(refreshTimer,SIGNAL(timeout()),this,SLOT(getQuatData()));
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
        QMessageBox::critical(this, tr("打开失败"), tr("未能打开串口 ") + portName + tr("\n该串口设备不存在或已被占用"), QMessageBox::Ok);
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
    myCom->mySerialPort->write(buf);
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
        char buf = COMMAND_TYPE_PAUSE;
        myCom->sendData(&buf,DATA_TYPE_COMMAND);
        ui->runpauseBtn->setText("run");
        refreshTimer->stop();
        isRun = false;
    }
    else
    {
        char buf = COMMAND_TYPE_RUN;
        myCom->sendData(&buf,DATA_TYPE_COMMAND);
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

float math_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking（对浮点数的邪恶位级hack）
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?（这他妈的是怎么回事？）
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration （第一次牛顿迭代）
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed（第二次迭代，可以删除）

    return y;
}
