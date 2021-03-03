#ifndef SEARCHLINE_H
#define SEARCHLINE_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>

class SearchLine : public QLineEdit
{
    Q_OBJECT
public:
    SearchLine(QWidget *parent=nullptr);
    void setRed();
signals:
    void enterPressed(QString, int);
public slots:
    void closeWindow();
    void onEnterPressed();
private:
    void checkLine();
    int searchIndex;
};

#endif // SEARCHLINE_H
