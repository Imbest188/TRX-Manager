#include "telnet.h"

Telnet::Telnet(QObject *parent) : QObject(parent)
{
    QObject::connect(&telnet, &QTcpSocket::readyRead, this, &Telnet::readSocketOutput);
}

Telnet::~Telnet()
{
    if( m_connectionState == Connected || m_connectionState == Timeout) {
        telnet.disconnectFromHost();
        qDebug() << QString("disconnected from host %1").arg(m_host);
    }
}

void Telnet::connectToNE(const QString& host, const QString& user, const QString& password)
{
    m_user = user;
    m_password = password;
    m_host = host;

    if(m_user.isEmpty() || m_host.isEmpty()) {
        emit errorOccured("host-adress or username is not defined.");
        return ;
    }

    m_connectionState = Connecting;
    telnet.connectToHost(host, 23);

}

void Telnet::command(const QString& command)
{
    m_commands << command;
    if(m_connectionState == Timeout) {
            telnet.write("\\u0003\r\n");
            m_connectionState = Connected;
    }
    if(m_commands.size() && !endFlag && m_connectionState == Connected) {
        telnet.write( m_commands.at(0).toUtf8() + "\r\n");
        m_commands.removeAt(0);
        endFlag = !endFlag;
    }
}

void Telnet::command(const QStringList& commands)
{
    m_commands << commands;
    command("");
}

void Telnet::operator <<(QString& text)
{
    command( text);
}

void Telnet::readSocketOutput()
{
    QString responce = QString(telnet.readAll());
    switch (m_state)
    {
        case Auth:
            handleAuth(responce);
            break;

        case MML:
            handleMML(responce);
            break;
    }
}

void Telnet::handleAuth(const QString responce)
{

    if (responce.contains("login name", Qt::CaseSensitivity::CaseInsensitive)) {
        telnet.write(m_user.toUtf8() + "\r\n");
    } else if (responce.contains("password", Qt::CaseSensitivity::CaseInsensitive)) {
        telnet.write(m_password.toUtf8() + "\r\n");
    } else if (responce.contains("domain", Qt::CaseSensitivity::CaseInsensitive)) {
        telnet.write("\r\n");
    } else if (responce.contains("c:\\winnt\\profiles", Qt::CaseSensitivity::CaseInsensitive)) {
        telnet.write("mml -a\r\n");
        m_state = MML;
        m_connectionState = Connected;
    }
}

void Telnet::handleMML(const QString responce)
{
    if(!responce.contains("mml -a") && !responce.contains("WO") && responce != "\u0003<")
        m_lastPrint.append( responce);

    if(responce.contains("END", Qt::CaseSensitivity::CaseInsensitive) && !responce.contains("DIPEND", Qt::CaseSensitivity::CaseInsensitive) &&
      !responce.contains("OPEND", Qt::CaseSensitivity::CaseInsensitive)) {
        emit commandExecuted( m_lastPrint);
        m_lastPrint.clear();
        endFlag = !endFlag;
    } else if(responce.contains("NOT ACCEPTED") || responce.contains("fault code", Qt::CaseSensitivity::CaseInsensitive)) {
        emit errorOccured( m_lastPrint);
        m_lastPrint.clear();
        endFlag = !endFlag;
    }
    else if(responce.contains("TIME OUT")) {
        m_connectionState = Timeout;
    }
    if(m_commands.size() && !endFlag) {
        telnet.write( m_commands.at(0).toUtf8() + "\r\n");
        m_commands.removeAt(0);
        endFlag = !endFlag;
    }
}
