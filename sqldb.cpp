#include "sqldb.h"

#include <QMessageBox>
#include <QDebug>


Sqldb::Sqldb(QObject *parent) :
    QObject(parent)
{
    if(QSqlDatabase::contains("GBond"))
    {
        db = QSqlDatabase::database("GBond");
    }
    else
    {
        db=QSqlDatabase::addDatabase("QSQLITE","GBond");
        db.setDatabaseName("Data.db");
    }
    if(!db.open())
    {
        QMessageBox::critical(NULL,tr("Tips"),tr("connect error"));
    }
}


QSqlDatabase Sqldb::getDB()
{
//    qDebug()<<"db"<<db.isOpen();
    return db;
}

Sqldb::~Sqldb()
{
    db.close();
}
