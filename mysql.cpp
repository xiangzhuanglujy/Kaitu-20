#include "mysql.h"

#include <QDebug>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QSqlError>

QSqlDatabase sqldb;                     //数据库全局变量

MySQL::MySQL()
{
}

void MySQL::addMysql()
{
    sqldb=QSqlDatabase::addDatabase("QMYSQL");
    sqldb.setHostName("127.0.0.1");
    sqldb.setPort(3306);
    sqldb.setUserName("root");
    sqldb.setPassword("601482");
    sqldb.setDatabaseName("family");
}

void MySQL::addSqlite()
{
    sqldb=QSqlDatabase::addDatabase("QSQLITE");
    sqldb.setDatabaseName("Data.db");
}

void MySQL::addSqlServer()
{
    sqldb=QSqlDatabase::addDatabase("QODBC");
    // 注意,对于express版本数据库, 一定要加\\sqlexpress这种后缀
    QString dsn="DRIVER={SQL SERVER\\sqlexpress};SERVER=192.168.44.1\\sqlexpress;DATABASE=sqlscada";
    sqldb.setDatabaseName(dsn);
    sqldb.setUserName("sa");
    sqldb.setPassword("scada");
}

bool MySQL::connect()
{
    bool bret=sqldb.open();
    qDebug()<<"database:"<<bret;
    if(!bret)
    {
        QFile file("error.txt");
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream stream(&file);
            stream<<sqldb.lastError().text();
            file.close();
        }
    }

    return bret;
}

void MySQL::disconnect()
{
    if(sqldb.isOpen())
        sqldb.close();
    QSqlDatabase::removeDatabase("QSQLITE");
}
