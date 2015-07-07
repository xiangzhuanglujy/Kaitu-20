#include "wdatastore.h"
#include "mysql.h"
#include "check.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QDebug>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QDateTime>

WDataStore::WDataStore(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("The Record of Temperature Data"));


    okBtn=new QPushButton(tr("Ok"));
    cancelBtn=new QPushButton(tr("Cancel"));
    edit=new QLineEdit();
    QRegExp regExp("[a-zA-Z0-9]{20}");
    edit->setValidator(new QRegExpValidator(regExp, this));
    label=new QLabel(tr("Please input the database name to be saved"));
    edit->setFocus();

    QGridLayout *layout=new QGridLayout(this);
    layout->addWidget(label,0,0);
    layout->addWidget(okBtn,0,1);
    layout->addWidget(cancelBtn,1,1);
    layout->addWidget(edit,2,0,1,2);

    connect(okBtn,SIGNAL(clicked()),this,SLOT(okBtn_clicked()));
    connect(cancelBtn,SIGNAL(clicked()),this,SLOT(close()));
}

void WDataStore::okBtn_clicked()
{
    if(0==_threadPM.size)
    {
        QMessageBox::information(this,tr("tips"),tr("NullData"));
        return;
    }

    QSqlQuery query(sqldb);
    int count=0;
    QString table=edit->text();
    QString sql=QString("select * from storeName where name='%1'").arg(table);
    query.exec(sql);
    while(query.next())
    {
        count++;
    }
    if(0==count)
    {
        QSqlDatabase::database().transaction();
        sql=QString("insert into storeName(name) values('%1')").arg(table);
        int bret=query.exec(sql);
        qDebug()<<"insert into"<<bret;
        if(!bret)
        {
            QSqlDatabase::database().rollback();
            query.clear();
            return;
        }
        sql=QString("CREATE TABLE '%1'"
                "(no INTEGER,"
                "time DATETIME,"
                "temp DOUBLE,"
                "humidity DOUBLE)").arg(table);
        bret=query.exec(sql);
        qDebug()<<"create table"<<bret;
        if(!bret)
        {
            QSqlDatabase::database().rollback();
            query.clear();
            return;
        }

        query.prepare("insert into report values(:name,:bianhao,:userinfo,:startdelaytime,:interval,:recordtimes,:starttime,"
                   ":endtime,:tempup,:tempdown,:humidup,:humiddown,:maxtemp,:mintemp,:avgtemp,"
                   ":maxhumid,:minhumid,:avghumid,:model,:clock,:frontTime,:estimateTime,:status)");
        query.bindValue(":name",table);
        query.bindValue(":bianhao",_recoder.bianHao);
        query.bindValue(":userinfo",_recoder.userInfo);
        query.bindValue(":startdelaytime",_recoder.startDelayTime);
        query.bindValue(":interval",_recoder.recordInterval);
        query.bindValue(":recordtimes",_recoder.recordTimes);
        query.bindValue(":starttime",_recoder.startTime);
        query.bindValue(":endtime",_recoder.estimateStopTime);
        query.bindValue(":tempup",_recoder.tempUp);
        query.bindValue(":tempdown",_recoder.tempDown);
        query.bindValue(":humidup",_recoder.humidityUp);
        query.bindValue(":humiddown",_recoder.humidityDown);
        query.bindValue(":maxtemp",_tempHumidity.maxTemp);
        query.bindValue(":mintemp",_tempHumidity.minTemp);
        query.bindValue(":avgtemp",_tempHumidity.avgTemp);
        query.bindValue(":maxhumid",_tempHumidity.maxHumidity);
        query.bindValue(":minhumid",_tempHumidity.minHumidity);
        query.bindValue(":avghumid",_tempHumidity.avgHumidity);
        query.bindValue(":model",_recoder.model);
        query.bindValue(":clock",_recoder.clock);
        query.bindValue(":frontTime",_recoder.frontTestTime);
        query.bindValue(":estimateTime",_recoder.estimateStopTime);
        query.bindValue(":status",_recoder.workStatus);
        bret=query.exec();
        qDebug()<<"insert into report"<<bret;
        if(!bret)
        {
            QSqlDatabase::database().rollback();
            query.clear();
            return;
        }

        int interval=_threadPM.interval;
        QDateTime dateTime=_threadPM.date;
        for(int i=0;i<_threadPM.size;i++)
        {
            sql=QString("insert into '%1'(no,time,temp,humidity) values('%2','%3','%4','%5')")
                    .arg(table)
                    .arg(i+1)
                    .arg(dateTime.toString("yyyy-MM-dd hh:mm:ss"))
                    .arg(_threadPM.pTempBuf[i])
                    .arg(_threadPM.pHumidBuf[i]);
            dateTime=dateTime.addSecs(interval);
            /*qDebug()<<*/query.exec(sql);
        }
        QSqlDatabase::database().commit();
    }
    else
    {
        query.clear();
        QMessageBox::information(this,tr("tips"),tr("same name has existed"));
        return;
    }
    query.clear();
    QMessageBox::information(this,tr("tips"),tr("Finish Data Saving"));
    accept();
}
