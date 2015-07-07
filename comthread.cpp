#include "comthread.h"
#include "serial/qextserialport.h"
#include "check.h"

#include <QDebug>
#include <QMutex>
#include <QSettings>

QMutex listMutex;

ComThread::ComThread(QObject *parent) :
    QThread(parent)
{

    QSettings *configIniRead = new QSettings(".\\config.ini", QSettings::IniFormat);
    _serialPort.baudRate = configIniRead->value("/SerialPort/baudrate").toInt();
    _serialPort.dataBits = configIniRead->value("/SerialPort/databits").toInt();
    _serialPort.parity = configIniRead->value("/SerialPort/parity").toInt();
    _serialPort.stopBits = configIniRead->value("/SerialPort/stopbits").toInt();
    delete configIniRead;

    if(_serialPort.baudRate==0)
    {
        _serialPort.baudRate=76800;
        _serialPort.dataBits=8;
        _serialPort.parity=0;
        _serialPort.stopBits=0;
    }
//    qDebug()<<"serialport"<<_serialPort.baudRate<<_serialPort.dataBits
//              <<_serialPort.parity<<_serialPort.stopBits;
}

ComThread::~ComThread()
{
}

void ComThread::getComInfo(QStringList _list)
{
    listMutex.lock();
    comList=_list;
    listMutex.unlock();
}

void ComThread::run()
{
    listMutex.lock();
    QStringList tempList=comList;
    listMutex.unlock();

    myCom=new QextSerialPort();
    myCom->setBaudRate((BaudRateType)_serialPort.baudRate);
    myCom->setDataBits((DataBitsType)_serialPort.dataBits);
    myCom->setParity((ParityType)_serialPort.parity);
    myCom->setStopBits((StopBitsType)_serialPort.stopBits);
    myCom->setFlowControl(FLOW_OFF); //设置数据流控制，我们使用无数据流控制的默认设置
    myCom->setTimeout(10); //设置延时

    QSettings *configIniRead = new QSettings(".\\config.ini", QSettings::IniFormat);
    int ipResult = configIniRead->value("/Sleep/msleep").toInt();
    delete configIniRead;

    int size=tempList.size();
    for(int i=0;i<size;i++)
    {
        flag=false;
        myCom->setPortName(tempList.at(i));
        if(myCom->open(QIODevice::ReadWrite))
        {
            QByteArray array;
            array.append(0x1B);
            array.append(0x9F);
            array.append(0x0D);
            myCom->write(array);
            msleep(ipResult);
            array.clear();
            array=myCom->readAll();
            qDebug()<<"thread recv size:"<<array.size();
            if(array.size()==15)
            {
                qDebug()<<"thread find device";
                myCom->close();
                delete myCom;
                myCom=NULL;
                QString name=QString("%1%2%3%4%5%6%7%8%9%10")
                        .arg(array.at(3))
                        .arg(array.at(4))
                        .arg(array.at(5))
                        .arg(array.at(6))
                        .arg(array.at(7))
                        .arg(array.at(8))
                        .arg(array.at(9))
                        .arg(array.at(10))
                        .arg(array.at(11))
                        .arg(array.at(12));
//                name=name.left(name.indexOf(' '));
                emit findCom(tempList.at(i),name);
                return;
            }
        }
    }
    if(myCom->isOpen())
        myCom->close();
    delete myCom;
    myCom=NULL;
    emit findCom("NoPenging","");
}
