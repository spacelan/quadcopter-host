//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QTimer>
#include "qextserialport/qextserialport.h"

enum DATA_TYPE
{
    DATA_TYPE_NONE = 0,
    DATA_TYPE_QUAT = 1,
    DATA_TYPE_ACCEL = DATA_TYPE_QUAT << 1,
    DATA_TYPE_GYRO = DATA_TYPE_ACCEL << 1,
    DATA_TYPE_COMMAND = DATA_TYPE_GYRO << 1,
    DATA_TYPE_PARAM = DATA_TYPE_COMMAND << 1,
    DATA_TYPE_MESSAGE = DATA_TYPE_PARAM << 1
};

enum COMMAND_TYPE
{
    COMMAND_TYPE_NONE = 0,
    COMMAND_TYPE_RUN = 1,
    COMMAND_TYPE_PAUSE = COMMAND_TYPE_RUN << 1,
    COMMAND_TYPE_RESTART = COMMAND_TYPE_PAUSE <<1
};

class Communication : public QObject
{
    Q_OBJECT
public:
    explicit Communication(QObject *parent = 0);
    Communication(const QString &name,const PortSettings &settings,QObject *parent = 0);
    ~Communication();
    void sendData(void *data,int dataType);
    void write(char *data,int length);
    bool getQuat(long *needQuat);
    bool getAccel(short *needAccel);
    bool getGyro(short *needGyro);

    QextSerialPort *mySerialPort;

signals:

public slots:
    void getData();

private:
    QTimer *refreshTimer;
    unsigned char isDataReady;
    long quat[4];
    short accel[3],gyro[3];
    COMMAND_TYPE cmd;
    int dataLength[129];    //DATA_TYPE中各数据长度
};

#endif // COMMUNICATION_H
