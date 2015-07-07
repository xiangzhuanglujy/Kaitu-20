#include "mailsetwidget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QListWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QErrorMessage>
#include <QDateTime>

#include "inputdialog.h"
#include "mail/SmtpMime"
#include "mysql.h"

MailSetWidget::MailSetWidget(QWidget *parent) :
    QWidget(parent)
{    
    topGB=new QGroupBox(tr("Email Topic"));
    centerGB=new QGroupBox(tr("Attachment"));
    bottomGB=new QGroupBox(tr("Receiver Email Address"));

    //
    smtpLB=new QLabel(tr("SMTP"));
    senderMailLB=new QLabel(tr("Sender's Email"));
    mailPwdLB=new QLabel(tr("EMail Password"));
    configLB=new QLabel(tr("Reenter"));
    smtpTipLB=new QLabel(tr("sender's SMTP. eg:smtp.sina.com"));
    smtpLE=new QLineEdit();
    smtpLE->setMaxLength(50);
    senderMailLE=new QLineEdit();
    senderMailLE->setMaxLength(50);
    QRegExp regExp("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
    senderMailLE->setValidator(new QRegExpValidator(regExp, this));
    mailPwdLE=new QLineEdit();
    mailPwdLE->setEchoMode(QLineEdit::Password);
    mailPwdLE->setMaxLength(20);
    configLE=new QLineEdit();
    configLE->setEchoMode(QLineEdit::Password);

    QGridLayout *grid1=new QGridLayout();
    grid1->addWidget(smtpLB,0,0);
    grid1->addWidget(smtpLE,0,1);
    grid1->addWidget(smtpTipLB,0,2);
    grid1->addWidget(senderMailLB,1,0);
    grid1->addWidget(senderMailLE,1,1);
    grid1->addWidget(mailPwdLB,2,0);
    grid1->addWidget(mailPwdLE,2,1);
    grid1->addWidget(configLB,3,0);
    grid1->addWidget(configLE,3,1);

    //
    customLB=new QLabel(tr("Define"));
    customLE=new QLineEdit();
    selectLB=new QLabel(tr("choose"));
    timeCB=new QCheckBox(tr("Time"));
    userInfoCB=new QCheckBox(tr("User Information"));
    QGridLayout *grid2=new QGridLayout(topGB);
    grid2->addWidget(customLB,0,0);
    grid2->addWidget(customLE,0,1);
    QHBoxLayout *temp=new QHBoxLayout();
    temp->addWidget(selectLB);
    temp->addStretch(1);
    temp->addWidget(timeCB);
    temp->addStretch(1);
    temp->addWidget(userInfoCB);
    grid2->addLayout(temp,1,0);
//    grid2->addWidget(selectLB,1,0);
//    grid2->addWidget(timeCB,1,1);
//    grid2->addWidget(userInfoCB,1,2);

    //
    wordCB=new QCheckBox(tr("WORD"));
    pdfCB=new QCheckBox(tr("PDF"));
    txtCB=new QCheckBox(tr("TEXT"));
    excelCB=new QCheckBox(tr("EXCEL"));
    QHBoxLayout *layout=new QHBoxLayout(centerGB);
    layout->addWidget(wordCB);
    layout->addWidget(excelCB);
    layout->addWidget(pdfCB);
    layout->addWidget(txtCB);

    //
    listLW=new QListWidget(this);
    addPB=new QPushButton(tr("Add"));
    removePB=new QPushButton(tr("Delete"));
    QGridLayout *grid3=new QGridLayout(bottomGB);
    grid3->addWidget(listLW,0,0,2,1);
    grid3->addWidget(addPB,0,1);
    grid3->addWidget(removePB,1,1);

    savePB=new QPushButton(tr("Save"));
    testPB=new QPushButton(tr("Send Test Mail"));
    QHBoxLayout *hLayout=new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(savePB);
    hLayout->addStretch();
    hLayout->addWidget(testPB);
    hLayout->addStretch();

    QVBoxLayout *main=new QVBoxLayout();
    main->addLayout(grid1);
    main->addWidget(topGB);
    main->addWidget(centerGB);
    main->addWidget(bottomGB);
    main->addLayout(hLayout);
    setLayout(main);

    //信号/槽
    connect(testPB,SIGNAL(clicked()),this,SLOT(testPB_clicked()));
    connect(savePB,SIGNAL(clicked()),this,SLOT(savePB_click()));
    connect(addPB,SIGNAL(clicked()),this,SLOT(addPBSlot()));
    connect(removePB,SIGNAL(clicked()),this,SLOT(removePB_click()));
    connect(listLW,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(listLW_itemClicked(QListWidgetItem*)));

    loadSqlite();
}

void MailSetWidget::addPBSlot()
{
    InputDialog dlg(this);
    dlg.exec();
}

void MailSetWidget::removePB_click()
{
//    int size=listLW->count();
    for(int i=0;i<listLW->count();i++)
    {
        QListWidgetItem *item=listLW->item(i);
        if(item->checkState()==Qt::Checked)
        {
            listLW->removeItemWidget(item);
            delete item;
            qDebug()<<"remove";
            i--;
        }
    }
}

void MailSetWidget::addRecviverAddress(QString addr)
{
    int size=listLW->count();
    for(int i=0;i<size;i++)
    {
        QListWidgetItem *it=listLW->item(i);
        if(it->text()==addr)
        {
            QMessageBox::critical(this,tr("Tips"),tr("address already exists"));
            return;
        }
    }

    QListWidgetItem *item=new QListWidgetItem(addr);
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setCheckState(Qt::Unchecked);
    listLW->addItem(item);
}


void MailSetWidget::savePB_click()
{
    if(smtpLE->text().isEmpty())
    {
        QMessageBox::critical(this,tr("Tips"),tr("smtp must be not null"));
        return;
    }

    if(mailPwdLE->text().isEmpty())
    {
        QMessageBox::critical(this,tr("Tips"),tr("password must be not null"));
        return;
    }

    if(mailPwdLE->text()!=configLE->text())
    {
        QMessageBox::critical(this,tr("Tips"),tr("password is error"));
        return;
    }

    QString sql1,sql2;
    QString smtp=smtpLE->text();
    QString sender=senderMailLE->text();
    QString password=mailPwdLE->text();
    QString theme=customLE->text();
    bool date;
    bool userInfo;
    bool word;
    bool excel;
    bool pdf;
    bool txt;

    if(timeCB->checkState()==Qt::Unchecked)
    {
        date=0;
    }
    else
    {
        date=1;
    }
    if(userInfoCB->checkState()==Qt::Unchecked)
    {
        userInfo=0;
    }
    else
    {
        userInfo=1;
    }
    if(wordCB->checkState()==Qt::Unchecked)
    {
        word=0;
    }
    else
    {
        word=1;
    }
    if(excelCB->checkState()==Qt::Unchecked)
    {
        excel=0;
    }
    else
    {
        excel=1;
    }
    if(pdfCB->checkState()==Qt::Unchecked)
    {
        pdf=0;
    }
    else
    {
        pdf=1;
    }
    if(txtCB->checkState()==Qt::Unchecked)
    {
        txt=0;
    }
    else
    {
        txt=1;
    }

    sql1=QString("update mail set smtp='%1',sender='%2',password='%3',theme='%4',word='%5',pdf='%6',excel='%7',txt='%8',date='%9',userInfo='%10';")
            .arg(smtp)
            .arg(sender)
            .arg(password)
            .arg(theme)
            .arg(word)
            .arg(pdf)
            .arg(excel)
            .arg(txt)
            .arg(date)
            .arg(userInfo);

    QStringList list;
    list.append(sql1);

    int size=listLW->count();
    for(int i=0;i<size;i++)
    {
        QListWidgetItem *item=listLW->item(i);
        if(item->checkState()==Qt::Checked)
        {
            sql2=QString("insert into address(flag,receiver) values('1','%1');").arg(item->text());
        }
        else
        {
            sql2=QString("insert into address(flag,receiver) values('0','%1');").arg(item->text());
        }
        list.append(sql2);
    }

//    sql2=QString("insert into address(flag,receiver) values('1','sss@qq.com');");
    QSqlQuery query(sqldb);
    query.exec("delete from address;");
    int flag=0;
    for(int i=0;i<list.size();i++)
    {
       if(true==query.exec(list.at(i)))
       {
           flag++;
       }
    }
    query.clear();
    if(flag==list.size())
    {
        QMessageBox::information(this,tr("Tips"),tr("Save Success"));
    }
}

void MailSetWidget::showMailParams(QStringList list1, QStringList list2)
{
    if(list1.size()<10)
    {
        return;
    }
    smtpLE->setText(list1.at(0));
    senderMailLE->setText(list1.at(1));
    mailPwdLE->setText(list1.at(2));
    configLE->setText(list1.at(2));
    customLE->setText(list1.at(3));
    wordCB->setChecked(list1.at(4).toInt());
    pdfCB->setChecked(list1.at(5).toInt());
    excelCB->setChecked(list1.at(6).toInt());
    txtCB->setChecked(list1.at(7).toInt());
    timeCB->setChecked(list1.at(8).toInt());
    userInfoCB->setChecked(list1.at(9).toInt());

    if(list2.size()%2!=0)
    {
        return;
    }
    int size=list2.size()/2;
    for(int i=0;i<size;i++)
    {
        QListWidgetItem *item=new QListWidgetItem(list2.at(2*i+1));
        item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        if(1==list2.at(2*i).toInt())
        {
            item->setCheckState(Qt::Checked);
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
        }
        listLW->addItem(item);
    }
}

void MailSetWidget::loadSqlite()
{
    QSqlQuery query(sqldb);
    query.exec("select * from mail");
    QStringList list1,list2;
    while(query.next())
    {
        list1.append(query.value(0).toString());
        list1.append(query.value(1).toString());
        list1.append(query.value(2).toString());
        list1.append(query.value(3).toString());
        list1.append(query.value(4).toString());
        list1.append(query.value(5).toString());
        list1.append(query.value(6).toString());
        list1.append(query.value(7).toString());
        list1.append(query.value(8).toString());
        list1.append(query.value(9).toString());
    }

    query.exec("select * from address;");
    while(query.next())
    {
        list2.append(query.value(0).toString());
        list2.append(query.value(1).toString());
    }
    query.clear();
    showMailParams(list1,list2);
}

void MailSetWidget::testPB_clicked()
{
    QString smtp_value=smtpLE->text();
    QString sender=senderMailLE->text();
    QString pwd=mailPwdLE->text();
    QString theme=customLE->text();

    if(timeCB->checkState()==Qt::Checked)
    {
        theme=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    if(userInfoCB->checkState()==Qt::Checked)
    {
        theme+="TestEmail";
    }

    SmtpClient smtp(smtp_value, 25, SmtpClient::TcpConnection);
    smtp.setUser(sender);
    smtp.setPassword(pwd);

    MimeMessage message;
    message.setSender(new EmailAddress(sender,sender));

    int size=listLW->count();
    for(int i=0;i<size;i++)
    {
        QListWidgetItem *item=listLW->item(i);
        if(item->checkState()==Qt::Checked)
        {
            message.addRecipient(new EmailAddress(item->text(),item->text().left(item->text().indexOf('@'))));
        }
    }
//    message.addRecipient(new EmailAddress("821185277@qq.com", "821185277@qq.com"));
    message.setSubject(theme);      //主题

    //附件
//    if(wordCB->checkState()==Qt::Checked)
//    {
//        MimeAttachment *temp = new MimeAttachment(new QFile(".\\tmp.doc"));
//        message.addPart(temp);
//    }
//    if(excelCB->checkState()==Qt::Checked)
//    {
//        MimeAttachment *temp = new MimeAttachment(new QFile(".\\tmp.xls"));
//        message.addPart(temp);
//    }
//    if(pdfCB->checkState()==Qt::Checked)
//    {
//        MimeAttachment *temp = new MimeAttachment(new QFile(".\\tmp.pdf"));
//        message.addPart(temp);
//    }
//    if(txtCB->checkState()==Qt::Checked)
//    {
//        MimeAttachment *temp = new MimeAttachment(new QFile(".\\tmp.txt"));
//        message.addPart(temp);
//    }

    MimeText text;          //内容
    text.setText("Hi,\nThis is a simple email message.\n");
    // Now add it to the mail
    message.addPart(&text);
    // Now we can send the mail
    if(!smtp.connectToHost())
    {
        errorMessage(tr("SMTP Connect Error \n Please ensure SMTP is correct"));
        smtp.quit();
        return;
    }

    if(!smtp.login())
    {
        errorMessage(tr("Login Error \n Please ensure username and password is correct"));
        smtp.quit();
        return;
    }

    if(!smtp.sendMail(message))
    {
        errorMessage(tr("Mail Send Error \n Please ensure receiver is correct"));
        smtp.quit();
        return;
    }
    smtp.quit();

    errorMessage(tr("Send Success"));
}

void MailSetWidget::listLW_itemClicked(QListWidgetItem *)
{
    int select=0;
    int size=listLW->count();
    for(int i=0;i<size;i++)
    {
        QListWidgetItem *item=listLW->item(i);
        if(item->checkState()==Qt::Checked)
        {
            select++;
        }
        if(select==4)
        {
            QMessageBox::information(this,tr("Tips"),tr("you can only select three items max"));
            item->setCheckState(Qt::Unchecked);
            return;
        }
    }
}

void MailSetWidget::errorMessage(const QString &message)
{
    QErrorMessage err (this);
    err.showMessage(message);
    err.exec();
}
