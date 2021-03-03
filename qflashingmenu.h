#ifndef QFLASHINGMENU_H
#define QFLASHINGMENU_H

#include <QObject>
#include <QWidget>
#include <QThread>
#include <QVector>
#include "label.h"

class QFlashingMenu : public QWidget
{
    Q_OBJECT
public:
    explicit QFlashingMenu(QWidget *parent = nullptr);

signals:

private:
    QThread *thread;
};

#endif // QFLASHINGMENU_H
