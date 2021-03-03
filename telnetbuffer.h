#ifndef TELNETBUFFER_H
#define TELNETBUFFER_H

#include <QString>
#include <QStringList>

class TelnetBuffer
{
public:
    TelnetBuffer();

    QString readAll();
    QString getAll();
    QString getMessage();
    bool contains(QString &text);
    bool contains(const char *text);
    void operator << (QString &text) ;
    void operator += (QString &text) ;
    QString __buffer;
private:
    //QString __buffer;
    size_t strlen(char *text);
};

#endif // TELNETBUFFER_H
