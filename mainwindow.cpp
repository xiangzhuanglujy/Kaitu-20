#include "mainwindow.h"
#include "settingwidget.h"
#include "uploadwidget.h"
#include "stmmailwidget.h"
#include "about.h"
#include "serial/qextserialport.h"
#include "msgwidget.h"
#include "mythread.h"
#include "mail/SmtpMime"
#include "mysql.h"
#include "comthread.h"
#include "wdatastore.h"
#include "check.h"
#include "finddevice.h"
#include "office/qexcel.h"

#include <QProcess>
#include <QAxWidget>
#include <QAxObject>
#include <QFile>
#include <QDebug>
#include <QPrinter>
#include <QTextBlock>
#include <QTextDocument>
#include <QDateTime>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <QTranslator>
#include <QStackedWidget>
#include <qt_windows.h>
#include <dbt.h>
#include <QSettings>
#include <QFileDialog>
#include <QProgressDialog>
#include <QLabel>
#include <QMessageBox>
#include <QErrorMessage>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QPainter>
#include <QProgressDialog>
#include <QBuffer>
#include <QProgressBar>
#include <QToolBar>
#include <QApplication>

uchar tempBuf[193000];                                      //缓冲区
int tempLen=0;                                              //实时接收长度
int data_end_length=192000;                                 //数据终止长度(可变)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    MySQL::addSqlite();                             //创建数据库驱动
    MySQL::connect();                               //连接数据库


    /**************设置窗口属性****************/
    setWindowTitle(tr("The Record of Temperature Data"));
    showMaximized();                                //最大化

    initDateFormat();                               //初始化日期格式
    init();                                         //初始化全局变量
    initControl();                                  //初始化控件
    initThread();                                   //初始化线程
    initToolBar();                                  //初始化工具栏
    initMenuBar();                                  //初始化菜单栏
    initSerialPort();                               //初始化串口

    /*******************初始化界面状态************************/
    startTickTime=0;
    setGuiEnabled(false);
    pDevice=new FindDevice();
    connect(this,SIGNAL(valueToProgressBar(int)),pDevice,SLOT(setValue(int)));
    connect(this,SIGNAL(setProgressBarMaxunum(int)),pDevice,SLOT(setMax(int)));
    connect(this,SIGNAL(setQStringVisible(bool)),pDevice,SLOT(setQString(bool)));
    pDevice->hide();
    stack->hide();
}

MainWindow::~MainWindow()
{
    if(NULL!=myCom)
    {
        if(myCom->isOpen())
        {
            myCom->close();
        }
        delete myCom;
        myCom=NULL;
    }

    pDevice->close();
    thread->wait();
    comThread->wait();

    MySQL::disconnect();
}

void MainWindow::initDateFormat()
{
    /***********初始化日期格式*******************************/
    QString format1,format2;
    QSqlQuery query(sqldb);
    query.exec("select dateCustom,timeCustom from system");
    while(query.next())
    {
        format1=query.value(0).toString();
        format2=query.value(1).toString();
    }
    query.clear();
    _dateFormat=format1==""?"yyyy-MM-dd":format1;
    _timeFormat=format2==""?"hh:mm:ss":format2;
    _displayFormat=QString("%1 %2").arg(_dateFormat).arg(_timeFormat);
    qDebug()<<"format"<<_dateFormat<<_timeFormat;
}

void MainWindow::initControl()
{
    stack=new QStackedWidget(this);
    setCentralWidget(stack);
    uploadWidget=new UploadWidget(this);
    settingWidget=new SettingWidget(this);
    stack->addWidget(uploadWidget);
    stack->addWidget(settingWidget);
    stack->setCurrentWidget(uploadWidget);

    connect(settingWidget,SIGNAL(numberSG(QByteArray)),this,SLOT(sendInfo(QByteArray)));
    connect(settingWidget,SIGNAL(clkSG(QByteArray)),this,SLOT(sendInfo(QByteArray)));
    connect(settingWidget,SIGNAL(userInfoSG(QByteArray)),this,SLOT(sendInfo(QByteArray)));
    connect(settingWidget,SIGNAL(dataStoreSG(QByteArray)),this,SLOT(sendInfo(QByteArray)));
    connect(this,SIGNAL(dataToSettingWidget()),settingWidget,SLOT(showParamFromDown()));
    connect(this,SIGNAL(findDevice()),uploadWidget,SIGNAL(findDevice()));
    connect(this,SIGNAL(setCurrentDateTime(QDateTime)),settingWidget,SLOT(updateClock(QDateTime)));
}

void MainWindow::initThread()
{
    thread=new MyThread(this);
    connect(this,SIGNAL(dataToThread(uchar*,int,QDateTime,QString)),
            thread,SLOT(getdata(uchar*,int,QDateTime,QString)));
    connect(thread,SIGNAL(saveCompletely()),uploadWidget,SIGNAL(maxMinAvg()));
    connect(thread,SIGNAL(saveCompletely()),this,SLOT(enableGuiWindow()));
    connect(thread,SIGNAL(plot(double*,double*,int,QDateTime,int)),
            uploadWidget,SLOT(readyPlot(double*,double*,int,QDateTime,int)));
}

void MainWindow::initToolBar()
{
    toolBar=new QToolBar(this);
    QFont font;
    font.setPointSize(12);
    toolBar->setFont(font);
    statusBar()->setFont(font);
    addToolBar(toolBar);
    toolBar->setMovable(false);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connectAct=new QAction(QIcon(":/png/png/connect"),"",this);
    uploadDataAct=new QAction(QIcon(":/png/png/upload"),"",this);
    paramSetAct=new QAction(QIcon(":/png/png/setting"),"",this);
    mailsetAct=new QAction(QIcon(":/png/png/mail"),"",this);
    queryDataAct=new QAction(QIcon(":/png/png/search"),"",this);
    storeDataAct=new QAction(QIcon(":/png/png/store"),"",this);
    exportExcelAct=new QAction(QIcon(":/png/png/excel"),"",this);
    exportPdfAct=new QAction(QIcon(":/png/png/pdf"),"",this);
    exportWordAct=new QAction(QIcon(":/png/png/word"),"",this);
    exportTxtAct=new QAction(QIcon(":/png/png/txt"),"",this);
    printAct=new QAction(QIcon(":/png/png/print"),"",this);
    deleteDataAct=new QAction(QIcon(":/png/png/del"),"",this);
    sendMailAct=new QAction(QIcon(":/png/png/send"),"",this);
    stopRecordAct=new QAction(QIcon(":/png/png/stop"),"",this);
    helpAct=new QAction(tr("Help"),this);
    aboutAct=new QAction(tr("About"),this);
    backUpAct=new QAction(tr("Data Backup"),this);
    reductionAct=new QAction(tr("Restore Data"),this);

    connectAct->setText(tr("Connection"));
    uploadDataAct->setText(tr("Upload Data"));
    paramSetAct->setText(tr("Parameter Set"));
    mailsetAct->setText(tr("System/Mail Set"));
    queryDataAct->setText(tr("Inquiry Data"));
    storeDataAct->setText(tr("Save Data"));
    exportExcelAct->setText(tr("Export to EXCEL"));
    exportPdfAct->setText(tr("Export to PDF"));
    exportWordAct->setText(tr("Export to Word"));
    exportTxtAct->setText(tr("Export to Txt"));
    printAct->setText(tr("Print"));
    deleteDataAct->setText(tr("Delete Data"));
    sendMailAct->setText(tr("Sendmail"));
    stopRecordAct->setText(tr("Stop Recoder"));

    toolBar->addAction(connectAct);
    toolBar->addAction(uploadDataAct);
    toolBar->addAction(paramSetAct);
    toolBar->addAction(mailsetAct);
    toolBar->addAction(queryDataAct);
    toolBar->addAction(storeDataAct);
    toolBar->addAction(exportExcelAct);
    toolBar->addAction(exportPdfAct);
    toolBar->addAction(exportWordAct);
    toolBar->addAction(exportTxtAct);
    toolBar->addAction(printAct);
    toolBar->addAction(deleteDataAct);
    toolBar->addAction(sendMailAct);
    toolBar->addAction(stopRecordAct);

    connect(helpAct,SIGNAL(triggered()),this,SLOT(showHelp()));
    connect(aboutAct,SIGNAL(triggered()),this,SLOT(aboutSlot()));
    connect(connectAct,SIGNAL(triggered()),this,SLOT(connectSlot()));
    connect(uploadDataAct,SIGNAL(triggered()),this,SLOT(uploadSlot()));
    connect(paramSetAct,SIGNAL(triggered()),this,SLOT(paramSetSlot()));
    connect(mailsetAct,SIGNAL(triggered()),this,SLOT(setSystemMail()));
    connect(queryDataAct,SIGNAL(triggered()),this,SLOT(queryDataSlot()));
    connect(storeDataAct,SIGNAL(triggered()),this,SLOT(storeDataSlot()));
    connect(exportExcelAct,SIGNAL(triggered()),this,SLOT(exportExcelSlot()));
    connect(exportPdfAct,SIGNAL(triggered()),this,SLOT(exportPdfSlot()));
    connect(exportWordAct,SIGNAL(triggered()),this,SLOT(exportWordSlot()));
    connect(exportTxtAct,SIGNAL(triggered()),this,SLOT(exportTxtSlot()));
    connect(printAct,SIGNAL(triggered()),this,SLOT(printSlot()));
    connect(deleteDataAct,SIGNAL(triggered()),this,SLOT(deleteDataSlot()));
    connect(sendMailAct,SIGNAL(triggered()),this,SLOT(sendMailSlot()));
    connect(stopRecordAct,SIGNAL(triggered()),this,SLOT(stopRecordSlot()));
    connect(backUpAct,SIGNAL(triggered()),this,SLOT(backUpDB()));
    connect(reductionAct,SIGNAL(triggered()),this,SLOT(reductionDB()));
}

void MainWindow::initMenuBar()
{
    //菜单栏
    fileMenu=new QMenu(this);
    fileMenu=menuBar()->addMenu(tr("File(&F)"));
    fileMenu->addAction(queryDataAct);
    fileMenu->addAction(storeDataAct);
    exportMenu=new QMenu(this);
    exportMenu=fileMenu->addMenu(tr("Export Data"));
    exportMenu->addAction(exportPdfAct);
    exportMenu->addAction(exportExcelAct);
    exportMenu->addAction(exportWordAct);
    exportMenu->addAction(exportTxtAct);
    fileMenu->addAction(deleteDataAct);
    fileMenu->addAction(printAct);
    fileMenu->addAction(sendMailAct);
    fileMenu->addAction(backUpAct);
    fileMenu->addAction(reductionAct);
    toolMenu=new QMenu(this);
    toolMenu=menuBar()->addMenu(tr("Tool(&T)"));
    toolMenu->addAction(connectAct);
    toolMenu->addAction(uploadDataAct);
    toolMenu->addAction(stopRecordAct);
    settingMenu=new QMenu(this);
    settingMenu=menuBar()->addMenu(tr("Settings(&S)"));
    settingMenu->addAction(paramSetAct);
    settingMenu->addAction(mailsetAct);
    helpMenu=new QMenu(this);
    helpMenu=menuBar()->addMenu(tr("Help(&H)"));
    helpMenu->addAction(helpAct);
    helpMenu->addAction(aboutAct);
    quitAct=new QAction(tr("Exit"),this);
    menuBar()->addAction(quitAct);

    connect(quitAct,SIGNAL(triggered()),this,SLOT(quitAct_clicked()));
}

void MainWindow::initSerialPort()
{
    comThread=new ComThread();
    connect(this,SIGNAL(comInfoToThread(QStringList)),comThread,SLOT(getComInfo(QStringList)));
    connect(comThread,SIGNAL(findCom(QString,QString)),this,SLOT(openCom(QString,QString)));

//    myCom=NULL;
//    myCom=new QextSerialPort();
//    connect(myCom,SIGNAL(readyRead()),this,SLOT(readCom()));
//    myCom->setBaudRate((BaudRateType)_serialPort.baudRate);
//    myCom->setDataBits((DataBitsType)_serialPort.dataBits);
//    myCom->setParity((ParityType)_serialPort.parity);
//    myCom->setStopBits((StopBitsType)_serialPort.stopBits);
//    myCom->setFlowControl(FLOW_OFF); //设置数据流控制，我们使用无数据流控制的默认设置
//    myCom->setTimeout(10); //设置延时

    queryCom();
    emit comInfoToThread(comVector);
    comThread->start();
}


QString MainWindow::GeneratePicPdf()
{
    QString html="";
    QString imagepath=QString(".\\tmp.png");
    QString userInfo=tr("User Information");
    QString no=tr("Number");
    QString recordInterval=tr("Interval");
    QString startDelayTime=tr("Delay Time");
    QString dataCount=tr("Data Sum");
    QString maxTemperature=tr("TemperatureMaximum");
    QString minTemperature=tr("TemperatureMinimum");
    QString avgTemperature=tr("TemperatureAverage");
    QString tempAlarmUp=tr("TemperatureAlarm Upper Limit");
    QString tempAlarmDown=tr("TemperatureAlarm Lower Limit");
    QString maxHumidity=tr("HumidityMaximum");
    QString minHumidity=tr("HumidityMinimum");
    QString avgHumidity=tr("HumidityAverage");
    QString humidityAlarmUp=tr("HumidityAlarm Upper Limit");
    QString humidityAlarmDn=tr("HumidityAlarm Lower Limit");
    QString startTime=tr("Start Time");
    QString endedTime=tr("End Time");
    QString timeFormat=tr("Date Format");
    QString tempUint=tr("Temperature Unit");

    if(CHINESE==lngType)
    {
        html+=QString("<html><head><style type=text/css>");
        html+=QString(".STYLE1{font-size: 18px;color: #000000;background-color:gray;}");
        html+=QString("</style></head><body><div align=center>");
        html+=QString("<p class=STYLE1>%1</p>").arg(_recoder.model);
        html+=QString("<div align=left>");
        html+=QString("<pre align=left>%1\t\t%2</pre>").arg("型号").arg(_recoder.model);
        html+=QString("<pre align=left>%1\t\t%2</pre>").arg(userInfo).arg(_recoder.userInfo);
        html+=QString("<pre align=left>%1\t\t%2</pre>").arg(no).arg(_recoder.bianHao);
        html+=QString("<pre align=left>%1\t\t%2 H:M:S </pre>").arg(recordInterval).arg(_recoder.recordInterval);
        html+=QString("<pre align=left>%1\t\t%2 H </pre>").arg(startDelayTime).arg(_recoder.startDelayTime);
        html+=QString("<pre align=left>%1\t\t%2 </pre>").arg(dataCount).arg(_recoder.recordTimes);
        html+=QString("<pre align=left>%1\t\t%2 °C</pre>").arg(maxTemperature).arg(_tempHumidity.maxTemp);
        html+=QString("<pre align=left>%1\t\t%2 °C</pre>").arg(minTemperature).arg(_tempHumidity.minTemp);
        html+=QString("<pre align=left>%1\t\t%2 °C</pre>").arg(avgTemperature).arg(_tempHumidity.avgTemp);
        html+=QString("<pre align=left>%1\t\t%2 °C</pre>").arg(tempAlarmUp).arg(_recoder.tempUp);
        html+=QString("<pre align=left>%1\t\t%2 °C</pre>").arg(tempAlarmDown).arg(_recoder.tempDown);
        if(_deviceMode!=TEMPERATUREONLY)
        {
            html+=QString("<pre align=left>%1\t\t%2 %RH</pre>").arg(maxHumidity).arg(_tempHumidity.maxHumidity);
            html+=QString("<pre align=left>%1\t\t%2 %RH</pre>").arg(minHumidity).arg(_tempHumidity.minHumidity);
            html+=QString("<pre align=left>%1\t\t%2 %RH</pre>").arg(avgHumidity).arg(_tempHumidity.avgHumidity);
            html+=QString("<pre align=left>%1\t\t%2 %RH</pre>").arg(humidityAlarmUp).arg(_recoder.humidityUp);
            html+=QString("<pre align=left>%1\t\t%2 %RH</pre>").arg(humidityAlarmDn).arg(_recoder.humidityDown);
        }
        html+=QString("<pre align=left>%1\t\t%2 </pre>").arg(startTime).arg(_recoder.startTime.toString(_displayFormat));
        html+=QString("<pre align=left>%1\t\t%2 </pre>").arg(endedTime).arg(_recoder.estimateStopTime.toString(_displayFormat));
        html+=QString("<pre align=left>%1\t\t%2 </pre>").arg(timeFormat).arg(_displayFormat);
        html+=QString("<pre align=left>%1\t\t°C </pre>").arg(tempUint);
        html+=QString("<pre align=left><img src= %1 width=570 height=350/></pre>").arg(imagepath);
        html+=QString("<p>&nbsp;</p>");
        html+=QString("<table width=610 border=1 bordercolor=#000000 cellspacing=0 style=border-collapse:collapse>");

        if(_deviceMode==TEMPERATUREONLY)
        {
            html+=QString("<tr><th scope=col>编号</th><th scope=col>时间</th><th scope=col>温度°C</th><th scope=col>编号</th><th scope=col>时间</th><th scope=col>温度°C</th></tr>");
        }
        else
        {
            html+=QString("<tr><th scope=col>编号</th><th scope=col>时间</th><th scope=col>温度°C</th><th scope=col>湿度%RH</th><th scope=col>编号</th><th scope=col>时间</th><th scope=col>温度°C</th><th scope=col>湿度%RH</th></tr>");
        }
        QDateTime datetime=_threadPM.date;
        int dataSize=_threadPM.size/2;
        int dataInterval=_threadPM.interval;
        double *pBuf1=_threadPM.pTempBuf;
        double *pBuf2=_threadPM.pHumidBuf;
        if(_deviceMode==TEMPERATUREONLY)
        {
            for(int i=0;i<dataSize;i++)
            {
                html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                .arg(2*i+1)
                .arg(datetime.toString(_displayFormat))
                .arg(pBuf1[2*i])
                .arg(2*i+2)
                .arg(datetime.addSecs(dataInterval).toString(_displayFormat))
                .arg(pBuf1[2*i+1]);
                datetime=datetime.addSecs(2*dataInterval);
            }
            if(_threadPM.size%2!=0)
            {
                html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
                .arg(_threadPM.size)
                .arg(datetime.toString(_displayFormat))
                .arg(pBuf1[_threadPM.size-1]);
            }
        }
        else
        {
            for(int i=0;i<dataSize;i++)
            {
                html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td><td>%8</td></tr>")
                .arg(2*i+1)
                .arg(datetime.toString(_displayFormat))
                .arg(pBuf1[2*i])
                .arg(pBuf2[2*i])
                .arg(2*i+2)
                .arg(datetime.addSecs(dataInterval).toString(_displayFormat))
                .arg(pBuf1[2*i+1])
                .arg(pBuf2[2*i+1]);
                datetime=datetime.addSecs(2*dataInterval);
            }
            if(_threadPM.size%2!=0)
            {
                html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
                .arg(_threadPM.size)
                .arg(datetime.toString(_displayFormat))
                .arg(pBuf1[_threadPM.size-1])
                .arg(pBuf2[_threadPM.size-1]);
            }
        }
        html+=QString("</table></div></div></body></html>");
    }
    else
    {
        html+=QString("<html><head><style type=text/css>");
        html+=QString(".STYLE1{font-size: 18px;color: #000000;background-color:gray;}");
        html+=QString("</style></head><body><div align=center>");
        html+=QString("<p class=STYLE1>%1</p>").arg(_recoder.model);
        html+=QString("<div align=left>");
        html+=QString("<pre align=left>%1\t\t\t%2</pre>").arg("Model").arg(_recoder.model);
        html+=QString("<pre align=left>%1\t\t%2</pre>").arg(userInfo).arg(_recoder.userInfo);
        html+=QString("<pre align=left>%1\t\t\t%2</pre>").arg(no).arg(_recoder.bianHao);
        html+=QString("<pre align=left>%1\t\t\t%2 H:M:S </pre>").arg(recordInterval).arg(_recoder.recordInterval);
        html+=QString("<pre align=left>%1\t\t\t%2 H </pre>").arg(startDelayTime).arg(_recoder.startDelayTime);
        html+=QString("<pre align=left>%1\t\t\t%2 </pre>").arg(dataCount).arg(_recoder.recordTimes);
        html+=QString("<pre align=left>%1\t\t%2 °C</pre>").arg(maxTemperature).arg(_tempHumidity.maxTemp);
        html+=QString("<pre align=left>%1\t\t%2 °C</pre>").arg(minTemperature).arg(_tempHumidity.minTemp);
        html+=QString("<pre align=left>%1\t\t%2 °C</pre>").arg(avgTemperature).arg(_tempHumidity.avgTemp);
        html+=QString("<pre align=left>%1\t%2 °C</pre>").arg(tempAlarmUp).arg(_recoder.tempUp);
        html+=QString("<pre align=left>%1\t%2 °C</pre>").arg(tempAlarmDown).arg(_recoder.tempDown);
        if(_deviceMode!=TEMPERATUREONLY)
        {
            html+=QString("<pre align=left>%1\t\t%2 %RH</pre>").arg(maxHumidity).arg(_tempHumidity.maxHumidity);
            html+=QString("<pre align=left>%1\t\t%2 %RH</pre>").arg(minHumidity).arg(_tempHumidity.minHumidity);
            html+=QString("<pre align=left>%1\t\t%2 %RH</pre>").arg(avgHumidity).arg(_tempHumidity.avgHumidity);
            html+=QString("<pre align=left>%1\t%2 %RH</pre>").arg(humidityAlarmUp).arg(_recoder.humidityUp);
            html+=QString("<pre align=left>%1\t%2 %RH</pre>").arg(humidityAlarmDn).arg(_recoder.humidityDown);
        }
        html+=QString("<pre align=left>%1\t\t\t%2 </pre>").arg(startTime).arg(_recoder.startTime.toString(_displayFormat));
        html+=QString("<pre align=left>%1\t\t\t%2 </pre>").arg(endedTime).arg(_recoder.estimateStopTime.toString(_displayFormat));
        html+=QString("<pre align=left>%1\t\t\t%2 </pre>").arg(timeFormat).arg(_displayFormat);
        html+=QString("<pre align=left>%1\t\t°C </pre>").arg(tempUint);
        html+=QString("<pre align=left><img src= %1 width=570 height=350/></pre>").arg(imagepath);
        html+=QString("<p>&nbsp;</p>");
        html+=QString("<table width=610 border=1 bordercolor=#000000 cellspacing=0 style=border-collapse:collapse>");

        if(_deviceMode==TEMPERATUREONLY)
        {
            html+=QString("<tr><th scope=col>Number</th><th scope=col>Time</th><th scope=col>Temperature°C</th><th scope=col>Number</th><th scope=col>Time</th><th scope=col>Temperature°C</th></tr>");
        }
        else
        {
            html+=QString("<tr><th scope=col>Number</th><th scope=col>Time</th><th scope=col>Temperature°C</th><th scope=col>Humidity%RH</th><th scope=col>Number</th><th scope=col>Time</th><th scope=col>Temperature°C</th><th scope=col>Humidity%RH</th></tr>");
        }
        QDateTime datetime=_threadPM.date;
        int dataSize=_threadPM.size/2;
        int dataInterval=_threadPM.interval;
        double *pBuf1=_threadPM.pTempBuf;
        double *pBuf2=_threadPM.pHumidBuf;
        if(_deviceMode==TEMPERATUREONLY)
        {
            for(int i=0;i<dataSize;i++)
            {
                html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                .arg(2*i+1)
                .arg(datetime.toString(_displayFormat))
                .arg(pBuf1[2*i])
                .arg(2*i+2)
                .arg(datetime.addSecs(dataInterval).toString(_displayFormat))
                .arg(pBuf1[2*i+1]);
                datetime=datetime.addSecs(2*dataInterval);
            }
            if(_threadPM.size%2!=0)
            {
                html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
                .arg(_threadPM.size)
                .arg(datetime.toString(_displayFormat))
                .arg(pBuf1[_threadPM.size-1]);
            }
        }
        else
        {
            for(int i=0;i<dataSize;i++)
            {
                html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td><td>%8</td></tr>")
                .arg(2*i+1)
                .arg(datetime.toString(_displayFormat))
                .arg(pBuf1[2*i])
                .arg(pBuf2[2*i])
                .arg(2*i+2)
                .arg(datetime.addSecs(dataInterval).toString(_displayFormat))
                .arg(pBuf1[2*i+1])
                .arg(pBuf2[2*i+1]);
                datetime=datetime.addSecs(2*dataInterval);
            }
            if(_threadPM.size%2!=0)
            {
                html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
                .arg(_threadPM.size)
                .arg(datetime.toString(_displayFormat))
                .arg(pBuf1[_threadPM.size-1])
                .arg(pBuf2[_threadPM.size-1]);
            }
        }
        html+=QString("</table></div></div></body></html>");
    }
    return html;
}

//QString MainWindow::printPdf()
//{
//    QString html="";
//    QString imagepath=QString(".\\tmp.png");
//    QString userInfo=tr("userInfo");
//    QString no=tr("No");
//    QString recordInterval=tr("recordInterval");
//    QString startDelayTime=tr("startDelayTime");
//    QString dataCount=tr("dataCount");
//    QString maxTemperature=tr("maxTemperature");
//    QString minTemperature=tr("minTemperature");
//    QString avgTemperature=tr("avgTemperature");
//    QString tempAlarmUp=tr("TempAlarmUp");
//    QString tempAlarmDown=tr("TempAlarmDown");
//    QString maxHumidity=tr("maxHumidity");
//    QString minHumidity=tr("minHumidity");
//    QString avgHumidity=tr("avgHumidity");
//    QString humidityAlarmUp=tr("HumidityAlarmUp");
//    QString humidityAlarmDn=tr("HumidityAlarmDn");
//    QString startTime=tr("startTime");
//    QString endedTime=tr("endedTime");
//    QString timeFormat=tr("timeFormat");
//    QString tempUint=tr("tempUint");

//    if(CHINESE==lngType)
//    {
//        html+=QString("<html><head><style type=text/css>");
//        html+=QString(".STYLE1{font-size: 18px;color: #000000;background-color:gray;}");
//        html+=QString("</style></head><body><div align=center>");
//        html+=QString("<p class=STYLE1>%1</p>").arg(_recoder.model);
//        html+=QString("<div align=left>");
//        html+=QString("<pre align=left>%1\t\t\t%2</pre>").arg(userInfo).arg(_recoder.userInfo);
//        html+=QString("<pre align=left>%1\t\t\t%2</pre>").arg(no).arg(_recoder.bianHao);
//        html+=QString("<pre align=left>%1\t\t\t%2 H:M:S </pre>").arg(recordInterval).arg(_recoder.recordInterval);
//        html+=QString("<pre align=left>%1\t\t\t%2 H </pre>").arg(startDelayTime).arg(_recoder.startDelayTime);
//        html+=QString("<pre align=left>%1\t\t\t%2 </pre>").arg(dataCount).arg(_recoder.totalSpace);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(maxTemperature).arg(_tempHumidity.maxTemp);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(minTemperature).arg(_tempHumidity.minTemp);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(avgTemperature).arg(_tempHumidity.avgTemp);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(tempAlarmUp).arg(_recoder.tempUp);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(tempAlarmDown).arg(_recoder.tempDown);
//        html+=QString("<pre align=left>%1\t\t\t%2 %RH</pre>").arg(maxHumidity).arg(_tempHumidity.maxHumidity);
//        html+=QString("<pre align=left>%1\t\t\t%2 %RH</pre>").arg(minHumidity).arg(_tempHumidity.minHumidity);
//        html+=QString("<pre align=left>%1\t\t\t%2 %RH</pre>").arg(avgHumidity).arg(_tempHumidity.avgHumidity);
//        html+=QString("<pre align=left>%1\t\t\t%2 %RH</pre>").arg(humidityAlarmUp).arg(_recoder.humidityUp);
//        html+=QString("<pre align=left>%1\t\t\t%2 %RH</pre>").arg(humidityAlarmDn).arg(_recoder.humidityDown);
//        html+=QString("<pre align=left>%1\t\t\t%2 </pre>").arg(startTime).arg(_recoder.startTime.toString(_displayFormat));
//        html+=QString("<pre align=left>%1\t\t\t%2 </pre>").arg(endedTime).arg(_recoder.realStopTime.toString(_displayFormat));
//        html+=QString("<pre align=left>%1\t\t\t%2 </pre>").arg(timeFormat).arg(_displayFormat);
//        html+=QString("<pre align=left>%1\t\t\t°C </pre>").arg(tempUint);
//        html+=QString("<pre align=left><img src= %1 width=570 height=350/></pre>").arg(imagepath);
//        html+=QString("<table width=560 border=\"1\" bordercolor=\"#000000\" cellspacing=\"0\">");
//        html+=QString("<tr><th scope=col>编号</th><th scope=col>时间</th><th scope=col>温度°C</th><th scope=col>湿度%RH</th><th scope=col>编号</th><th scope=col>时间</th><th scope=col>温度°C</th><th scope=col>湿度%RH</th></tr>");

//        QDateTime datetime=_threadPM.date;
//        int dataSize=_threadPM.size/2;
//        int dataInterval=_threadPM.interval;
//        double *pBuf1=_threadPM.pTempBuf;
//        double *pBuf2=_threadPM.pHumidBuf;
//        for(int i=0;i<dataSize;i++)
//        {
//            html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td><td>%8</td></tr>")
//            .arg(2*i+1)
//            .arg(datetime.toString(_displayFormat))
//            .arg(pBuf1[2*i])
//            .arg(pBuf2[2*i])
//            .arg(2*i+2)
//            .arg(datetime.addSecs(dataInterval).toString(_displayFormat))
//            .arg(pBuf1[2*i+1])
//            .arg(pBuf2[2*i+1]);
//            datetime=datetime.addSecs(2*dataInterval);
//            emit valueToProgressBar(2*i);
//        }
//        if(_threadPM.size%2!=0)
//        {
//            html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
//            .arg(_threadPM.size)
//            .arg(datetime.toString(_displayFormat))
//            .arg(pBuf1[_threadPM.size-1])
//            .arg(pBuf2[_threadPM.size-1]);
//            emit valueToProgressBar(_threadPM.size);
//        }
//        html+=QString("</table></div></div></body></html>");
//    }
//    else
//    {
//        html+=QString("<html><head><style type=text/css>");
//        html+=QString(".STYLE1{font-size: 18px;color: #000000;background-color:gray;}");
//        html+=QString("</style></head><body><div align=center>");
//        html+=QString("<p class=STYLE1>%1</p>").arg(_recoder.model);
//        html+=QString("<div align=left>");
//        html+=QString("<pre align=left>%1\t\t\t\t%2</pre>").arg(userInfo).arg(_recoder.userInfo);
//        html+=QString("<pre align=left>%1\t\t\t\t%2</pre>").arg(no).arg(_recoder.bianHao);
//        html+=QString("<pre align=left>%1\t\t\t%2 H:M:S </pre>").arg(recordInterval).arg(_recoder.recordInterval);
//        html+=QString("<pre align=left>%1\t\t\t%2 H </pre>").arg(startDelayTime).arg(_recoder.startDelayTime);
//        html+=QString("<pre align=left>%1\t\t\t\t%2 </pre>").arg(dataCount).arg(_recoder.totalSpace);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(maxTemperature).arg(_tempHumidity.maxTemp);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(minTemperature).arg(_tempHumidity.minTemp);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(avgTemperature).arg(_tempHumidity.avgTemp);
//        html+=QString("<pre align=left>%1\t\t\t\t%2 °C</pre>").arg(tempAlarmUp).arg(_recoder.tempUp);
//        html+=QString("<pre align=left>%1\t\t\t%2 °C</pre>").arg(tempAlarmDown).arg(_recoder.tempDown);
//        html+=QString("<pre align=left>%1\t\t\t\t%2 %RH</pre>").arg(maxHumidity).arg(_tempHumidity.maxHumidity);
//        html+=QString("<pre align=left>%1\t\t\t\t%2 %RH</pre>").arg(minHumidity).arg(_tempHumidity.minHumidity);
//        html+=QString("<pre align=left>%1\t\t\t\t%2 %RH</pre>").arg(avgHumidity).arg(_tempHumidity.avgHumidity);
//        html+=QString("<pre align=left>%1\t\t\t%2 %RH</pre>").arg(humidityAlarmUp).arg(_recoder.humidityUp);
//        html+=QString("<pre align=left>%1\t\t\t%2 %RH</pre>").arg(humidityAlarmDn).arg(_recoder.humidityDown);
//        html+=QString("<pre align=left>%1\t\t\t\t%2 </pre>").arg(startTime).arg(_recoder.startTime.toString(_displayFormat));
//        html+=QString("<pre align=left>%1\t\t\t\t%2 </pre>").arg(endedTime).arg(_recoder.realStopTime.toString(_displayFormat));
//        html+=QString("<pre align=left>%1\t\t\t\t%2 </pre>").arg(timeFormat).arg(_displayFormat);
//        html+=QString("<pre align=left>%1\t\t\t\t°C </pre>").arg(tempUint);
//        html+=QString("<pre align=left><img src= %1 width=570 height=350/></pre>").arg(imagepath);
//        html+=QString("<p>&nbsp;</p>");
//        html+=QString("<table width=560 border=1 cellspacing=0 bordercolor=black>");
//        html+=QString("<tr><th scope=col>Number</th><th scope=col>DateTime</th><th scope=col>Temperature°C</th><th scope=col>Humidity%RH</th><th scope=col>Number</th><th scope=col>DateTime</th><th scope=col>Temperature°C</th><th scope=col>Humidity%RH</th></tr>");

//        QDateTime datetime=_threadPM.date;
//        int dataSize=_threadPM.size/2;
//        int dataInterval=_threadPM.interval;
//        double *pBuf1=_threadPM.pTempBuf;
//        double *pBuf2=_threadPM.pHumidBuf;
//        for(int i=0;i<dataSize;i++)
//        {
//            html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td><td>%8</td></tr>")
//            .arg(2*i+1)
//            .arg(datetime.toString(_displayFormat))
//            .arg(pBuf1[2*i])
//            .arg(pBuf2[2*i])
//            .arg(2*i+2)
//            .arg(datetime.addSecs(dataInterval).toString(_displayFormat))
//            .arg(pBuf1[2*i+1])
//            .arg(pBuf2[2*i+1]);
//            datetime=datetime.addSecs(2*dataInterval);
//        }
//        if(_threadPM.size%2!=0)
//        {
//            html+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
//            .arg(_threadPM.size)
//            .arg(datetime.toString(_displayFormat))
//            .arg(pBuf1[_threadPM.size-1])
//            .arg(pBuf2[_threadPM.size-1]);
//        }
//        html+=QString("</table></div></div></body></html>");
//    }
//    return html;
//}

//HKEY_LOCAL_MACHINE\Software\WOW6432Node
void MainWindow::queryCom()
{
    comVector.clear();
    QString path =QString("HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM\\");
    QSettings *settings = new QSettings( path, QSettings::NativeFormat);
    QStringList key = settings->allKeys();
    int num = (int)key.size();
    QString value;
    for(int i=num-1; i>=0; i--)
    {
        value = getcomm(i,"value");
        if(value.left(3)=="COM")
        {
            qDebug()<<"COM:"<<value;
            comVector.append(value);
        }
    }
    delete settings;
    settings=NULL;
}

QString MainWindow::getcomm(int index, QString keyorvalue)
{
    HKEY hKey;
    wchar_t keyname[256]; //键名数组
    char keyvalue[256];  //键值数组
    DWORD keysize,type,valuesize;
    int indexnum;

    QString commresult;
    if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),0,KEY_READ,&hKey)!=0)
    {
        QString error="error";
        return error;
    }
    QString keymessage;//键名
    QString message;
    QString valuemessage;//键值
    indexnum = index;//要读取键值的索引号
    keysize=sizeof(keyname);
    valuesize=sizeof(keyvalue);
     if(::RegEnumValue(hKey,indexnum,keyname,&keysize,0,&type,(BYTE*)keyvalue,&valuesize)==0)//列举键名和值
     {
        for(DWORD i=0;i<keysize;i++)
        {
            message=keyname[i];
            keymessage.append(message);
        }
        for(DWORD j=0;j<valuesize;j++)
        {
            if(keyvalue[j]!=0x00)
            {
                valuemessage.append(keyvalue[j]);
            }
        }
        if(keyorvalue=="key")
        {
            commresult=keymessage;
        }
        else if(keyorvalue=="value")
        {
            commresult=valuemessage;
        }
        }
        else
        {
            commresult="nokey";
        }
        ::RegCloseKey(hKey);//关闭注册表
        return commresult;
}

bool MainWindow::winEvent(MSG *msg, long *result)
{
    Q_UNUSED(result);
    int msgType = msg->message;
    if(msgType == WM_DEVICECHANGE)
    {
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        switch(msg->wParam)
        {
        case DBT_DEVICEARRIVAL:
            if (lpdb->dbch_devicetype == DBT_DEVTYP_PORT)
            {
//                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
//                qDebug()<<"flags"<<lpdbv->dbcv_flags;
                qDebug() <<"USB_Arrived"<<endl ;
                tempLen=0;
                if(!isDevice)
                {
                    queryCom();
                    emit comInfoToThread(comVector);
                    setGuiEnabled(false);
                    comThread->start();
                }
            }
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            if (lpdb->dbch_devicetype == DBT_DEVTYP_PORT)
            {
//                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
//                qDebug()<<"flag"<<lpdbv->dbcv_flags;
                qDebug()<<"USB_Removed"<<endl;
//                QTime dieTime = QTime::currentTime().addMSecs(1000);
//                while(QTime::currentTime()<dieTime)
//                {
//                    QCoreApplication::processEvents(QEventLoop::AllEvents,100);
//                }
                tempLen=0;
                queryCom();
                if(testDriverMove(portName))        //没找到返回true
                {
                    _threadPM.size=0;
                    if(NULL!=myCom)
                    {
                        delete myCom;
                        myCom=NULL;
                    }
                    isDevice=false;
                    stack->setVisible(false);
                    MsgWidget *msgWidget=new MsgWidget(this);
                    msgWidget->exec();
                    qDebug()<<"com removed";
                    emit findDevice();
                }
            }
            break;
        default:
            break;
        }
    }
    return false;
}

void MainWindow::showHelp()
{
    QProcess *mp_helpProcess = new QProcess(this);
    QStringList argument("Help.chm");
    mp_helpProcess->start("hh.exe", argument);//运行另一个程序片段的代码
}

void MainWindow::setSystemMail()
{
    StmMailWidget stmMailWidget(this);
    stmMailWidget.exec();
}

void MainWindow::connectSlot()
{
    tempLen=0;
    if(isDevice==false)
    {
        stack->setVisible(false);
        QMessageBox::critical(this,tr("Tips"),tr("Connection Failed"));
    }
    else
    {
        stack->setVisible(true);
        stack->setCurrentWidget(settingWidget);
        emit connection();
//        QMessageBox::information(this,tr("Tips"),tr("device has connected"));
    }
}

void MainWindow::uploadSlot()
{
    tempLen=0;
    if(isDevice==false)
    {
        stack->setVisible(false);
        QMessageBox::critical(this,tr("Tips"),tr("Connection Failed"));
        return;
    }
    else
    {
        stack->setVisible(false);
        setGuiEnabled(false);
        emit setQStringVisible(true);
        pDevice->show();
        QByteArray array;
        array.append(0x1B);
        array.append(0x0B);
        array.append(0x0D);
        sendQByteArray(array);
    }
}

void MainWindow::paramSetSlot()
{
    tempLen=0;
    if(isDevice==false)
    {
        stack->setVisible(false);
        QMessageBox::critical(this,tr("Tips"),tr("Connection Failed"));
        return;
    }
    stack->setVisible(true);
    stack->setCurrentWidget(settingWidget);
    emit clickParamSetting();
}

void MainWindow::queryDataSlot()
{
    if(_threadPM.size==0)
    {
        QMessageBox::information(this,tr("Tips"),tr("NullData"));
        return;
    }

    stack->setVisible(true);
    stack->setCurrentWidget(uploadWidget);
    uploadWidget->changeDateFormat();
}

void MainWindow::storeDataSlot()
{
    WDataStore store(this);
    store.exec();
}

void MainWindow::exportExcelSlot()
{
    QFileInfo dir("C:\\Program Files\\Microsoft Office\\Office15\\EXCEL.EXE");
    if(dir.exists())
    {
        QMessageBox::information(this,tr("Tips"),tr("Export Failed"));
        return;
    }

    if(0==_threadPM.size)
    {
        QMessageBox::information(this,tr("Tips"),tr("NullData"));
        return;
    }

    QString xlsPath=QFileDialog::getSaveFileName(0,tr("Save"),"","Xls(*.xls)");
    if(xlsPath.isEmpty())
    {
        return;
    }
    excel_event(xlsPath);
}

void MainWindow::exportPdfSlot()
{
    if(0==_threadPM.size)
    {
        QMessageBox::information(this,tr("Tips"),tr("NullData"));
        return;
    }

    QString pdfPath=QFileDialog::getSaveFileName(0,tr("Save"),"","Pdf(*.pdf)");
    if(pdfPath.isEmpty())
    {
        return;
    }
    qDebug()<<"pdf path"<<pdfPath;
    pdf_event(pdfPath);
}

// 11 12 14 15--03 07 10 13
void MainWindow::exportWordSlot()
{
    QFileInfo dir("C:\\Program Files\\Microsoft Office\\Office15\\EXCEL.EXE");
    if(dir.exists())
    {
        QMessageBox::information(this,tr("Tips"),tr("Export Failed"));
        return;
    }

    if(0==_threadPM.size)
    {
        QMessageBox::information(this,tr("Tips"),tr("NullData"));
        return;
    }

    QString wordPath=QFileDialog::getSaveFileName(0,tr("Save"),"","Word(*.doc)");
    if(wordPath.isEmpty())
    {
        return;
    }
    word_event(wordPath);
}


void MainWindow::exportTxtSlot()
{
    if(0==_threadPM.size)
    {
        QMessageBox::information(this,tr("Tips"),tr("NullData"));
        return;
    }

    QString path=QFileDialog::getSaveFileName(0,tr("Save"),"","TxT(*.txt)");
    if(path.isEmpty())
    {
        return;
    }
    qDebug()<<"txt path"<<path;
    txt_event(path);
}

void MainWindow::printSlot()
{
    if(0==_threadPM.size)
    {
        QMessageBox::information(this,tr("Tips"),tr("NullData"));
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer,this);
    preview.setMinimumSize(680,480);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)),this,SLOT(plotPic(QPrinter *)));
    printer.setOutputFormat(QPrinter::NativeFormat);
    preview.exec();
}

void MainWindow::deleteDataSlot()
{
    tempLen=0;

    if(isDevice)
    {
//        int count=0;
//        QSqlQuery query(sqldb);
//        query.exec("select name from storeName");
//        while(query.next())
//            count++;
//        query.clear();
//        if(0==count)
//        {
            QMessageBox::information(this,tr("Tips"),tr("NullData"));
            return;
//        }
    }

    if(QMessageBox::Ok==QMessageBox::information(this,tr("Tips"),
                                                 tr("Are you sure to delete the selected data record from the database?"),
                                                 QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel))
    {
        QString table=_recoder.dataName;
        if(table.isEmpty())
        {
            QMessageBox::information(this,tr("Tips"),tr("NullData"));
            return;
        }

        QSqlDatabase::database().transaction();
        QSqlQuery query(sqldb);
//        query.prepare("delete from storeName where name=:name");
//        query.bindValue(":name",table);
        bool bret=query.exec(QString("delete from storeName where name='%1'").arg(table));
        qDebug()<<bret;
        if(!bret)
        {
            QSqlDatabase::database().rollback();
            query.clear();
            QMessageBox::information(this,tr("Tips"),tr("Delete Data Failed"));
            return;
        }
//        query.prepare("delete from report where name=:name");
//        query.bindValue(":name",table);
        bret=query.exec(QString("delete from report where name='%1'").arg(table));
        if(!bret)
        {
            QSqlDatabase::database().rollback();
            query.clear();
            QMessageBox::information(this,tr("Tips"),tr("Delete Data Failed"));
            return;
        }

        bret=query.exec(QString("drop table '%1'").arg(table));
        qDebug()<<"drop"<<bret;
        if(!bret)
        {
            QSqlDatabase::database().rollback();
            query.clear();
            QMessageBox::information(this,tr("Tips"),tr("Delete Data Failed"));
            return;
        }
        QSqlDatabase::database().commit();
        query.clear();
        stack->setVisible(false);
        emit deleteFromDB(table);
    }
//    _threadPM.size=0;
}

void MainWindow::sendMailSlot()
{
    if(QMessageBox::Cancel==QMessageBox::information(this,tr("Tips"),
                                                 tr("sending email?(to ensure an unblocked network)"),
                                                 QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok))
    {
        return;
    }

    QString smtp_value;
    QString sender;
    QString pwd;
    QString theme;
    bool word=false;
    bool excel=false;
    bool pdf=false;
    bool txt=false;
    bool date=false;
    bool userInfo=false;
    QStringList list;

    QSqlQuery query(sqldb);
    qDebug()<<query.exec("select * from mail");
    while(query.next())
    {
        smtp_value=query.value(0).toString();
        sender=query.value(1).toString();
        pwd=query.value(2).toString();
        theme=query.value(3).toString();
        word=query.value(4).toBool();
        pdf=query.value(5).toBool();
        excel=query.value(6).toBool();
        txt=query.value(7).toBool();
        date=query.value(8).toBool();
        userInfo=query.value(9).toBool();
    }

    qDebug()<<query.exec("select receiver from address where flag=1");
    while(query.next())
    {
        list.append(query.value(0).toString());
    }
    qDebug()<<list.size();

    SmtpClient smtp(smtp_value, 25, SmtpClient::TcpConnection);
    smtp.setUser(sender);
    smtp.setPassword(pwd);

    MimeMessage message;
    message.setSender(new EmailAddress(sender,sender));

    for(int i=0;i<list.size();i++)
    {
        QString temp=list.at(i);
        message.addRecipient(new EmailAddress(temp,temp.left(temp.indexOf('@'))));
    }
    message.setSubject(theme);      //主题

    //附件
    if(word)
    {
        word_event(".\\tmp.doc");
        MimeAttachment *temp = new MimeAttachment(new QFile(".\\tmp.doc"));
        message.addPart(temp);
    }
    if(excel)
    {
        excel_event(".\\tmp.xls");
        MimeAttachment *temp = new MimeAttachment(new QFile(".\\tmp.xls"));
        message.addPart(temp);
    }
    if(pdf)
    {
        pdf_event(".\\tmp.pdf");
        MimeAttachment *temp = new MimeAttachment(new QFile(".\\tmp.pdf"));
        message.addPart(temp);
    }
    if(txt)
    {
        txt_event(".\\tmp.txt");
        MimeAttachment *temp = new MimeAttachment(new QFile(".\\tmp.txt"));
        message.addPart(temp);
    }

    MimeText text;          //内容
    if(date)
        text.setText(QString("test %1").arg(QDateTime::currentDateTime().toString(_displayFormat)));
    else
        text.setText(QString("test"));
    message.addPart(&text);
    if(!smtp.connectToHost())
    {
        errorMessage(tr("SMTP Connect Error \n Please ensure SMTP is correct"));
        smtp.quit();
        return;
    }
    if(!smtp.login())
    {
        errorMessage(tr("Login Error \n Please ensure username and password is correct"));
        smtp.quit();
        return;
    }
    if(!smtp.sendMail(message))
    {
        errorMessage(tr("Mail Send Error \n Please ensure receiver is correct"));
        smtp.quit();
        return;
    }
    smtp.quit();
    errorMessage(tr("Send Mail Success"));
}

void MainWindow::stopRecordSlot()
{
    tempLen=0;
    if(isDevice==false)
    {
        QMessageBox::critical(this,tr("Tips"),tr("connect failed,please check device"));
        return;
    }
    QByteArray array;
    array.append(0x1B);
    array.append(0x80);
    array.append(0x0D);
    sendQByteArray(array);
}

void MainWindow::aboutSlot()
{
    About dlg;
    dlg.exec();
}

/**************************************************
 *串口接收
 **************************************************/
void MainWindow::readCom()
{
    QByteArray temp=myCom->readAll();
    int size=temp.size();
    if(0==size)
        return;

    for(int i=0;i<size;i++)
    {
        tempBuf[tempLen+i]=(uchar)temp.at(i);
    }
    tempLen+=size;

    if(15==tempLen)             //编号
    {
        if(0x1B==tempBuf[0]&&0x03==tempBuf[1]&&0x03==tempBuf[2]&&0x0D==tempBuf[14])
        {
            tempLen=0;
            recv_bianhao();
            QByteArray array;
            array.append(0x1B);
            array.append(0x03);
            array.append(0x06);
            array.append(0x0D);
            myCom->write(array);
            qDebug()<<"编号 recv";
        }
    }
    else if(17==tempLen)        //时间
    {
        if(0x1B==tempBuf[0]&&0x03==tempBuf[1]&&0x04==tempBuf[2]&&0x0D==tempBuf[16])
        {
            tempLen=0;
            recv_shijian();
            QTime dieTime = QTime::currentTime().addMSecs(1000);
            while(QTime::currentTime()<dieTime)
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents,500);
            }
            QByteArray array;
            array.append(0x1B);
            array.append(0x03);
            array.append(0x02);
            array.append(0x0D);
            myCom->write(array);
            qDebug()<<"时间 recv";
        }
    }
    else if(53==tempLen)        //工作状态
    {
        if(0x1B==tempBuf[0]&&0x03==tempBuf[1]&&0x05==tempBuf[2]&&0x0D==tempBuf[52])
        {
            tempLen=0;
            recv_workStatus();
        }
    }
    else if(46==tempLen)        //记录仪属性
    {
        if(0x1B==tempBuf[0]&&0x03==tempBuf[1]&&0x06==tempBuf[2]&&0x0D==tempBuf[45])
        {
            tempLen=0;
            recv_property();
            QByteArray array;
            array.append(0x1B);
            array.append(0x03);
            array.append(0x05);
            array.append(0x0D);
            myCom->write(array);
            qDebug()<<"记录仪属性 recv";
        }
    }
    else if(105==tempLen)
    {
        if(0x1B==tempBuf[0]&&0x03==tempBuf[1]&&0x02==tempBuf[2]&&0x0D==tempBuf[104])
        {
            tempLen=0;
            recv_userInfo();
            QByteArray array;
            array.append(0x1B);
            array.append(0x03);
            array.append(0x03);
            array.append(0x0D);
            myCom->write(array);
            qDebug()<<"用户信息 recv";
        }
    }
    else if(data_end_length==tempLen)
    {
        qDebug()<<"len+++++"<<tempLen;

        emit valueToProgressBar(tempLen);
        tempLen=0;
        pDevice->hide();
        emit dataToThread(tempBuf,_recoder.recordTimes,_recoder.startTime,_recoder.recordInterval);
        thread->start();
    }
    else if(tempLen>data_end_length)
    {
        qDebug()<<"len+++++"<<tempLen;

        tempLen=0;
        pDevice->hide();
        setGuiEnabled(true);
    }
    qDebug()<<"len+++++"<<tempLen;
    emit valueToProgressBar(tempLen);
}

void MainWindow::sendInfo(QByteArray array)
{
    tempLen=0;
    if(NULL!=myCom&&myCom->isOpen())
    {
        myCom->write(array);
        QMessageBox::information(this,tr("Tips"),tr("Set Success"));
    }
}

void MainWindow::backUpDB()
{
    QFileDialog *file = new QFileDialog(this);
    QString Dir = file->getExistingDirectory(this, tr("Open Directory"),"",
                       QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(Dir.isEmpty())
    {
        return;
    }
    QStringList List = Dir.split("/");
    Dir = List[0];
    for(int i=1;i<List.count();i++)
    {
        Dir += "\\\\" + List[i];
    }
    qDebug()<<"backUp"<<Dir;
    bool bRet = Dir.contains(QRegExp("[\\x4e00-\\x9fa5]+"));
    if(bRet)
    {
        QMessageBox::information(this,tr("Tips"),tr("please don't choose Chinese directory"));
        return;
    }
    QString Text = QString("sqlite3.exe Data.db \".dump\" > %2\\Data.sql\n").arg(Dir);
    QProcess *Pro=new QProcess(this);
    Pro->start("Cmd");
    Pro->waitForStarted();
    Pro->write(Text.toLatin1().data());
    qDebug()<<Text.toLatin1().data();
    Pro->closeWriteChannel();
    Pro->waitForFinished();
    Pro->close();
}

void MainWindow::reductionDB()
{
    QFileDialog *file = new QFileDialog(this);
    QString Name = file->getOpenFileName(this, tr("Open DataBase"), "", tr("DataBase Files(*.sql)"));
    if(Name.isEmpty())
    {
        return;
    }
    QStringList List = Name.split("/");
    Name = List[0];
    for(int i=1;i<List.count();i++)
    {
       Name += "\\" + List[i];
    }
    bool bRet = Name.contains(QRegExp("[\\x4e00-\\x9fa5]+"));
    if(bRet)
    {
        QMessageBox::information(this,tr("Tips"),tr("please don't choose Chinese directory"));
        return;
    }
    QString Text = QString("sqlite3.exe Data.db < %1 \n").arg(Name);
    QProcess *Pro=new QProcess(this);
    Pro->start("Cmd");
    Pro->waitForStarted();
    Pro->write(Text.toLatin1().data());
    Pro->closeWriteChannel();
    Pro->waitForFinished();
    Pro->close();
}

void MainWindow::quitAct_clicked()
{
    if(QMessageBox::Ok==QMessageBox::information(this,tr("Tips"),
                                                 tr("Exit before all data is saved"),
                                                 QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel))
    {
        qApp->quit();
    }
    else
    {
        return;
    }
}

void MainWindow::openCom(QString str, QString _model)
{
    tempLen=0;
    _recoder.model=_model;
    _deviceMode=queryKindsOfDevice(_model);
    qDebug()<<"MainWindow"<<"_model"<<_model;
    myCom=NULL;
    portName=str;
    if(str=="NoPenging")
    {
        isDevice=false;
        emit findDevice();
        qDebug()<<"MainWindow"<<"NoPenging";
        QMessageBox::information(this,tr("Tips"),tr("Find No Device"));
        return;
    }
    myCom=new QextSerialPort(str);
    connect(myCom,SIGNAL(readyRead()),this,SLOT(readCom()));
    myCom->setBaudRate((BaudRateType)_serialPort.baudRate);
    myCom->setDataBits((DataBitsType)_serialPort.dataBits);
    myCom->setParity((ParityType)_serialPort.parity);
    myCom->setStopBits((StopBitsType)_serialPort.stopBits);
    myCom->setFlowControl(FLOW_OFF); //设置数据流控制，我们使用无数据流控制的默认设置
    myCom->setTimeout(10); //设置延时

    QMessageBox::information(this,tr("Tips"),tr("Find Device"));

    if(myCom->open(QIODevice::ReadWrite))
    {
        qDebug()<<"MainWindow"<<"com open success";
        isDevice=true;
        emit findDevice();
        startTickTime=QDateTime::currentDateTime().toTime_t();
        QByteArray array;
        array.append(0x1B);
        array.append(0x03);
        array.append(0x04);
        array.append(0x0D);
        sendQByteArray(array);
        stack->setVisible(true);
        stack->setCurrentWidget(settingWidget);
        emit connection();
    }
}

void MainWindow::plotPic(QPrinter *print)
{
    QTextDocument text_document;
    QString html = GeneratePicPdf();//自定义的函数，用来生成html代码
    text_document.setHtml(html);
    text_document.print(print);
    text_document.end();
}

void MainWindow::enableGuiWindow()
{
    stack->setVisible(true);
    setGuiEnabled(true);
    stack->setCurrentWidget(uploadWidget);
}

void MainWindow::dbHasData(QString name)
{
    if(name.isEmpty())
    {
        setGuiEnabled(true);
        return;
    }
    emit dataToSettingWidget();
}

void MainWindow::hasFinished(bool bret)
{
    setGuiEnabled(bret);
}

bool MainWindow::testDriverMove(QString device)
{
    int size=comVector.size();
    for(int i=0;i<size;i++)
    {
        if(comVector.at(i)==device)
        {
            return false;
        }
    }
    return true;
}

void MainWindow::errorMessage(const QString &message)
{
    QErrorMessage err(this);
    err.showMessage(message);
    err.exec();
}

void MainWindow::txt_event(QString path)
{    
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
    {
        return;
    }
    setGuiEnabled(false);
    QTextStream stream(&file);
    if(CHINESE==lngType)
    {
        stream<<endl;
        stream<<tr("Model")<<"\t\t\t\t"<<_recoder.model<<endl;
        stream<<tr("User Information")<<"\t\t\t"<<_recoder.userInfo<<endl;
        stream<<tr("Number")<<"\t\t\t\t"<<_recoder.bianHao<<endl;
        stream<<tr("Interval")<<"\t\t\t"<<QString("%1 H:M:S").arg(_recoder.recordInterval)<<endl;
        stream<<tr("Delay Time")<<"\t\t\t"<<QString("%1 H").arg(_recoder.startDelayTime)<<endl;
        stream<<tr("Data Sum")<<"\t\t\t"<<QString("%1").arg(_recoder.recordTimes)<<endl;
        stream<<tr("TemperatureMaximum")<<"\t\t\t"<<QString("%1 °C").arg(_tempHumidity.maxTemp)<<endl;
        stream<<tr("TemperatureMinimum")<<"\t\t\t"<<QString("%1 °C").arg(_tempHumidity.minTemp)<<endl;
        stream<<tr("TemperatureAverage")<<"\t\t\t"<<QString("%1 °C").arg(_tempHumidity.avgTemp)<<endl;
        stream<<tr("TemperatureAlarm Upper Limit")<<"\t\t\t"<<QString("%1 °C").arg(_recoder.tempUp)<<endl;
        stream<<tr("TemperatureAlarm Lower Limit")<<"\t\t\t"<<QString("%1 °C").arg(_recoder.tempDown)<<endl;
        if(_deviceMode!=TEMPERATUREONLY)
        {
            stream<<tr("HumidityMaximum")<<"\t\t\t"<<QString("%1 %RH").arg(_tempHumidity.maxHumidity)<<endl;
            stream<<tr("HumidityMinimum")<<"\t\t\t"<<QString("%1 %RH").arg(_tempHumidity.minHumidity)<<endl;
            stream<<tr("HumidityAverage")<<"\t\t\t"<<QString("%1 %RH").arg(_tempHumidity.avgHumidity)<<endl;
            stream<<tr("HumidityAlarm Upper Limit")<<"\t\t\t"<<QString("%1 %RH").arg(_recoder.humidityUp)<<endl;
            stream<<tr("HumidityAlarm Lower Limit")<<"\t\t\t"<<QString("%1 %RH").arg(_recoder.humidityDown)<<endl;
        }
        stream<<tr("Start Time")<<"\t\t\t"<<_recoder.startTime.toString(_displayFormat)<<endl;
        stream<<tr("End Time")<<"\t\t\t"<<_recoder.estimateStopTime.toString(_displayFormat)<<endl;
        stream<<tr("Date Format")<<"\t\t\t"<<_displayFormat<<endl;
        stream<<tr("Temperature Unit")<<"\t\t\t"<<QString("°C")<<endl;
        stream<<endl;

        if(_deviceMode==TEMPERATUREONLY)
        {
            stream<<tr("Number")<<"\t\t\t"<<tr("Time")<<"\t\t\t"
                 <<tr("Temperature")+QString("°C")<<endl;
            QDateTime datetime=_threadPM.date;
            int dataSize=_threadPM.size;
            int dataInterval=_threadPM.interval;
            double *pBuf1=_threadPM.pTempBuf;
            for(int i=0;i<dataSize;i++)
            {
                stream<<QString("%1\t\t%2\t\t%3")
                        .arg(i+1)
                        .arg(datetime.toString(_displayFormat))
                        .arg(pBuf1[i])<<endl;
                datetime=datetime.addSecs(dataInterval);
            }
        }
        else
        {
            stream<<tr("Number")<<"\t\t\t"<<tr("Time")<<"\t\t\t"
                 <<tr("Temperature")+QString("°C")+"\t\t"+tr("Humidity")+QString("%RH")<<endl;
            QDateTime datetime=_threadPM.date;
            int dataSize=_threadPM.size;
            int dataInterval=_threadPM.interval;
            double *pBuf1=_threadPM.pTempBuf;
            double *pBuf2=_threadPM.pHumidBuf;
            for(int i=0;i<dataSize;i++)
            {
                stream<<QString("%1\t\t%2\t\t%3\t\t%4")
                        .arg(i+1)
                        .arg(datetime.toString(_displayFormat))
                        .arg(pBuf1[i])
                        .arg(pBuf2[i])<<endl;
                datetime=datetime.addSecs(dataInterval);
            }
        }
    }
    else
    {
        stream<<endl;
        stream<<tr("Model")<<"\t\t\t\t"<<_recoder.model<<endl;
        stream<<tr("User Information")<<"\t\t"<<_recoder.userInfo<<endl;
        stream<<tr("Number")<<"\t\t\t\t"<<_recoder.bianHao<<endl;
        stream<<tr("Interval")<<"\t\t\t"<<QString("%1 H:M:S").arg(_recoder.recordInterval)<<endl;
        stream<<tr("Delay Time")<<"\t\t\t"<<QString("%1 H").arg(_recoder.startDelayTime)<<endl;
        stream<<tr("Data Sum")<<"\t\t\t"<<QString("%1").arg(_recoder.recordTimes)<<endl;
        stream<<tr("TemperatureMaximum")<<"\t\t"<<QString("%1 °C").arg(_tempHumidity.maxTemp)<<endl;
        stream<<tr("TemperatureMinimum")<<"\t\t"<<QString("%1 °C").arg(_tempHumidity.minTemp)<<endl;
        stream<<tr("TemperatureAverage")<<"\t\t"<<QString("%1 °C").arg(_tempHumidity.avgTemp)<<endl;
        stream<<tr("TemperatureAlarm Upper Limit")<<"\t"<<QString("%1 °C").arg(_recoder.tempUp)<<endl;
        stream<<tr("TemperatureAlarm Lower Limit")<<"\t"<<QString("%1 °C").arg(_recoder.tempDown)<<endl;
        if(_deviceMode!=TEMPERATUREONLY)
        {
            stream<<tr("HumidityMaximum")<<"\t\t\t"<<QString("%1 %RH").arg(_tempHumidity.maxHumidity)<<endl;
            stream<<tr("HumidityMinimum")<<"\t\t\t"<<QString("%1 %RH").arg(_tempHumidity.minHumidity)<<endl;
            stream<<tr("HumidityAverage")<<"\t\t\t"<<QString("%1 %RH").arg(_tempHumidity.avgHumidity)<<endl;
            stream<<tr("HumidityAlarm Upper Limit")<<"\t"<<QString("%1 %RH").arg(_recoder.humidityUp)<<endl;
            stream<<tr("HumidityAlarm Lower Limit")<<"\t"<<QString("%1 %RH").arg(_recoder.humidityDown)<<endl;
        }
        stream<<tr("Start Time")<<"\t\t\t"<<_recoder.startTime.toString(_displayFormat)<<endl;
        stream<<tr("End Time")<<"\t\t\t"<<_recoder.estimateStopTime.toString(_displayFormat)<<endl;
        stream<<tr("Date Format")<<"\t\t\t"<<_displayFormat<<endl;
        stream<<tr("Temperature Unit")<<"\t\t"<<QString("°C")<<endl;
        stream<<endl;

        if(_deviceMode==TEMPERATUREONLY)
        {
            stream<<tr("Number")<<"\t\t\t"<<tr("Time")<<"\t\t\t"
                 <<tr("Temperature")+QString("°C")<<endl;
            QDateTime datetime=_threadPM.date;
            int dataSize=_threadPM.size;
            int dataInterval=_threadPM.interval;
            double *pBuf1=_threadPM.pTempBuf;
            for(int i=0;i<dataSize;i++)
            {
                stream<<QString("%1\t\t%2\t\t%3")
                        .arg(i+1)
                        .arg(datetime.toString(_displayFormat))
                        .arg(pBuf1[i])<<endl;
                datetime=datetime.addSecs(dataInterval);
            }
        }
        else
        {
            stream<<tr("Number")<<"\t\t\t"<<tr("Time")<<"\t\t\t"
                 <<tr("Temperature")+QString("°C")+"\t\t"+tr("Humidity")+QString("%RH")<<endl;
            QDateTime datetime=_threadPM.date;
            int dataSize=_threadPM.size;
            int dataInterval=_threadPM.interval;
            double *pBuf1=_threadPM.pTempBuf;
            double *pBuf2=_threadPM.pHumidBuf;
            for(int i=0;i<dataSize;i++)
            {
                stream<<QString("%1\t\t%2\t\t%3\t\t\t%4")
                        .arg(i+1)
                        .arg(datetime.toString(_displayFormat))
                        .arg(pBuf1[i])
                        .arg(pBuf2[i])<<endl;
                datetime=datetime.addSecs(dataInterval);
            }
        }
    }
    file.close();
    qDebug()<<"txt export sucess"<<endl;
    setGuiEnabled(true);
    QMessageBox::information(this,tr("Tips"),tr("Export Success"));
}

void MainWindow::word_event(QString wordPath)
{
    setGuiEnabled(false);
    QAxWidget *word=new QAxWidget("Word.Application",0,Qt::MSWindowsOwnDC);
    word->setProperty("Visible",false);
    QAxObject *documents=word->querySubObject("Documents");
    QDir dir;
    QString path=dir.currentPath();
    QString exePath=QString("%1/tmp.dot").arg(path);
    qDebug()<<exePath<<endl;
    documents->dynamicCall("Open(const QString&)",QDir::toNativeSeparators(exePath));
    QAxObject *document=word->querySubObject("ActiveDocument");
    QString html,str;
    if(lngType==CHINESE)
    {
        str=QString("%1\t\t\t\t\t\t\t%2\n").arg(tr("Model")).arg(_recoder.model);
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("User Information")).arg(_recoder.userInfo);
        html+=str;
        str=QString("%1\t\t\t\t\t\t\t%2\n").arg(tr("Number")).arg(_recoder.bianHao);
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("Interval")).arg(_recoder.recordInterval);
        html+=str;
        str=QString("%1\t\t\t\t\t%2H\n").arg(tr("Delay Time")).arg(_recoder.startDelayTime);
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("Data Sum")).arg(_recoder.recordTimes);
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2°C\n").arg(tr("TemperatureMaximum")).arg(_tempHumidity.maxTemp);
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2°C\n").arg(tr("TemperatureMinimum")).arg(_tempHumidity.minTemp);
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2°C\n").arg(tr("TemperatureAverage")).arg(_tempHumidity.avgTemp);
        html+=str;
        str=QString("%1\t\t\t\t\t%2°C\n").arg(tr("TemperatureAlarm Upper Limit")).arg(_recoder.tempUp);
        html+=str;
        str=QString("%1\t\t\t\t\t%2°C\n").arg(tr("TemperatureAlarm Lower Limit")).arg(_recoder.tempDown);
        html+=str;
        if(_deviceMode!=TEMPERATUREONLY)
        {
            str=QString("%1\t\t\t\t\t\t%2%RH\n").arg(tr("HumidityMaximum")).arg(_tempHumidity.maxHumidity);
            html+=str;
            str=QString("%1\t\t\t\t\t\t%2%RH\n").arg(tr("HumidityMinimum")).arg(_tempHumidity.minHumidity);
            html+=str;
            str=QString("%1\t\t\t\t\t\t%2%RH\n").arg(tr("HumidityAverage")).arg(_tempHumidity.avgHumidity);
            html+=str;
            str=QString("%1\t\t\t\t\t%2%RH\n").arg(tr("HumidityAlarm Upper Limit")).arg(_recoder.humidityUp);
            html+=str;
            str=QString("%1\t\t\t\t\t%2%RH\n").arg(tr("HumidityAlarm Lower Limit")).arg(_recoder.humidityDown);
            html+=str;
        }
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("Start Time")).arg(_recoder.startTime.toString(_displayFormat));
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("End Time")).arg(_recoder.estimateStopTime.toString(_displayFormat));
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("Date Format")).arg(_displayFormat);
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("Temperature Unit")).arg("°C");
        html+=str;
        if(_deviceMode==TEMPERATUREONLY)
        {
            str=QString("%1\t\t\t%2\t\t\t\t%3°C\n")
                    .arg(tr("Number")).arg(tr("Time")).arg(tr("Temperature"));
        }
        else
        {
            str=QString("%1\t\t\t%2\t\t\t\t%3°C\t\t\t%4%RH\n")
                    .arg(tr("Number")).arg(tr("Time")).arg(tr("Temperature")).arg(tr("Humidity"));
        }
        html+=str;
    }
    else
    {
        str=QString("%1\t\t\t\t\t\t\t%2\n").arg(tr("Model")).arg(_recoder.model);
        html+=str;
        str=QString("%1\t\t\t\t\t%2\n").arg(tr("User Information")).arg(_recoder.userInfo);
        html+=str;
        str=QString("%1\t\t\t\t\t\t\t%2\n").arg(tr("Number")).arg(_recoder.bianHao);
        html+=str;
        str=QString("%1\t\t\t\t\t\t\t%2\n").arg(tr("Interval")).arg(_recoder.recordInterval);
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2H\n").arg(tr("Delay Time")).arg(_recoder.startDelayTime);
        html+=str;
        str=QString("%1\t\t\t\t\t\t\t%2\n").arg(tr("Data Sum")).arg(_recoder.recordTimes);
        html+=str;
        str=QString("%1\t\t\t\t%2°C\n").arg(tr("TemperatureMaximum")).arg(_tempHumidity.maxTemp);
        html+=str;
        str=QString("%1\t\t\t\t%2°C\n").arg(tr("TemperatureMinimum")).arg(_tempHumidity.minTemp);
        html+=str;
        str=QString("%1\t\t\t\t%2°C\n").arg(tr("TemperatureAverage")).arg(_tempHumidity.avgTemp);
        html+=str;
        str=QString("%1\t\t%2°C\n").arg(tr("TemperatureAlarm Upper Limit")).arg(_recoder.tempUp);
        html+=str;
        str=QString("%1\t\t%2°C\n").arg(tr("TemperatureAlarm Lower Limit")).arg(_recoder.tempDown);
        html+=str;
        if(_deviceMode!=TEMPERATUREONLY)
        {
            str=QString("%1\t\t\t\t%2%RH\n").arg(tr("HumidityMaximum")).arg(_tempHumidity.maxHumidity);
            html+=str;
            str=QString("%1\t\t\t\t\t%2%RH\n").arg(tr("HumidityMinimum")).arg(_tempHumidity.minHumidity);
            html+=str;
            str=QString("%1\t\t\t\t\t%2%RH\n").arg(tr("HumidityAverage")).arg(_tempHumidity.avgHumidity);
            html+=str;
            str=QString("%1\t\t\t%2%RH\n").arg(tr("HumidityAlarm Upper Limit")).arg(_recoder.humidityUp);
            html+=str;
            str=QString("%1\t\t\t%2%RH\n").arg(tr("HumidityAlarm Lower Limit")).arg(_recoder.humidityDown);
            html+=str;
        }
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("Start Time")).arg(_recoder.startTime.toString(_displayFormat));
        html+=str;
        str=QString("%1\t\t\t\t\t\t\t%2\n").arg(tr("End Time")).arg(_recoder.estimateStopTime.toString(_displayFormat));
        html+=str;
        str=QString("%1\t\t\t\t\t\t%2\n").arg(tr("Date Format")).arg(_displayFormat);
        html+=str;
        str=QString("%1\t\t\t\t\t%2\n").arg(tr("Temperature Unit")).arg("°C");
        html+=str;
        if(_deviceMode==TEMPERATUREONLY)
        {
            str=QString("%1\t\t\t%2\t\t\t\t%3°C\n")
                    .arg("Number").arg("Time").arg("Temperature");
        }
        else
        {
            str=QString("%1\t\t\t%2\t\t\t\t%3°C\t\t%4%RH\n")
                    .arg("Number").arg("Time").arg("Temperature").arg("Humidity");
        }
        html+=str;
    }
    QDateTime datetime=_threadPM.date;
    int dataSize=_threadPM.size;
    int dataInterval=_threadPM.interval;
    double *pBuf1=_threadPM.pTempBuf;
    double *pBuf2=_threadPM.pHumidBuf;

    if(_deviceMode==TEMPERATUREONLY)
    {
        for(int i=0;i<dataSize;i++)
        {
            str=QString("%1\t\t\t%2\t\t\t%3\n")
                    .arg(i+1)
                    .arg(datetime.toString(_displayFormat))
                    .arg(pBuf1[i]);
            datetime=datetime.addSecs(dataInterval);
            html+=str;
        }
    }
    else
    {
        for(int i=0;i<dataSize;i++)
        {
            str=QString("%1\t\t\t%2\t\t\t%3\t\t\t\t%4\n")
                    .arg(i+1)
                    .arg(datetime.toString(_displayFormat))
                    .arg(pBuf1[i])
                    .arg(pBuf2[i]);
            datetime=datetime.addSecs(dataInterval);
            html+=str;
        }
    }

    QAxObject *obj1=word->querySubObject("Selection");
    obj1->dynamicCall("Text",html);
    QAxObject *bookmark_pic=document->querySubObject("Bookmarks(QVariant)","label1");
    if(!bookmark_pic->isNull())
    {
        bookmark_pic->dynamicCall("Select(void)");
        QAxObject *range = bookmark_pic->querySubObject("Range");
        QVariant tmp = range->asVariant();
        QList<QVariant>qList;
        exePath=QString("%1/tmp.png").arg(path);
        qList<<QVariant(QDir::toNativeSeparators(exePath));
        qList<<QVariant(false);
        qList<<QVariant(true);
        qList<<tmp;
        QAxObject *Inlineshapes = document->querySubObject("InlineShapes");
        Inlineshapes->dynamicCall("AddPicture(const QString&, QVariant, QVariant ,QVariant)",qList);
    }
    document->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(wordPath));
    document->dynamicCall("Close(boolean)",false);
    word->dynamicCall("Quit()");
    if(word!=NULL)
        delete word;
    word=NULL;
    qDebug()<<"word success"<<endl;
    setGuiEnabled(true);
    QMessageBox::information(this,tr("Tips"),tr("Export Success"));
}

void MainWindow::excel_event(QString xlsPath)
{
    setGuiEnabled(false);
    QAxWidget *excel=new QAxWidget("Excel.Application");
    excel->setProperty("Visible", false);
    QAxObject *workbooks = excel->querySubObject("WorkBooks");
    workbooks->dynamicCall("Add");
    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
    QAxObject *worksheet = workbook->querySubObject("WorkSheets(int)", 1);
    worksheet->setProperty("Name",QVariant(tr("DataTable")));
    QAxObject *range = worksheet->querySubObject("Columns(A:A)");
    range->setProperty("ColumnWidth",QVariant("29.13"));
    range = worksheet->querySubObject("Columns(B:B)");
    range->setProperty("ColumnWidth",QVariant("21.38"));
    range = worksheet->querySubObject("Columns(C:C)");
    range->setProperty("ColumnWidth",QVariant("13.38"));

    int index=1;
    range = worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Model")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(_recoder.model));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("User Information")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(_recoder.userInfo));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Number")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(_recoder.bianHao));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Interval")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("%1 H:M:S").arg(_recoder.recordInterval)));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Delay Time")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("%1 H").arg(_recoder.startDelayTime)));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Data Sum")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("%1").arg(_recoder.recordTimes)));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("TemperatureMaximum")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("%1 °C").arg(_tempHumidity.maxTemp)));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("TemperatureMinimum")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("%1 °C").arg(_tempHumidity.minTemp)));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("TemperatureAverage")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("%1 °C").arg(_tempHumidity.avgTemp)));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("TemperatureAlarm Upper Limit")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("%1 °C").arg(_recoder.tempUp)));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("TemperatureAlarm Lower Limit")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("%1 °C").arg(_recoder.tempDown)));
    index++;

    if(_deviceMode!=TEMPERATUREONLY)
    {
        range=worksheet->querySubObject("Cells(int,int)",index,1);
        range->setProperty("Value", QVariant(tr("HumidityMaximum")));
        range = worksheet->querySubObject("Cells(int,int)",index,2);
        range->setProperty("Value", QVariant(QString("%1 %RH").arg(_tempHumidity.maxHumidity)));
        index++;
        range=worksheet->querySubObject("Cells(int,int)",index,1);
        range->setProperty("Value", QVariant(tr("HumidityMinimum")));
        range = worksheet->querySubObject("Cells(int,int)",index,2);
        range->setProperty("Value", QVariant(QString("%1 %RH").arg(_tempHumidity.minHumidity)));
        index++;
        range=worksheet->querySubObject("Cells(int,int)",index,1);
        range->setProperty("Value", QVariant(tr("HumidityAverage")));
        range = worksheet->querySubObject("Cells(int,int)",index,2);
        range->setProperty("Value", QVariant(QString("%1 %RH").arg(_tempHumidity.avgHumidity)));
        index++;
        range=worksheet->querySubObject("Cells(int,int)",index,1);
        range->setProperty("Value", QVariant(tr("HumidityAlarm Upper Limit")));
        range = worksheet->querySubObject("Cells(int,int)",index,2);
        range->setProperty("Value", QVariant(QString("%1 %RH").arg(_recoder.humidityUp)));
        index++;
        range=worksheet->querySubObject("Cells(int,int)",index,1);
        range->setProperty("Value", QVariant(tr("HumidityAlarm Lower Limit")));
        range = worksheet->querySubObject("Cells(int,int)",index,2);
        range->setProperty("Value", QVariant(QString("%1 %RH").arg(_recoder.humidityDown)));
        index++;
    }
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Start Time")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("NumberFormatLocal",QVariant(_displayFormat));
    range->setProperty("Value", QVariant(QString("%1").arg(_recoder.startTime.toString(_displayFormat))));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("End Time")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("NumberFormatLocal",QVariant(_displayFormat));
    range->setProperty("Value", QVariant(QString("%1").arg(_recoder.estimateStopTime.toString(_displayFormat))));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Date Format")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString(_displayFormat)));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Temperature Unit")));
    range = worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(QString("°C")));
    index++;
    range=worksheet->querySubObject("Cells(int,int)",index,1);
    range->setProperty("Value", QVariant(tr("Number")));
    range=worksheet->querySubObject("Cells(int,int)",index,2);
    range->setProperty("Value", QVariant(tr("Time")));
    range=worksheet->querySubObject("Cells(int,int)",index,3);
    range->setProperty("Value", QVariant(tr("Temperature")));
    if(_deviceMode!=TEMPERATUREONLY)
    {
        range=worksheet->querySubObject("Cells(int,int)",index,4);
        range->setProperty("Value", QVariant(tr("Humidity")));
    }
    index++;

    QAxObject *merge_range = worksheet->querySubObject("Range(const QString&)","B3");
    merge_range->setProperty("HorizontalAlignment",-4131);
    merge_range = worksheet->querySubObject("Range(const QString&)","B6");
    merge_range->setProperty("HorizontalAlignment",-4131);

    QString col1,col2,col3,col4;
    QDateTime datetime=_threadPM.date;
    int dataSize=_threadPM.size;
    int interval=_threadPM.interval;
    double *pBuf1=_threadPM.pTempBuf;
    double *pBuf2=_threadPM.pHumidBuf;

    if(_deviceMode==TEMPERATUREONLY)
    {
        QList<QVariant> cells;
        QList<QStringList> rows;
        QStringList cell;

        for(int i=0;i<dataSize;i++)
        {
            col1=QString("%1").arg(i+1);
            col2=datetime.toString(_displayFormat);
            col3=QString::number(pBuf1[i],'f',1);
            cell.clear();
            cell.append(col1);
            cell.append(col2);
            cell.append(col3);
            rows.append(cell);
            datetime=datetime.addSecs(interval);
        }
        cell.clear();
        foreach (QStringList row, rows)
        {
            for (int i = 0; i < row.size(); i++)
                    cell.append(row.at(i));
            cells.append(cell);
            cell.clear();
        }
        QString position;
        position.append(QChar(1 - 1 + 'A'));
        position.append(QString::number(index));
        position.append(":");
        position.append(QChar(3 - 1 + 'A'));
        position.append(QString::number(index+dataSize-1));

        range = worksheet->querySubObject("Range(const QString&)",QString("B%1:B%2").arg(index).arg(index+dataSize-1));
        range->setProperty("NumberFormatLocal",QVariant(_displayFormat));

        range = worksheet->querySubObject("Range(const QString&)", position);
        if (!range->isNull())
            range->dynamicCall("SetValue2(QVariant)", QVariant(cells));
    }
    else
    {
        QList<QVariant> cells;
        QList<QStringList> rows;
        QStringList cell;
        for(int i=0;i<dataSize;i++)
        {
            col1=QString("%1").arg(i+1);
            col2=datetime.toString(_displayFormat);
            col3=QString::number(pBuf1[i],'f',1);
            col4=QString::number(pBuf2[i],'f',1);

            cell.clear();
            cell.append(col1);
            cell.append(col2);
            cell.append(col3);
            cell.append(col4);
            rows.append(cell);

            datetime=datetime.addSecs(interval);
        }
        cell.clear();
        foreach (QStringList row, rows)
        {
            for (int i = 0; i < row.size(); i++)
                    cell.append(row.at(i));
            cells.append(cell);
            cell.clear();
        }
        QString position;
        position.append(QChar(1 - 1 + 'A'));
        position.append(QString::number(index));
        position.append(":");
        position.append(QChar(4 - 1 + 'A'));
        position.append(QString::number(index+dataSize-1));

        range = worksheet->querySubObject("Range(const QString&)",QString("B%1:B%2").arg(index).arg(index+dataSize-1));
        range->setProperty("NumberFormatLocal",QVariant(_displayFormat));

        range = worksheet->querySubObject("Range(const QString&)", position);
        if (!range->isNull())
            range->dynamicCall("SetValue2(QVariant)", QVariant(cells));
    }

    worksheet = workbook->querySubObject("WorkSheets(int)",2);
    worksheet->setProperty("Name",QVariant(tr("Diagram")));
    QAxObject *shape=worksheet->querySubObject("Shapes");
//    QAxObject *shape=worksheet->querySubObject("Pictures");

    QDir dir;
    QString path=dir.currentPath();
    QString exePath=QString("%1/tmp.png").arg(path);
    qDebug()<<"exepath"<<exePath;
    shape->dynamicCall("AddPicture(const QString&,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)",
                       QDir::toNativeSeparators(exePath),true,true,10,10,620,298);
//    shape->dynamicCall("Insert(const QString&)",QDir::toNativeSeparators(exePath),10,10,620,298);

    excel->setProperty("DisplayAlerts",false);

//    QFileInfo fi(xlsPath);
//    QList<QVariant> params;
//    params << QDir::toNativeSeparators(xlsPath);
//    params << (fi.suffix().toLower().compare("xlsx") == 0 ? 51 : 56);
//    params << QString();
//    params << QString();
//    params << false;
//    workbook->dynamicCall("SaveAs(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)", params);
    workbook->dynamicCall("SaveAs(QVariant)", QDir::toNativeSeparators(xlsPath));

    excel->setProperty("DisplayAlerts",true);
    workbook->dynamicCall("Close(Boolean)",false);
    excel->dynamicCall("Quit(void)");
    if(excel!=NULL)
        delete excel;
    excel=NULL;
    qDebug()<<"export excel success";
    setGuiEnabled(true);

    QMessageBox::information(this,tr("Tips"),tr("Export Success"));
}

void MainWindow::pdf_event(QString pdfPath)
{
    setGuiEnabled(false);
    QPrinter printer_text(QPrinter::HighResolution);
    printer_text.setOutputFormat(QPrinter::PdfFormat);
    printer_text.setOutputFileName(pdfPath);//pdfname为要保存的pdf文件名
    QTextDocument text_document;
    QString html = GeneratePicPdf();//自定义的函数，用来生成html代码
    text_document.setHtml(html);
    text_document.print(&printer_text);
    text_document.end();
    qDebug()<<"pdf sucess"<<endl;
    setGuiEnabled(true);
    QMessageBox::information(this,tr("Tips"),tr("Export Success"));
}

void MainWindow::recv_userInfo()
{
    QByteArray array;
    for(int i=0;i<100;i++)
    {
        array.append(tempBuf[i+3]);
    }
    _recoder.userInfo=array;
    qDebug()<<"用户信息"<<_recoder.userInfo;
}

void MainWindow::recv_bianhao()
{
    QByteArray array;
    for(int i=0;i<10;i++)
    {
        array.append(tempBuf[i+3]);
    }
    _recoder.bianHao=array;
    qDebug()<<"bianhao"<<_recoder.bianHao;
}

void MainWindow::recv_shijian()
{
    QString str= QString("20%1%2-%3%4-%5%6 %7%8:%9%10:%11%12")
            .arg(tempBuf[3]).arg(tempBuf[4]).arg(tempBuf[5]).arg(tempBuf[6])
            .arg(tempBuf[7]).arg(tempBuf[8]).arg(tempBuf[9]).arg(tempBuf[10])
            .arg(tempBuf[11]).arg(tempBuf[12]).arg(tempBuf[13]).arg(tempBuf[14]);
    qDebug()<<"wain window clock"<<str;
    _recoder.clock=QDateTime::fromString(str,DEFAULT_TIME_FORMAT);

    uint value=_recoder.clock.toTime_t();
    int rst=startTickTime-value;
    qDebug()<<"mainwindow time"<<value<<startTickTime;
    if(rst>5||rst<-5)
    {
        QSqlQuery query(sqldb);
        query.exec("select autoSynchronous from system");
        bool flag=false;
        while(query.next())
        {
            flag=query.value(0).toBool();
        }
        query.clear();
        if(flag)
        {
            QByteArray dateBA;
            QString str=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
            dateBA.append(0x1B);
            dateBA.append(0x02);
            dateBA.append(0x04);
            dateBA.append(str.left(1).toInt());
            dateBA.append(str.mid(1,1).toInt());
            dateBA.append(str.mid(3,1).toInt());
            dateBA.append(str.mid(4,1).toInt());
            dateBA.append(str.mid(6,1).toInt());
            dateBA.append(str.mid(7,1).toInt());
            dateBA.append(str.mid(9,1).toInt());
            dateBA.append(str.mid(10,1).toInt());
            dateBA.append(str.mid(12,1).toInt());
            dateBA.append(str.mid(13,1).toInt());
            dateBA.append(str.mid(15,1).toInt());
            dateBA.append(str.right(1).toInt());
            dateBA.append(accumulationAnd(dateBA));
            dateBA.append(0x0D);
            sendQByteArray(dateBA);
        }
        else
        {
            if(QMessageBox::Ok==QMessageBox::information(this,tr("Tips"),tr("set time synchronoused with the recorder?")
                                                                 ,QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel))
            {
                QByteArray dateBA;
                QString str=QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
                dateBA.append(0x1B);
                dateBA.append(0x02);
                dateBA.append(0x04);
                dateBA.append(str.left(1).toInt());
                dateBA.append(str.mid(1,1).toInt());
                dateBA.append(str.mid(3,1).toInt());
                dateBA.append(str.mid(4,1).toInt());
                dateBA.append(str.mid(6,1).toInt());
                dateBA.append(str.mid(7,1).toInt());
                dateBA.append(str.mid(9,1).toInt());
                dateBA.append(str.mid(10,1).toInt());
                dateBA.append(str.mid(12,1).toInt());
                dateBA.append(str.mid(13,1).toInt());
                dateBA.append(str.mid(15,1).toInt());
                dateBA.append(str.right(1).toInt());
                dateBA.append(accumulationAnd(dateBA));
                dateBA.append(0x0D);
                sendQByteArray(dateBA);
            }
        }
    }
}

void MainWindow::recv_workStatus()
{
    _recoder.totalSpace=tempBuf[4]*10000+tempBuf[5]*1000+tempBuf[6]*100+tempBuf[7]*10+tempBuf[8];
    _recoder.recordTimes=tempBuf[9]*10000+tempBuf[10]*1000+tempBuf[11]*100+tempBuf[12]*10+tempBuf[13];
    data_end_length=FACTOR*_recoder.recordTimes;
    switch(tempBuf[14])
    {
    case 0:
        _recoder.workStatus=tr("Wait For Record");
        break;
    case 1:
        _recoder.workStatus=tr("Recorded");
        break;
    case 2:
        _recoder.workStatus=tr("Stopped");
        break;
    default:
        _recoder.workStatus="";
        break;
    }

    QString str=QString("20%1%2-%3%4-%5%6 %7%8:%9%10:%11%12")
            .arg(tempBuf[15]).arg(tempBuf[16]).arg(tempBuf[17]).arg(tempBuf[18])
            .arg(tempBuf[19]).arg(tempBuf[20]).arg(tempBuf[21]).arg(tempBuf[22])
            .arg(tempBuf[23]).arg(tempBuf[24]).arg(tempBuf[25]).arg(tempBuf[26]);
    _recoder.startTime=QDateTime::fromString(str,DEFAULT_TIME_FORMAT);
    str=QString("20%1%2-%3%4-%5%6 %7%8:%9%10:%11%12")
            .arg(tempBuf[27]).arg(tempBuf[28]).arg(tempBuf[29]).arg(tempBuf[30])
            .arg(tempBuf[31]).arg(tempBuf[32]).arg(tempBuf[33]).arg(tempBuf[34])
            .arg(tempBuf[35]).arg(tempBuf[36]).arg(tempBuf[37]).arg(tempBuf[38]);
    _recoder.frontTestTime=QDateTime::fromString(str,DEFAULT_TIME_FORMAT);
    str=QString("20%1%2-%3%4-%5%6 %7%8:%9%10:%11%12")
            .arg(tempBuf[39]).arg(tempBuf[40]).arg(tempBuf[41]).arg(tempBuf[42])
            .arg(tempBuf[43]).arg(tempBuf[44]).arg(tempBuf[45]).arg(tempBuf[46])
            .arg(tempBuf[47]).arg(tempBuf[48]).arg(tempBuf[49]).arg(tempBuf[50]);
    _recoder.realStopTime=QDateTime::fromString(str,DEFAULT_TIME_FORMAT);

    qDebug()<<"totalSpace"<<_recoder.totalSpace<<_recoder.recordTimes;
    qDebug()<<"data_end_length"<<data_end_length;
    qDebug()<<"workStatus"<<_recoder.workStatus;
    qDebug()<<"startTime"<<_recoder.startTime.toString(DEFAULT_TIME_FORMAT);
    qDebug()<<"frontTestTime"<<_recoder.frontTestTime.toString(DEFAULT_TIME_FORMAT);
    qDebug()<<"realStopTime"<<_recoder.realStopTime.toString(DEFAULT_TIME_FORMAT);

    QDateTime date=_recoder.startTime;
    int time=0;
    if(_recoder.recordInterval.length()==8)
    {
        time=_recoder.recordInterval.left(2).toInt()*3600
                +_recoder.recordInterval.mid(3,2).toInt()*60
                +_recoder.recordInterval.right(2).toInt();
        qDebug()<<"recordInterval"<<time;
    }
    date=date.addSecs(time*(_recoder.recordTimes-1));
    _recoder.estimateStopTime=date;

    qDebug()<<"estimateStopTime"<<_recoder.estimateStopTime.toString(DEFAULT_TIME_FORMAT);
    qDebug()<<"工作状态 recv"<<data_end_length;

    emit dataToSettingWidget();

    if(data_end_length==0)
    {
        pDevice->hide();
        setGuiEnabled(true);
        return;
    }
    emit setProgressBarMaxunum(data_end_length);
    emit setQStringVisible(true);

    QSqlQuery query(sqldb);
    query.exec("select autoUpload from system");
    bool flag=false;
    while(query.next())
    {
        flag=query.value(0).toBool();
    }
    query.clear();
    if(flag)
    {
        pDevice->show();
        QByteArray array;
        array.append(0x1B);
        array.append(0x0B);
        array.append(0x0D);
        myCom->write(array);
    }
    else
    {
        stack->setVisible(true);
        setGuiEnabled(true);
    }
}

void MainWindow::recv_property()
{
    _recoder.recordInterval=QString("%1%2:%3%4:%5%6")
            .arg(tempBuf[3]).arg(tempBuf[4])
            .arg(tempBuf[5]).arg(tempBuf[6])
            .arg(tempBuf[7]).arg(tempBuf[8]);
    qDebug()<<"interval"<<_recoder.recordInterval;

    _recoder.startDelayTime=QString("%1.%2")
            .arg(tempBuf[9]).arg(tempBuf[10]).toDouble();

    qDebug()<<"startDelayTime"<<_recoder.startDelayTime;
    qDebug()<<"station"<<tempBuf[11]<<tempBuf[12]<<tempBuf[13];

    _recoder.recordNo=tempBuf[11]*100+tempBuf[12]*10+tempBuf[13];

    qDebug()<<"recordNo"<<_recoder.recordNo;

    _recoder.keyStop=tempBuf[14]==0x00?false:true;
    _recoder.alarmSetting=tempBuf[15];
    _recoder.toneSetting=tempBuf[16]==0x00?false:true;
    _recoder.tempUint=tempBuf[17]==0x00?false:true;

    qDebug()<<"keyStop"<<_recoder.keyStop<< _recoder.alarmSetting<<_recoder.toneSetting
              <<_recoder.tempUint;


    if(tempBuf[18]==0x00)
    {
        _recoder.tempUp=QString("%1%2.%3").arg(tempBuf[20]).arg(tempBuf[21]).arg(tempBuf[22]).toDouble();
    }
    else
    {
        _recoder.tempUp=QString("-%1%2.%3").arg(tempBuf[20]).arg(tempBuf[21]).arg(tempBuf[22]).toDouble();
    }
    if(tempBuf[23]==0x00)
    {
        _recoder.tempDown=QString("%1%2.%3").arg(tempBuf[25]).arg(tempBuf[26]).arg(tempBuf[27]).toDouble();
    }
    else
    {
        _recoder.tempDown=QString("-%1%2.%3").arg(tempBuf[25]).arg(tempBuf[26]).arg(tempBuf[27]).toDouble();
    }
    if(tempBuf[28]==0x00)
    {
        _recoder.tempCorrect=QString("%1.%2").arg(tempBuf[30]).arg(tempBuf[31]).toDouble();
    }
    else
    {
        _recoder.tempCorrect=QString("-%1.%2").arg(tempBuf[30]).arg(tempBuf[31]).toDouble();
    }

    qDebug()<<"temp"<<_recoder.tempUp<<_recoder.tempDown<<_recoder.tempCorrect;

    _recoder.humidityUp=QString("%1%2.%3").arg(tempBuf[33]).arg(tempBuf[34]).arg(tempBuf[35]).toDouble();
    _recoder.humidityDown=QString("%1%2.%3").arg(tempBuf[37]).arg(tempBuf[38]).arg(tempBuf[39]).toDouble();
    if(tempBuf[40]==0x00)
    {
        _recoder.humidityCorrect=QString("%1.%2").arg(tempBuf[42]).arg(tempBuf[43]).toDouble();
    }
    else
    {
        _recoder.humidityCorrect=QString("-%1.%2").arg(tempBuf[42]).arg(tempBuf[43]).toDouble();
    }

    qDebug()<<"humidityUp"<<_recoder.humidityUp<<_recoder.humidityDown<<_recoder.humidityCorrect;
}

void MainWindow::setGuiEnabled(bool bret)
{
    toolBar->setEnabled(bret);
    stack->setEnabled(bret);
}

void MainWindow::sendQByteArray(QByteArray array)
{
    if(NULL!=myCom&&myCom->isOpen())
    {
        myCom->write(array);
    }
}

void MainWindow::translateLanguage()
{

}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    ev->ignore();
    quitAct_clicked();
}
