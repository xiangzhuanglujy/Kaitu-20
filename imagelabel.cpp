#include "imagelabel.h"

#include <QPainter>
#include <QDir>
#include <QDebug>

ImageLabel::ImageLabel(QWidget *parent) :
    QLabel(parent)
{
    QDir dir;
    imagePath=QString("%1/tmp.png").arg(dir.currentPath());
//    qDebug()<<imagePath;
}

void ImageLabel::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    paint.drawPixmap(rect(),QPixmap(imagePath));
}
