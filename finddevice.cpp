#include "finddevice.h"

//#include <QMovie>
#include <QLabel>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QCloseEvent>

FindDevice::FindDevice(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("The Record of Temperature Data"));

    setWindowFlags(Qt::Dialog/*|Qt::FramelessWindowHint*/);
    setAttribute(Qt::WA_DeleteOnClose);

//    movie=new QMovie(":/qss/load1");
//    LB_movie=new QLabel();
//    LB_movie->setFixedSize(350,30);
//    LB_movie->setMovie(movie);
    LB_device=new QLabel();
    LB_device->setText(tr("find device"));
    LB_device->setFixedHeight(30);
    LB_device->setAlignment(Qt::AlignCenter);

    progressBar=new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMinimumWidth(200);

    QHBoxLayout *layout=new QHBoxLayout(this);
//    layout->addWidget(LB_movie);
    layout->addWidget(LB_device);
    layout->addWidget(progressBar);
    //    movie->start();

    LB_device->setObjectName("deviceLabel");
    setObjectName("deviceWidget");
}

void FindDevice::closeEvent(QCloseEvent *event)
{
    event->ignore();
//    hide();
    showMinimized();
}

void FindDevice::setMax(int max)
{
    progressBar->setMaximum(max);
}

void FindDevice::setValue(int value)
{
    progressBar->setValue(value);
}

void FindDevice::setQString(bool bret)
{
    if(bret)
    {
        LB_device->setText(tr("Uploading Data"));
    }
    else
    {
        LB_device->setText("");
    }
}
