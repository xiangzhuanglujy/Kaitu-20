//系统设置

#ifndef STMSETWIDGET_H
#define STMSETWIDGET_H

#include <QWidget>

class QCheckBox;
class QGroupBox;
class QLabel;
class QRadioButton;
class QLineEdit;
class QPushButton;
class QComboBox;

class StmSetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StmSetWidget(QWidget *parent = 0);
    void loadSqlite();
//    void showDefaultSetting(QStringList list);

signals:
//    void dateFormatChanged();

public slots:
    void executeSql();

private:
    QGroupBox *topGB;
    QGroupBox *centerGB;
    QGroupBox *bottomGB;

    QCheckBox *autoUploadCB;
    QCheckBox *autoSynchronousClockCB;

    QRadioButton *dateDefaultRB;
    QRadioButton *dateCustomRB;
    QLabel *dateLB;
    QLineEdit *dateLE;

    QRadioButton *timeDefaultRB;
    QRadioButton *timeCustomRB;
    QLabel *timeLB;
    QLineEdit *timeLE;
//    QComboBox *timeLE;

    QPushButton *saveParamPB;
};

#endif // STMSETWIDGET_H
