#include "stmmailwidget.h"

#include <QTabWidget>
#include <QGridLayout>

#include "stmsetwidget.h"
#include "mailsetwidget.h"

StmMailWidget::StmMailWidget(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("The Record of Temperature Data"));

    tab=new QTabWidget(this);
    stmSetWidget=new StmSetWidget(this);
    mailSetWidget=new MailSetWidget(this);
    tab->addTab(stmSetWidget,tr("System/Email"));
    tab->addTab(mailSetWidget,tr("EmailParameterSet"));
    tab->setCurrentWidget(stmSetWidget);

    QGridLayout *grid=new QGridLayout(this);
    grid->addWidget(tab);

//    connect(stmSetWidget,SIGNAL(dateFormatChanged()),parent,SLOT(changeDateFormat()));
}
