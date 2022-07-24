#include "sqlstructuresync.h"
#include "QFile"

SqlStructureSync::SqlStructureSync(QString db_1,QString db_2)
{

    this->db_1 = db_1;
    this->db_2 = db_2;

    //connect db 1,2
}


bool SqlStructureSync::initDb()
{
    originalDb = QSqlDatabase::addDatabase("QSQLITE","c1");
    syncToDb = QSqlDatabase::addDatabase("QSQLITE","c2");

    originalDb.setDatabaseName(db_1);
    syncToDb.setDatabaseName(db_2);


    if(QFile::exists(db_1) && QFile::exists(db_2))
    {

        if(originalDb.open() && syncToDb.open())
        {

            return true;
        }

        else
        {
            return false;
        }

    }
    else
    {
        return false;
    }

}


void SqlStructureSync::startSync()
{
    QSqlQuery query_ori(originalDb);
    QSqlQuery query_syncTo(syncToDb);

    setp_one_sync_table(query_ori,query_syncTo);

    setp_two_sync_table_structure(query_ori,query_syncTo);
}

void SqlStructureSync::setp_one_sync_table(QSqlQuery query_ori, QSqlQuery query_sync)
{
    QStringList oriTables = originalDb.tables();
    QStringList syncTables = syncToDb.tables();

    QStringList missingTabel;

    for(auto &i:oriTables)
    {
        if(syncTables.contains(i))
        {
            ;
        }
        else
        {
            missingTabel.append(i);
            qDebug() << QString("table missing find :").arg(i);
        }
    }

    //在syncTo 里面创建表
    if(!missingTabel.empty())
    {
        QStringList sqls;
        QString sql;

        QStringList result_sqls_create_tables;
        for(auto &i:missingTabel)
        {
            sql = QString("select sql from sqlite_master where type='table' and name='%1';").arg(i);
            sqls.push_back(sql);
        }

        for(auto &sql:sqls)
        {
            if(!query_ori.exec(sql))
                qDebug() << query_ori.lastError();

            query_ori.next();

            result_sqls_create_tables.push_back(query_ori.value(0).toString());
        }

        //sync exec
        for(auto &sql:result_sqls_create_tables)
        {
            qDebug() << sql;
            if(!query_sync.exec(sql))
                qDebug() << query_sync.lastError();

        }
    }


}

void SqlStructureSync::setp_two_sync_table_structure(QSqlQuery query_ori, QSqlQuery query_syncTo)
{
    QString sql_get_table_info = "PRAGMA table_info(%1);";
    QString sql_alter_table = "ALTER TABLE %1 ADD COLUMN %2 %3%4 %5;";//ALTER TABLE FaceControlRecord ADD COLUMN sAlcohol Text;

//    query_ori.exec(sql_get_table_info.arg("audio"));
//    query_ori.next();

//    while(query_ori.next())
//    {
//        if(query_ori.value(4).toString().isEmpty())
//            qDebug() << query_ori.value(4);
//    }

    //提取主表原始数据
    QString sql;
    QStringList ori_tables_alter_str;
    QStringList sync_tables_alter_str;
    QStringList different_tables;
    for(auto &table_name:originalDb.tables())
    {
        sql = sql_get_table_info.arg(table_name);

        if(!query_ori.exec(sql))
            qDebug() << query_ori.lastError();
        if(!query_syncTo.exec(sql))
            qDebug() << query_syncTo.lastError();

        //遍历表结构 ori alter
        while(query_ori.next())
        {
           //cid | name  | type  | notnull | dflt_value  | pk
            QString sql;

            sql = sql_alter_table.arg(table_name).arg(query_ori.value(1).toString()).arg(query_ori.value(2).toString());
            //not null flag
            if(query_ori.value(3).toInt() == 1)
            {
                sql = sql.arg(" NOT NULL");
            }
            else
            {
                sql = sql.arg("");
            }

            if(query_ori.value(4).toString().isEmpty())
            {
                sql = sql.arg("");
            }
            else
            {
                sql = sql.arg("DEFAULT %1").arg(query_ori.value(4).toString());
            }

            ori_tables_alter_str.append(sql);
        }

        //遍历表结构 sync alter
        while(query_syncTo.next())
        {
           //cid | name  | type  | notnull | dflt_value  | pk
            QString sql;

            sql = sql_alter_table.arg(table_name).arg(query_syncTo.value(1).toString()).arg(query_syncTo.value(2).toString());
            if(query_syncTo.value(3).toInt() == 1)
            {
                sql = sql.arg(" NOT NULL");
            }
            else
            {
                sql = sql.arg("");
            }

            if(query_syncTo.value(4).toString().isEmpty())
            {
                sql = sql.arg("");
            }
            else
            {
                sql = sql.arg("DEFAULT %1").arg(query_syncTo.value(4).toString());
            }


            sync_tables_alter_str.append(sql);

        }

    }

    int cnt_change = 0;
    for(auto &i:ori_tables_alter_str)
    {
        if(!sync_tables_alter_str.contains(i))
        {
            cnt_change++;
            qDebug() << i;

            if(!query_syncTo.exec(i))
                qDebug() << query_syncTo.lastError();
        }
    }

    if(cnt_change == 0)
    {
        qDebug() << "all tables are up to date";
    }
    else
    {
        qDebug() << cnt_change << "tables changed";
    }

//    for(QString &i:ori_tables_alter_str)
//    {
//        if(i.contains("INT"))
//        {
//            qDebug() << i;
//        }
//    }
}


bool SqlStructureSync::outputDatabaseInfo(QSqlDatabase database)
{
    QStringList tabels =  database.tables();

    QStringListIterator itr(tabels);
    while (itr.hasNext())
    {
        QString tabel = itr.next();
        qDebug() << QString("表名:") << tabel;

        outPutTableInfo(tabel);
    }

    return true;
}

bool SqlStructureSync::outPutTableInfo(QString tabNmae)
{
    QSqlQuery query(originalDb);
    QString strTableNmae = tabNmae;
    QString str = "PRAGMA table_info(" + strTableNmae + ")";

    query.prepare(str);
    if (query.exec())
    {
        while (query.next())
        {
            qDebug() << QString(QString("%1 %2 %3")).arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
        }
    }
    else
    {
        qDebug() << query.lastError();
        return false;
    }
    return true;
}
