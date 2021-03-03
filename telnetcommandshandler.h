#ifndef TELNETCOMMANDSHANDLER_H
#define TELNETCOMMANDSHANDLER_H

#include <QObject>
#include "telnetericsson.h"
#include <QVector>
#include <QThread>

class TelnetCommandsHandler: public QThread
{
    Q_OBJECT
public:
    TelnetCommandsHandler();
    void connect(QString host, QString user, QString password);
    //bool getAlarms();
    bool getFaultCodes(QString CF);
    QString getCF(QString cfString);
    QString tmp;
    QString getFaultsFromCF(QString);
    QString getBrokenObjects();
    TelnetEricsson telnet;
    bool isMyObject(QString);

signals:
     void executionCompleted(QVector<QStringList> result);
     QString faultCodesReceived(QString tg);

public slots:
    bool getAlarms();

private slots:
    //void getData(); 

private:
    //TelnetEricsson telnet;
    QString buffer;
    QVector<QStringList> alarms;
    QStringList tokens = {"RXOCF","RXOCON","RXOIS","RXORX","RXOTF","RXOTRX","RXOTS","RXOTX"};

    QString contain(QString);
    QString untilNextSpace(QString, int);
    void parseData(QString executionResult);
    void parseAlarms(QString executionResult);
    void parseErrorCode(QString executionResult);
    void parseErrorLog(QString executionResult);
    void parseBrokenObjects(QString executionResult);
    QString generateFaultSequence(QString faultString);
    QString uncode(QString);
};

#endif // TELNETCOMMANDSHANDLER_H
