//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#include "communication/communication.h"
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

Communication::Communication(QObject *parent) :
    QObject(parent)
{
    mySerialPort = NULL;
    refreshTimer = NULL;
    isDataReady = 0;
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

bool Communication::openSerialPort(const PortSettings &settings)
{
    mySerialPort = new QSerialPort(settings.PortName,this);
    if(!mySerialPort->open(QIODevice::ReadWrite)) return false;
    mySerialPort->setBaudRate(settings.BaudRate);
    mySerialPort->setDataBits(settings.DataBits);
    mySerialPort->setParity(settings.Parity);
    mySerialPort->setStopBits(settings.StopBits);
    mySerialPort->setFlowControl(settings.FlowControl);
    connect(mySerialPort,SIGNAL(readyRead()),this,SLOT(getData()));
    return true;
}

void Communication::sendData(void *data, int dataType)
{
    char bufHead[] = {0xaa,0x55,dataType};
    writeByte(bufHead,3);
    writeByte((char*)data,dataLength[dataType]);
}

void Communication::writeByte(char *data, int length)
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

void Communication::writeByte(QByteArray &data)
{
    mySerialPort->write(data);
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
    static GET_DATA_STATE state = NEED_AA;
    static DATA_TYPE type = DATA_TYPE_NONE;
    static int length = 0;
    char byte;

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
        if(mySerialPort->bytesAvailable() < 1) return;
        mySerialPort->read(&byte,1);
        int i;
        for(i=0;i<6;i++)
            if((DATA_TYPE)byte == 1<<i) break;
        if(i>=6)
        {
            qDebug()<<"TYPE_WRONG "<<(int)byte;
            if(byte == (char)0xaa)
                state = NEED_55;
            else
                state = NEED_AA;
            return;
        }
        type = (DATA_TYPE)byte;
        length = dataLength[type];
        state = NEED_DATA;
    }
    if(state == NEED_DATA)
    {
        if(mySerialPort->bytesAvailable() < length + 2) return; //这个地方一定要+2，不然收到的crc可能会为0
        char temp[18];
        unsigned short crc;
        mySerialPort->read(temp,length);
        mySerialPort->read((char*)&crc,2);
        if(crc != math_crc16(0,temp,length))
        {
            qDebug()<<"CRC WRONGGGGGGGG "<<crc<<"!="<<math_crc16(0,temp,length);
            mySerialPort->clear();
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
        isDataReady |= (unsigned char)type;
        emit dataReady(type);
        state = NEED_AA;
        qDebug()<<"GET!!!!!!!!!!!!!!!!!!!!!!!!";
        qDebug()<<mySerialPort->bytesAvailable();
        if(mySerialPort->bytesAvailable()>40) mySerialPort->clear();
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
