#include "telnetbuffer.h"

TelnetBuffer::TelnetBuffer()
{
    __buffer = "";
}

QString TelnetBuffer::readAll()
{
    return __buffer;
}

QString TelnetBuffer::getAll()
{
    QString result = __buffer;
    __buffer.clear();
    return result;
}

QString TelnetBuffer::getMessage()
{
    QStringList parts = __buffer.split("END");
    __buffer = "";
    if(parts.size() > 1){
        QString new_buffer("");
        for(int index = 1; index < parts.size(); index++){
            if (parts[index].size() > 10)
                new_buffer.append(parts[index] + " END");
        }
        __buffer = new_buffer + __buffer;
    }
    return parts[0];
}

bool TelnetBuffer::contains(QString &arg)
{
    bool result = __buffer.contains(arg, Qt::CaseSensitivity::CaseInsensitive);
    //if (result)  __buffer.clear();
    return result;
}

bool TelnetBuffer::contains(const char *text)
{
    QString normal_str(text);
    return contains(normal_str);
}
void TelnetBuffer::operator << (QString &text)
{
    this->operator+=(text);
}

void TelnetBuffer::operator += (QString &text)
{
    __buffer += text;
}
