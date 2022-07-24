#include <QCoreApplication>
#include "sqlstructuresync.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc != 3)
    {
        qDebug() << "input param error";
        return -1;
    }

    SqlStructureSync syncer(argv[1],argv[2]);

    if(syncer.initDb())
    {
        qDebug() << "sync start";
    }
    else
    {
        qDebug() << "db open failed";
    }

    syncer.startSync();


    return 0;
}
