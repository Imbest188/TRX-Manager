#include "searchline.h"

SearchLine::SearchLine(QWidget *parent)
    : QLineEdit(parent)
{
    this->setStyleSheet("QLineEdit {background-color: lightgreen;}");
    searchIndex = 0;
    connect(this,&QLineEdit::textChanged,[=](){
        searchIndex = 0;
        this->setStyleSheet("QLineEdit {background-color: lightgreen;}");
        checkLine();
    });
}

void SearchLine::setRed()
{
    this->setStyleSheet("QLineEdit {background-color: yellow;}");
}

void SearchLine::closeWindow()
{
    this->setHidden(true);
}

void SearchLine::onEnterPressed()
{
    qDebug() << "call enter on line";
    if(this->isVisible()){
        searchIndex++;
        checkLine();
    }
}

void SearchLine::checkLine()
{
    if(this->text().size()){
        emit enterPressed(this->text(), searchIndex);
    }
}
