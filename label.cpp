#include "label.h"


Label::Label(QWidget *parent) : QLabel(parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground);
    this->setAttribute(Qt::WA_TranslucentBackground);
    rotateState = false;
    _propertyAnimation = new QPropertyAnimation(this,"geometry");
    _propertyAnimation->setDuration(1800);
    _propertyAnimation->setEasingCurve(QEasingCurve::OutCubic);
    onMoved = false;
    animated = 0;
    connect(_propertyAnimation, &QPropertyAnimation::finished,[=]{onMoved = !onMoved;});
    currentAngle = 0;
    originalPixmap = nullptr;//QPixmap(":/img/excelButtonIcon.png");
    back = false;
}

void Label::enterEvent(QEvent *event)
{
    emit hovered();
    rotateState = true;
    back = false;
    QLabel::enterEvent(event);
    return;
    if(!onMoved && animated != 1){
        animated = 1;
        QRectF firstPosition = this->geometry();
        QRectF endPosition = QRectF(this->geometry().x()-200, this->geometry().y(), this->geometry().width(), this->geometry().height());
        _propertyAnimation->setStartValue(firstPosition);
        _propertyAnimation->setEndValue(endPosition);
        _propertyAnimation->start();
        animated = 0;
    }
}

void Label::leaveEvent(QEvent *event)
{
    back = true;
    inVisible = true;
    return;
    emit leave();
    QLabel::leaveEvent(event);
    return;
    if(!onMoved && animated != -1){
        animated = -1;
        QRectF firstPosition = this->geometry();
        QRectF endPosition = QRectF(this->geometry().x()+200, this->geometry().y(), this->geometry().width(), this->geometry().height());
        _propertyAnimation->setStartValue(firstPosition);
        _propertyAnimation->setEndValue(endPosition);
        _propertyAnimation->start();
        animated = 0;
    }
}

bool Label::event(QEvent *event)
{
    if(event->type() == QEvent :: MouseButtonRelease)
        emit clicked();
    return QWidget::event(event);
}

void Label::setThread(SelfThread *rotThread)
{
    this->rotationThread = rotThread;
    connect(this, &Label::hovered ,rotationThread, &SelfThread::startRotation);
    connect(this, &Label::leave ,rotationThread, &SelfThread::stopRotation);
    connect(rotationThread, &SelfThread::finished, this, &Label::rotate);
    if(!originalPixmap){
        originalPixmap = new QPixmap(*this->pixmap());
    }
    setLowOpacity();
}
#include <QPainter>
void Label::rotate()
{
    if(rotateState){
        int angle = 3;
        if(back){
            if(!currentAngle){
                emit leave();
                rotateState = false;
                back = false;
                setLowOpacity();
                return;
            }
            if(360 - currentAngle > 180){
                angle = -3;
            }
        }

        currentAngle += angle;
        if(currentAngle == 360)
            currentAngle = 0;

        //back = false;
        QMatrix rm;
        rm.rotate(currentAngle);
        QPixmap rotated = originalPixmap->transformed(rm);
        QPixmap rotated2;

        int xoffset = (rotated.width() - originalPixmap->width()) / 2;
        int yoffset = (rotated.height() - originalPixmap->height()) / 2;
        rotated = rotated.copy(xoffset, yoffset, originalPixmap->width(), originalPixmap->height());

        this->setPixmap(rotated);
    }
}

void Label::setLowOpacity()
{
    QSize size = this->pixmap()->size();
    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter p(&image);
    p.setOpacity(0.4);
    p.drawPixmap(0, 0, *this->pixmap());
    p.end();

    QPixmap output = QPixmap::fromImage(image);
    this->setPixmap(output);
}
