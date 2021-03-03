#ifndef TELNETERICSSON_H
#define TELNETERICSSON_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include "telnetbuffer.h"
#include <QException>
#include <QFile>

class TelnetEricsson : public QObject
{
    Q_OBJECT
public:
    TelnetEricsson();
    //TelnetEricsson(QString host, QString login, QString password);
    ~TelnetEricsson();

    void connect(QString host, QString user, QString password);
    void send(QString command);

   TelnetBuffer buffer;
   void operator << (QString &command) ;
   TelnetEricsson& operator ->() {
       return (*this);
   }

signals:
   QString answerReceived(QString answer);

private slots:
    void dataReceived();

private:
    QTcpSocket telnet;
    void auth(QString);

    QString host;
    QString user;
    QString password;
    QStringList command;
    QStringList commandBuffer;

    QString state;
    bool checkState(QString message="");
    bool mml;
    bool busy;
};

#endif // TELNETERICSSON_H
