#include "about.h"

#include <QLabel>
#include <QGridLayout>

About::About(QWidget *parent) :
    QDialog(parent)
{
    setFixedSize(300,100);
    setWindowTitle(tr("The Record of Temperature Data"));

    productTipLB=new QLabel(tr("Product Name:"));
    productNameLB=new QLabel(tr("Temperature Data Record"));
    versionTipLB=new QLabel(tr("Version:"));
    versionLB=new QLabel(tr("v1.0"));
//    authorTipLB=new QLabel();
//    authorLB=new QLabel();

    QGridLayout *grid=new QGridLayout(this);
    grid->addWidget(productTipLB,0,0);
    grid->addWidget(productNameLB,0,1);
    grid->addWidget(versionTipLB,1,0);
    grid->addWidget(versionLB,1,1);
//    grid->addWidget(authorTipLB,2,0);
//    grid->addWidget(authorLB,2,1);
}
