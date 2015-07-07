#include "uploadwidget.h"

#include <QTabWidget>
#include <QGridLayout>
#include <QDebug>

#include "datatablewidget.h"
#include "diagramwidget.h"
#include "reportwidget.h"


UploadWidget::UploadWidget(QWidget *parent) :
    QWidget(parent)
{
    tabWidget=new QTabWidget(this);
    dataTableWidget=new DataTableWidget(this);
    diagramWidget=new DiagramWidget(this);
    report=new ReportWidget(this);
    tabWidget->addTab(diagramWidget,tr("Curve Graph"));
    tabWidget->addTab(dataTableWidget,tr("Data Table"));
    tabWidget->addTab(report,tr("Report"));
    tabWidget->setCurrentWidget(diagramWidget);

    QGridLayout *grid=new QGridLayout(this);
    grid->addWidget(tabWidget);

    connect(dataTableWidget,SIGNAL(plotFromSql(QString)),diagramWidget,SLOT(sqldbPlot(QString)));
    connect(dataTableWidget,SIGNAL(plotFromSql(QString)),report,SLOT(getParams()));
    connect(dataTableWidget,SIGNAL(plotFromSql(QString)),report,SLOT(getMaxMinAvg()));
    connect(dataTableWidget,SIGNAL(plotFromSql(QString)),parent,SLOT(dbHasData(QString)));
    connect(dataTableWidget,SIGNAL(guiEnabled(bool)),parent,SLOT(hasFinished(bool)));
    connect(diagramWidget,SIGNAL(plot_over()),report,SLOT(label_pixmap()));
    connect(diagramWidget,SIGNAL(load_over()),dataTableWidget,SLOT(loadData()));
    connect(this,SIGNAL(maxMinAvg()),diagramWidget,SLOT(getMaxMinAvg()));
    connect(this,SIGNAL(maxMinAvg()),report,SLOT(getMaxMinAvg()));
    connect(this,SIGNAL(maxMinAvg()),dataTableWidget,SLOT(getTempHumidity()));
    connect(parent,SIGNAL(dataToSettingWidget()),diagramWidget,SLOT(getParams()));
    connect(parent,SIGNAL(dataToSettingWidget()),report,SLOT(getParams()));
    connect(parent,SIGNAL(dataToSettingWidget()),dataTableWidget,SLOT(getParams()));
    connect(parent,SIGNAL(dataToSettingWidget()),dataTableWidget,SLOT(resetTime()));
    connect(parent,SIGNAL(deleteFromDB(QString)),dataTableWidget,SIGNAL(deleteFromDB(QString)));
}

void UploadWidget::changeDateFormat()
{
    dataTableWidget->changeDateFormat();
    report->updateLabelWithDate();
    diagramWidget->updateLabelWithDate();
}


void UploadWidget::readyPlot(double *addr1, double *addr2, int size, QDateTime date, int interval)
{
    diagramWidget->custom_plot(addr1,addr2,size,date,interval);
    dataTableWidget->showTable(addr1,addr2,size,date,interval);
}
