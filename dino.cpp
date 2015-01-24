#include "dino.h"
#include <QDebug>
#include <QPicture>
#include <QSvgRenderer>

Dino::Dino(const QPointF &position, qreal width)
    : position(position), cdir(LEFT), width(width), speed(2)
{
    QSvgRenderer renderer(QString(":/data/dino1.svg"));
    QImage img = QImage(width, width, QImage::Format_ARGB32);
    QPainter painter(&img);
    renderer.render(&painter);
    dinoImageL = QPixmap::fromImage(img);
    dinoImageR = QPixmap::fromImage(img.mirrored(true, false));
}

void Dino::drawDino(QPainter *painter)
{
    painter->save();
    painter->translate(position.x(), position.y());
    switch(cdir) {
    case LEFT:
        painter->drawPixmap(0, 0, dinoImageL);
        break;
    case RIGHT:
        painter->drawPixmap(0, 0, dinoImageR);
        break;
    default:
        break;
    }
    painter->restore();
}

void Dino::move(const QRect &bbox)
{
    position += vel;
    qreal leftOverflow = position.x() - bbox.left();
    qreal rightOverflow = position.x() + width - bbox.right();
    qreal topOverflow = position.y() - bbox.top();
    qreal bottomOverflow = position.y() + width - bbox.bottom();

    if (leftOverflow < 0.0) {
        position.setX(position.x() - leftOverflow);
    } else if (rightOverflow > 0.0) {
        position.setX(position.x() - rightOverflow);
    }

    if (topOverflow < 0.0) {
        position.setY(position.y() - topOverflow);
    } else if (bottomOverflow > 0.0) {
        position.setY(position.y() - bottomOverflow);
    }
}

QRectF Dino::rect()
{
    return QRectF(position.x() - width, position.y() - width,
                  2 * width, 2 * width);
}

void Dino::setDirection(Dino::Direction dir)
{
    if (dir == LEFT || dir == RIGHT)
        cdir = dir;
    switch(dir) {
    case LEFT:
        vel = QPointF(-speed, 0);
        break;
    case RIGHT:
        vel = QPointF(speed, 0);
        break;
    case UP:
        vel = QPointF(0, -speed);
        break;
    case DOWN:
        vel = QPointF(0, speed);
        break;
    case STOP:
        vel = QPointF(0, 0);
        break;
    default:
        qDebug() << "Unhandled direction" << dir;
    }
}
