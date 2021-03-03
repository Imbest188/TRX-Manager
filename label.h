#ifndef LABEL_H
#define LABEL_H
#include <QLabel>
#include <QEnterEvent>
#include <QThread>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QDebug>

class ActionTimer : public QThread
{
   Q_OBJECT
public:
    ActionTimer(int msec = 2){
        treshold = msec;
    };
signals:
    void finished();
public slots:
    void start(){QThread::msleep(treshold); emit finished();};
private:
    int treshold;
};

class SelfThread : public QThread
{
    Q_OBJECT
public:
    SelfThread(){
        rotation = false;
        connectedWidgets = 0;
        connect(this, &SelfThread::start, &timer, &ActionTimer::start);
        connect(&timer, &ActionTimer::finished, this, &SelfThread::partFinished);
        timer.moveToThread(&thread);
        thread.start();
    };
    ~SelfThread(){
        thread.terminate();
        thread.wait();
        thread.deleteLater();
    }
signals:
    void finished();
    void start();
public slots:
    void startRotation(){
        connectedWidgets += 1;
        if(!rotation){
            emit start();
        }
        rotation = true;
    };
    void stopRotation(){
        connectedWidgets -= 1;
        //rotation = false;
    };
    void partFinished(){
        emit finished();
        if(rotation && connectedWidgets){
            emit start();
        } else {
            rotation = false;
        }
    };

private:
    ActionTimer timer;
    QThread thread;
    bool rotation;
    int connectedWidgets;
};

class Label : public QLabel
{
    Q_OBJECT
public:
    Label(QWidget *parent = 0);
    ~Label(){rotationThread->stopRotation(); rotationThread->terminate(); if(originalPixmap)delete originalPixmap;};
protected:
    void enterEvent(QEvent *event) override ;
    void leaveEvent(QEvent *event) override ;
    bool event(QEvent *event) override;
signals :
    void hovered();
    void leave();
    void startRotation();
    void clicked();
public slots:
    void setThread(SelfThread *rotThread);
    void rotate();
    void changeVisibleMode(){inVisible = !inVisible; this->setHidden(inVisible);};
private:
    bool rotateState;
    QPropertyAnimation* _propertyAnimation;
    bool onMoved;
    int animated;
    int currentAngle;
    bool back;
    bool inVisible;
    QPixmap *originalPixmap;

    SelfThread *rotationThread;

    void setLowOpacity();
};

#endif // LABEL_H
