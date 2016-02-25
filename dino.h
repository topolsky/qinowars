#ifndef DINO_H
#define DINO_H


#include "movable.h"

class Dino : public Movable
{
public:
    Dino(const QPointF &position, qreal width);

    void draw(QPainter *painter);
    void move(const QRect &bbox);
    QRectF rect();
    void placeDino(const QRect &bbox);

    void setDirection(Direction dir);
    bool collide(QPointF center, qreal radius);
    void setCoolDown(bool cd);

private:
    bool coolDown;
    bool aligned;//finish moving into align position
    QPointF position;
    qint32 hstep;
    qint32 vstep;
    QPixmap dinoImageL;
    QPixmap dinoImageR;
    Direction cdir;
    QPointF vel;
    qreal width;
    qreal speed;
};

#endif // DINO_H
