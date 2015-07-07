#ifndef WDATASTORE_H
#define WDATASTORE_H

#include <QDialog>

class QPushButton;
class QLineEdit;
class QLabel;

class WDataStore : public QDialog
{
    Q_OBJECT
public:
    explicit WDataStore(QWidget *parent = 0);
    
signals:
    
public slots:
    void okBtn_clicked();
    
private:
    QPushButton *okBtn;
    QPushButton *cancelBtn;
    QLineEdit *edit;
    QLabel *label;
};

#endif // WDATASTORE_H
