#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QObject>
#include <QLabel>

class CustomLabel : public QLabel
{
public:
    CustomLabel();
    CustomLabel(char* text) : QLabel(text) { }

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // CUSTOMLABEL_H
