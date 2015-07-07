//常规项

#ifndef GENERATEWIDGET_H
#define GENERATEWIDGET_H

#include <QWidget>

#include "check.h"

class QGroupBox;
class QLabel;
class QLineEdit;
class QTextEdit;

class GenerateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GenerateWidget(QWidget *parent = 0);
    void getMaxMinAvg();                                                //获得温度湿度基本信息
    void getParams();                                                   //获得记录仪基本信息
    void updateLabelWithDate();

signals:
    
public slots:

private:
    QGroupBox *topGB;
    QGroupBox *bottomGB;

    QLabel *userInfoLB;
    QLabel *noLB;
    QLabel *recordIntervalLB;
    QLabel *startDelayLB;
    QLabel *tempAlarmUpLB;
    QLabel *tempAlarmDownLB;
    QLabel *humidityAlarmUpLB;
    QLabel *humidityAlarmDownLB;

    QTextEdit *userInfoTE;
    QLineEdit *noLE;
    QLineEdit *recordIntervalLE;
    QLineEdit *startDelayLE;
    QLineEdit *tempAlarmUpLE;
    QLineEdit *tempAlarmDownLE;
    QLineEdit *humidityAlarmUpLE;
    QLineEdit *humidityAlarmDownLE;

    QLabel *dataCountLB;
    QLabel *tempMaxLB;
    QLabel *tempMinLB;
    QLabel *humidityMaxLB;
    QLabel *humidityMinLB;
    QLabel *tempAvgLB;
    QLabel *humidityAvgLB;
    QLabel *startTimeLB;
    QLabel *endTimeLB;

    QLineEdit *dataCountLE;
    QLineEdit *tempMaxLE;
    QLineEdit *tempMinLE;
    QLineEdit *humidityMaxLE;
    QLineEdit *humidityMinLE;
    QLineEdit *tempAvgLE;
    QLineEdit *humidityAvgLE;
    QLineEdit *startTimeLE;
    QLineEdit *endTimeLE;

};

#endif // GENERATEWIDGET_H
