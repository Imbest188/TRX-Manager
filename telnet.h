#ifndef TELNET_H
#define TELNET_H

#include <QObject>
#include <QTcpSocket>
#include <QString>


class Telnet : public QObject
{
    enum State {
        MML, Auth
    };

    enum ConnectionState {
        Disconnect, Connected, Connecting, Timeout
    };

    Q_OBJECT
public:
    explicit Telnet(QObject *parent = nullptr);
    ~Telnet() ;
    void connectToNE(const QString &host, const QString &user, const QString &password);
    void command (const QString &command) ;
    void command (const QStringList &commands) ;
    void operator << (QString &text) ;

signals:
    void errorOccured (const QString errorString);
    void commandExecuted( const QString responce);

private slots:
    void readSocketOutput();
    void handleAuth(const QString);
    void handleMML(const QString);

private:
    QTcpSocket telnet;
    State m_state = Auth;
    ConnectionState m_connectionState = Disconnect;
    QString m_user;
    QString m_host;
    QString m_password;
    QString m_lastPrint;
    QStringList m_commands;
    bool endFlag = false;
};

#endif // TELNET_H
