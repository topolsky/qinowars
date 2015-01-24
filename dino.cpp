#include "dino.h"
#include <QDebug>
Dino::Dino(const QPointF &position, qreal width)
    : position(position), width(width)
{
}

void Dino::drawDino(QPainter *painter)
{
    painter->save();
    painter->translate(position.x() - width, position.y() - width);
    painter->drawPixmap(0, 0, QPixmap(":/data/dino.png").scaled(width, width));
    painter->restore();
}

void Dino::move(const QRect &bbox)
{
    position += vel;
    qreal leftOverflow = position.x() - width - bbox.left();
    qreal rightOverflow = position.x() + width - bbox.right();
    qreal topOverflow = position.y() - width - bbox.top();
    qreal bottomOverflow = position.y() + width - bbox.bottom();

    if (leftOverflow < 0.0) {
        position.setX(position.x() - 2 * leftOverflow);
        vel.setX(-vel.x());
    } else if (rightOverflow > 0.0) {
        position.setX(position.x() - 2 * rightOverflow);
        vel.setX(-vel.x());
    }

    if (topOverflow < 0.0) {
        position.setY(position.y() - 2 * topOverflow);
        vel.setY(-vel.y());
    } else if (bottomOverflow > 0.0) {
        position.setY(position.y() - 2 * bottomOverflow);
        vel.setY(-vel.y());
    }
}

QRectF Dino::rect()
{
    return QRectF(position.x() - width, position.y() - width,
                  2 * width, 2 * width);
}

void Dino::setDirection(Dino::Direction dir)
{
    switch(dir) {
    case LEFT:
        vel = QPointF(-10, 0);
        break;
    case RIGHT:
        vel = QPointF(10, 0);
        break;
    case UP:
        vel = QPointF(0, -10);
        break;
    case DOWN:
        vel = QPointF(0, 10);
        break;
    case STOP:
        vel = QPointF(0, 0);
        break;
    default:
        qDebug() << "Unhandled direction" << dir;
    }
}
