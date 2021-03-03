#ifndef FAULTCOLLECTOR_H
#define FAULTCOLLECTOR_H

#include <QVector>
#include <QString>
#include "fault.h"


#include <QDebug>

struct TransferingGroup
{
public:
    TransferingGroup(){inAlarm = false;}
    TransferingGroup(QString mo, QString rbs, QString fault)
    {
        addObject(mo, rbs, fault);
    }
    void addObject(QString mo, QString rbs, QString fault){
        if(rbs != this->rbs)
            this->rbs = rbs;
        if(!objects.size()){
            tg = getTransferingGroup(mo);
        }

        if(mo.contains("RXOCF")){
            if(fault.contains("OML") || fault.contains("LOCAL")){
                inAlarm = true;
            }
        }
        if(inAlarm){
            if(fault.contains("OML") || fault.contains("LOCAL"))
                return;
        }
        if(mo.contains("RXOTS")){
            return;
        }
        objects.append(Fault(mo, fault));
    }
    QString getTransferingGroup()
    {
        return tg;
    }
    QString getReasons()
    {
        return reasons;
    }
    void addReason(QString reason)
    {
        if(!reasons.endsWith('\n') && reasons.size()){
            reasons.append('\n');
        }
        reasons.append(reason);
    }
    bool compareTransferingGroup(QString mo)
    {
        QStringList splitted = mo.split('-');
        if(splitted.size() > 1){
            return splitted[1] == this->tg;
            /*if(splitted[1] == this->tg){
                return true;
            }*/
        }
        return false;
    }
    QStringList getAllObjectsInfo()
    {
        QString moty;
        for(auto obj: objects){
            QString data = obj.getData().join(' ');
            if(moty.size())
                moty.append('\n');
            moty.append(data.trimmed());
        }
        return {moty, rbs, reasons};
    }

private:
    QVector<Fault> objects;
    bool inAlarm;
    QString tg;
    QString reasons;
    QString rbs;
    QString getTransferingGroup(QString arg)
    {
        QStringList splittedText = arg.trimmed().split('-');
        if(splittedText.size() > 1){
            return splittedText[1];
        }
        return "-1";
    }
};

//#include <QDebug>

class FaultCollector
{
public:
    FaultCollector(){};
    void addObject(QString obj){
        QStringList splittedObject = obj.split(' ');
        addObject(splittedObject);
    }
    void addObject(QStringList splittedObject){
        QString mo = splittedObject[0];
        QString fault = "";
        QString rbs = "";
        if(splittedObject.size() > 1){
            rbs = splittedObject[1];
            for(int i = 2; i < splittedObject.size(); i++){
                fault.append(splittedObject[i] + ' ');
            }
        }
        addObject(mo, rbs, fault);
    }
    void addObject(QString mo, QString rbs, QString fault){
        placeTransferingGroup(mo)->addObject(mo, rbs, fault);
    }
    void addReasons(QString tg, QString reasons){
        //qDebug() << placeTransferingGroup(tg)->getReasons();
        placeTransferingGroup(tg)->addReason(reasons);
    }
    QVector<QStringList> getAllObjects(){
        QVector<QStringList> result;
        for(TransferingGroup object: faults){
            result.push_back(object.getAllObjectsInfo());
        }
        return result;
    }
    QStringList getAllTransferingGroupNum(){
        QStringList result;
        for(TransferingGroup object: faults){
            result.append(object.getTransferingGroup());
        }
        return result;
    }
    QStringList getAllTransferingGroupNoReasonNum(){
        QStringList result;
        for(TransferingGroup object: faults){
            if(object.getReasons().size()){}
            else
                result.append(object.getTransferingGroup());
        }
        return result;
    }

private:
    QVector<TransferingGroup> faults;
    TransferingGroup* placeTransferingGroup(QString mo){
        for(auto &fault: faults){
            if(fault.compareTransferingGroup(mo)){
                return &fault;
            }
        }
        faults.push_back(TransferingGroup());
        return &faults[faults.size() - 1];
    }
};

#endif // FAULTCOLLECTOR_H
