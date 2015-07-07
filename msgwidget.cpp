#include "msgwidget.h"

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

MsgWidget::MsgWidget(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("The Record of Temperature Data"));

    setAttribute(Qt::WA_DeleteOnClose);

    msgLB=new QLabel(tr("Disconnect between data logger and computer,please connect agagin"));
    okPB=new QPushButton(tr("OK"));

    QGridLayout *grid=new QGridLayout();
    grid->addWidget(msgLB,0,0,Qt::AlignLeft);
    grid->addWidget(okPB,1,0,Qt::AlignCenter);
    setLayout(grid);

    connect(okPB,SIGNAL(clicked()),this,SLOT(close()));
}
