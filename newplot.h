#ifndef NEWPLOT_H
#define NEWPLOT_H

#include <QWidget>
#include "qcustomplot.h"


class NewPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit NewPlot(QWidget *parent = 0);
    
signals:
    
public slots:
    
protected:
};

#endif // NEWPLOT_H
