//数据表

#ifndef DATATABLEWIDGET_H
#define DATATABLEWIDGET_H

#include <QWidget>
#include <QDateTime>

#include "check.h"

class QTabWidget;
class GenerateWidget;
class ManageWidget;
class QTableWidget;

class DataTableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataTableWidget(QWidget *parent = 0);
    void changeDateFormat();
    bool isTemperatureInRange(double temperature);
    bool isHumidityInRange(double humidity);

signals:
    void plotFromSql(QString);
    void guiEnabled(bool);
    void deleteFromDB(QString);

public slots:
    void getParams();                                                       //临时过渡
    void getTempHumidity();                                                                 //温度湿度
    void resetTime();
    void showTable(double *addr1,double *addr2,int size,QDateTime date,int interval);         //显示数据
    void sortTable(QDateTime, QDateTime, QString, QString, QString, QString);               //按条件排序
    void loadData();

private:
    QTableWidget *tableWidget;
    QTabWidget *tabWidget;

    ManageWidget *managerWidget;                                                            //数据管理
    GenerateWidget *generateWidget;                                                         //常规项
};

#endif // DATATABLEWIDGET_H
