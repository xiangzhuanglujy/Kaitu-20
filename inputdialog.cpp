#include "inputdialog.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QRegExpValidator>

InputDialog::InputDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("The Record of Temperature Data"));

    titleLB=new QLabel(tr("please input email address"));
    okPB=new QPushButton(tr("Ok"));
    cancelPB=new QPushButton(tr("Cancel"));
    inputLE=new QLineEdit();

//    QRegExp regExp("^([a-zA-Z0-9_\.\-])+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+$");
    QRegExp regExp("^([a-zA-Z0-9_.-])+@(([a-zA-Z0-9-])+.)+([a-zA-Z0-9]{2,4})+$");
    inputLE->setValidator(new QRegExpValidator(regExp, this));
    inputLE->setFocus();

    QGridLayout *grid=new QGridLayout();
    grid->addWidget(titleLB,0,0);
    grid->addWidget(okPB,0,1,Qt::AlignRight);
    grid->addWidget(cancelPB,1,1,Qt::AlignRight);
    grid->addWidget(inputLE,2,0,1,2);
    setLayout(grid);

    connect(okPB,SIGNAL(clicked()),this,SLOT(okPB_pushed()));
    connect(this,SIGNAL(sendAddr(QString)),parent,SLOT(addRecviverAddress(QString)));
    connect(cancelPB,SIGNAL(clicked()),this,SLOT(close()));
}


void InputDialog::okPB_pushed()
{
    if(inputLE->text().isEmpty())
    {
        QMessageBox::critical(this,tr("tip"),tr("addr can't be null"));
        return;
    }
    emit sendAddr(inputLE->text());
    accept();
}

