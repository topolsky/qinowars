#ifndef OBJECT_H
#define OBJECT_H

#include <QPainter>
#include "utils.h"

class Movable
{
public:
    Movable() { }
    Movable(const QPointF &pos, const QPointF &vel) :
        _position(pos), _vel(vel)
    { }
    virtual void draw(QPainter *painter) = 0;
    virtual void move(const QRect &bbox) = 0;
    virtual QRectF rect() = 0;
    void setPos(QPointF pos) { _position = pos; }
    QPointF position() { return _position; }
    void setVel(QPointF vel) { _vel = vel; }
    QPointF vel() { return _vel; }

protected:
    QPointF _position;
    QPointF _vel;
};

#endif // OBJECT_H
