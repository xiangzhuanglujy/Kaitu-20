/************************************************************************************
槽函数的线程和主线程是一样的！
如果你看过Qt自带的例子，你会发现 QThread 中 slot 和 run 函数共同操作的对象，都会用QMutex锁住。
为什么？因为slot和run处于不同线程，需要线程间的同步！
*************************************************************************************/

#include "mythread.h"
#include "check.h"

#include <QDateTime>
#include <QDebug>
#include <QMutex>


double temperatureBuf[16010];
double humidityBuf[16010];

QMutex mutex;                                   //互斥锁

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
    _threadPM.size=0;
    _threadPM.interval=0;
    _threadPM.date=QDateTime::currentDateTime();
    _threadPM.pHumidBuf=humidityBuf;
    _threadPM.pTempBuf=temperatureBuf;
}

void MyThread::getdata(uchar *data, int len, QDateTime _date, QString _interval)
{
    mutex.lock();
    value_t=data;
    length_t=len;
    date_t=_date;
    interval_t=_interval;
    mutex.unlock();
}

void MyThread::run()
{
    mutex.lock();
    uchar *value=value_t;
    int size=length_t;
    QDateTime date=date_t;
    QString interval=interval_t;
    mutex.unlock();

    int time=0;
    if(interval.length()==8)
    {
        time=interval.left(2).toInt()*3600+interval.mid(3,2).toInt()*60+interval.right(2).toInt();
    }
    qDebug()<<"thread interval time"<<time;

    double temp=0.0;            //记录点温度
    double humidity=0.0;        //记录点湿度
    size=size>16000?16000:size;

    for(int i=0;i<size;i++)
    {
//        if(value[FACTOR*i+1]==0x00)
//        {
//            temp=QString("%1%2.%3")
//                    .arg(value[3+FACTOR*i])
//                    .arg(value[4+FACTOR*i])
//                    .arg(value[5+FACTOR*i])
//                    .toDouble();
//        }
//        else
//        {
//            temp=QString("-%1%2.%3")
//                    .arg(value[3+FACTOR*i])
//                    .arg(value[4+FACTOR*i])
//                    .arg(value[5+FACTOR*i])
//                    .toDouble();
//        }
//        humidity=QString("%1%2.%3")
//                .arg(value[7+FACTOR*i])
//                .arg(value[8+FACTOR*i])
//                .arg(value[9+FACTOR*i])
//                .toDouble();

        if(value[FACTOR*i+1]==0x00)
        {
            temp=QString("%1%2.%3")
                    .arg(value[8+FACTOR*i])
                    .arg(value[9+FACTOR*i])
                    .arg(value[10+FACTOR*i])
                    .toDouble();
        }
        else
        {
            temp=QString("-%1%2.%3")
                    .arg(value[8+FACTOR*i])
                    .arg(value[9+FACTOR*i])
                    .arg(value[10+FACTOR*i])
                    .toDouble();
        }
        humidity=QString("%1%2.%3")
                .arg(value[12+FACTOR*i])
                .arg(value[13+FACTOR*i])
                .arg(value[14+FACTOR*i])
                .toDouble();

        temperatureBuf[i]=temp;
        humidityBuf[i]=humidity;
    }
    _threadPM.date=date;
    _threadPM.interval=time;
    _threadPM.pHumidBuf=humidityBuf;
    _threadPM.pTempBuf=temperatureBuf;
    _threadPM.size=size;

    qDebug()<<"thread devide success"<<temperatureBuf[0]<<humidityBuf[0];
    emit plot(temperatureBuf,humidityBuf,size,date,time);           //绘图

    double maxT=temperatureBuf[0];
    double minT=temperatureBuf[0];
    double avgT=temperatureBuf[0];
    double maxH=humidityBuf[0];
    double minH=humidityBuf[0];
    double avgH=humidityBuf[0];

    for(int i=1;i<size;i++)
    {
        if(temperatureBuf[i]>maxT)
        {
            maxT=temperatureBuf[i];
        }
        if(temperatureBuf[i]<minT)
        {
            minT=temperatureBuf[i];
        }
        avgT+=temperatureBuf[i];

        if(humidityBuf[i]>maxH)
        {
            maxH=humidityBuf[i];
        }
        if(humidityBuf[i]<minH)
        {
            minH=humidityBuf[i];
        }
        avgH+=humidityBuf[i];
    }
    avgH/=size;
    avgT/=size;

    _tempHumidity.maxTemp=maxT;
    _tempHumidity.minTemp=minT;
    _tempHumidity.avgTemp=QString::number(avgT,'f',1).toDouble();
    _tempHumidity.maxHumidity=maxH;
    _tempHumidity.minHumidity=minH;
    _tempHumidity.avgHumidity=QString::number(avgH,'f',1).toDouble();

    emit saveCompletely();                              //温湿度显示
}
