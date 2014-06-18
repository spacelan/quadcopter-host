//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>

enum DATA_TYPE
{
    DATA_TYPE_NONE = 0,
    DATA_TYPE_QUAT = 1,
    DATA_TYPE_ACCEL = DATA_TYPE_QUAT << 1,
    DATA_TYPE_GYRO = DATA_TYPE_ACCEL << 1,
    DATA_TYPE_EULER = DATA_TYPE_GYRO << 1,
    DATA_TYPE_THROTTLE = DATA_TYPE_EULER << 1,//!!!!
    DATA_TYPE_COMMAND = DATA_TYPE_THROTTLE << 1,
    DATA_TYPE_PARAM = DATA_TYPE_COMMAND << 1,
    DATA_TYPE_MESSAGE = DATA_TYPE_PARAM << 1,
    DATA_TYPE_ALL = DATA_TYPE_QUAT | DATA_TYPE_ACCEL | DATA_TYPE_GYRO | DATA_TYPE_EULER
        | DATA_TYPE_THROTTLE | DATA_TYPE_COMMAND | DATA_TYPE_PARAM | DATA_TYPE_MESSAGE
};

enum COMMAND_TYPE
{
    COMMAND_TYPE_NONE = 0,
    COMMAND_TYPE_RUN = 1,
    COMMAND_TYPE_RESTART = COMMAND_TYPE_RUN << 1,
    COMMAND_TYPE_SEND_QUAT = COMMAND_TYPE_RESTART << 1,
    COMMAND_TYPE_SEND_ACCEL = COMMAND_TYPE_SEND_QUAT << 1,
    COMMAND_TYPE_SEND_GYRO = COMMAND_TYPE_SEND_ACCEL << 1,
    COMMAND_TYPE_SEND_THROTTLE = COMMAND_TYPE_SEND_GYRO << 1,//!!!!
    COMMAND_TYPE_All = COMMAND_TYPE_RUN | COMMAND_TYPE_RESTART
        | COMMAND_TYPE_SEND_QUAT | COMMAND_TYPE_SEND_ACCEL | COMMAND_TYPE_SEND_GYRO | COMMAND_TYPE_SEND_THROTTLE
};

struct PortSettings
{
    QString PortName;
    QSerialPort::BaudRate BaudRate;
    QSerialPort::DataBits DataBits;
    QSerialPort::Parity Parity;
    QSerialPort::StopBits StopBits;
    QSerialPort::FlowControl FlowControl;
};

class Communication : public QObject
{
    Q_OBJECT
public:
    explicit Communication(QObject *parent = 0);
    ~Communication();

    bool openSerialPort(const PortSettings &settings);
    void sendData(void *data,int dataType);
    void sendDataXmodem(char *data, int length);
    void writeByte(char *data,int length);
    void writeByte(QByteArray &data);
    bool getQuat(long *needQuat);
    bool getAccel(short *needAccel);
    bool getGyro(short *needGyro);
    bool getThrottle(uint8_t *needThrottle);

signals:
    void dataReady(DATA_TYPE type);//成功获取一次数据发射

public slots:
    void getData();//获取数据

private:
    QSerialPort *mySerialPort;
    QTimer *refreshTimer;
    unsigned char isDataReady;//每一位分别记录数据是否更新，每读取一次相应位置零，没获取一次相应位置一
    int dataLength[129];    //DATA_TYPE中各数据长度
    long quat[4];
    short accel[3],gyro[3];
    uint8_t throttle[4];
};

#endif // COMMUNICATION_H
