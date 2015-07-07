#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class InputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InputDialog(QWidget *parent = 0);
    
signals:
    void sendAddr(QString addr);

public slots:
    void okPB_pushed();

private:
    QLabel *titleLB;
    QPushButton *okPB;
    QPushButton *cancelPB;
    QLineEdit *inputLE;
    
};

#endif // INPUTDIALOG_H
