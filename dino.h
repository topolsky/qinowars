#ifndef DINO_H
#define DINO_H

#include <QPainter>


class Dino
{
public:
    enum Direction {
        STOP,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Dino(const QPointF &position, qreal width);

    void drawDino(QPainter *painter);
    void move(const QRect &bbox);
    QRectF rect();
    void setDirection(Direction dir);

private:
    QPointF position;
    QPixmap dinoImageL;
    QPixmap dinoImageR;
    Direction cdir;
    QPointF vel;
    qreal width;
    qreal speed;
};

#endif // DINO_H
