//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#include "communication.h"
#include <QEventLoop>
#include <QDebug>

enum GET_DATA_STATE
{
    NEED_AA = 0,
    NEED_55,
    NEED_TYPE,
    NEED_DATA
};

uint16_t math_crc16(uint16_t crc,const void * data,uint16_t len);

Communication::Communication(const QString &name, const PortSettings &settings, QObject *parent) :
    QObject(parent)
{
    mySerialPort = new QSerialPort(name,this);
    mySerialPort->setBaudRate(settings.BaudRate);
    mySerialPort->setDataBits(settings.DataBits);
    mySerialPort->setParity(settings.Parity);
    mySerialPort->setStopBits(settings.StopBits);
    mySerialPort->setFlowControl(settings.FlowControl);
    connect(mySerialPort,SIGNAL(readyRead()),this,SLOT(getData()));

    refreshTimer = new QTimer();
    refreshTimer->setInterval(5);
    refreshTimer->start();
//    connect(refreshTimer,SIGNAL(timeout()),this,SLOT(getData()));

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
    delete refreshTimer;
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
    char byte;
    static char type = DATA_TYPE_NONE;
    static int length = 0;

    if(state == NEED_AA)
    {
        qDebug()<<"NEED_AA";
        if(mySerialPort->bytesAvailable() < 1) return;
        mySerialPort->read(&byte,1);
        if(byte != (char)0xaa) return;
        state = NEED_55;
    }
    if(state == NEED_55)
    {
        qDebug()<<"NEED_55";
        if(mySerialPort->bytesAvailable() < 1) return;
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
//        qDebug()<<"NEED_TYPE";
        if(mySerialPort->bytesAvailable() < 1) return;
        mySerialPort->read(&byte,1);
        int i;
        for(i=0;i<6;i++)
            if((DATA_TYPE)byte == 1<<i) break;
        if(i>=6)
        {
            qDebug()<<"TYPE_WRONG "<<(int)byte;
//            qDebug()<<mySerialPort->bytesAvailable();
//            mySerialPort->clear();
            if(byte == (char)0xaa)
                state = NEED_55;
            else
                state = NEED_AA;
            return;
        }
        type = (DATA_TYPE)byte;
        length = dataLength[(int)byte];
        state = NEED_DATA;
    }
    if(state == NEED_DATA)
    {
//        qDebug()<<"NEED_DATA";
        if(mySerialPort->bytesAvailable() < length + 2) return; //这个地方一定要+2，不然收到的crc可能会为0
        char temp[18];
        unsigned short crc;
        mySerialPort->read(temp,length);
        mySerialPort->read((char*)&crc,2);
        if(crc != math_crc16(0,temp,length))
        {
            qDebug()<<"CRC WRONGGGGGGGG "<<crc<<"!="<<math_crc16(0,temp,length);
            mySerialPort->clear();
//            qDebug()<<mySerialPort->bytesAvailable();
//            mySerialPort->flush();
//            mySerialPort->close();
//            mySerialPort->open(QIODevice::ReadWrite);
            state = NEED_AA;
            return;
        }
        switch ((DATA_TYPE)type)
        {
        case DATA_TYPE_QUAT:
            for(int i=0;i<4;i++)
                quat[i] = ((long*)temp)[i];
            break;
        case DATA_TYPE_ACCEL:
            for(int i=0;i<3;i++)
                accel[i] = ((short*)temp)[i];
            break;
        case DATA_TYPE_GYRO:
            for(int i=0;i<3;i++)
                gyro[i] = ((short*)temp)[i];
            break;
        default:
            break;
        }
        isDataReady |= (DATA_TYPE)type;
        state = NEED_AA;
        qDebug()<<"GET!!!!!!!!!!!!!!!!!!!!!!!!";
    }
}

uint16_t math_crc16(uint16_t crc,const void * data,uint16_t len)
{
    const static uint16_t crc_tab[16] =
    {
        0x0000 , 0x1021 , 0x2042 , 0x3063 , 0x4084 , 0x50A5 , 0x60C6 , 0x70E7 ,
        0x8108 , 0x9129 , 0xA14A , 0xB16B , 0xC18C , 0xD1AD , 0xE1CE , 0xF1EF
    };
    uint8_t h_crc;
    const uint8_t * ptr = (const uint8_t *)data;
    //
    while(len --)
    {
        h_crc = (uint8_t)(crc >> 12);
        crc <<= 4;
        crc ^= crc_tab[h_crc ^ ((*ptr) >> 4)];
        //
        h_crc = crc >> 12;
        crc <<= 4;
        crc ^= crc_tab[h_crc ^ ((*ptr) & 0x0F)];
        //
        ptr ++;
    }
    //
    return crc;
}
