#include "diagramwidget.h"
#include "mysql.h"

#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QToolButton>
#include <QDateTime>
#include <QToolBar>

double xMin=0.0;
double xMax=0.0;
const int smaller=5;                        //增大/缩小 因子
int step=6;                                 //横坐标分割

DiagramWidget::DiagramWidget(QWidget *parent) :
    QWidget(parent)
{
    tableWidget=new QTableWidget();
    QStringList list;
    list.append(tr("Type"));
    list.append(tr("Record Date"));
    list.append(tr("Time"));
    list.append(tr("Selected Value"));
    list.append(tr("Unit"));
    list.append(tr("Maximum Value"));
    list.append(tr("Minimum Value"));
    list.append(tr("Average Value"));
    list.append(tr("Upper Limit"));
    list.append(tr("Lower Limit"));
    list.append(tr("Number"));
    tableWidget->setColumnCount(11);
    tableWidget->setRowCount(2);
    tableWidget->setHorizontalHeaderLabels(list);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableWidget->setItem(0,0,new QTableWidgetItem(tr("Temperature")));
    tableWidget->setItem(1,0,new QTableWidgetItem(tr("Humidity")));
    tableWidget->setItem(0,4,new QTableWidgetItem("°C"));
    tableWidget->setItem(1,4,new QTableWidgetItem("%RH"));

    bigAct=new QAction(QIcon(":/png/png/big"),"",0);
    smallAct=new QAction(QIcon(":/png/png/small"),"",0);
    reductionAct=new QAction(QIcon(":/png/png/back"),"",0);
    radioRB1=new QRadioButton(tr("Temperature+Humidity"));
    radioRB2=new QRadioButton(tr("Temperature"));
    radioRB3=new QRadioButton(tr("Humidity"));
    radioRB1->setChecked(true);
    bigAct->setToolTip(tr("curves zoom out"));
    smallAct->setToolTip(tr("curves zoom in"));
    reductionAct->setText(tr("Rstore"));

    QToolBar *toolBar=new QToolBar();
    toolBar->addSeparator();
    toolBar->addAction(bigAct);
    toolBar->addAction(smallAct);
    toolBar->addAction(reductionAct);
    toolBar->addSeparator();
    toolBar->addWidget(radioRB1);
    toolBar->addWidget(radioRB2);
    toolBar->addWidget(radioRB3);

    customPlot=new QCustomPlot();
    customPlot->legend->setVisible(true);
    customPlot->legend->setIconSize(10,5);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);
    customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    customPlot->addGraph();
    customPlot->addGraph();
    customPlot->addGraph();
    customPlot->addGraph();
    customPlot->addGraph();
    customPlot->addGraph();
    customPlot->addGraph();
    customPlot->graph(0)->setName(tr("Temperature"));
    customPlot->graph(1)->setName(tr("Humidity"));
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(0,0,255));
    customPlot->graph(0)->setPen(pen);
    pen.setColor(QColor(0,128,0));
    customPlot->graph(1)->setPen(pen);
    pen.setColor(QColor(128,64,0));
    pen.setWidth(1);
    customPlot->graph(2)->setPen(pen);
    pen.setColor(QColor(255,0,0));
    pen.setStyle(Qt::DashLine);
    customPlot->graph(3)->setPen(pen);
    customPlot->graph(4)->setPen(pen);
    pen.setColor(QColor(255,0,255));
    customPlot->graph(5)->setPen(pen);
    customPlot->graph(6)->setPen(pen);
//    customPlot->graph(2)->setVisible(false);
    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    QString format=QString("%1\n%2").arg(_dateFormat).arg(_timeFormat);
    customPlot->xAxis->setDateTimeFormat(format);
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->legend->removeAt(2);
    customPlot->legend->removeAt(3);
    customPlot->legend->removeAt(4);
    customPlot->legend->removeAt(5);
    customPlot->legend->removeAt(6);

    QVBoxLayout *hLayout=new QVBoxLayout(this);
    hLayout->addWidget(toolBar);
    hLayout->addWidget(customPlot,5);
    hLayout->addWidget(tableWidget,1);


    //信号/槽
    connect(reductionAct,SIGNAL(triggered()),this,SLOT(reduction()));
    connect(customPlot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(mousemoved(QMouseEvent*)));
    connect(customPlot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(mousePress(QMouseEvent*)));
    connect(radioRB1,SIGNAL(clicked()),this,SLOT(showAll()));
    connect(radioRB2,SIGNAL(clicked()),this,SLOT(showTemperature()));
    connect(radioRB3,SIGNAL(clicked()),this,SLOT(showHumidity()));
    connect(bigAct,SIGNAL(triggered()),this,SLOT(bigAct_trogged()));
    connect(smallAct,SIGNAL(triggered()),this,SLOT(smallAct_trogged()));
}

void DiagramWidget::custom_plot(double *addr1, double *addr2, int size, QDateTime date, int interval)
{
    plotWithoutSave(addr1,addr2,size,date,interval);

    customPlot->savePng(".\\tmp.png",680,370,1.0);
    emit plot_over();
}

void DiagramWidget::plotWithoutSave(double *addr1, double *addr2, int size, QDateTime date, int interval)
{
    customPlot->graph(0)->clearData();
    customPlot->graph(1)->clearData();
    customPlot->graph(3)->clearData();
    customPlot->graph(4)->clearData();
    customPlot->graph(5)->clearData();
    customPlot->graph(6)->clearData();

    if(_deviceMode==TEMPERATUREONLY)
    {
        customPlot->graph(1)->setName("");
        QPen pen;
        pen.setColor(QColor(255,255,255));
        customPlot->graph(1)->setPen(pen);
        customPlot->graph(1)->setVisible(false);
        radioRB3->setEnabled(false);
        radioRB1->setEnabled(false);
        radioRB2->setChecked(true);        
    }
    else
    {
        QPen pen;
        pen.setColor(QColor(0,128,0));
        customPlot->graph(1)->setPen(pen);
        customPlot->graph(1)->setName(tr("Humidity"));
        customPlot->graph(1)->setVisible(true);
        radioRB3->setEnabled(true);
        radioRB1->setEnabled(true);
    }

    QString format=QString("%1\n%2").arg(_dateFormat).arg(_timeFormat);
    customPlot->xAxis->setDateTimeFormat(format);
    QDateTime dt=date;
    double now = dt.toTime_t();
    QVector<double> time(size), value(size);
    for (int i=0; i<size; ++i)
    {
        time[i] = now + interval*i;
        value[i] = addr1[i];
    }
    customPlot->graph(0)->setData(time, value);

    for (int i=0; i<size; ++i)
    {
        value[i] = _recoder.tempUp;
    }
    customPlot->graph(3)->setData(time, value);

    for (int i=0; i<size; ++i)
    {
        value[i] = _recoder.tempDown;
    }
    customPlot->graph(4)->setData(time, value);

    if(_deviceMode!=TEMPERATUREONLY)
    {
        for (int i=0; i<size; ++i)
        {
            value[i] = addr2[i];
        }
        customPlot->graph(1)->setData(time, value);

        for (int i=0; i<size; ++i)
        {
            value[i] = _recoder.humidityUp;
        }
        customPlot->graph(5)->setData(time, value);

        for (int i=0; i<size; ++i)
        {
            value[i] = _recoder.humidityDown;
        }
        customPlot->graph(6)->setData(time, value);
    }

//    yMax=_tempHumidity.maxHumidity>_tempHumidity.maxTemp?_tempHumidity.maxHumidity:_tempHumidity.maxTemp;
//    yMin=_tempHumidity.minHumidity<_tempHumidity.minTemp?_tempHumidity.minHumidity:_tempHumidity.minTemp;
    xMin=now;
    xMax=now+(size-1)*interval;

    customPlot->xAxis->setTickStep(interval*(size-1)/step);
    customPlot->xAxis->setRange(xMin,xMax);
    if(_deviceMode==TEMPERATUREONLY)
    {
        customPlot->yAxis->setRange(-30,60);
    }
    else
    {
        customPlot->yAxis->setRange(-30,90);
    }
    customPlot->replot();
}


void DiagramWidget::reduction()
{
    step=6;
    customPlot->xAxis->setTickStep(_threadPM.interval*(_threadPM.size-1)/step);
    customPlot->xAxis->setRange(xMin,xMax);
    if(customPlot->graph(0)->visible()&&customPlot->graph(1)->visible())
    {
        customPlot->yAxis->setRange(-30,90);
    }
    else if(customPlot->graph(0)->visible()&&!customPlot->graph(1)->visible())
    {
        customPlot->yAxis->setRange(-30,60);
    }
    else if(!customPlot->graph(0)->visible()&&customPlot->graph(1)->visible())
    {
        customPlot->yAxis->setRange(20,90);
    }
    customPlot->replot();
//    plotWithoutSave(_threadPM.pTempBuf,
//                    _threadPM.pHumidBuf,
//                    _threadPM.size,
//                    _threadPM.date,
//                    _threadPM.interval);
//    qDebug()<<"huanyuan";
}

void DiagramWidget::getMaxMinAvg()
{
    tableWidget->setItem(0,5,new QTableWidgetItem(QString::number(_tempHumidity.maxTemp,'f',1)));
    tableWidget->setItem(0,6,new QTableWidgetItem(QString::number(_tempHumidity.minTemp,'f',1)));
    tableWidget->setItem(0,7,new QTableWidgetItem(QString::number(_tempHumidity.avgTemp,'f',1)));
    if(_deviceMode==TEMPERATUREONLY)
    {
        tableWidget->setItem(1,5,new QTableWidgetItem(""));
        tableWidget->setItem(1,6,new QTableWidgetItem(""));
        tableWidget->setItem(1,7,new QTableWidgetItem(""));
    }
    else
    {
        tableWidget->setItem(1,5,new QTableWidgetItem(QString::number(_tempHumidity.maxHumidity,'f',1)));
        tableWidget->setItem(1,6,new QTableWidgetItem(QString::number(_tempHumidity.minHumidity,'f',1)));
        tableWidget->setItem(1,7,new QTableWidgetItem(QString::number(_tempHumidity.avgHumidity,'f',1)));
    }
}

void DiagramWidget::getParams()
{
    tableWidget->setItem(0,8,new QTableWidgetItem(QString::number(_recoder.tempUp,'f',1)));
    tableWidget->setItem(0,9,new QTableWidgetItem(QString::number(_recoder.tempDown,'f',1)));
    if(_deviceMode==TEMPERATUREONLY)
    {
        tableWidget->setItem(1,8,new QTableWidgetItem(""));
        tableWidget->setItem(1,9,new QTableWidgetItem(""));
    }
    else
    {
        tableWidget->setItem(1,8,new QTableWidgetItem(QString::number(_recoder.humidityUp,'f',1)));
        tableWidget->setItem(1,9,new QTableWidgetItem(QString::number(_recoder.humidityDown,'f',1)));
    }
}

void DiagramWidget::mousemoved(QMouseEvent *ev)
{
    if(ev->pos().x()<=customPlot->axisRect()->left()
        ||ev->pos().x()>=customPlot->axisRect()->right()
        ||ev->pos().y()<=customPlot->axisRect()->top()
        ||ev->pos().y()>=customPlot->axisRect()->bottom())
    {
        return;
    }

    double range=customPlot->xAxis->range().size();
    double horizontal1=customPlot->axisRect()->width()*1.0;
    double horizontal2=(ev->pos().x()-customPlot->axisRect()->left()-1)*range*1.0;
    double pos=horizontal2/horizontal1;
    customPlot->graph(2)->clearData();
    QVector<double> rx(2), ry(2);
    double temp=pos+customPlot->xAxis->range().lower;
    rx[0]=rx[1]=temp;
    ry[0]=-1000;
    ry[1]=1000;
    customPlot->graph(2)->setData(rx,ry);
    customPlot->replot();
    if(temp<xMin||temp>xMax)
    {
        tableWidget->setItem(0,10,new QTableWidgetItem(""));
        tableWidget->setItem(0,3,new QTableWidgetItem(""));
        tableWidget->setItem(1,3,new QTableWidgetItem(""));
    }
    else
    {
        QDateTime date=_threadPM.date;
        int index=(temp-xMin)/_threadPM.interval;
        date=date.addSecs(index*_threadPM.interval);
        tableWidget->setItem(0,1,new QTableWidgetItem(date.toString(_displayFormat).left(10)));
        tableWidget->setItem(0,2,new QTableWidgetItem(date.toString(_displayFormat).right(8)));
        tableWidget->setItem(0,10,new QTableWidgetItem(QString::number(index+1)));
        if(customPlot->graph(0)->visible())
            tableWidget->setItem(0,3,new QTableWidgetItem(QString::number(_threadPM.pTempBuf[index],'f',1)));
        else
            tableWidget->setItem(0,3,new QTableWidgetItem(""));

        if(customPlot->graph(1)->visible())
            tableWidget->setItem(1,3,new QTableWidgetItem(QString::number(_threadPM.pHumidBuf[index],'f',1)));
        else
            tableWidget->setItem(1,3,new QTableWidgetItem(""));
    }
}

void DiagramWidget::mousePress(QMouseEvent *ev)
{
//    static bool isHide=false;
    if(ev->button()==Qt::RightButton)
    {
        if(customPlot->graph(2)->visible())
        {
            customPlot->graph(2)->setVisible(false);
            customPlot->replot();
        }
        else
        {
            customPlot->graph(2)->setVisible(true);
            customPlot->replot();
        }
//        isHide=!isHide;
    }
}

void DiagramWidget::showAll()
{
    customPlot->yAxis->setRange(-30,90);
    customPlot->graph(0)->setVisible(true);
    customPlot->graph(1)->setVisible(true);
    customPlot->graph(3)->setVisible(true);
    customPlot->graph(4)->setVisible(true);
    customPlot->graph(5)->setVisible(true);
    customPlot->graph(6)->setVisible(true);
    customPlot->replot();
}

void DiagramWidget::showTemperature()
{
    customPlot->yAxis->setRange(-30,60);
    customPlot->graph(0)->setVisible(true);
    customPlot->graph(1)->setVisible(false);
    customPlot->graph(3)->setVisible(true);
    customPlot->graph(4)->setVisible(true);
    customPlot->graph(5)->setVisible(false);
    customPlot->graph(6)->setVisible(false);
    customPlot->replot();
}

void DiagramWidget::showHumidity()
{
    customPlot->yAxis->setRange(20,90);
    customPlot->graph(0)->setVisible(false);
    customPlot->graph(1)->setVisible(true);
    customPlot->graph(3)->setVisible(false);
    customPlot->graph(4)->setVisible(false);
    customPlot->graph(5)->setVisible(true);
    customPlot->graph(6)->setVisible(true);
    customPlot->replot();
}

void DiagramWidget::tensileAct_trogged()
{
}

void DiagramWidget::narrowAct_trogged()
{
}

void DiagramWidget::smallAct_trogged()
{
    if(step==6)
    {
        step=5;
    }
    else if(step==5)
    {
        step=3;
    }
    else
    {
        step=6;
        return;
    }

    double inter=_threadPM.interval*(_threadPM.size-1)/step;
    double low=customPlot->yAxis->range().lower;
    double high=customPlot->yAxis->range().upper;
    double low1=customPlot->xAxis->range().lower;
    double high2=customPlot->xAxis->range().upper;
    customPlot->xAxis->setTickStep(inter);
    customPlot->xAxis->setRange(low1-2*inter,high2+2*inter);
    customPlot->yAxis->setRange(low-smaller,high+smaller);
    customPlot->replot();
}

void DiagramWidget::bigAct_trogged()
{
    double low=customPlot->yAxis->range().lower;
    double high=customPlot->yAxis->range().upper;
    int yS=low+smaller;
    int yE=high-smaller;
    double low1=customPlot->xAxis->range().lower;
    double high2=customPlot->xAxis->range().upper;
    double inter=_threadPM.interval*(_threadPM.size-1)/step;

    if(yS>=yE)
    {
        return;
    }
    if(low1+2*inter>=high2-2*inter)
    {
        return;
    }
    customPlot->xAxis->setRange(low1+2*inter,high2-2*inter);
    customPlot->yAxis->setRange(yS,yE);
    customPlot->replot();
}

void DiagramWidget::sqldbPlot(QString name)
{
    qDebug()<<"start load data";

    int index=0;
    QSqlQuery query(sqldb);
    query.exec(QString("select temp,humidity from '%1'").arg(name));
    QDateTime date=_recoder.startTime;
    while(query.next())
    {
        _threadPM.pTempBuf[index]=query.value(0).toDouble();
        _threadPM.pHumidBuf[index]=query.value(1).toDouble();
        index++;
    }
    query.clear();

    emit load_over();

    _threadPM.date=date;
    if(_recoder.recordInterval.length()==8)
    {
        _threadPM.interval=_recoder.recordInterval.left(2).toInt()*3600
                +_recoder.recordInterval.mid(3,2).toInt()*60
                +_recoder.recordInterval.right(2).toInt();
    }
    else
    {
        _threadPM.interval=0;
    }
    _threadPM.size=index>16000?16000:index;
    qDebug()<<"DiagramWidget"<<_threadPM.size;

    custom_plot(_threadPM.pTempBuf,_threadPM.pHumidBuf,index,date,_threadPM.interval);

    getParams();
    getMaxMinAvg();
}

void DiagramWidget::updateLabelWithDate()
{
    QString format=QString("%1\n%2").arg(_dateFormat).arg(_timeFormat);
    customPlot->xAxis->setDateTimeFormat(format);
    customPlot->replot();
}
