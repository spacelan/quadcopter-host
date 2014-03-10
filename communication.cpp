//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#include "communication.h"
#include "QEventLoop"

enum GET_DATA_STATE
{
    NEED_AA = 0,
    NEED_55,
    NEED_TYPE,
    NEED_DATA
};

Communication::Communication(QObject *parent) :
    QObject(parent)
{
    mySerialPort = new QextSerialPort;
    refreshTimer = new QTimer();
    refreshTimer->setInterval(10);
    refreshTimer->start();
    connect(refreshTimer,SIGNAL(timeout()),this,SLOT(getData()));
    isDataReady = DATA_TYPE_NONE;
}

Communication::Communication(const QString &name, const PortSettings &settings, QObject *parent) :
    QObject(parent)
{
    mySerialPort = new QextSerialPort(name,settings);
    refreshTimer = new QTimer();
    refreshTimer->setInterval(10);
    refreshTimer->start();
    connect(refreshTimer,SIGNAL(timeout()),this,SLOT(getData()));
    isDataReady = false;

    dataLength[DATA_TYPE_NONE] = 0;
    dataLength[DATA_TYPE_QUAT] = 16;
    dataLength[DATA_TYPE_ACCEL] = 6;
    dataLength[DATA_TYPE_GYRO] = 6;
    dataLength[DATA_TYPE_COMMAND] = 1;
}

Communication::~Communication()
{
    delete mySerialPort;
    mySerialPort = NULL;
    delete refreshTimer;
    refreshTimer = NULL;
}

void Communication::sendData(void *data, int dataType)
{
    char bufHead[] = {0xaa,0x55,dataType};
    write(bufHead,3);
    write((char*)data,dataLength[dataType]);
}

void Communication::write(char *data, int length)
{
    QEventLoop myLoop;
    for(int i=0;i<length;i++)
    {
        mySerialPort->write(data,1);
        data++;
        //延个时
        QTimer::singleShot(1,&myLoop,SLOT(quit()));
        myLoop.exec();
    }
}

bool Communication::getQuat(long *needQuat)
{
    for(int i=0;i<4;i++) needQuat[i] = quat[i];
    if((isDataReady & DATA_TYPE_QUAT) == 0) return false;
    isDataReady &= ~((unsigned char)DATA_TYPE_QUAT);
    return true;
}

bool Communication::getAccel(short *needAccel)
{
    for(int i=0;i<3;i++) needAccel[i] = accel[i];
    if((isDataReady & DATA_TYPE_ACCEL) == 0) return false;
    isDataReady &= ~((unsigned char)DATA_TYPE_ACCEL);
    return true;
}

bool Communication::getGyro(short *needGyro)
{
    for(int i=0;i<3;i++) needGyro[i] = gyro[i];
    if((isDataReady & DATA_TYPE_GYRO) == 0) return false;
    isDataReady &= ~((unsigned char)DATA_TYPE_GYRO);
    return true;
}

void Communication::getData()
{
    static int state = NEED_AA;
    static char byte;

    if(state == NEED_AA)
    {
        if(mySerialPort->size() < 1) return;
        mySerialPort->read(&byte,1);
        if(byte != (char)0xaa) return;
        state = NEED_55;
    }
    if(state == NEED_55)
    {
        if(mySerialPort->size() < 1) return;
        mySerialPort->read(&byte,1);
        if(byte != (char)0x55)
        {
            if(byte == (char)0xaa) return;
            state = NEED_AA;
            return;
        }
        state = NEED_TYPE;
    }
    if(state == NEED_TYPE)
    {
        if(mySerialPort->size() < 1) return;
        mySerialPort->read(&byte,1);
        if((DATA_TYPE)byte == DATA_TYPE_NONE)
        {
            if(byte == (char)0xaa)
                state = NEED_55;
            else
                state = NEED_AA;
            return;
        }
        state = NEED_DATA;
    }
    if(state == NEED_DATA)
    {
        if(mySerialPort->size() < dataLength[(int)byte]) return;
        switch ((DATA_TYPE)byte)
        {
        case DATA_TYPE_QUAT:
            mySerialPort->read((char*)quat, dataLength[(int)byte]);
            break;
        case DATA_TYPE_ACCEL:
            mySerialPort->read((char*)accel,dataLength[(int)byte]);
            break;
        case DATA_TYPE_GYRO:
            mySerialPort->read((char*)gyro, dataLength[(int)byte]);
            break;
        default:
            break;
        }
        isDataReady |= (DATA_TYPE)byte;
        state = NEED_AA;
    }
}
