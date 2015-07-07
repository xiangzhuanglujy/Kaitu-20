#ifndef MSGWIDGET_H
#define MSGWIDGET_H

#include <QDialog>

class QLabel;
class QPushButton;

class MsgWidget : public QDialog
{
    Q_OBJECT
public:
    explicit MsgWidget(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QPushButton *okPB;
    QLabel *msgLB;
    
};

#endif // MSGWIDGET_H
