#include "telnetericsson.h"

#include <qdebug.h>

TelnetEricsson::TelnetEricsson()
{
    QObject::connect(&telnet, &QTcpSocket::readyRead, this, &TelnetEricsson::dataReceived);
    state = "auth";
    mml = false;
    busy = false;
}

TelnetEricsson::~TelnetEricsson()
{
    telnet.disconnectFromHost();
}

void TelnetEricsson::connect(QString host, QString user, QString password)
{
     TelnetEricsson::host = host;
     TelnetEricsson::user = user;
     TelnetEricsson::password = password;
     telnet.connectToHost(host, 23);
}

void TelnetEricsson::send(QString command)
{

    TelnetEricsson::command.append(command);
    TelnetEricsson::command.removeDuplicates();
    if (!busy)
        checkState("");
}

void TelnetEricsson::operator <<(QString &command)
{
    send(command);
}

void TelnetEricsson::dataReceived()
{
    QString responce = QString(telnet.readAll());
    if (checkState(responce)){
        buffer += responce;
        while(buffer.contains("END")){
            emit answerReceived(buffer.getMessage());
        }
    }
    else {}
}

void TelnetEricsson::auth(QString message)
{
        if(message.contains("login name", Qt::CaseSensitivity::CaseInsensitive)){
            telnet.write(user.toUtf8() + "\r\n");
        }
        else if(message.contains("password", Qt::CaseSensitivity::CaseInsensitive)){
            telnet.write(password.toUtf8() + "\r\n");
        }
        else if(message.contains("domain", Qt::CaseSensitivity::CaseInsensitive)){
            telnet.write("\r\n");
        }
        else if(message.contains("c:\\winnt\\profiles", Qt::CaseSensitivity::CaseInsensitive)){
            telnet.write("mml -a\r\n");
        }
        else if(message.contains("wo ", Qt::CaseSensitivity::CaseInsensitive) || message.contains("ex-", Qt::CaseSensitivity::CaseInsensitive))
        {
            state = "mml";
            checkState();
        }
}
#include <QThread>
bool TelnetEricsson::checkState(QString message)
{
    //CONNECTION INTERRUPTED
    if (message.contains("time out", Qt::CaseSensitivity::CaseInsensitive) || message.contains("\u0004", Qt::CaseSensitivity::CaseInsensitive)){
        telnet.write("\r\n");
        return false;
    }
    if (message.contains("login", Qt::CaseSensitivity::CaseInsensitive)){
        state = "auth";
    }
    if (message.contains("disconnect", Qt::CaseSensitivity::CaseInsensitive)){
        state = "auth";
        telnet.write("\u0003\r\n");
        return false;
    }
    if (state == "auth"){
        auth(message);
        return false;
    }
    else if(command.size()){
        while(command.size()){
            telnet.write("\r\n");
            busy = true;
            while (command.size()){
                try {
                    telnet.write(command[0].toUtf8() + "\r\n");
                }  catch (QException e) {
                    QFile file("telnetExceptions.log");
                    file.open(file.exists() ? QIODevice::Append : QIODevice::WriteOnly);
                    file.write(QString(e.what()).toUtf8() + '\n');
                }
                command.pop_front();
            }
            busy = false;
        }
    }
    else{
        return true;
    }
    return false;
}
