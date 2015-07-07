#include "reportwidget.h"

#include <QLabel>
#include <QGridLayout>
#include <QDebug>
#include <QDateTime>
#include <QTextBrowser>

ReportWidget::ReportWidget(QWidget *parent) :
    QWidget(parent)
{
    name=new QLabel();
    name->setFont(QFont("Times",14));
    date=new QLabel();
    bianhao=new QLabel(tr("Number"));
    bianhao_value=new QLabel();
    userInfo=new QLabel(tr("User Information"));
    userInfo_value=new QTextBrowser();
    userInfo_value->setFixedSize(200,50);
    userInfo_value->setObjectName("userTextBrowser");
    startDelayTime=new QLabel(tr("Delay Time"));
    startDelayTime_value=new QLabel();
    recordInterval=new QLabel(tr("Interval"));
    recordInterval_value=new QLabel();
    dataCount=new QLabel(tr("Data Sum"));
    dataCount_value=new QLabel();
    tempUint=new QLabel(tr("Temperature Unit"));
    tempUint_value=new QLabel("°C");
    startTime=new QLabel(tr("Start Time"));
    startTime_value=new QLabel();
    endTime=new QLabel(tr("End Time"));
    endTime_value=new QLabel();
    maxtemp=new QLabel(tr("TemperatureMaximum Value"));
    maxtemp_value=new QLabel();
    mintemp=new QLabel(tr("TemperatureMinimum Value"));
    mintemp_value=new QLabel();
    tempAlarmUp=new QLabel(tr("TemperatureAlarm Upper Limit"));
    tempAlarmUp_value=new QLabel();
    tempAlarmDown=new QLabel(tr("TemperatureAlarm Lower Limit"));
    tempAlarmDown_value=new QLabel();
    maxhumidity=new QLabel(tr("HumidityMaximum Value"));
    maxhumidity_value=new QLabel();
    minhumidity=new QLabel(tr("HumidityMinimum Value"));
    minhumidity_value=new QLabel();
    humidityAlarmUp=new QLabel(tr("HumidityAlarm Upper Limit"));
    humidityAlarmUp_value=new QLabel();
    humidityAlarmDown=new QLabel(tr("HumidityAlarm Lower Limit"));
    humidityAlarmDown_value=new QLabel();
    tempAvg=new QLabel(tr("TemperatureAverage"));
    tempAvg_value=new QLabel();
    humidityAvg=new QLabel(tr("HumidityAverage"));
    humidityAvg_value=new QLabel();
    img=new ImageLabel();
    img->setFixedSize(680,370);
    img->setVisible(false);

    QGridLayout *grid=new QGridLayout(this);
    grid->addWidget(name,0,1,1,2,Qt::AlignCenter);
    grid->addWidget(date,0,3);
    grid->addWidget(bianhao,1,0);
    grid->addWidget(bianhao_value,1,1);
    grid->addWidget(userInfo,1,2);
    grid->addWidget(userInfo_value,1,3);
    grid->addWidget(startDelayTime,2,0);
    grid->addWidget(startDelayTime_value,2,1);
    grid->addWidget(recordInterval,2,2);
    grid->addWidget(recordInterval_value,2,3);
    grid->addWidget(dataCount,3,0);
    grid->addWidget(dataCount_value,3,1);
    grid->addWidget(tempUint,3,2);
    grid->addWidget(tempUint_value,3,3);
    grid->addWidget(startTime,4,0);
    grid->addWidget(startTime_value,4,1);
    grid->addWidget(endTime,4,2);
    grid->addWidget(endTime_value,4,3);
    grid->addWidget(maxtemp,5,0);
    grid->addWidget(maxtemp_value,5,1);
    grid->addWidget(mintemp,5,2);
    grid->addWidget(mintemp_value,5,3);
    grid->addWidget(tempAlarmUp,6,0);
    grid->addWidget(tempAlarmUp_value,6,1);
    grid->addWidget(tempAlarmDown,6,2);
    grid->addWidget(tempAlarmDown_value,6,3);
    grid->addWidget(maxhumidity,7,0);
    grid->addWidget(maxhumidity_value,7,1);
    grid->addWidget(minhumidity,7,2);
    grid->addWidget(minhumidity_value,7,3);
    grid->addWidget(humidityAlarmUp,8,0);
    grid->addWidget(humidityAlarmUp_value,8,1);
    grid->addWidget(humidityAlarmDown,8,2);
    grid->addWidget(humidityAlarmDown_value,8,3);
    grid->addWidget(tempAvg,9,0);
    grid->addWidget(tempAvg_value,9,1);
    grid->addWidget(humidityAvg,9,2);
    grid->addWidget(humidityAvg_value,9,3);
    grid->addWidget(img,10,0,8,4,Qt::AlignCenter);
    grid->setContentsMargins(200,5,200,0);
}

void ReportWidget::getParams()
{
    name->setText(_recoder.model);
    date->setText(QDateTime::currentDateTime().toString(_displayFormat));
    bianhao_value->setText(_recoder.bianHao);
    userInfo_value->setText(_recoder.userInfo);
    startDelayTime_value->setText(QString::number(_recoder.startDelayTime,'f',1)+" H");
    recordInterval_value->setText(_recoder.recordInterval+" H:M:S");
    dataCount_value->setText(QString::number(_recoder.recordTimes));
    startTime_value->setText(_recoder.startTime.toString(_displayFormat));
    endTime_value->setText(_recoder.estimateStopTime.toString(_displayFormat));
    tempAlarmUp_value->setText(QString::number(_recoder.tempUp,'f',1)+"°C");
    tempAlarmDown_value->setText(QString::number(_recoder.tempDown,'f',1)+"°C");

    if(_deviceMode==TEMPERATUREONLY)
    {
        humidityAlarmUp_value->setEnabled(false);
        humidityAlarmDown_value->setEnabled(false);
    }
    else
    {
        humidityAlarmUp_value->setEnabled(true);
        humidityAlarmDown_value->setEnabled(true);
        humidityAlarmUp_value->setText(QString::number(_recoder.humidityUp,'f',1)+"%RH");
        humidityAlarmDown_value->setText(QString::number(_recoder.humidityDown,'f',1)+"%RH");
    }
    qDebug()<<"report get params"<<name->text();
}

void ReportWidget::getMaxMinAvg()
{
    maxtemp_value->setText(QString::number(_tempHumidity.maxTemp,'f',1)+"°C");
    mintemp_value->setText(QString::number(_tempHumidity.minTemp,'f',1)+"°C");
    tempAvg_value->setText(QString::number(_tempHumidity.avgTemp,'f',1)+"°C");

    if(_deviceMode==TEMPERATUREONLY)
    {
        maxhumidity_value->setEnabled(false);
        minhumidity_value->setEnabled(false);
        humidityAvg_value->setEnabled(false);
    }
    else
    {
        maxhumidity_value->setEnabled(true);
        minhumidity_value->setEnabled(true);
        humidityAvg_value->setEnabled(true);
        maxhumidity_value->setText(QString::number(_tempHumidity.maxHumidity,'f',1)+"%RH");
        minhumidity_value->setText(QString::number(_tempHumidity.minHumidity,'f',1)+"%RH");
        humidityAvg_value->setText(QString::number(_tempHumidity.avgHumidity,'f',1)+"%RH");
    }
    qDebug()<<"report get maxminavg";
}

void ReportWidget::label_pixmap()
{
    if(_recoder.model.isEmpty())
    {
        img->setVisible(false);
    }
    else
    {
        img->setVisible(true);
        img->update();
    }
}

void ReportWidget::updateLabelWithDate()
{
    date->setText(QDateTime::currentDateTime().toString(_displayFormat));
    startTime_value->setText(_recoder.startTime.toString(_displayFormat));
    endTime_value->setText(_recoder.realStopTime.toString(_displayFormat));
}
