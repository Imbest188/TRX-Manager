#include "tablehandler.h"

TableHandler::TableHandler()
{
    widget = 0;
}

TableHandler::~TableHandler()
{
    widget = 0;
}

void TableHandler::setWidget(QTableView *widget)
{
    horizontalHeader.append("MO");
    horizontalHeader.append("RSITE");
    horizontalHeader.append("OMT");
    widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //widget->setSelectionMode(QAbstractItemView::SingleSelection);
    model.setHorizontalHeaderLabels(horizontalHeader);
    this->widget = widget;
    widget->setModel(&model);
    widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void TableHandler::addRow(QStringList row)
{
    if (row.size() < 2 || !row[0].contains("RXO") || !row[1].contains("LU")){
        return;
    }
    int nextPosition = model.rowCount();
    for(int columnCount = 0; columnCount < row.size(); columnCount++)
    {
        model.setItem(nextPosition, columnCount, new QStandardItem(row[columnCount]));
    }
    resize();
   //widget->resizeRowsToContents();
   //widget->resizeColumnsToContents();
}

void TableHandler::addFaultToRow(QString tg, QString fault) //осторожно, костыль
{
    QString resultFault;
    for(auto subfault: fault.split(',')){
        QString singleFault = subfault;
        if(!singleFault.endsWith(']') && singleFault.size())
            singleFault.append(']');
        if(singleFault.size())
            resultFault.append(decoder.decode(singleFault));
    }
    for(int i = 0; i < widget->model()->rowCount(); i++){
        QStringList columnInParts = widget->model()->index(i, 0).data().toString().split('\n');
        if(columnInParts.size()){
            QStringList toTG = columnInParts[0].split('-');
            if(toTG.size() > 1){
                if(tg == toTG[1].split(' ')[0]){
                    model.setData(widget->model()->index(i, 2), resultFault);//fault);
                }
            }
        }
    }
    resize();
    //widget->resizeRowsToContents();
    //widget->resizeColumnsToContents();
}
void TableHandler::clear()
{
    while (widget->model()->rowCount()){
        widget->model()->removeRow(0);
    }
}

QList<QStringList> TableHandler::getData()
{
    QList<QStringList> result;
    for(int i = 0; i < widget->model()->rowCount(); i++){
        QStringList columnInParts;
        for(int j = 0; j < 3; j++){
            columnInParts.append(widget->model()->index(i, j).data().toString());
        }
        result.append(columnInParts);
    }
    return result;
}

int TableHandler::find(QString arg, int row = 0)
{
    for(int i = row; i < widget->model()->rowCount(); i++){
        QStringList columnInParts;
        for(int j = 0; j < 3; j++){
            if(widget->model()->index(i, j).data().toString().contains(arg)){
                return i;
            }
        }
    }
    return -1;
}
#include <QHeaderView>
void TableHandler::resize()
{
    widget->resizeRowsToContents();
    widget->resizeColumnToContents(0);
    widget->resizeColumnToContents(1);
    widget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}
