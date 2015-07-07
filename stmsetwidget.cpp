#include "stmsetwidget.h"
#include "mysql.h"
#include "check.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>

StmSetWidget::StmSetWidget(QWidget *parent) :
    QWidget(parent)
{
    topGB=new QGroupBox();
    centerGB=new QGroupBox(tr("DataFormat"));
    bottomGB=new QGroupBox(tr("TimeFormat"));
    saveParamPB=new QPushButton(tr("Save Parameter"));

    //top
    autoUploadCB=new QCheckBox(tr("Auto Upload"));
//    autoUploadCB->setChecked(true);
    autoSynchronousClockCB=new QCheckBox(tr("Self-synchronizing Data"));
    QVBoxLayout *layout1=new QVBoxLayout(topGB);
    layout1->addWidget(autoUploadCB);
    layout1->addWidget(autoSynchronousClockCB);

    //center
    dateDefaultRB=new QRadioButton(tr("Default"));
    dateDefaultRB->setChecked(true);
    dateCustomRB=new QRadioButton(tr("Define"));
//    dateCustomRB->setEnabled(false);
    dateLB=new QLabel("yyyy-MM-dd");
    dateLE=new QLineEdit();

    QGridLayout *grid1=new QGridLayout(centerGB);
    grid1->addWidget(dateDefaultRB,0,0);
    grid1->addWidget(dateLB,0,1);
    grid1->addWidget(dateCustomRB,1,0);
    grid1->addWidget(dateLE,1,1);

    //bottom
    timeDefaultRB=new QRadioButton(tr("Default"));
    timeCustomRB=new QRadioButton(tr("Define"));
//    timeCustomRB->setEnabled(false);
    timeLB=new QLabel("hh:mm:ss");
    timeLE=new QLineEdit();

    QGridLayout *grid2=new QGridLayout(bottomGB);
    grid2->addWidget(timeDefaultRB,0,0);
    grid2->addWidget(timeLB,0,1);
    grid2->addWidget(timeCustomRB,1,0);
    grid2->addWidget(timeLE,1,1);

    QVBoxLayout *layout=new QVBoxLayout();
    layout->addWidget(topGB);
    layout->addWidget(centerGB);
    layout->addWidget(bottomGB);
    QHBoxLayout *hLayout=new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(saveParamPB);
    hLayout->addStretch();
    layout->addLayout(hLayout);
    setLayout(layout);

    connect(dateDefaultRB,SIGNAL(clicked()),dateLE,SLOT(clear()));
    connect(timeDefaultRB,SIGNAL(clicked()),timeLE,SLOT(clear()));
    connect(dateCustomRB,SIGNAL(toggled(bool)),dateLE,SLOT(setEnabled(bool)));
    connect(timeCustomRB,SIGNAL(toggled(bool)),timeLE,SLOT(setEnabled(bool)));
    connect(saveParamPB,SIGNAL(clicked()),this,SLOT(executeSql()));

    loadSqlite();
}

void StmSetWidget::loadSqlite()
{
    bool autoUpload=false;
    bool autoSynchronous=false;
    bool isDate=false;
    bool isTime=false;
    QString dateCustom;
    QString timeCustom;

    QSqlQuery query(sqldb);
    query.exec("select * from system");
    while(query.next())
    {
        autoUpload=query.value(0).toBool();
        autoSynchronous=query.value(1).toBool();
        isDate=query.value(2).toBool();
        isTime=query.value(3).toBool();
        dateCustom=query.value(4).toString();
        timeCustom=query.value(5).toString();
    }
    query.clear();

    autoUploadCB->setChecked(autoUpload);
    autoSynchronousClockCB->setChecked(autoSynchronous);

    qDebug()<<"isdate"<<isDate;
    dateDefaultRB->setChecked(isDate);
    dateCustomRB->setChecked(!isDate);
    dateLE->setEnabled(!isDate);
    timeDefaultRB->setChecked(isTime);
    timeCustomRB->setChecked(!isTime);
    timeLE->setEnabled(!isTime);
    dateLE->setText(dateCustom);
    timeLE->setText(timeCustom);
}


void StmSetWidget::executeSql()
{
    bool autoUpload=false;
    bool autoSynchronous=false;
    QString dateCustom=dateLE->text();
    QString timeCustom=timeLE->text();

    bool bret=false;                                //日期正则表达式判断

    if(!dateCustom.isEmpty())
    {
        QRegExp dateExp;
        dateExp.setPattern("[y]{1,4}[-/.][M]{2}[-/.][d]{2}|[y]{1,4}[-/.][d]{2}[-/.][M]{2}"
                           "|[M]{2}[-/.][d]{2}[-/.][y]{1,4}|[M]{2}[-/.][y]{1,4}[-/.][d]{2}"
                           "|[d]{2}[-/.][y]{1,4}[-/.][M]{2}|[d]{2}[-/.][M]{2}[-/.][y]{1,4}");
        bret=dateExp.exactMatch(dateCustom);
        if(!bret)
        {
            QMessageBox::information(this,tr("tips"),tr("date format error"));
            return;
        }
    }

    if(!timeCustom.isEmpty())
    {
        QRegExp regExp[12];
        regExp[0].setPattern("hh:mm:ss");
        regExp[1].setPattern("mm:hh:ss");
        regExp[2].setPattern("mm:ss:hh");
        regExp[3].setPattern("ss:mm:hh");
        regExp[4].setPattern("hh:ss:mm");
        regExp[5].setPattern("ss:hh:mm");
        regExp[6].setPattern("hh:mm");
        regExp[7].setPattern("hh:ss");
        regExp[8].setPattern("mm:ss");
        regExp[9].setPattern("mm:hh");
        regExp[10].setPattern("ss:hh");
        regExp[11].setPattern("ss:mm");
        for(int i=0;i<12;i++)
        {
            bret=regExp[i].exactMatch(timeCustom);
            qDebug()<<bret;
            if(bret)
                break;
        }
        if(!bret)
        {
            QMessageBox::information(this,tr("tips"),tr("time format error"));
            return;
        }
    }

    if(autoUploadCB->checkState()==Qt::Unchecked)
    {
        autoUpload=false;
    }
    else
    {
        autoUpload=true;
    }

    if(autoSynchronousClockCB->checkState()==Qt::Unchecked)
    {
        autoSynchronous=false;
    }
    else
    {
        autoSynchronous=true;
    }

    QSqlQuery query(sqldb);
    query.prepare("update system set autoUpload=:autoUpload,autoSynchronous=:autoSynchronous,"
                  "dateDefault=:dateDefault,timeDefault=:timeDefault,"
                  "dateCustom=:dateCustom,timeCustom=:timeCustom");
    query.bindValue(":autoUpload",autoUpload);
    query.bindValue(":autoSynchronous",autoSynchronous);
    query.bindValue(":dateDefault",dateDefaultRB->isChecked());
    query.bindValue(":timeDefault",timeDefaultRB->isChecked());
    query.bindValue(":dateCustom",dateCustom);
    query.bindValue(":timeCustom",timeCustom);

    if(query.exec())
    {
        QMessageBox::information(this,tr("Tips"),tr("Save Success"));
        _dateFormat=dateCustom==""?"yyyy-MM-dd":dateCustom;
        _timeFormat=timeCustom==""?"hh:mm:ss":timeCustom;
        _displayFormat=QString("%1 %2").arg(_dateFormat).arg(_timeFormat);
//        emit dateFormatChanged();
    }
    query.clear();
}

//void StmSetWidget::showDefaultSetting(QStringList list)
//{
//    if(list.size()<6)
//    {
//        return;
//    }
//    autoUploadCB->setChecked(list.at(0).toInt());
//    autoSynchronousClockCB->setChecked(list.at(1).toInt());
//    if(0==list.at(2).toInt())
//    {
//        dateDefaultRB->setChecked(true);
//        dateLE->setEnabled(false);
//    }
//    else
//    {
//        dateCustomRB->setChecked(true);
//    }
//    qDebug()<<list.at(3).toInt();
//    if(1==list.at(3).toInt())
//    {
//        timeCustomRB->setChecked(true);
//    }
//    else
//    {
//        timeDefaultRB->setChecked(true);
//        timeLE->setEnabled(false);
//    }
//    dateLE->setText(list.at(4));
//    timeLE->setText(list.at(5));
//}
