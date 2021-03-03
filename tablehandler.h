#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H

#include <QWidget>
#include <QTableView>
#include "QStandardItemModel"
#include "QStandardItem"
#include "faultdecoder.h"

class TableHandler
{
public:
    TableHandler();
    ~TableHandler();
    void setWidget(QTableView *widget);
    void addRow(QStringList row);
    void addFaultToRow(QString tg, QString fault);
    void clear();
    QList<QStringList> getData();
    //void addToRow(QString);
    QStringList existCodes(){
        return decoder.existCodes();
    };
    int find(QString, int);

private:
    QStandardItemModel model;
    QStandardItem item;

    QStringList horizontalHeader;
    QTableView *widget;
    FaultDecoder decoder;
    void resize();
};

#endif // TABLEHANDLER_H
