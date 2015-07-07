//系统/邮件设置

#ifndef STMMAILWIDGET_H
#define STMMAILWIDGET_H

#include <QDialog>

class QTabWidget;
class StmSetWidget;
class MailSetWidget;

class StmMailWidget : public QDialog
{
    Q_OBJECT
public:
    explicit StmMailWidget(QWidget *parent = 0);
    
signals:

public slots:

private:
    QTabWidget *tab;
    StmSetWidget *stmSetWidget;
    MailSetWidget *mailSetWidget;
    
};

#endif // STMMAILWIDGET_H
