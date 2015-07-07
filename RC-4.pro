#-------------------------------------------------
#
# Project created by QtCreator 2014-07-28T13:00:14
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LR-20
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    uploadwidget.cpp \
    settingwidget.cpp \
    diagramwidget.cpp \
    datatablewidget.cpp \
    generatewidget.cpp \
    managewidget.cpp \
    stmmailwidget.cpp \
    stmsetwidget.cpp \
    mailsetwidget.cpp \
    inputdialog.cpp \
    about.cpp \
    msgwidget.cpp \
    check.cpp \
    reportwidget.cpp \
    lngwidget.cpp \
    mythread.cpp \
    mail/smtpclient.cpp \
    mail/quotedprintable.cpp \
    mail/mimetext.cpp \
    mail/mimepart.cpp \
    mail/mimemultipart.cpp \
    mail/mimemessage.cpp \
    mail/mimeinlinefile.cpp \
    mail/mimehtml.cpp \
    mail/mimefile.cpp \
    mail/mimecontentformatter.cpp \
    mail/mimeattachment.cpp \
    mail/emailaddress.cpp \
    comthread.cpp \
    wdatastore.cpp \
    mysql.cpp \
    serial/qextserialport.cpp\
    serial/qextserialport_win.cpp \
    imagelabel.cpp \
    finddevice.cpp \



HEADERS  += mainwindow.h \
    qcustomplot.h \
    uploadwidget.h \
    settingwidget.h \
    diagramwidget.h \
    datatablewidget.h \
    generatewidget.h \
    managewidget.h \
    stmmailwidget.h \
    stmsetwidget.h \
    mailsetwidget.h \
    inputdialog.h \
    about.h \
    msgwidget.h \
    check.h \
    reportwidget.h \
    lngwidget.h \
    mythread.h \
    mail/smtpclient.h \
    mail/quotedprintable.h \
    mail/mimetext.h \
    mail/mimepart.h \
    mail/mimemultipart.h \
    mail/mimemessage.h \
    mail/mimeinlinefile.h \
    mail/mimehtml.h \
    mail/mimefile.h \
    mail/mimecontentformatter.h \
    mail/mimeattachment.h \
    mail/emailaddress.h \
    mail/SmtpMime \
    comthread.h \
    wdatastore.h \
    mysql.h \
    serial/qextserialport_global.h \
    serial/qextserialport.h \
    imagelabel.h \
    finddevice.h \



RESOURCES += \
    src.qrc

CONFIG+=qaxcontainer

RC_FILE +=myico.rc

TRANSLATIONS = TDR_chinese.ts
