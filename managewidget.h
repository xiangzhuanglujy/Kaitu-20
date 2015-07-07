//数据管理

#ifndef MANAGEWIDGET_H
#define MANAGEWIDGET_H

#include <QWidget>
#include <QDateTime>

class QLabel;
class QRadioButton;
class QLineEdit;
class QComboBox;
class QDateTimeEdit;

class ManageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ManageWidget(QWidget *parent = 0);
    void clearSelect();
    void updateLabelWithDate();

signals:
    void changeDataName(QString);
    void queryTable(QDateTime,QDateTime,QString,QString,QString,QString);
    void enableWindow(bool);
    
public slots:
    void tempQueryCB_clicked();
    void humidityQueryCB_clicked();
//    void createQrueySql();
    void setQueryTable();
    void dataNameCB_textChanged(QString);
    void changeFlag();
    void removeFromQCombo(QString);

private:
    QLabel *dataNameLB;
    QLabel *startTineLB;
    QLabel *endTimeLB;
    QLabel *tempUpLB;
    QLabel *tempDownLB;
    QLabel *humidityUpLB;
    QLabel *humidityDownLB;

    QDateTimeEdit *startTineDTE;
    QDateTimeEdit *endTineDTE;

    QRadioButton *tempQueryCB;
    QRadioButton *humidityQueryCB;

    QLineEdit *tempUpLE;
    QLineEdit *tempDownLE;
    QLineEdit *humidityUpLE;
    QLineEdit *humidityDownLE;

    QComboBox *dataNameCB;
};

#endif // MANAGEWIDGET_H
