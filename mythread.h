#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QDateTime>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);

signals:
    void saveCompletely();                                  //插入数据完成
    void plot(double*,double*,int,QDateTime,int);           //绘图参数计算完成
    
public slots:
    void getdata(uchar *data,int len,QDateTime _date,QString _interval);      //获得计算的基本参数

protected:
    void run();
    
private:
    int length_t;                           //采样个数
    uchar *value_t;                         //数组首地址
    QDateTime date_t;                       //开始时间
    QString interval_t;                     //采样间隔
};

#endif // MYTHREAD_H
