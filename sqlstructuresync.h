#ifndef SQLSTRUCTURESYNC_H
#define SQLSTRUCTURESYNC_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>

class SqlStructureSync
{
public:
    SqlStructureSync(QString db_1,QString db_2);

public:
    bool initDb();
    void startSync();

    void setp_one_sync_table(QSqlQuery query_ori,QSqlQuery query_sync);
    void setp_two_sync_table_structure(QSqlQuery query_ori,QSqlQuery query_sync);

    bool outputDatabaseInfo(QSqlDatabase database);
    bool outPutTableInfo(QString tabNmae);

    QString db_1;
    QString db_2;

    QSqlDatabase syncToDb;
    QSqlDatabase originalDb;


};

#endif // SQLSTRUCTURESYNC_H
