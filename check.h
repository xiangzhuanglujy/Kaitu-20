#ifndef CHECK_H
#define CHECK_H

#include <QByteArray>
#include <QString>
#include <QDateTime>

//#define FACTOR      12
#define FACTOR      17

const QString DEFAULT_TIME_FORMAT=     "yyyy-MM-dd hh:mm:ss";

/*************************************************
 *记录仪基本参数
 ************************************************/
class Recoder
{
public:
    Recoder()
    {
    }

    QString dataName;               //数据库名
    QString model;                  //型号
    QString workStatus;             //工作状态
    QDateTime startTime;              //启动时间
    QDateTime frontTestTime;          //上次联调时间
    QDateTime estimateStopTime;       //预计停止时间
    QDateTime realStopTime;           //实际停止时间
    QString recordInterval;         //记录间隔
    QDateTime clock;                  //记录仪时钟
    QString bianHao;                //编号
    QString userInfo;               //用户信息
    bool keyStop;                   //按键停止
    bool toneSetting;               //提示音设置
    bool tempUint;                  //温度单位
    ushort recordNo;                //仪表站号
    ushort alarmSetting;            //报警设置
    uint totalSpace;                //总空间
    uint recordTimes;               //已记录次数
    double startDelayTime;          //启动延时时间
    double tempUp;                  //温度上限
    double tempDown;                //温度下限
    double tempCorrect;             //温度校正
    double humidityUp;              //湿度上限
    double humidityDown;            //湿度下限
    double humidityCorrect;         //湿度校正
};


/*****仪器型号********************/
typedef enum _DeviceMode
{
    TEMPERATUREONLY,
    HUMIDITYONLY,
    BOTHHAVING,
    UNKNOWN_DEVIDE
}DeviceMode;


/**********************************************
 *温湿度基本参数
 *********************************************/
typedef struct empHumidity
{
    double maxTemp;                //最大温度
    double minTemp;                //最小温度
    double avgTemp;                //平均温度
    double maxHumidity;            //最大湿度
    double minHumidity;            //最小湿度
    double avgHumidity;            //平均湿度
}TempHumidity;

typedef struct threadPM
{
    double *pTempBuf;
    double *pHumidBuf;
    int size;
    int interval;
    QDateTime date;
}ThreadPM;

typedef struct _serialport
{
    int baudRate;
    int dataBits;
    int parity;
    int stopBits;
}SerialPort;

extern Recoder _recoder;                                                        //记录仪基本信息
extern TempHumidity _tempHumidity;                                              //温度湿度基本信息
extern ThreadPM _threadPM;
extern bool isDevice;
extern DeviceMode _deviceMode;
extern QString _dateFormat;
extern QString _timeFormat;
extern QString _displayFormat;
extern SerialPort _serialPort;

uchar accumulationAnd(const QByteArray &array);                                 //累加和校验
void init();                                                                    //初始化
DeviceMode queryKindsOfDevice(QString _model/*,char _ch='H'*/);                 //查询某种设备

#endif // CHECK_H
