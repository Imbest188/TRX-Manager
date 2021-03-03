#include "telnetcommandshandler.h"

TelnetCommandsHandler::TelnetCommandsHandler()
{
    QObject::connect(&telnet, &TelnetEricsson::answerReceived, [=](QString s){
        parseData(s);
    });
}

void TelnetCommandsHandler::connect(QString host, QString user, QString password)
{
    telnet.connect(host, user, password);
}

bool TelnetCommandsHandler::getAlarms()
{
    telnet.send("RXASP:MOTY=RXOTG;");
    return true;
}

bool TelnetCommandsHandler::getFaultCodes(QString CF)
{
    telnet.send("RXMFP:MO=RXOCF-" + getCF(CF) + ",FAULTY,SUBORD;");
    return true;
}

QString TelnetCommandsHandler::getCF(QString cfString) // осторожно, костыль
{
    QStringList splittedString = cfString.split('-');
    if(splittedString.size() > 1){
        if(QString::number(splittedString[1].toInt()) == "0")
            return splittedString[1].split(',')[0].split('-')[0];
        return QString::number(splittedString[1].toInt()); // no to int()!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    if(QString::number(cfString.toInt()) == "0")
        return cfString.split(',')[0].split('-')[0];
    return QString::number(cfString.toInt());
}

QString TelnetCommandsHandler::getFaultsFromCF(QString CF)
{
    QString cf1 = getCF(CF);
    for(auto line: tmp.split('\n')){
        QStringList slist = line.split(' ');
        QString cf2 = getCF(slist[0]);
        if(cf1 == cf2){
            QString result;
            if(slist.size() > 1){
                for(int i = 1; i < slist.size(); i++)
                    if(slist[i].size()){
                        if(result.size())
                            result.append(',');
                        result += slist[i];
                    }
                return result;
            }
        }
    }
    return "";
}

QString TelnetCommandsHandler::getBrokenObjects()
{
    telnet.send("RXASP:MOTY=RXOCF;");
    return "";
}

bool TelnetCommandsHandler::isMyObject(QString arg)
{
    for(auto obj: alarms){
        if(obj.contains(arg)){
            return true;
        }
    }
    return false;
}

QString TelnetCommandsHandler::contain(QString arg)
{
    for(auto token: TelnetCommandsHandler::tokens){
        if(arg.contains(token, Qt::CaseSensitivity::CaseInsensitive)){
            return token;
        }
    }
    return "";
}

QString TelnetCommandsHandler::untilNextSpace(QString arg, int pos)
{
    QString result = "";
    if (arg.contains('-')){
        for(int i = pos; i < arg.size(); i++){
            if(arg[i] == ' ')
                break;
            result += arg[i];
        }
    }
    return result;
}

void TelnetCommandsHandler::parseData(QString executionResult)
{
    if(executionResult.contains("RXASP", Qt::CaseSensitivity::CaseInsensitive)){
        parseAlarms(executionResult);
    }
    if(executionResult.contains("<RXMFP", Qt::CaseSensitivity::CaseInsensitive)){
        parseErrorCode(executionResult);
    }
}

void TelnetCommandsHandler::parseAlarms(QString executionResult)
{
    QStringList result = executionResult.split("\r\n");
    for(int row = 5; row < result.size(); row++){
        alarms.append(result[row].split(QRegExp("\\s{2,}")));
    }
    emit executionCompleted(alarms);
}

void TelnetCommandsHandler::parseErrorCode(QString executionResult)
{
    QString result;
    QString currentToken = "RXOCF";
    QString mo = "";
    QStringList textInLine = executionResult.replace("\r","").split("\n");
    bool nextLineIsCode = false;
    for(auto line:textInLine){
        if (nextLineIsCode){
            QStringList codes = line.split(' ');
            for(auto code: codes){
                if (code.size()){
                    result += " F" + code.trimmed();
                }
            }
            nextLineIsCode = false;
        }
        else{
            QString tokenInLine = contain(line);
            if (tokenInLine.size()){
                currentToken = tokenInLine;
                QString maybeItsMo = untilNextSpace(line, line.indexOf(currentToken));
                if (maybeItsMo.size()){
                    mo = maybeItsMo;
                    if(result.size() > 1){
                        for(auto line: result.split('\n')){
                            tmp += result.split(' ')[0] + ' ' + generateFaultSequence(result) + '\n';
                        }
                        result.clear();
                    }
                }
            }

            if (line.contains("FAULT CODES CLASS")){
                if(result.size()){
                    result += '\n';
                }
                result += mo;
                if (line.contains("EXTERNAL")){
                    result += " EXT";
                }
                QStringList splittedLine = line.split(' ');
                result += ' ' + splittedLine[splittedLine.size() - 1];
                nextLineIsCode = true;
            }

            if (line.contains("REPLACEMENT UNITS")){
                result += " RU:";
                nextLineIsCode = true;
            }
        }
    }

    if(result.size() > 1 && result.split(' ')[0].size()){
        for(auto line: result.split('\n')){
            tmp += result.split(' ')[0].trimmed() + ' ' + generateFaultSequence(line) + '\n';
        }
        result.clear();
    }
    emit(faultCodesReceived(getCF(mo)));
}

void TelnetCommandsHandler::parseErrorLog(QString executionResult)
{
    executionResult.clear();
}

QString TelnetCommandsHandler::generateFaultSequence(QString faultString)
{
    if(!faultString.size()){
        return "";
    }
    QString result = "[6";
    if(faultString.contains("EXT")){
        result += 'E';
        faultString.replace(" EXT","");
    }
    else{
        result +='I';
    }
    QStringList separateReplacementUnit = faultString.split("RU: ");
    QStringList splittedFaultText = separateReplacementUnit[0].split(' ');
    QString mo = splittedFaultText[0].split('-')[0].replace("RXO","");
    if(mo == "TRX"){
        mo += 'C';
    }
    result += mo;
    result += splittedFaultText[1].trimmed();

    result += "F#] ";
    QString subResult = "";
    for (int i = 2; i < splittedFaultText.size(); i++){
        if(splittedFaultText[i].contains('F')){
            QString fault = splittedFaultText[i].trimmed().replace("F","");
            QString _result = result;
            subResult.append(_result.replace("F#", fault));
        }
    }

    if(separateReplacementUnit.size() > 1){
        //Добавить описание RU
    }
    return subResult + ' ';
}

QString TelnetCommandsHandler::uncode(QString arg)
{
    QString result;
    if(arg.size()){
        result += "2206 ";
        if(arg[2] == 'I'){
            result += "INTERNAL ";
        }
        else{
            result += "EXTERNAL ";
        }
        if(arg.contains("TRX")){
            result += "TRXC ";
            result += arg[7];
            result += arg[8];
            result += ' ';
            for(int i = 9; i < arg.size() - 2 && arg[i] != ']'; i++){
                result += arg[i];
            }
        }else{
            result += arg[3];
            result += arg[4];
            result += ' ';
            result += arg[5];
            result += arg[6];
            result += ' ';
            for(int i = 7; i < arg.size() - 2 && arg[i] != ']'; i++){
                result += arg[i];
            }
        }
    }
    return result;
}
