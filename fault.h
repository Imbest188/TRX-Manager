#ifndef FAULT_H
#define FAULT_H

#include <QString>
#include <QStringList>

class Fault
{
public:
    Fault(){}
    Fault(QString mo, QString fault)
    {
        this->mo = mo;
        this->fault = fault;
    };
    Fault(QString arg)
    {
        QStringList subArray = arg.split('\n');
    };

    QString getManagemantObject()
    {
        return mo;
    }
    QString getFault()
    {
        return fault;
    }
    QStringList getData()
    {
        return {mo, fault};
    }

private:
    QString mo;
    QString fault;
};

#endif // FAULT_H
