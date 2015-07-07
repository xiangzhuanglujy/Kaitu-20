//邮件设置

#ifndef MAILSETWIDGET_H
#define MAILSETWIDGET_H

#include <QWidget>

class QGroupBox;
class QPushButton;
class QLabel;
class QCheckBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;

class MailSetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MailSetWidget(QWidget *parent = 0);
    
    void errorMessage(const QString &message);
    void showMailParams(QStringList list1,QStringList list2);
    void loadSqlite();

signals:
    void saveMailToDB(QStringList sql1);
    
public slots:
    void addPBSlot();
    void removePB_click();
    void addRecviverAddress(QString addr);
    void savePB_click();
    void testPB_clicked();

    void listLW_itemClicked(QListWidgetItem *item);
private:
    QGroupBox *topGB;
    QGroupBox *centerGB;
    QGroupBox *bottomGB;

    //
    QLabel *smtpLB;
    QLabel *senderMailLB;
    QLabel *mailPwdLB;
    QLabel *configLB;
    QLabel *smtpTipLB;
    QLineEdit *smtpLE;
    QLineEdit *senderMailLE;
    QLineEdit *mailPwdLE;
    QLineEdit *configLE;

    //
    QLabel *customLB;
    QLabel *selectLB;
    QLineEdit *customLE;
    QCheckBox *timeCB;
    QCheckBox *userInfoCB;

    //
    QCheckBox *wordCB;
    QCheckBox *pdfCB;
    QCheckBox *txtCB;
    QCheckBox *excelCB;

    //
    QListWidget *listLW;
    QPushButton *addPB;
    QPushButton *removePB;

    //
    QPushButton *savePB;
    QPushButton *testPB;
};

#endif // MAILSETWIDGET_H
