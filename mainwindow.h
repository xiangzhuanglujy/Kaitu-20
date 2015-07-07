#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>

#include "check.h"

class QStackedWidget;
class UploadWidget;
class SettingWidget;
class QextSerialPort;
class MyThread;
class QLabel;
class QProgressBar;
class QToolBar;
class ComThread;
class FindDevice;


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initDateFormat();                              //初始化日期格式
    void initControl();                                 //初始化控件
    void initThread();                                  //初始化线程
    void initToolBar();                                 //初始化工具栏
    void initMenuBar();                                 //初始化菜单栏
    void initSerialPort();                              //初始化串口
    QString GeneratePicPdf();                           //生成pdfhtml
//    QString printPdf();                                 //生成打印
    void queryCom();                                    //查询可用端口
    QString getcomm(int index,QString keyorvalue);      //搜索注册表
    bool testDriverMove(QString device);                //检测设备拔掉
    void errorMessage(const QString &message);          //消息提示框
    void txt_event(QString path);                       //保存为txt
    void word_event(QString path);                      //保存为word
    void excel_event(QString path);                     //保存为excel
    void pdf_event(QString path);                       //保存为pdf
    void recv_userInfo();                               //分离用户信息
    void recv_bianhao();                                //分离编号
    void recv_shijian();                                //分离时间
    void recv_workStatus();                             //分离工作状态
    void recv_property();                               //分离属性
    void setGuiEnabled(bool);                           //界面变灰
    void sendQByteArray(QByteArray array);              //串口发送数据
    void translateLanguage();                           //更新语言

    enum Language                                       //语言枚举
    {
        ENGLISH,
        CHINESE
    };
    int lngType;                                        //语言类型 0-Eng 1-Cha

protected:
    bool winEvent(MSG *msg, long *result);                                      //插拔事件
    void closeEvent(QCloseEvent *);                                             //关闭事件

signals:
    void connection();                                                          //点击链接界面可以用
    void clickParamSetting();                                                   //点击参数设置
    void dataToThread(uchar* ch,int len,QDateTime date,QString interval);       //发送给线程的参数
    void dataToSettingWidget();                                                 //发送到设置界面的参数
    void comInfoToThread(QStringList list);                                     //发送串口号到线程
    void findDevice();                                                          //发现设备
    void valueToProgressBar(int);                                               //更新滑动条
    void setProgressBarMaxunum(int);                                            //设置滑动条最大值
    void setQStringVisible(bool);                                               //进度条信息不显示
    void setCurrentDateTime(QDateTime);                                         //设置时钟时间
    void deleteFromDB(QString);                                                 //从数据库删除

private slots:
    void showHelp();                                    //显示帮助槽
    void setSystemMail();                               //系统邮件设置槽
    void connectSlot();                                 //连接槽
    void uploadSlot();                                  //上传数据槽
    void paramSetSlot();                                //参数设置槽
    void queryDataSlot();                               //查询数据槽
    void storeDataSlot();                               //存储数据槽
    void exportExcelSlot();                             //导出excel
    void exportPdfSlot();                               //导出pdf
    void exportWordSlot();                              //导出word
    void exportTxtSlot();                               //导出txt
    void printSlot();                                   //打印
    void deleteDataSlot();                              //删除数据
    void sendMailSlot();                                //发邮件
    void stopRecordSlot();                              //停止记录仪
    void aboutSlot();                                   //关于
    void readCom();                                     //串口接收事件
    void sendInfo(QByteArray array);                    //发送编号
    void backUpDB();                                    //备份数据库
    void reductionDB();                                 //还原数据库
    void quitAct_clicked();                             //退出按钮
    void openCom(QString str,QString _model);           //打开串口
    void plotPic(QPrinter *);                           //打印数据
    void enableGuiWindow();                             //窗口恢复
    void dbHasData(QString);                            //数据库是否有数据
    void hasFinished(bool);                             //数据库数据加载完

private:
    QextSerialPort *myCom;                              //串口变量
    MyThread *thread;                                   //保存线程
    ComThread *comThread;                               //串口查找线程
    FindDevice *pDevice;                                //进度条框
    QToolBar *toolBar;                                  //工具栏
    QAction *connectAct;                                //连接
    QAction *uploadDataAct;                             //上传
    QAction *paramSetAct;                               //参数设置
    QAction *mailsetAct;                                //邮件设置
    QAction *queryDataAct;                              //查询数据
    QAction *storeDataAct;                              //保存数据
    QAction *exportExcelAct;                            //导出Excel
    QAction *exportPdfAct;                              //导出Pdf
    QAction *exportWordAct;                             //导出Word
    QAction *exportTxtAct;                              //导出Txt
    QAction *printAct;                                  //打印
    QAction *deleteDataAct;                             //删除数据
    QAction *sendMailAct;                               //发送邮件
    QAction *stopRecordAct;                             //存储数据
    QAction *helpAct;                                   //帮助
    QAction *aboutAct;                                  //关于
    QAction *quitAct;                                   //退出
    QAction *backUpAct;                                 //备份数据库
    QAction *reductionAct;                              //还原数据库
    QMenu *fileMenu;                                    //文件菜单
    QMenu *exportMenu;                                  //导出菜单
    QMenu *toolMenu;                                    //工具菜单
    QMenu *settingMenu;                                 //设置菜单
    QMenu *helpMenu;                                    //帮助菜单
    QStackedWidget *stack;                              //stack控件
    SettingWidget *settingWidget;                       //设置窗口
    UploadWidget *uploadWidget;                         //上传窗口
    QStringList comVector;                              //可用串口列表
    QString portName;                                   //串口名称
    uint startTickTime;                                 //发送计时
//    bool isConnected;                                   //是否进行同步
};

#endif // MAINWINDOW_H
