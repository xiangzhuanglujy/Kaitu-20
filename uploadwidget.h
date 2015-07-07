//上传数据
#ifndef UPLOADWIDGET_H
#define UPLOADWIDGET_H

#include <QWidget>

#include "check.h"

class QTabWidget;
class DataTableWidget;
class DiagramWidget;
class ReportWidget;

class UploadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UploadWidget(QWidget *parent = 0);
    void changeDateFormat();

signals:
//    void dataToSettingWidget(Recoder*);
    void maxMinAvg();
    void findDevice();

public slots:
    void readyPlot(double* addr1,double *addr2,int size,QDateTime date,int interval);
    
private:
    QTabWidget *tabWidget;

    DataTableWidget *dataTableWidget;
    DiagramWidget *diagramWidget;
    ReportWidget *report;
};

#endif // UPLOADWIDGET_H
