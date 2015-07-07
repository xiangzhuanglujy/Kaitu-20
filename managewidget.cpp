#include "managewidget.h"
#include "mysql.h"
#include "check.h"

#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>

ManageWidget::ManageWidget(QWidget *parent) :
    QWidget(parent)
{
    dataNameLB=new QLabel(tr("Data Name"));
    startTineLB=new QLabel(tr("Search Start Time"));
    endTimeLB=new QLabel(tr("Search End Time"));
    tempUpLB=new QLabel(tr("TemperatureAlarm Upper Limit")+QString("°C"));
    tempDownLB=new QLabel(tr("TemperatureAlarm Lower Limit")+QString("°C"));
    humidityUpLB=new QLabel(tr("HumidityAlarm Upper Limit")+QString("%RH"));
    humidityDownLB=new QLabel(tr("HumidityAlarm Lower Limit")+QString("%RH"));

    startTineDTE=new QDateTimeEdit();
    startTineDTE->setWrapping(true);
    endTineDTE=new QDateTimeEdit();
    endTineDTE->setWrapping(true);
    startTineDTE->setDisplayFormat(_displayFormat);
    endTineDTE->setDisplayFormat(_displayFormat);

    tempUpLE=new QLineEdit();
    tempDownLE=new QLineEdit();
    humidityUpLE=new QLineEdit();
    humidityDownLE=new QLineEdit();
    humidityUpLE->setEnabled(false);
    humidityDownLE->setEnabled(false);

    tempQueryCB=new QRadioButton(tr("TemperatureInquiry"));
    tempQueryCB->setChecked(true);
    humidityQueryCB=new QRadioButton(tr("HumidityInquiry"));

    dataNameCB=new QComboBox();

    QVBoxLayout *vLayout=new QVBoxLayout(this);
    vLayout->addWidget(dataNameLB);
    vLayout->addWidget(dataNameCB);
    vLayout->addWidget(startTineLB);
    vLayout->addWidget(startTineDTE);
    vLayout->addWidget(endTimeLB);
    vLayout->addWidget(endTineDTE);
    vLayout->addWidget(tempQueryCB);
    vLayout->addWidget(tempUpLB);
    vLayout->addWidget(tempUpLE);
    vLayout->addWidget(tempDownLB);
    vLayout->addWidget(tempDownLE);
    vLayout->addWidget(humidityQueryCB);
    vLayout->addWidget(humidityUpLB);
    vLayout->addWidget(humidityUpLE);
    vLayout->addWidget(humidityDownLB);
    vLayout->addWidget(humidityDownLE);
    vLayout->setContentsMargins(0,0,400,0);

    connect(startTineDTE,SIGNAL(dateTimeChanged(QDateTime)),this,SLOT(setQueryTable()));
    connect(endTineDTE,SIGNAL(dateTimeChanged(QDateTime)),this,SLOT(setQueryTable()));
    connect(tempUpLE,SIGNAL(textChanged(QString)),this,SLOT(setQueryTable()));
    connect(tempDownLE,SIGNAL(textChanged(QString)),this,SLOT(setQueryTable()));
    connect(humidityUpLE,SIGNAL(textChanged(QString)),this,SLOT(setQueryTable()));
    connect(humidityDownLE,SIGNAL(textChanged(QString)),this,SLOT(setQueryTable()));

    connect(tempQueryCB,SIGNAL(clicked()),this,SLOT(tempQueryCB_clicked()));
    connect(humidityQueryCB,SIGNAL(clicked()),this,SLOT(humidityQueryCB_clicked()));
    connect(dataNameCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(dataNameCB_textChanged(QString)));
}

void ManageWidget::clearSelect()
{
    tempQueryCB->setChecked(true);
    tempDownLE->clear();
    tempUpLE->clear();
    humidityDownLE->clear();
    humidityUpLE->clear();
    startTineDTE->setDateTime(_recoder.startTime);
    endTineDTE->setDateTime(_recoder.realStopTime);
}

void ManageWidget::setQueryTable()
{
    emit queryTable(startTineDTE->dateTime(),
                    endTineDTE->dateTime(),
                    tempUpLE->text(),
                    tempDownLE->text(),
                    humidityUpLE->text(),
                    humidityDownLE->text());
}

void ManageWidget::dataNameCB_textChanged(QString name)
{
    if(name.isEmpty())
    {
        return;
    }
    QSqlQuery query(sqldb);
//    query.prepare("select * from report where name=:name");
//    query.bindValue(":name",name);
//    query.exec();
    query.exec(QString("select * from report where name='%1'").arg(name));
    while(query.next())
    {
        _recoder.dataName=query.value(0).toString();
        _recoder.bianHao=query.value(1).toString();
        _recoder.userInfo=query.value(2).toString();
        _recoder.startDelayTime=query.value(3).toDouble();
        _recoder.recordInterval=query.value(4).toString();
        _recoder.recordTimes=query.value(5).toUInt();
        _recoder.startTime=query.value(6).toDateTime();
        _recoder.startTime=_recoder.startTime.addSecs(8*3600);
//        qDebug()<<"queryyyyyyyyyyyyyyyyyyyyyyyy"<<query.value(6).toString();
        _recoder.realStopTime=query.value(7).toDateTime();
        _recoder.realStopTime=_recoder.realStopTime.addSecs(8*3600);
//        qDebug()<<"queryyyyyyyyyyyyyyyyyyyyyyyy"<<_recoder.realStopTime.toString(DEFAULT_TIME_FORMAT);
        _recoder.tempUp=query.value(8).toDouble();
        _recoder.tempDown=query.value(9).toDouble();
        _recoder.humidityUp=query.value(10).toDouble();
        _recoder.humidityDown=query.value(11).toDouble();
        _tempHumidity.maxTemp=query.value(12).toDouble();
        _tempHumidity.minTemp=query.value(13).toDouble();
        _tempHumidity.avgTemp=query.value(14).toDouble();
        _tempHumidity.maxHumidity=query.value(15).toDouble();
        _tempHumidity.minHumidity=query.value(16).toDouble();
        _tempHumidity.avgHumidity=query.value(17).toDouble();
        _recoder.model=query.value(18).toString();
        _recoder.clock=query.value(19).toDateTime();
        _recoder.frontTestTime=query.value(20).toDateTime();
//        _recoder.frontTestTime=_recoder.frontTestTime.addSecs(8*3600);
        _recoder.estimateStopTime=query.value(21).toDateTime();
        _recoder.estimateStopTime=_recoder.estimateStopTime.addSecs(8*3600);
        _recoder.workStatus=query.value(22).toString();
    }
    query.clear();

    //清空时间选择
//    clearSelect();

    //判断仪器型号
    _deviceMode=queryKindsOfDevice(_recoder.model);
    if(_deviceMode==TEMPERATUREONLY)
    {
        humidityQueryCB->setEnabled(false);
    }
    else
    {
        humidityQueryCB->setEnabled(true);
    }

    emit changeDataName(name);
}

void ManageWidget::changeFlag()
{
    qDebug()<<"find device over"<<_deviceMode;
    if(_deviceMode==TEMPERATUREONLY)
    {
        tempQueryCB->setChecked(true);
        humidityQueryCB->setEnabled(false);
    }
    else
    {
        humidityQueryCB->setEnabled(true);
    }

    if(!isDevice)
    {
        QSqlQuery query(sqldb);
        dataNameCB->clear();
        query.exec("select name from storeName");
        while(query.next())
        {
            dataNameCB->addItem(query.value(0).toString());
        }
        query.clear();
        if(dataNameCB->count()==0)
        {
            emit enableWindow(true);
        }
    }
    else
    {
        dataNameCB->clear();
        dataNameCB->setEnabled(false);
    }
}

void ManageWidget::removeFromQCombo(QString name)
{
    int index=dataNameCB->findText(name);
    dataNameCB->removeItem(index);

    if(dataNameCB->count()==0)
    {
        _threadPM.size=0;
    }
}

void ManageWidget::updateLabelWithDate()
{
    startTineDTE->setDisplayFormat(_displayFormat);
    endTineDTE->setDisplayFormat(_displayFormat);
}

//void ManageWidget::createQrueySql()
//{
//    QString table=dataNameCB->currentText();
//    if(table.isEmpty())
//    {
//        return;
//    }
//    QString startTime=startTineDTE->dateTime().toString(DEFAULT_TIME_FORMAT);
//    QString endTime=endTineDTE->dateTime().toString(DEFAULT_TIME_FORMAT);

//    QString sql=QString("select * from '%1' where time between '%2' and '%3'")
//            .arg(table).arg(startTime).arg(endTime);

//    if(tempQueryCB->isChecked()==true)
//    {
//        if(!tempUpLE->text().isEmpty())
//        {
//            sql+=QString("And temp < '%1' ").arg(tempUpLE->text().toDouble());
//        }
//        if(!tempDownLE->text().isEmpty())
//        {
//            sql+=QString("And temp > '%1';").arg(tempDownLE->text().toDouble());
//        }
//    }

//    if(humidityQueryCB->isChecked()==true)
//    {
//        if(!humidityUpLE->text().isEmpty())
//        {
//            sql+=QString("And humidity < '%1'").arg(humidityUpLE->text().toDouble());
//        }
//        if(!humidityDownLE->text().isEmpty())
//        {
//            sql+=QString("And humidity > '%1'").arg(humidityDownLE->text().toDouble());
//        }
//    }

//    emit querySql(sql);
//}

void ManageWidget::tempQueryCB_clicked()
{
    tempUpLE->setEnabled(true);
    tempDownLE->setEnabled(true);
    humidityUpLE->clear();
    humidityDownLE->clear();
    humidityUpLE->setEnabled(false);
    humidityDownLE->setEnabled(false);
}

void ManageWidget::humidityQueryCB_clicked()
{
    tempUpLE->clear();
    tempDownLE->clear();
    tempUpLE->setEnabled(false);
    tempDownLE->setEnabled(false);
    humidityUpLE->setEnabled(true);
    humidityDownLE->setEnabled(true);
}
