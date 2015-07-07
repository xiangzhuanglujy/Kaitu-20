#include "lngwidget.h"

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QPainter>

LngWidget::LngWidget(QDialog *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("The Record of Temperature Data"));

    setFixedSize(350,240);

    label=new QLabel(tr("Language select:"));
    combo=new QComboBox();
    combo->setIconSize(QSize(25,50));
    combo->addItem(QIcon(":/png/png/english"),tr("English"));
    combo->addItem(QIcon(":/png/png/china"),tr("Chinese"));
    combo->setCurrentIndex(0);

    okPB=new QPushButton(tr("OK"));
    quitPB=new QPushButton(tr("Quit"));

    label->setObjectName("selectLabel");
    combo->setObjectName("selectCombo");
    okPB->setObjectName("selectButton");
    quitPB->setObjectName("selectButton");

    QGridLayout *layout=new QGridLayout(this);
    layout->addWidget(label,0,0);
    layout->addWidget(combo,0,1);
    layout->addWidget(okPB,1,0);
    layout->addWidget(quitPB,1,1);

    connect(quitPB,SIGNAL(clicked()),this,SLOT(close()));
    connect(okPB,SIGNAL(clicked()),this,SLOT(okPB_clicked()));
}

void LngWidget::okPB_clicked()
{
    choose=combo->currentIndex();
    accept();
}

void LngWidget::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    QLinearGradient linear(0,0,0,rect().height());
    linear.setColorAt(0.0,QColor(192,192,192));
    linear.setColorAt(0.5,QColor(232,232,232));
    linear.setColorAt(1.0,QColor(255,255,255));
    paint.setBrush(QBrush(linear));
    paint.drawRect(rect());
}
