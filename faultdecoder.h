#ifndef FAULTDECODER_H
#define FAULTDECODER_H

#include <QtSql>
#include <QMessageBox>

class FaultDecoder
{
public:
    FaultDecoder();
    QString decode(QString);
    QStringList existCodes(){
        return exists;
    };

private:
    QSqlDatabase sdb;
    QStringList exists;
    void addIfNotExist(QString arg){
        if(exists.contains(arg)){}
        else
            exists.append(arg);
    };
};

#endif // FAULTDECODER_H
