#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QDebug>

#include "customlabel.h"
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    //Нужно вынести в отдельную функцию)
    model.setWidget(ui->tableView);
    searchLine = new SearchLine(this);
    searchLine->setHidden(true);
    setupButtons();
    bsc1.connect("host","login","password");
    bsc3.connect("host","login","password");

    connect(&bsc1, &TelnetCommandsHandler::executionCompleted, this, &MainWindow::addToTable);//[=]
    connect(&bsc1, &TelnetCommandsHandler::faultCodesReceived, this, &MainWindow::addFaultToRow);

    connect(&bsc3, &TelnetCommandsHandler::executionCompleted, this, &MainWindow::addToTable);//[=]
    connect(&bsc3, &TelnetCommandsHandler::faultCodesReceived, this, &MainWindow::addFaultToRow);


    ui->gridLayout->setContentsMargins(0, 0, 0, 0);

    bsc1.moveToThread(&test1);
    bsc3.moveToThread(&test2);
    test1.start();
    test2.start();
    keyCtrlF = new QShortcut(this);
    keyCtrlC = new QShortcut(this);
    keyCtrlF->setKey(Qt::CTRL + Qt::Key_F);
    keyCtrlC->setKey(Qt::CTRL + Qt::Key_C);
    connect(keyCtrlF, SIGNAL(activated()), this, SLOT(slotShortcutCtrlF()));
    connect(keyCtrlC, SIGNAL(activated()), this, SLOT(copyToBuffer()));

    connect(this, &MainWindow::startWorking, &bsc1, &TelnetCommandsHandler::getAlarms);
    connect(this, &MainWindow::startWorking, &bsc3, &TelnetCommandsHandler::getAlarms);


    connect(searchLine, SIGNAL(enterPressed(QString, int)), this, SLOT(search(QString, int)));
    connect(this, SIGNAL(closeChildrenWin()), searchLine, SLOT(closeWindow()));
    connect(this, SIGNAL(keyEnterPressed()), searchLine, SLOT(onEnterPressed()));
    searchLine->setPlaceholderText("Найти..");
    searchLine->setGeometry(this->size().width() - searchLine->size().width() - 70, 20, searchLine->size().width(), searchLine->size().height());
    ui->tableView->setStyleSheet("QTableView:focus {  background-color: white; }");
    ui->tableView->setStyleSheet("QTableView {  background-color: white; }");
    ui->tableView->setSortingEnabled(true);


    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));

    emit startWorking();
}

MainWindow::~MainWindow()
{
    delete ui;
    emit closeChildrenWin();
    test1.terminate();
    test2.terminate();
    test1.wait();
    test2.wait();
    delete searchLine; //->deleteLater();
}

void MainWindow::copyToBuffer(int mode)
{
    if(ui->tableView->selectionModel()->selection().size()){
        QClipboard *clip = QApplication::clipboard();
        clip->clear();
        QString text;
        QList<int> rows;
        for(auto currentRow : ui->tableView->selectionModel()->selection().indexes())
            if(!rows.contains(currentRow.row()))
                    rows.append(currentRow.row());
        for(auto currentRow :rows ){
            if(mode == 3)
                text.append(ui->tableView->model()->index(currentRow, 1).data().toString() + '\n');
            if(mode != 2)
                text.append(ui->tableView->model()->index(currentRow, 0).data().toString() + '\n');
            if(mode != 1)
                text.append(ui->tableView->model()->index(currentRow, 2).data().toString() + '\n');
        }
        clip->setText(text);
    }
}

void MainWindow::copyToBuffer()
{
    copyToBuffer(3);
}

void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    QMenu * menu = new QMenu(this);
    QAction * copyDevice1 = new QAction(trUtf8("Копировать первые столбцы"), this);
    QAction * copyDevice2 = new QAction(trUtf8("Копировать последние столбцы"), this);
    QAction * copyDevice3 = new QAction(trUtf8("Копировать все столбцы"), this);
    connect(copyDevice1, &QAction::triggered, this, [=] () {copyToBuffer(1);});
    connect(copyDevice2, &QAction::triggered, this, [=] () {copyToBuffer(2);});
    connect(copyDevice3, &QAction::triggered, this, [=] () {copyToBuffer(3);});

    menu->addAction(copyDevice1);
    menu->addAction(copyDevice2);
    menu->addAction(copyDevice3);
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    qDebug() << ui->tableView->model()->index(index.row(), 0).data().toString();
}

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::addToTable(QVector<QStringList> result)
{
    for(int row = 0; row < result.size(); row++){
        if(result[row].size() > 1){
            faults.addObject(result[row]);
        }
    }

    for(auto tg:faults.getAllTransferingGroupNum()){
        faults.addReasons(tg, bsc3.getFaultsFromCF(tg));
    }

    for(auto obj:faults.getAllObjects()){
        model.addRow(obj);
    }
    checkFaultsOnTG(faults.getAllTransferingGroupNum());
}

void MainWindow::addFaultToRow(QString tg)
{
    model.addFaultToRow(tg, bsc3.getFaultsFromCF(tg));
}

void MainWindow::slotShortcutCtrlF()
{
    if(searchLine->isHidden()){
        searchLine->setGeometry(this->size().width() - searchLine->size().width() - 70, 20, searchLine->size().width(), searchLine->size().height());
        searchLine->setVisible(true);
        ui->tableView->setStyleSheet("QTableView {  background-color: grey; }");
        searchLine->setFocus();
        searchLine->selectAll();
    } else {
        emit closeChildrenWin();
        ui->tableView->setStyleSheet("QTableView {  background-color: white; }");
    }
}

void MainWindow::checkFaultsOnTG(QStringList transferingGroups)
{
    transferingGroups.removeDuplicates();
    for(int i = 0; i < transferingGroups.size(); i++){
        if(transferingGroups[i].size())
            bsc3.getFaultCodes(transferingGroups[i]);
    }
}

void MainWindow::moveButton(int x, int y)
{
    label->setGeometry(x, y, label->size().width(), label->size().height());
    label_2->setGeometry(x - 70, y, label->size().width(), label->size().height());
    label_3->setGeometry(x - 140, y, label->size().width(), label->size().height());
    label_4->setGeometry(x - 210, y, label->size().width(), label->size().height());
    searchLine->setGeometry(this->size().width() - searchLine->size().width() - 70, 20, 
                            searchLine->size().width(), searchLine->size().height());
    this->updateGeometry();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    moveButton(event->size().width() - 98, event->size().height() - 72);
    QMainWindow::resizeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Delete){
        if(ui->tableView->selectionModel()->selection().size()){
            QList<int> rows;
            for(auto currentRow : ui->tableView->selectionModel()->selection().indexes()){
                rows.append(currentRow.row());
            }
            QMessageBox msgBox(this);
            msgBox.setObjectName("Удаление объекта");
            if(rows.size() / 3 == 1){
                msgBox.setText("Вы уверены что хотите удалить строку?");
                msgBox.setInformativeText("Данные по сайту " + ui->tableView->model()->index(rows[0], 1).data().toString());
            } else {
                msgBox.setText("Вы уверены что хотите удалить строки?");
                msgBox.setInformativeText("Количество строк: " + QString::number(rows.size() / 3));
            }
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int ret = msgBox.exec();
            if(ret==QMessageBox::Ok){
                for(int row: rows)
                    ui->tableView->model()->removeRow(row);
                ui->tableView->selectionModel()->clear();
            } else {
                return;
            };
        }
    }
    if(e->key() == Qt::Key_Escape){
        ui->tableView->setStyleSheet("QTableView {  background-color: white; }");
        emit closeChildrenWin();
    } else if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return){
        qDebug() << "enter press";
        emit keyEnterPressed();
    }
    QMainWindow::keyPressEvent(e);
}

void MainWindow::setupButtons()
{
    label = new Label(this);
    label_2 = new Label(this);
    label_3 = new Label(this);
    label_4 = new Label(this);
    label->setScaledContents(true);
    label_2->setScaledContents(true);
    label_3->setScaledContents(true);
    label_4->setScaledContents(true);
    label->setPixmap(QPixmap(":/img/decoderDisabled.png"));
    label_3->setPixmap(QPixmap(":/img/excelButtonIcon3.png"));
    label_2->setPixmap(QPixmap(":/img/checkButtonIcon.png"));
    label_4->setPixmap(QPixmap(":/img/permanentButton.png"));


    label->setGeometry(1073, 650, 60, 60);
    moveButton(1073, 650);
    setRotationThread();

    connect(label_3, &Label::clicked, [=] {
        QStringList headers = {"MO","RSITE","OMT FAULT DESCRIPTION"};
        QString date = QString::number(QDate::currentDate().day()) + '.' + QString::number(QDate::currentDate().month()) + '.' + QString::number(QDate::currentDate().year());
        XlsxWritter::saveToFile(headers, model.getData(), QFileDialog::getSaveFileName(this, "Сохранить отчет", QDir::homePath() + "/Desktop/Список неработающих передатчиков " + date, "Excel (*.xlsx)"));
    });

    connect(label_2, &Label::clicked, [=] (){

    });
}

void MainWindow::on_pushButton_3_clicked()
{
    this->close();
    exit(0);
}

void MainWindow::search(QString arg, int searchIndex)
{
    QList<int> result;
    for(int i = 0; i < ui->tableView->model()->rowCount(); i++){
        QStringList columnInParts;
        for(int j = 0; j < 3; j++){
            if(ui->tableView->model()->index(i, j).data().toString().contains(arg)){
                if(!result.contains(i))
                    result.append(i);
            }
        }
    }
    if(result.size()){
        ui->tableView->selectRow(result[searchIndex % result.size()]);
    } else {
        searchLine->setRed();
        ui->tableView->selectionModel()->clear();
    }
}

void MainWindow::setRotationThread()
{
    this->rotationThread = new SelfThread();
    label->setThread(rotationThread);
    label_2->setThread(rotationThread);
    label_3->setThread(rotationThread);
    label_4->setThread(rotationThread);
}
