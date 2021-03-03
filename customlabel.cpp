#include "customlabel.h"
#include <QDebug>
CustomLabel::CustomLabel()
{

}
void CustomLabel::enterEvent(QEvent *event) {

    qDebug() << "Entered!";
    // Change your text here
}

void CustomLabel::leaveEvent(QEvent *event) {

    qDebug() << "Left!";
}
