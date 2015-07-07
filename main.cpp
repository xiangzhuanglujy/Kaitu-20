#include "mainwindow.h"
#include "lngwidget.h"

#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QDebug>
//#include <QSettings>
#include <QSharedMemory>
#include <QSettings>
//#include <QFile>
//#include <QDir>
//#include <qt_windows.h>
//#include <QLibrary>

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

//bool IsWin64 = false;
//#ifdef Q_OS_WIN
//    OSVERSIONINFOEX osvi;
//    SYSTEM_INFO si;
//    typedef void WINAPI (*PGNSI)(LPSYSTEM_INFO) ;
//    PGNSI pGNSI=NULL;

//    ZeroMemory(&si, sizeof(SYSTEM_INFO));
//    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
//    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
//    GetVersionEx((OSVERSIONINFO*)&osvi);

//    QLibrary lib("kernel32.dll");
//    if(lib.load())
//        pGNSI = (PGNSI)lib.resolve("GetNativeSystemInfo");
//    if(NULL != pGNSI)
//        pGNSI(&si);
//    else
//        GetSystemInfo(&si);

//    if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
//        IsWin64 = true;
//    else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
//        IsWin64 = false;

//    if(lib.isLoaded())
//        lib.unload();
//#endif

//    qDebug()<<"system bit"<<IsWin64;


    QSettings *configIniRead = new QSettings(".\\config.ini", QSettings::IniFormat);
    int ipResult = configIniRead->value("/Language/language").toInt();
    delete configIniRead;

    QTranslator translate;
    translate.load(":/TDR_chinese.qm");

    if(ipResult==1)
    {
        app.installTranslator(&translate);
    }

    qDebug()<<"main"<<ipResult;

    QFile file(":/qss/style.qss");
    if(file.open(QIODevice::ReadOnly))
    {
        app.setStyleSheet(file.readAll());
        file.close();
    }

    QSharedMemory shared_memory;
    shared_memory.setKey(QString("main_window"));

    if(shared_memory.attach())
    {
        return 0;
    }
    if(shared_memory.create(1))
    {
        MainWindow w;
        if(1==ipResult)
            w.lngType=MainWindow::CHINESE;
        else
            w.lngType=MainWindow::ENGLISH;

        w.show();

        return app.exec();
    }
    return 0;


//    QSettings *configIniRead = new QSettings(".\\config.ini", QSettings::IniFormat);
//    QString ipResult = configIniRead->value("/Language/language").toString();
//    delete configIniRead;

//    if(0==ipResult.toInt())
//    {
//        LngWidget lng;
//        if(lng.exec()==QDialog::Accepted)
//        {
//            if(LngWidget::Chinese==lng.choose)
//            {
//                QTranslator translate;
//                translate.load(":/png/rc4.qm");
//                app.installTranslator(&translate);

//                QSettings *writeIni = new QSettings(".\\config.ini", QSettings::IniFormat);
//                writeIni->setValue("/Language/language",2);
//                delete writeIni;

//                QSharedMemory shared_memory;
//                shared_memory.setKey(QString("main_window"));

//                if(shared_memory.attach())
//                {
//                    return 0;
//                }
//                if(shared_memory.create(1))
//                {
//                    MainWindow w;
//                    w.lngType=MainWindow::CHINESE;
//                    w.show();
//                    return app.exec();
//                }
//                return 0;
//            }
//            else
//            {
//                QSettings *writeIni = new QSettings(".\\config.ini", QSettings::IniFormat);
//                writeIni->setValue("/Language/language",1);
//                delete writeIni;

//                QSharedMemory shared_memory;
//                shared_memory.setKey(QString("main_window"));

//                if(shared_memory.attach())
//                {
//                    return 0;
//                }
//                if(shared_memory.create(1))
//                {
//                    MainWindow w;
//                    w.lngType=MainWindow::ENGLISH;
//                    w.show();
//                    return app.exec();
//                }
//                return 0;
//            }
//        }
//        return 0;
//    }
//    else if(1==ipResult.toInt())
//    {
//        QSharedMemory shared_memory;
//        shared_memory.setKey(QString("main_window"));

//        if(shared_memory.attach())
//        {
//            return 0;
//        }
//        if(shared_memory.create(1))
//        {
//            MainWindow w;
//            w.lngType=MainWindow::ENGLISH;
//            w.show();
//            return app.exec();
//        }
//        return 0;
//    }
//    else if(2==ipResult.toInt())
//    {
//        QTranslator translate;
//        translate.load(":/png/rc4.qm");
//        app.installTranslator(&translate);

//        QSharedMemory shared_memory;
//        shared_memory.setKey(QString("main_window"));

//        if(shared_memory.attach())
//        {
//            return 0;
//        }
//        if(shared_memory.create(1))
//        {
//            MainWindow w;
//            w.lngType=MainWindow::CHINESE;
//            w.show();
//            return app.exec();
//        }
//        return 0;
//    }
//    return 0;


}


