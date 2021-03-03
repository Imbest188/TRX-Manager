#include "faultdecoder.h"

FaultDecoder::FaultDecoder()
{
    sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName("faults");
    if (!sdb.open()) {
           QMessageBox::critical(nullptr, "Ошибка при запуске", "База данных не открыта");
    }
}

QString FaultDecoder::decode(QString arg)
{
    QSqlQuery query(sdb);
    query.exec(QString("SELECT descritpion FROM faults WHERE name = '%1'").arg(arg));
    while(query.next()){
        if(query.value(0).toString().size()){
            return query.value(0).toString().replace('\r','\n');
        }
    }
    addIfNotExist(arg);
    return arg;
}
