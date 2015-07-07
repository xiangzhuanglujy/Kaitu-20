#ifndef REPORTWIDGET_H
#define REPORTWIDGET_H

#include <QWidget>

#include "check.h"
#include "imagelabel.h"

class QLabel;
class QTextBrowser;

class ReportWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ReportWidget(QWidget *parent = 0);
    void updateLabelWithDate();                                      //更新日期

signals:
    
public slots:
    void getParams();                                                   //获得记录仪基本参数
    void getMaxMinAvg();                                                //获得温湿度参数
    void label_pixmap();                                                //加载图片

private:
    QLabel *name;
    QLabel *date;
    QLabel *bianhao;
    QLabel *bianhao_value;
    QLabel *userInfo;
    QTextBrowser *userInfo_value;
    QLabel *startDelayTime;
    QLabel *startDelayTime_value;
    QLabel *recordInterval;
    QLabel *recordInterval_value;
    QLabel *dataCount;
    QLabel *dataCount_value;
    QLabel *tempUint;
    QLabel *tempUint_value;
    QLabel *startTime;
    QLabel *startTime_value;
    QLabel *endTime;
    QLabel *endTime_value;
    QLabel *maxtemp;
    QLabel *maxtemp_value;
    QLabel *mintemp;
    QLabel *mintemp_value;
    QLabel *tempAlarmUp;
    QLabel *tempAlarmUp_value;
    QLabel *tempAlarmDown;
    QLabel *tempAlarmDown_value;
    QLabel *maxhumidity;
    QLabel *maxhumidity_value;
    QLabel *minhumidity;
    QLabel *minhumidity_value;
    QLabel *humidityAlarmUp;
    QLabel *humidityAlarmUp_value;
    QLabel *humidityAlarmDown;
    QLabel *humidityAlarmDown_value;
    QLabel *tempAvg;
    QLabel *tempAvg_value;
    QLabel *humidityAvg;
    QLabel *humidityAvg_value;
    ImageLabel *img;   
};

#endif // REPORTWIDGET_H
