#include "mainwindow.h"

#include <QApplication>
#include <QClipboard>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.resize(w.size().width()+1, w.size().height());
    return a.exec();
}
