#include "settingwidget.h"

#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimeEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QMutex>

//QMutex timeMutex;

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent)
{    
    dateTime=QDateTime::currentDateTime();
    //group
    workGroup=new QGroupBox(tr("Work Status"),this);
    propertyGroup=new QGroupBox(tr("Record Property"),this);
    clockGroup=new QGroupBox(tr("Clock"),this);
    noGroup=new QGroupBox(tr("Number"),this);
    userGroup=new QGroupBox(tr("User Information"),this);

    //红字提示
    tipLB=new QLabel(tr("Click Save Parameter will clear all data of Data Logger"));
    tipLB->setObjectName("tipedLabel");

    //按钮
    saveBtn=new QPushButton(tr("Save Parameter"),this);
    quitBtn=new QPushButton(tr("Exit"),this);

    //group1
    modelLB=new QLabel(tr("Model"));
    totalSpaceLB=new QLabel(tr("Total Space"));
    recordCountLB=new QLabel(tr("Recorded Count"));
    workStatusLB=new QLabel(tr("Work Status"));
    startTimeLB=new QLabel(tr("Start Time"));
    frontConnectTimeLB=new QLabel(tr("Last Online time"));
    stopTimeLB=new QLabel(tr("Expected Stop Time"));
    realStopTimeLB=new QLabel(tr("Actual Stop Time"));

    modelLE=new QLineEdit();
    totalSpaceLE=new QLineEdit();
    recordCountLE=new QLineEdit();
    workStatusLE=new QLineEdit();
    startTimeLE=new QLineEdit();
    frontConnectTimeLE=new QLineEdit();
    stopTimeLE=new QLineEdit();
    realStopTimeLE=new QLineEdit();

    QGridLayout *subGrid=new QGridLayout(workGroup);
    subGrid->addWidget(modelLB,0,0);
    subGrid->addWidget(modelLE,0,2);
    subGrid->addWidget(totalSpaceLB,1,0);
    subGrid->addWidget(totalSpaceLE,1,2);
    subGrid->addWidget(recordCountLB,2,0);
    subGrid->addWidget(recordCountLE,2,2);
    subGrid->addWidget(workStatusLB,3,0);
    subGrid->addWidget(workStatusLE,3,2);
    subGrid->addWidget(startTimeLB,4,0);
    subGrid->addWidget(startTimeLE,4,2);
    subGrid->addWidget(frontConnectTimeLB,5,0);
    subGrid->addWidget(frontConnectTimeLE,5,2);
    subGrid->addWidget(stopTimeLB,6,0);
    subGrid->addWidget(stopTimeLE,6,2);
    subGrid->addWidget(realStopTimeLB,7,0);
    subGrid->addWidget(realStopTimeLE,7,2);

    //group2
    recordIntervalLB=new QLabel(tr("Record Interval"));
    recordLengthLB=new QLabel(tr("Record Time Length"));
    startDelayTimeLB=new QLabel(tr("Delay Time"));
    stationNoLB=new QLabel(tr("Station No"));
    keyStopedLB=new QLabel(tr("Stop by button_press"));
    alarmSettingLB=new QLabel(tr("Alarm Setting"));
    toneSettingLB=new QLabel(tr("Tone Set"));
    tempUintLB=new QLabel(tr("Temperature Unit"));
    tempUpLB=new QLabel(tr("Temperature Upper Limit"));
    tempDownLB=new QLabel(tr("Temperature Lower Limit"));
    tempCorrectLB=new QLabel(tr("Temperature Calibration"));
    humidityUpLB=new QLabel(tr("Humidity Upper Limit"));
    humidityDownLB=new QLabel(tr("Humidity Lower Limit"));
    humidityCorrectLB=new QLabel(tr("Humidity Calibration"));
    timeFormatLB=new QLabel("H:M:S");
    delayFormatLB=new QLabel(QString("H"));
    tempFormat1LB=new QLabel(QString("°C"));
    tempFormat2LB=new QLabel(QString("°C"));
    tempFormat3LB=new QLabel(QString("°C"));
    humidityFormat1LB=new QLabel(QString("%RH"));
    humidityFormat2LB=new QLabel(QString("%RH"));
    humidityFormat3LB=new QLabel(QString("%RH"));

    recordIntervalTE=new QTimeEdit();
    recordIntervalTE->setDisplayFormat("hh:mm:ss");
    recordIntervalTE->setWrapping(true);
    recordIntervalTE->setMinimumTime(QTime(0,0,10));
    recordIntervalTE->setTime(QTime(0,0,10));
    recordLengthLE=new QLineEdit();
    startDelayTimeDSB=new QDoubleSpinBox();
//    startDelayTimeDSB->setKeyboardTracking(true);
    startDelayTimeDSB->setDecimals(1);
    startDelayTimeDSB->setSingleStep(0.5);
    startDelayTimeDSB->setRange(0.0,6.0);
    stationNoLE=new QLineEdit();
    stationNoLE->setMaxLength(3);
    QRegExp regExp("[0-9]{3}");
    stationNoLE->setValidator(new QRegExpValidator(regExp, this));
    stationNoLE->setToolTip("0-255");
    keyStopedCB=new QComboBox();    
    keyStopedCB->addItem(tr("Prohibit"));
    keyStopedCB->addItem(tr("Permit"));
    toneSettingCB=new QComboBox();
    toneSettingCB->addItem(tr("Prohibit"));
    toneSettingCB->addItem(tr("Permit"));
    alarmSettingCB=new QComboBox();
    alarmSettingCB->addItem(tr("Prohibit"));
    alarmSettingCB->addItem(tr("3Times"));
    alarmSettingCB->addItem(tr("10Times"));
    tempUintCB=new QComboBox();
    tempUintCB->addItem(QString("°C"));
    tempUintCB->addItem(QString("°F"));
    tempUpLE=new QLineEdit();
    tempUpLE->setMaxLength(5);
    regExp.setPattern("[-+]?[0-9]{1,3}[.][0-9]");
    tempUpLE->setValidator(new QRegExpValidator(regExp, this));
    tempDownLE=new QLineEdit();
    tempDownLE->setMaxLength(5);
    tempDownLE->setValidator(new QRegExpValidator(regExp, this));
    tempCorrectDSB=new QDoubleSpinBox();
    tempCorrectDSB->setSingleStep(0.1);
    tempCorrectDSB->setDecimals(1);
    tempCorrectDSB->setRange(-5.0,5.0);
    humidityUpLE=new QLineEdit();
    humidityUpLE->setMaxLength(4);
    regExp.setPattern("[0-9]{1,2}[.][0-9]");
    humidityUpLE->setValidator(new QRegExpValidator(regExp, this));
    humidityDownLE=new QLineEdit();
    humidityDownLE->setMaxLength(4);
    humidityDownLE->setValidator(new QRegExpValidator(regExp, this));
    humidityCorrectDSB=new QDoubleSpinBox();
    humidityCorrectDSB->setSingleStep(0.1);
    humidityCorrectDSB->setRange(-5.0,5.0);
    humidityCorrectDSB->setDecimals(1);

    QGridLayout *subGrid1=new QGridLayout(propertyGroup);
    subGrid1->addWidget(recordIntervalLB,0,0);
    subGrid1->addWidget(recordIntervalTE,0,1);
    subGrid1->addWidget(timeFormatLB,0,2);
    subGrid1->addWidget(recordLengthLB,1,0);
    subGrid1->addWidget(recordLengthLE,1,1);
    subGrid1->addWidget(startDelayTimeLB,2,0);
    subGrid1->addWidget(startDelayTimeDSB,2,1);
    subGrid1->addWidget(delayFormatLB,2,2);
    subGrid1->addWidget(stationNoLB,3,0);
    subGrid1->addWidget(stationNoLE,3,1);
    subGrid1->addWidget(keyStopedLB,4,0);
    subGrid1->addWidget(keyStopedCB,4,1);
    subGrid1->addWidget(alarmSettingLB,5,0);
    subGrid1->addWidget(alarmSettingCB,5,1);
    subGrid1->addWidget(toneSettingLB,6,0);
    subGrid1->addWidget(toneSettingCB,6,1);
    subGrid1->addWidget(tempUintLB,7,0);
    subGrid1->addWidget(tempUintCB,7,1);
    subGrid1->addWidget(tempUpLB,8,0);
    subGrid1->addWidget(tempUpLE,8,1);
    subGrid1->addWidget(tempFormat1LB,8,2);
    subGrid1->addWidget(tempDownLB,9,0);
    subGrid1->addWidget(tempDownLE,9,1);
    subGrid1->addWidget(tempFormat2LB,9,2);
    subGrid1->addWidget(tempCorrectLB,10,0);
    subGrid1->addWidget(tempCorrectDSB,10,1);
    subGrid1->addWidget(tempFormat3LB,10,2);
    subGrid1->addWidget(humidityUpLB,11,0);
    subGrid1->addWidget(humidityUpLE,11,1);
    subGrid1->addWidget(humidityFormat1LB,11,2);
    subGrid1->addWidget(humidityDownLB,12,0);
    subGrid1->addWidget(humidityDownLE,12,1);
    subGrid1->addWidget(humidityFormat2LB,12,2);
    subGrid1->addWidget(humidityCorrectLB,13,0);
    subGrid1->addWidget(humidityCorrectDSB,13,1);
    subGrid1->addWidget(humidityFormat3LB,13,2);

    //group3
    clockLB=new QLineEdit();

    setClockBtn=new QPushButton(tr("Set Clock Of Data Logger"));
    QVBoxLayout *subVLayout=new QVBoxLayout(clockGroup);
    subVLayout->addWidget(clockLB);
    subVLayout->addWidget(setClockBtn);

    //group4
    noTE=new QLineEdit();
//    noTE->setMaxLength(10);
    regExp.setPattern("[A-Za-z0-9]{1,10}");
    noTE->setValidator(new QRegExpValidator(regExp, this));

    noLB=new QLabel(tr("Maximum input characters or numbers 10"));
    setNoBtn=new QPushButton(tr("Set Numbers"));
    QVBoxLayout *subVLayout1=new QVBoxLayout(noGroup);
    subVLayout1->addWidget(noTE);
    subVLayout1->addWidget(noLB);
    subVLayout1->addWidget(setNoBtn);

    //group5
    infoTE=new QTextEdit();
//    regExp.setPattern(QString("{10}"));
//    infoTE->setValidator(new QRegExpValidator(regExp, this));
    infoLB=new QLabel(tr("Maximum Input Characters 100"));
    setInfoBtn=new QPushButton(tr("Set User Information"));
    QVBoxLayout *subVLayout2=new QVBoxLayout(userGroup);
    subVLayout2->addWidget(infoTE);
    subVLayout2->addWidget(infoLB);
    subVLayout2->addWidget(setInfoBtn);

    //布局
    QHBoxLayout *hLayout=new QHBoxLayout();
    hLayout->addWidget(saveBtn);
    hLayout->addSpacing(10);
    hLayout->addWidget(quitBtn);

    QVBoxLayout *vLayout=new QVBoxLayout();
    vLayout->addWidget(clockGroup);
    vLayout->addWidget(noGroup);
    vLayout->addWidget(userGroup);
    vLayout->addWidget(tipLB,1,Qt::AlignCenter);
    vLayout->addStretch(1);
    vLayout->addLayout(hLayout);

    QGridLayout *grid=new QGridLayout(this);
    grid->setColumnStretch(0,1);
    grid->setColumnStretch(1,1);
    grid->setColumnStretch(2,1);
    grid->addWidget(workGroup,0,0);
    grid->addWidget(propertyGroup,0,1);
    grid->addLayout(vLayout,0,2);
    grid->setContentsMargins(200,50,200,50);

    timer=new QTimer(this);
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_out()));
    timer->start();

    //信号/槽
    connect(parent,SIGNAL(connection()),this,SLOT(connection()));
    connect(parent,SIGNAL(connection()),this,SLOT(updateLabelWithDate()));
    connect(quitBtn,SIGNAL(clicked()),this,SLOT(hide()));
    connect(parent,SIGNAL(clickParamSetting()),this,SLOT(setting()));
    connect(parent,SIGNAL(clickParamSetting()),this,SLOT(updateLabelWithDate()));
    connect(tempUintCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboIndexChange(QString)));
    connect(saveBtn,SIGNAL(clicked()),this,SLOT(compeleteSave()));
    connect(setClockBtn,SIGNAL(clicked()),this,SLOT(setRecorderClock()));
    connect(setNoBtn,SIGNAL(clicked()),this,SLOT(setNumber()));
    connect(setInfoBtn,SIGNAL(clicked()),this,SLOT(setUserInfo()));
    connect(recordIntervalTE,SIGNAL(timeChanged(QTime)),this,SLOT(slotTimeChanged(QTime)));

    //验证输入正确性
    connect(tempUpLE,SIGNAL(editingFinished()),this,SLOT(correctCheck1()));
    connect(tempDownLE,SIGNAL(editingFinished()),this,SLOT(correctCheck2()));
    connect(humidityUpLE,SIGNAL(editingFinished()),this,SLOT(correctCheck3()));
    connect(humidityDownLE,SIGNAL(editingFinished()),this,SLOT(correctCheck4()));
    connect(infoTE,SIGNAL(textChanged()),this,SLOT(correctCheck5()));
}

QByteArray SettingWidget::GetByteFromQString(QString str)
{
    QByteArray byte;
    uchar sz=0x00;
    if(str.isEmpty()||str.isNull())
    {
        byte.append(sz);
        byte.append(sz);
        byte.append(sz);
        return byte;
    }

    int index=str.indexOf(".");
    int size=str.length();
    if(index==-1)
    {
        if(1==size)
        {
            byte.append(sz);
            byte.append(str.toUInt());
            byte.append(sz);
            return byte;
        }
        else if(2==size)
        {
            byte.append(str.left(1).toUInt());
            byte.append(str.right(1).toUInt());
            byte.append(sz);
            return byte;
        }
        else if(3==size)
        {
            byte.append(str.left(1).toUInt());
            byte.append(str.mid(1,1).toUInt());
            byte.append(str.right(1).toUInt());
            return byte;
        }
        else
        {
            byte.append(sz);
            byte.append(sz);
            byte.append(sz);
            return byte;
        }
    }
    else if(1==index)
    {
        if(2==size)
        {
            byte.append(sz);
            byte.append(str.left(1).toUInt());
            byte.append(sz);
            return byte;
        }
        else if(3==size)
        {
            byte.append(sz);
            byte.append(str.left(1).toUInt());
            byte.append(str.right(1).toUInt());
            return byte;
        }
        else
        {
            byte.append(sz);
            byte.append(sz);
            byte.append(sz);
            return byte;
        }
    }
    else if(index==2)
    {
        if(size==3)
        {
            byte.append(str.left(1).toUInt());
            byte.append(str.mid(1,1).toUInt());
            byte.append(sz);
            return byte;
        }
        else if(size==4)
        {
            byte.append(str.left(1).toUInt());
            byte.append(str.mid(1,1).toUInt());
            byte.append(str.right(1).toUInt());
            return byte;
        }
        else
        {
            byte.append(sz);
            byte.append(sz);
            byte.append(sz);
            return byte;
        }
    }
    else
    {
        byte.append(sz);
        byte.append(sz);
        byte.append(sz);
        return byte;
    }

    byte.append(sz);
    byte.append(sz);
    byte.append(sz);
    return byte;
}

void SettingWidget::connection()
{
    this->setEnabled(true);
   workGroup->setEnabled(false);
   propertyGroup->setEnabled(false);
   clockGroup->setEnabled(false);
   noGroup->setEnabled(false);
   userGroup->setEnabled(false);
   quitBtn->setEnabled(true);
   saveBtn->setVisible(false);
   saveBtn->setEnabled(false);
   tipLB->setVisible(false);
}

void SettingWidget::setting()
{
    this->setEnabled(true);
    workGroup->setEnabled(false);
    propertyGroup->setEnabled(true);
    clockGroup->setEnabled(true);
    setClockBtn->setEnabled(true);
    clockLB->setEnabled(false);
    noGroup->setEnabled(true);
    userGroup->setEnabled(true);
    saveBtn->setVisible(true);
    saveBtn->setEnabled(true);
    tipLB->setVisible(true);
    recordLengthLE->setEnabled(false);
}

void SettingWidget::comboIndexChange(QString str)
{
    tempFormat1LB->setText(str);
    tempFormat2LB->setText(str);
    tempFormat3LB->setText(str);

    if(str=="°C")
    {
        double db1=(tempUpLE->text().toDouble()-32.0)*5.0/9.0;
        tempUpLE->setText(QString::number(db1,'f',1));
        db1=(tempDownLE->text().toDouble()-32.0)*5.0/9.0;
        tempDownLE->setText(QString::number(db1,'f',1));
    }
    else
    {
        double db1=tempUpLE->text().toDouble()*9.0/5.0+32.0;
        tempUpLE->setText(QString::number(db1,'f',1));
        db1=tempDownLE->text().toDouble()*9.0/5.0+32.0;
        tempDownLE->setText(QString::number(db1,'f',1));
    }

    tempCorrectDSB->setValue(0.0);
}

void SettingWidget::setRecorderClock()
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
    emit clkSG(dateBA);
}

void SettingWidget::setNumber()
{
    QByteArray temp;
    temp.append(0x1B);
    temp.append(0x02);
    temp.append(0x03);

    QString str=noTE->text();
    QByteArray array=str.toAscii();
    int size=array.size();
    size=10-size;
    for(int i=0;i<size;i++)
    {
        temp.append(0x20);
    }
    temp.append(array);
    temp.append(accumulationAnd(temp));
    temp.append(0x0D);
    emit numberSG(temp);
}

void SettingWidget::setUserInfo()
{
    QByteArray temp;
    temp.append(0x1B);
    temp.append(0x02);
    temp.append(0x02);

    QByteArray array= infoTE->toPlainText().toAscii();
    int size=array.size();
    temp.append(array);
    size=100-size;
    for(int i=0;i<size;i++)
    {
        temp.append(0x20);
    }
    temp.append(accumulationAnd(temp));
    temp.append(0x0D);
    emit numberSG(temp);
}

void SettingWidget::compeleteSave()
{
    if(tempUpLE->text().toDouble()<tempDownLE->text().toDouble())
    {
        QMessageBox::information(this,tr("tips"),tr("the maximum temp cann't be less than the minimum temp"));
        return;
    }
    if(humidityUpLE->text().toDouble()<humidityDownLE->text().toDouble())
    {
        QMessageBox::information(this,tr("tips"),tr("the maximum humidity cann't be less than the minimum humidity"));
        return;
    }

    uchar sz=0x00;
    uchar one=0x01;

    QByteArray propertyBA;
    propertyBA.append(0x1B);
    propertyBA.append(0x02);
    propertyBA.append(0x06);

    QString str=recordIntervalTE->text();
    if(8==str.length())
    {
        propertyBA.append(str.left(1).toUInt());
        propertyBA.append(str.mid(1,1).toUInt());
        propertyBA.append(str.mid(3,1).toUInt());
        propertyBA.append(str.mid(4,1).toUInt());
        propertyBA.append(str.mid(6,1).toUInt());
        propertyBA.append(str.right(1).toUInt());
    }
    else
    {
        propertyBA.append(sz);
        propertyBA.append(sz);
        propertyBA.append(sz);
        propertyBA.append(sz);
        propertyBA.append(sz);
        propertyBA.append(sz);
    }

    str=startDelayTimeDSB->text();
    if(3==str.length())
    {
        propertyBA.append(str.left(1).toUInt());
        propertyBA.append(str.right(1).toUInt());
    }
    else
    {
        propertyBA.append(sz);
        propertyBA.append(sz);
    }

    str=stationNoLE->text();
    if(str.length()==1)
    {
        propertyBA.append(sz);
        propertyBA.append(sz);
        propertyBA.append(str.left(1).toUInt());
    }
    else if(2==str.length())
    {
        propertyBA.append(sz);
        propertyBA.append(str.left(1).toUInt());
        propertyBA.append(str.right(1).toUInt());
    }
    else if(3==str.length())
    {
        propertyBA.append(str.left(1).toUInt());
        propertyBA.append(str.mid(1,1).toUInt());
        propertyBA.append(str.right(1).toUInt());
    }

    propertyBA.append(keyStopedCB->currentIndex());
//    propertyBA.append(alarmSettingCB->currentIndex());
    switch(alarmSettingCB->currentIndex())
    {
    case 1:
        propertyBA.append(0x03);
        break;
    case 2:
        propertyBA.append(0x0A);
        break;
    default:
        propertyBA.append(sz);
        break;
    }

    propertyBA.append(toneSettingCB->currentIndex());
//    propertyBA.append(tempUintCB->currentIndex());
    propertyBA.append(sz);

    if(tempUintCB->currentIndex()==0)
    {
        str=tempUpLE->text();
    }
    else
    {
        double db1=(tempUpLE->text().toDouble()-32.0)*5.0/9.0;
        str=QString::number(db1,'f',1);
    }
    if(str.toDouble()>=0)
    {
        propertyBA.append(sz);
        propertyBA.append(sz);
        QByteArray byte=GetByteFromQString(str);
        propertyBA.append(byte);
    }
    else
    {
        propertyBA.append(one);
        propertyBA.append(sz);
        QByteArray byte=GetByteFromQString(str.right(str.length()-1));
        propertyBA.append(byte);
    }

    if(tempUintCB->currentIndex()==0)
    {
        str=tempDownLE->text();
    }
    else
    {
        double db1=(tempDownLE->text().toDouble()-32.0)*5.0/9.0;
        str=QString::number(db1,'f',1);
    }
    if(str.toDouble()>=0)
    {
        propertyBA.append(sz);
        propertyBA.append(sz);
        QByteArray byte=GetByteFromQString(str);
        propertyBA.append(byte);
    }
    else
    {
        propertyBA.append(one);
        propertyBA.append(sz);
        QByteArray byte=GetByteFromQString(str.right(str.length()-1));
        propertyBA.append(byte);
    }

    str=tempCorrectDSB->text();
    if(str.toDouble()>=0)
    {
        propertyBA.append(sz);
        propertyBA.append(sz);
        propertyBA.append(str.left(1).toUInt());
        propertyBA.append(str.right(1).toUInt());
    }
    else
    {
        propertyBA.append(one);
        propertyBA.append(sz);
        propertyBA.append(str.mid(1,1).toUInt());
        propertyBA.append(str.right(1).toUInt());
    }

    str=humidityUpLE->text();
    propertyBA.append(sz);
    QByteArray byte1=GetByteFromQString(str);
    propertyBA.append(byte1);

    str=humidityDownLE->text();
    propertyBA.append(sz);
    QByteArray byte2=GetByteFromQString(str);
    propertyBA.append(byte2);

    str=humidityCorrectDSB->text();
    if(str.toDouble()>=0)
    {
        propertyBA.append(sz);
        propertyBA.append(sz);
        propertyBA.append(str.left(1).toUInt());
        propertyBA.append(str.right(1).toUInt());
    }
    else
    {
        propertyBA.append(one);
        propertyBA.append(sz);
        propertyBA.append(str.mid(1,1).toUInt());
        propertyBA.append(str.right(1).toUInt());
    }

    propertyBA.append(accumulationAnd(propertyBA));
    propertyBA.append(0x0D);

    if(QMessageBox::Ok==QMessageBox::information(this,tr("tips"),tr("this will clear data of the _recoder,is running?"),
                                                 QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel))
    {
        emit dataStoreSG(propertyBA);
    }
}

void SettingWidget::showParamFromDown()
{
    dateTime=_recoder.clock;

    clockLB->setText(dateTime.toString(_displayFormat));

    modelLE->setText(_recoder.model);
    totalSpaceLE->setText(QString::number(_recoder.totalSpace));
    recordCountLE->setText(QString::number(_recoder.recordTimes));

    workStatusLE->setText(_recoder.workStatus);
    startTimeLE->setText(_recoder.startTime.toString(_displayFormat));
    frontConnectTimeLE->setText(_recoder.frontTestTime.toString(_displayFormat));
    stopTimeLE->setText(_recoder.estimateStopTime.toString(_displayFormat));
    realStopTimeLE->setText(_recoder.realStopTime.toString(_displayFormat));

    QTime time=QTime::fromString(_recoder.recordInterval,"hh:mm:ss");
    uint totalTime=time.hour()*3600+time.minute()*60+ time.second();

    if(totalTime<10)
    {
        recordIntervalTE->setTime(QTime(0,0,10));
    }
    else
    {
        recordIntervalTE->setTime(time);
    }
    totalTime*=16000;                     //转换成秒
    int days=totalTime/(24*3600);
    totalTime-=days*24*3600;
    int hours=totalTime/3600;
    totalTime-=hours*3600;
    int minutes=totalTime/60;
    recordLengthLE->setText(QString("%1%2 %3%4 %5%6")
                            .arg(days).arg(tr("D"))
                            .arg(hours).arg(tr("H"))
                            .arg(minutes).arg(tr("M")));

    startDelayTimeDSB->setValue(_recoder.startDelayTime);
    stationNoLE->setText(QString::number(_recoder.recordNo));
    keyStopedCB->setCurrentIndex(_recoder.keyStop);
    switch(_recoder.alarmSetting)
    {
    case 0x03:
        alarmSettingCB->setCurrentIndex(1);
        break;
    case 0x0A:
        alarmSettingCB->setCurrentIndex(2);
        break;
    default:
        alarmSettingCB->setCurrentIndex(0);
        break;
    }
    toneSettingCB->setCurrentIndex(_recoder.toneSetting);
    tempUintCB->setCurrentIndex(_recoder.tempUint);
    tempUpLE->setText(QString::number(_recoder.tempUp));
    tempDownLE->setText(QString::number(_recoder.tempDown));
    tempCorrectDSB->setValue(_recoder.tempCorrect);
    humidityUpLE->setText(QString::number(_recoder.humidityUp));
    humidityDownLE->setText(QString::number(_recoder.humidityDown));
    humidityCorrectDSB->setValue(_recoder.humidityCorrect);

    clockLB->setText(_recoder.clock.toString(_displayFormat));
    noTE->setText(_recoder.bianHao);
    infoTE->setText(_recoder.userInfo);

    if(_deviceMode==TEMPERATUREONLY)
    {
        humidityUpLE->setEnabled(false);
        humidityDownLE->setEnabled(false);
        humidityCorrectDSB->setEnabled(false);
    }
    else
    {
        humidityUpLE->setEnabled(true);
        humidityDownLE->setEnabled(true);
        humidityCorrectDSB->setEnabled(true);
    }

    qDebug()<<"settingWidget get params successful";
}

void SettingWidget::timer_out()
{
    clockLB->setText(QDateTime::currentDateTime().toString(_displayFormat));
}


void SettingWidget::updateLabelWithDate()
{
    clockLB->setText(dateTime.toString(_displayFormat));
    startTimeLE->setText(_recoder.startTime.toString(_displayFormat));
    frontConnectTimeLE->setText(_recoder.frontTestTime.toString(_displayFormat));
    realStopTimeLE->setText(_recoder.realStopTime.toString(_displayFormat));
    stopTimeLE->setText(_recoder.estimateStopTime.toString(_displayFormat));
}

void SettingWidget::correctCheck1()
{
    double temp=0.0;
    temp=tempUpLE->text().toDouble();
    if(0==tempUintCB->currentIndex())
    {
        if(temp>60.0||temp<-30.0)
        {
            QMessageBox::information(this,tr("tips"),tr("correct value is between -30 and 60"));
            tempUpLE->setText("60.0");
            return;
        }
    }
    else
    {
        if(temp>140.0||temp<-22.0)
        {
            QMessageBox::information(this,tr("tips"),tr("correct value is between -22 and 140"));
            tempUpLE->setText("140.0");
            return;
        }
    }
}

void SettingWidget::correctCheck2()
{
    double temp=tempDownLE->text().toDouble();
    if(0==tempUintCB->currentIndex())
    {
        if(temp>60.0||temp<-30.0)
        {
            QMessageBox::information(this,tr("tips"),tr("correct value is between -30 and 60"));
            tempDownLE->setText("-30.0");
            return;
        }
    }
    else
    {
        if(temp>140.0||temp<-22.0)
        {
            QMessageBox::information(this,tr("tips"),tr("correct value is between -22 and 140"));
            tempDownLE->setText("-22.0");
            return;
        }
    }
}

void SettingWidget::correctCheck3()
{
    double temp=humidityUpLE->text().toDouble();
    if(temp>90.0||temp<20.0)
    {
        QMessageBox::information(this,tr("tips"),tr("correct value is between 20 and 90"));
        humidityUpLE->setText("90.0");
        return;
    }
}

void SettingWidget::correctCheck4()
{
    double temp=humidityDownLE->text().toDouble();
    if(temp>90.0||temp<20.0)
    {
        QMessageBox::information(this,tr("tips"),tr("correct value is between 20 and 90"));
        humidityDownLE->setText("20.0");
        return;
    }
}

void SettingWidget::correctCheck5()
{
    QString text=infoTE->toPlainText();
    QRegExp regExp(".{0,100}");
    bool bret=regExp.exactMatch(text);
    if(!bret)
    {
        QMessageBox::information(this,tr("tips"),tr("User Information input error"));
        infoTE->clear();
        return;
    }
}

void SettingWidget::updateClock(QDateTime date)
{
//    if(timer!=NULL)
//    {
//        delete timer;
//        timer=NULL;
//    }
//    timer->stop();
//    timeMutex.lock();
    dateTime=date;
//    qDebug()<<"clock set"<<dateTime.toString(DEFAULT_TIME_FORMAT);
//    timeMutex.unlock();
//    testTimer=new QTimer(this);
//    testTimer->setInterval(1000);
//    connect(testTimer,SIGNAL(timeout()),this,SLOT(timer_out1()));
    //    testTimer->start();
}

void SettingWidget::slotTimeChanged(QTime time)
{
    uint totalTime=time.hour()*3600+time.minute()*60+ time.second();
    totalTime*=16000;                     //转换成秒
    int days=totalTime/(24*3600);
    totalTime-=days*24*3600;
    int hours=totalTime/3600;
    totalTime-=hours*3600;
    int minutes=totalTime/60;
    recordLengthLE->setText(QString("%1%2 %3%4 %5%6")
                            .arg(days).arg(tr("D"))
                            .arg(hours).arg(tr("H"))
                            .arg(minutes).arg(tr("M")));
}
