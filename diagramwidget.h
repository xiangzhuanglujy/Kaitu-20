//曲线图

#ifndef DIAGRAMWIDGET_H
#define DIAGRAMWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QAction>

#include "check.h"
#include "qcustomplot.h"

class QTableWidget;
class QRadioButton;

class DiagramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DiagramWidget(QWidget *parent = 0);
    void custom_plot(double *addr1,double *addr2,int size,QDateTime date,int interval);       //绘图带保存
    void plotWithoutSave(double *addr1,double *addr2,int size,QDateTime date,int interval);              //绘图不带保存
    void updateLabelWithDate();

signals:
    void plot_over();                                               //绘制结束
    void load_over();
    
public slots:
    void reduction();                                               //还原
    void getMaxMinAvg();                                            //获得计算的值
    void getParams();                                               //临时过渡
    void mousemoved(QMouseEvent* ev);
    void mousePress(QMouseEvent* ev);
    void showAll();
    void showTemperature();
    void showHumidity();
    void tensileAct_trogged();
    void narrowAct_trogged();
    void smallAct_trogged();
    void bigAct_trogged();
    void sqldbPlot(QString);

private:
    QCustomPlot *customPlot;                                        //图形
    QTableWidget *tableWidget;                                      //记录表

    QRadioButton *radioRB1;                                         //温度+湿度
    QRadioButton *radioRB2;                                         //单温度
    QRadioButton *radioRB3;                                         //单湿度
//    QAction *tensileAct;                                            //横轴拉伸
//    QAction *narrowAct;                                             //横轴缩小
    QAction *bigAct;                                                //放大
    QAction *smallAct;                                              //缩小
    QAction *reductionAct;                                          //还原
};

#endif // DIAGRAMWIDGET_H
