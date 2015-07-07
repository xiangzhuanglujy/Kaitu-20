#include "djindu.h"

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

DJindu::DJindu(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    label=new QLabel();
    progressBar=new QProgressBar();
    progressBar->setMinimum(0);

    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(progressBar);
}

void DJindu::setRange(int current, int max)
{
    label->setText(QString("%1/%2").arg(current).arg(max));
    progressBar->setMaximum(max);
    progressBar->setValue(current);
    if(current>=max)
    {
        close();
    }
}
