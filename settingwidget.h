//参数设置

#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

#include "check.h"

class QPushButton;
class QGroupBox;
class QLabel;
class QLineEdit;
class QTimeEdit;
class QComboBox;
class QDoubleSpinBox;
class QTextEdit;
class QTimer;

class SettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingWidget(QWidget *parent = 0);

    QByteArray GetByteFromQString(QString str);
    
signals:
    void clkSG(QByteArray array);
    void numberSG(QByteArray array);
    void userInfoSG(QByteArray array);
    void dataStoreSG(QByteArray array);
    
public slots:
    void connection();
    void setting();
    void comboIndexChange(QString str);

    void setRecorderClock();                            //设置时钟同步
    void setNumber();                                   //设置编号
    void setUserInfo();                                 //设置用户信息
    void compeleteSave();                               //保存参数
    void showParamFromDown();                           //显示下位机回传的参数
    void timer_out();
    void updateLabelWithDate();                         //更新带有时间的标签
    void correctCheck1();                               //验证输入正确性
    void correctCheck2();                               //验证输入正确性
    void correctCheck3();                               //验证输入正确性
    void correctCheck4();                               //验证输入正确性
    void correctCheck5();                               //验证输入正确性
    void updateClock(QDateTime);
    void slotTimeChanged(QTime);

private:
    QGroupBox *workGroup;                               //工作状态
    QGroupBox *propertyGroup;                           //记录仪属性
    QGroupBox *clockGroup;                              //记录仪时钟
    QGroupBox *noGroup;                                 //编号
    QGroupBox *userGroup;                               //用户信息

    //group1
    QLabel *modelLB;
    QLabel *totalSpaceLB;
    QLabel *recordCountLB;
    QLabel *workStatusLB;
    QLabel *startTimeLB;
    QLabel *frontConnectTimeLB;
    QLabel *stopTimeLB;
    QLabel *realStopTimeLB;

    QLineEdit *modelLE;
    QLineEdit *totalSpaceLE;
    QLineEdit *recordCountLE;
    QLineEdit *workStatusLE;
    QLineEdit *startTimeLE;
    QLineEdit *frontConnectTimeLE;
    QLineEdit *stopTimeLE;
    QLineEdit *realStopTimeLE;

    //group2
    QLabel *recordIntervalLB;
    QLabel *recordLengthLB;
    QLabel *startDelayTimeLB;
    QLabel *stationNoLB;
    QLabel *keyStopedLB;
    QLabel *alarmSettingLB;
    QLabel *toneSettingLB;
    QLabel *tempUintLB;
    QLabel *tempUpLB;
    QLabel *tempDownLB;
    QLabel *tempCorrectLB;
    QLabel *humidityUpLB;
    QLabel *humidityDownLB;
    QLabel *humidityCorrectLB;
    QLabel *timeFormatLB;
    QLabel *delayFormatLB;
    QLabel *tempFormat1LB;
    QLabel *tempFormat2LB;
    QLabel *tempFormat3LB;
    QLabel *humidityFormat1LB;
    QLabel *humidityFormat2LB;
    QLabel *humidityFormat3LB;

    QTimeEdit *recordIntervalTE;
    QLineEdit *recordLengthLE;
    QDoubleSpinBox *startDelayTimeDSB;
    QLineEdit *stationNoLE;
    QComboBox *keyStopedCB;
    QComboBox *alarmSettingCB;
    QComboBox *toneSettingCB;
    QComboBox *tempUintCB;
    QLineEdit *tempUpLE;
    QLineEdit *tempDownLE;
    QDoubleSpinBox *tempCorrectDSB;
    QLineEdit *humidityUpLE;
    QLineEdit *humidityDownLE;
    QDoubleSpinBox *humidityCorrectDSB;

    //group3
    QLineEdit *clockLB;
    QPushButton *setClockBtn;

    //group4
    QLineEdit *noTE;
    QLabel *noLB;
    QPushButton *setNoBtn;

    //group5
    QTextEdit *infoTE;
    QLabel *infoLB;
    QPushButton *setInfoBtn;

    QLabel *tipLB;

    QPushButton *saveBtn;
    QPushButton *quitBtn;

    QTimer *timer;
    QDateTime dateTime;
};

#endif // SETTINGWIDGET_H
