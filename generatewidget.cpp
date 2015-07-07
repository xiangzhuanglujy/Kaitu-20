#include "generatewidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>

GenerateWidget::GenerateWidget(QWidget *parent) :
    QWidget(parent)
{
    topGB=new QGroupBox(this);
    bottomGB=new QGroupBox(this);
//    topGB->setEnabled(false);
//    bottomGB->setEnabled(false);

    //topGroup
    userInfoLB=new QLabel(tr("User Information"));
    noLB=new QLabel(tr("Number"));
    recordIntervalLB=new QLabel(tr("Interval"));
    startDelayLB=new QLabel(tr("Delay Time"));
    tempAlarmUpLB=new QLabel(tr("TemperatureAlarm Upper Limit"));
    tempAlarmDownLB=new QLabel(tr("TemperatureAlarm Lower Limit"));
    humidityAlarmUpLB=new QLabel(tr("HumidityAlarm Upper Limit"));
    humidityAlarmDownLB=new QLabel(tr("HumidityAlarm Lower Limit"));

    userInfoTE=new QTextEdit();
    userInfoTE->setReadOnly(true);
    noLE=new QLineEdit();
    noLE->setReadOnly(true);
    recordIntervalLE=new QLineEdit();
    recordIntervalLE->setReadOnly(true);
    startDelayLE=new QLineEdit();
    startDelayLE->setReadOnly(true);
    tempAlarmUpLE=new QLineEdit();
    tempAlarmUpLE->setReadOnly(true);
    tempAlarmDownLE=new QLineEdit();
    tempAlarmDownLE->setReadOnly(true);
    humidityAlarmUpLE=new QLineEdit();
    humidityAlarmUpLE->setReadOnly(true);
    humidityAlarmDownLE=new QLineEdit();
    humidityAlarmDownLE->setReadOnly(true);

    QGridLayout *grid1=new QGridLayout(topGB);
    grid1->addWidget(userInfoLB,0,0);
    grid1->addWidget(userInfoTE,0,1);
    grid1->addWidget(noLB,1,0);
    grid1->addWidget(noLE,1,1);
    grid1->addWidget(recordIntervalLB,2,0);
    grid1->addWidget(recordIntervalLE,2,1);
    grid1->addWidget(tempAlarmUpLB,3,0);
    grid1->addWidget(tempAlarmUpLE,3,1);
    grid1->addWidget(humidityAlarmUpLB,3,2);
    grid1->addWidget(humidityAlarmUpLE,3,3);
    grid1->addWidget(tempAlarmDownLB,4,0);
    grid1->addWidget(tempAlarmDownLE,4,1);
    grid1->addWidget(humidityAlarmDownLB,4,2);
    grid1->addWidget(humidityAlarmDownLE,4,3);

    dataCountLB=new QLabel(tr("Data Sum"));
    tempMaxLB=new QLabel(tr("TemperatureMaximum"));
    tempMinLB=new QLabel(tr("TemperatureMinimum"));
    humidityMaxLB=new QLabel(tr("HumidityMaximum"));
    humidityMinLB=new QLabel(tr("HumidityMinimum"));
    tempAvgLB=new QLabel(tr("TemperatureAverage"));
    humidityAvgLB=new QLabel(tr("HumidityAverage"));
    startTimeLB=new QLabel(tr("Start Time"));
    endTimeLB=new QLabel(tr("End Time"));

    dataCountLE=new QLineEdit();
    dataCountLE->setReadOnly(true);
    tempMaxLE=new QLineEdit();
    tempMaxLE->setReadOnly(true);
    tempMinLE=new QLineEdit();
    tempMinLE->setReadOnly(true);
    humidityMaxLE=new QLineEdit();
    humidityMaxLE->setReadOnly(true);
    humidityMinLE=new QLineEdit();
    humidityMinLE->setReadOnly(true);
    tempAvgLE=new QLineEdit();
    tempAvgLE->setReadOnly(true);
    humidityAvgLE=new QLineEdit();
    humidityAvgLE->setReadOnly(true);
    startTimeLE=new QLineEdit();
    startTimeLE->setReadOnly(true);
    endTimeLE=new QLineEdit();
    endTimeLE->setReadOnly(true);

    QGridLayout *grid2=new QGridLayout(bottomGB);
    grid2->addWidget(dataCountLB,0,0);
    grid2->addWidget(dataCountLE,0,1);
    grid2->addWidget(tempMaxLB,1,0);
    grid2->addWidget(tempMaxLE,1,1);
    grid2->addWidget(humidityMaxLB,1,2);
    grid2->addWidget(humidityMaxLE,1,3);
    grid2->addWidget(tempMinLB,2,0);
    grid2->addWidget(tempMinLE,2,1);
    grid2->addWidget(humidityMinLB,2,2);
    grid2->addWidget(humidityMinLE,2,3);
    grid2->addWidget(tempAvgLB,3,0);
    grid2->addWidget(tempAvgLE,3,1);
    grid2->addWidget(humidityAvgLB,3,2);
    grid2->addWidget(humidityAvgLE,3,3);
    grid2->addWidget(startTimeLB,4,0);
    grid2->addWidget(startTimeLE,4,1);
    grid2->addWidget(endTimeLB,5,0);
    grid2->addWidget(endTimeLE,5,1);

    QVBoxLayout *vLayout=new QVBoxLayout(this);
    vLayout->addWidget(topGB,2);
    vLayout->addWidget(bottomGB,1);
}

void GenerateWidget::getParams()
{
    userInfoTE->setText(_recoder.userInfo);
    noLE->setText(_recoder.bianHao);
    recordIntervalLE->setText(_recoder.recordInterval+"H:M:S");
    startDelayLE->setText(QString("%1 H").arg(_recoder.startDelayTime));
    tempAlarmUpLE->setText(QString::number(_recoder.tempUp)+"°C");
    tempAlarmDownLE->setText(QString::number(_recoder.tempDown)+"°C");
    dataCountLE->setText(QString::number(_recoder.recordTimes));
    startTimeLE->setText(_recoder.startTime.toString(_displayFormat));
    endTimeLE->setText(_recoder.estimateStopTime.toString(_displayFormat));

    if(_deviceMode==TEMPERATUREONLY)
    {
        humidityAlarmUpLE->setEnabled(false);
        humidityAlarmDownLE->setEnabled(false);
        humidityAlarmUpLE->clear();
        humidityAlarmDownLE->clear();
    }
    else
    {
        humidityAlarmUpLE->setEnabled(true);
        humidityAlarmDownLE->setEnabled(true);
        humidityAlarmUpLE->setText(QString::number(_recoder.humidityUp)+"%RH");
        humidityAlarmDownLE->setText(QString::number(_recoder.humidityDown)+"%RH");
    }
    qDebug()<<"generateWidget receive successful";
}

void GenerateWidget::updateLabelWithDate()
{
    startTimeLE->setText(_recoder.startTime.toString(_displayFormat));
    endTimeLE->setText(_recoder.realStopTime.toString(_displayFormat));
}

void GenerateWidget::getMaxMinAvg()
{
    tempMaxLE->setText(QString::number(_tempHumidity.maxTemp,'f',1)+"°C");
    tempMinLE->setText(QString::number(_tempHumidity.minTemp,'f',1)+"°C");
    tempAvgLE->setText(QString::number(_tempHumidity.avgTemp,'f',1)+"°C");

    if(_deviceMode==TEMPERATUREONLY)
    {
        humidityMaxLE->setEnabled(false);
        humidityMinLE->setEnabled(false);
        humidityAvgLE->setEnabled(false);
        humidityMaxLE->clear();
        humidityMinLE->clear();
        humidityAvgLE->clear();
    }
    else
    {
        humidityMaxLE->setEnabled(true);
        humidityMinLE->setEnabled(true);
        humidityAvgLE->setEnabled(true);
        humidityMaxLE->setText(QString::number(_tempHumidity.maxHumidity,'f',1)+"%RH");
        humidityMinLE->setText(QString::number(_tempHumidity.minHumidity,'f',1)+"%RH");
        humidityAvgLE->setText(QString::number(_tempHumidity.avgHumidity,'f',1)+"%RH");
    }
    qDebug()<<"generate receive maxminavg";
}
