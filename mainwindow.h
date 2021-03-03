#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFile>
#include "faultdecoder.h"

#include "tablehandler.h"
#include "telnetcommandshandler.h"
#include <QVector>
#include "faultcollector.h"
#include "label.h"
#include <QThread>
#include <QShortcut>
#include "searchline.h"
#include "xlsxwritter.h"
#include <QClipboard>
#include <QMenu>
#include <QAction>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:
    void startWorking();
    void closeChildrenWin();
    void keyEnterPressed();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();
    void addToTable(QVector<QStringList>);
    void addFaultToRow(QString tg);
    void slotShortcutCtrlF();
    void copyToBuffer(int mode);
    void copyToBuffer();
    void slotCustomMenuRequested(QPoint);

//    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
    void search(QString arg, int searchIndex);

private:
    Ui::MainWindow *ui;
    TableHandler model;
    TelnetCommandsHandler bsc1;
    TelnetCommandsHandler bsc3;
    FaultCollector faults;
    QVector<TelnetCommandsHandler> controllers;

    void setRotationThread();
    void checkFaultsOnTG(QStringList);

    QShortcut *keyCtrlF;
    QShortcut *keyCtrlC;
    QStringList tableRows;
    SearchLine *searchLine;
    Label *label;
    Label *label_2;
    Label *label_3;
    Label *label_4;

    void moveButton(int x, int y);
    virtual void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    void setupButtons();
    SelfThread *rotationThread;
    QThread test1, test2;
};
#endif // MAINWINDOW_H
