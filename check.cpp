#include "check.h"

/*************************************************
 *4个全局变量
 ************************************************/
SerialPort _serialPort;
Recoder _recoder;                                                   //记录仪属性
TempHumidity _tempHumidity;                                         //绘图属性
ThreadPM _threadPM;                                                 //温湿度属性
bool isDevice=false;                                                //是否找到通信设备
DeviceMode _deviceMode=BOTHHAVING;                                  //仪器类型
QString _dateFormat="yyyy-MM-dd";                                   //日期格式
QString _timeFormat="hh:mm:ss";                                     //时间格式
QString _displayFormat="yyyy-MM-dd hh:mm:ss";                       //总格式

/********************************************
 *累加和校验
 *******************************************/
uchar accumulationAnd(const QByteArray &array)
{
    uchar sz=0;
    uint temp=0;
    int size=array.size();
    for(int i=0;i<size;i++)
    {
        temp+=(uchar)(array.at(i));
    }
    sz=temp%256;
    return sz;
}

DeviceMode queryKindsOfDevice(QString _model/*,char _ch*/)
{
    int fh= _model.indexOf('H');
    if(-1==fh)
    {
        return TEMPERATUREONLY;
    }
    return BOTHHAVING;

//    int fc= _model.indexOf('C');

//    if(fc!=-1&&fh!=-1)
//    {
//        return BOTHHAVING;
//    }
//    else if(fc!=-1&&fh==-1)
//    {
//        return TEMPERATUREONLY;
//    }
//    else if(fc==-1&&fh!=-1)
//    {
//        return HUMIDITYONLY;
//    }

//    return UNKNOWN_DEVIDE;
}

/*****************************************************
 *初始化
 ****************************************************/
void init()
{
    _recoder.dataName="";
    _recoder.alarmSetting=0;
    _recoder.bianHao="1234567890";
    _recoder.clock=QDateTime::currentDateTime();
    _recoder.estimateStopTime=QDateTime::currentDateTime();
    _recoder.frontTestTime=QDateTime::currentDateTime();
    _recoder.humidityCorrect=0.0;
    _recoder.humidityDown=0.0;
    _recoder.humidityUp=0.0;
    _recoder.keyStop=0;
    _recoder.model="";
    _recoder.realStopTime=QDateTime::currentDateTime();
    _recoder.recordInterval="00:00:10";
    _recoder.recordNo=123;
    _recoder.recordTimes=16000;
    _recoder.startDelayTime=0.0;
    _recoder.startTime=QDateTime::currentDateTime();
    _recoder.tempCorrect=0.0;
    _recoder.tempDown=0.0;
    _recoder.tempUint=0;
    _recoder.tempUp=0.0;
    _recoder.toneSetting=0;
    _recoder.totalSpace=16000;
    _recoder.userInfo="hello";
    _recoder.workStatus="Stopped";

    _tempHumidity.avgHumidity=0.0;
    _tempHumidity.avgTemp=0.0;
    _tempHumidity.maxHumidity=0.0;
    _tempHumidity.maxTemp=0.0;
    _tempHumidity.minHumidity=0.0;
    _tempHumidity.minTemp=0.0;
}
