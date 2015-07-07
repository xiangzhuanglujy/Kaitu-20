#include "datatablewidget.h"

#include <QTableWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QDebug>
#include <QHeaderView>

#include "generatewidget.h"
#include "managewidget.h"
#include "check.h"
#include "mysql.h"

DataTableWidget::DataTableWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle(tr("The Record of Temperature Data"));

    tableWidget=new QTableWidget();
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //使其不可编辑
    tableWidget->setColumnCount(4);
    QStringList list;
    tableWidget->verticalHeader()->setVisible(false);
    list<<tr("Number")<<tr("Time")<<tr("Temperature")+QString("°C")<<tr("Humidity%RH");
    tableWidget->setHorizontalHeaderLabels(list);
    tableWidget->setColumnWidth(0,60);
    tableWidget->setColumnWidth(1,150);
    tableWidget->setColumnWidth(2,100);
    tableWidget->setColumnWidth(3,100);

    tabWidget=new QTabWidget();
    generateWidget=new GenerateWidget(this);
    managerWidget=new ManageWidget(this);
    tabWidget->addTab(generateWidget,tr("Ordinary Item"));
    tabWidget->addTab(managerWidget,tr("Data Management"));

    QHBoxLayout *hLayout=new QHBoxLayout(this);
    hLayout->addWidget(tableWidget,1);
    hLayout->addWidget(tabWidget,2);


    //上传数据筛选
    connect(managerWidget,SIGNAL(queryTable(QDateTime,QDateTime,QString,QString,QString,QString)),
            this,SLOT(sortTable(QDateTime,QDateTime,QString,QString,QString,QString)));
    //数据库数据筛选
    //切换存储名称
    connect(managerWidget,SIGNAL(changeDataName(QString)),this,SIGNAL(plotFromSql(QString)));
    connect(managerWidget,SIGNAL(changeDataName(QString)),this,SLOT(getParams()));
    connect(managerWidget,SIGNAL(changeDataName(QString)),this,SLOT(getTempHumidity()));
    connect(parent,SIGNAL(findDevice()),managerWidget,SLOT(changeFlag()));
    connect(managerWidget,SIGNAL(enableWindow(bool)),this,SIGNAL(guiEnabled(bool)));
    connect(this,SIGNAL(deleteFromDB(QString)),managerWidget,SLOT(removeFromQCombo(QString)));
}

void DataTableWidget::changeDateFormat()
{
    managerWidget->updateLabelWithDate();
    generateWidget->updateLabelWithDate();
    showTable(_threadPM.pTempBuf,
              _threadPM.pHumidBuf,
              _threadPM.size,
              _threadPM.date,
              _threadPM.interval);
}

bool DataTableWidget::isTemperatureInRange(double temperature)
{
    if(temperature>_recoder.tempUp||temperature<_recoder.tempDown)
    {
        return false;
    }
    return true;
}

bool DataTableWidget::isHumidityInRange(double humidity)
{
    if(humidity>_recoder.humidityUp||humidity<_recoder.humidityDown)
    {
        return false;
    }
    return true;
}

void DataTableWidget::getParams()
{
    generateWidget->getParams();
}

void DataTableWidget::getTempHumidity()
{
    generateWidget->getMaxMinAvg();
}

void DataTableWidget::resetTime()
{
    managerWidget->clearSelect();
}

void DataTableWidget::showTable(double *addr1, double *addr2, int size, QDateTime date, int interval)
{
    qDebug()<<"nimazenmehui zheyang a 111111111111111111111111 showTable";

    tableWidget->setRowCount(0);

    if(_deviceMode==TEMPERATUREONLY)
    {
        tableWidget->setColumnWidth(3,0);
    }
    else
    {
        tableWidget->setColumnWidth(3,100);
    }

    QDateTime dateTime=date;
    double value1,value2;
    for(int i=0;i<size;i++)
    {
        tableWidget->insertRow(i);
        QTableWidgetItem *item1=new QTableWidgetItem();
        item1->setText(QString::number(i+1));
        QTableWidgetItem *item2=new QTableWidgetItem();
        item2->setText(dateTime.toString(_displayFormat));
        QTableWidgetItem *item3=new QTableWidgetItem();
        value1=addr1[i];
        if(isTemperatureInRange(value1))
            item3->setTextColor(QColor(0,0,0));
        else
            item3->setTextColor(QColor(255,0,0));
        item3->setText(QString::number(value1,'f',1));
        QTableWidgetItem *item4=new QTableWidgetItem();
        value2=addr2[i];
        item4->setText(QString::number(value2,'f',1));
        if(isHumidityInRange(value2))
            item4->setTextColor(QColor(0,0,0));
        else
            item4->setTextColor(QColor(255,0,0));
        tableWidget->setItem(i,0,item1);
        tableWidget->setItem(i,1,item2);
        tableWidget->setItem(i,2,item3);
        tableWidget->setItem(i,3,item4);
        dateTime=dateTime.addSecs(interval);
    }
}

void DataTableWidget::sortTable(QDateTime startDt, QDateTime endDt, QString upTemp, QString downTemp, QString upHumid, QString downHumid)
{
    qDebug()<<"nimazenmehui zheyang a 111111111111111111111111sortTable";

    tableWidget->setRowCount(0);
    uint startFrom= startDt.toTime_t();
    uint endTo=endDt.toTime_t();
    qDebug()<<"DataTableWidget"<<startFrom<<endTo;
    int index=0;
    if(startFrom>endTo)
    {
        return;
    }

    int indexS=0;
    int indexE=0;

    QDateTime dateTime=_threadPM.date;
    uint begin=dateTime.toTime_t();
    int interval=_threadPM.interval;

    while(begin+indexS*interval<startFrom)
    {
        indexS++;
    }
    while(begin+indexE*interval<=endTo)
    {
        indexE++;
    }
    indexE-=1;

    qDebug()<<"end"<<indexS<<indexE;

    if(!upTemp.isEmpty()&&!downTemp.isEmpty())
    {
        double temp1=upTemp.toDouble();
        double temp2=downTemp.toDouble();
        qDebug()<<"temp"<<temp1<<temp2;

        if(temp1<temp2)
        {
            return;
        }
        index=0;
        for(int i=indexS;i<=indexE;i++)
        {
            if(_threadPM.pTempBuf[i]>=temp2&&_threadPM.pTempBuf[i]<=temp1)
            {
                tableWidget->insertRow(index);
                tableWidget->setItem(index,0,new QTableWidgetItem(QString::number(index+1)));
                tableWidget->setItem(index,1,new QTableWidgetItem(dateTime.addSecs(i*interval).toString(_displayFormat)));
                tableWidget->setItem(index,2,new QTableWidgetItem(QString::number(_threadPM.pTempBuf[i],'f',1)));
                tableWidget->setItem(index,3,new QTableWidgetItem(QString::number(_threadPM.pHumidBuf[i],'f',1)));
                index++;
            }
        }
        return;
    }

    if(!upTemp.isEmpty()&&downTemp.isEmpty())
    {
        double temp1=upTemp.toDouble();
        qDebug()<<"temp"<<temp1;
        index=0;
        for(int i=indexS;i<=indexE;i++)
        {
            if(_threadPM.pTempBuf[i]<=temp1)
            {
                tableWidget->insertRow(index);
                tableWidget->setItem(index,0,new QTableWidgetItem(QString::number(index+1)));
                tableWidget->setItem(index,1,new QTableWidgetItem(dateTime.addSecs(i*interval).toString(_displayFormat)));
                tableWidget->setItem(index,2,new QTableWidgetItem(QString::number(_threadPM.pTempBuf[i],'f',1)));
                tableWidget->setItem(index,3,new QTableWidgetItem(QString::number(_threadPM.pHumidBuf[i],'f',1)));
                index++;
            }
        }
        return;
    }

    if(upTemp.isEmpty()&&!downTemp.isEmpty())
    {
        double temp2=downTemp.toDouble();
        qDebug()<<"temp"<<temp2;

        index=0;
        for(int i=indexS;i<=indexE;i++)
        {
            if(_threadPM.pTempBuf[i]>=temp2)
            {
                tableWidget->insertRow(index);
                tableWidget->setItem(index,0,new QTableWidgetItem(QString::number(index+1)));
                tableWidget->setItem(index,1,new QTableWidgetItem(dateTime.addSecs(i*interval).toString(_displayFormat)));
                tableWidget->setItem(index,2,new QTableWidgetItem(QString::number(_threadPM.pTempBuf[i],'f',1)));
                tableWidget->setItem(index,3,new QTableWidgetItem(QString::number(_threadPM.pHumidBuf[i],'f',1)));
                index++;
            }
        }
        return;
    }


    if(!upHumid.isEmpty()&&!downHumid.isEmpty())
    {
        double humid1=upHumid.toDouble();
        double humid2=downHumid.toDouble();

        if(humid1<humid2)
        {
            return;
        }

        index=0;
        for(int i=indexS;i<=indexE;i++)
        {
            if(_threadPM.pHumidBuf[i]>humid1||_threadPM.pHumidBuf[i]<humid2)
            {
                continue;
            }
            tableWidget->insertRow(index);
            tableWidget->setItem(index,0,new QTableWidgetItem(QString::number(index+1)));
            tableWidget->setItem(index,1,new QTableWidgetItem(dateTime.addSecs(i*interval).toString(_displayFormat)));
            tableWidget->setItem(index,2,new QTableWidgetItem(QString::number(_threadPM.pTempBuf[i],'f',1)));
            tableWidget->setItem(index,3,new QTableWidgetItem(QString::number(_threadPM.pHumidBuf[i],'f',1)));
            index++;
        }
        return;
    }

    if(!upHumid.isEmpty()&&downHumid.isEmpty())
    {
        double humid1=upHumid.toDouble();
        index=0;
        for(int i=indexS;i<=indexE;i++)
        {
            if(_threadPM.pHumidBuf[i]>humid1)
            {
                continue;
            }
            tableWidget->insertRow(index);
            tableWidget->setItem(index,0,new QTableWidgetItem(QString::number(index+1)));
            tableWidget->setItem(index,1,new QTableWidgetItem(dateTime.addSecs(i*interval).toString(_displayFormat)));
            tableWidget->setItem(index,2,new QTableWidgetItem(QString::number(_threadPM.pTempBuf[i],'f',1)));
            tableWidget->setItem(index,3,new QTableWidgetItem(QString::number(_threadPM.pHumidBuf[i],'f',1)));
            index++;
        }
        return;
    }

    if(upHumid.isEmpty()&&!downHumid.isEmpty())
    {
        double humid2=downHumid.toDouble();
        index=0;
        for(int i=indexS;i<=indexE;i++)
        {
            if(_threadPM.pHumidBuf[i]<humid2)
            {
                continue;
            }
            tableWidget->insertRow(index);
            tableWidget->setItem(index,0,new QTableWidgetItem(QString::number(index+1)));
            tableWidget->setItem(index,1,new QTableWidgetItem(dateTime.addSecs(i*interval).toString(_displayFormat)));
            tableWidget->setItem(index,2,new QTableWidgetItem(QString::number(_threadPM.pTempBuf[i],'f',1)));
            tableWidget->setItem(index,3,new QTableWidgetItem(QString::number(_threadPM.pHumidBuf[i],'f',1)));
            index++;
        }
        return;
    }


    if(upTemp.isEmpty()&&upHumid.isEmpty())
    {
        qDebug()<<"DataTableWidget all catch";
        index=0;
        for(int i=indexS;i<=indexE;i++)
        {
            tableWidget->insertRow(index);
            tableWidget->setItem(index,0,new QTableWidgetItem(QString::number(index+1)));
            tableWidget->setItem(index,1,new QTableWidgetItem(dateTime.addSecs(i*interval).toString(_displayFormat)));
            tableWidget->setItem(index,2,new QTableWidgetItem(QString::number(_threadPM.pTempBuf[i],'f',1)));
            tableWidget->setItem(index,3,new QTableWidgetItem(QString::number(_threadPM.pHumidBuf[i],'f',1)));
            index++;
        }
        return;
    }
}

void DataTableWidget::loadData()
{
    qDebug()<<"DataTableWidget load data";

    showTable(_threadPM.pTempBuf,
              _threadPM.pHumidBuf,
              _threadPM.size,
              _threadPM.date,
              _threadPM.interval);

    emit guiEnabled(true);
}
