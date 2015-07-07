/***********************************************************************
 *查找记录仪所占的串口号
 *
 *
 *
 *
 **********************************************************************/

#ifndef COMTHREAD_H
#define COMTHREAD_H

#include <QThread>
#include <QStringList>

class QextSerialPort;

class ComThread : public QThread
{
    Q_OBJECT
public:
    explicit ComThread(QObject *parent = 0);
    ~ComThread();

signals:
    void findCom(QString info,QString model);                       //查找到记录仪串口号
    
public slots:
    void getComInfo(QStringList _list);                             //获得可用串口号

protected:
    void run();

private:
    QextSerialPort *myCom;

    QStringList comList;                                            //串口号存放数组
    bool flag;                                                      //发现标志
};

#endif // COMTHREAD_H
