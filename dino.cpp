#include "dino.h"
#include <QDebug>
#include <QPicture>
#include <QSvgRenderer>

Dino::Dino(const QPointF &position, qreal width)
    : Movable(position, QPointF(0, 0)),
      coolDown(false),
      aligned(false),
      position(position),
      hstep(40),
      vstep(40),
      cdir(Direction::LEFT),
      width(width),
      speed(4)
{
    QSvgRenderer renderer(QString(":/data/dino1.svg"));
    QImage img = QImage(width, width, QImage::Format_ARGB32);
    QPainter painter(&img);
    renderer.render(&painter);
    dinoImageL = QPixmap::fromImage(img);
    dinoImageR = QPixmap::fromImage(img.mirrored(true, false));
}

void Dino::draw(QPainter *painter)
{
    painter->save();
    if(coolDown)
        painter->setOpacity(0.5);
    painter->translate(position.x(), position.y());
    switch(cdir) {
    case Direction::LEFT:
    case Direction::UP:
        painter->drawPixmap(0, 0, dinoImageL);
        break;
    case Direction::RIGHT:
    case Direction::DOWN:
        painter->drawPixmap(0, 0, dinoImageR);
        break;
    case Direction::STOP:
        painter->drawPixmap(0, 0, dinoImageR);
        break;
    default:
        break;
    }
    painter->restore();
}

void Dino::move(const QRect &bbox)
{
    if ((cdir == Direction::STOP && !aligned) || cdir != Direction::STOP) {
        if (cdir == Direction::STOP && (qint32)position.x() % hstep == 0
                && (qint32)position.y() % vstep == 0) {
            aligned = true;
        } else {
            position += vel;
        }
    }
    placeDino(bbox);
}

void Dino::placeDino(const QRect &bbox)
{
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
    //qDebug() << "DIno position:" << position;
}

QRectF Dino::rect()
{
    return QRectF(position.x() - width, position.y() - width,
                  2 * width, 2 * width);
}

void Dino::setDirection(Direction dir)
{
    cdir = dir;
    switch(dir) {
    case Direction::LEFT:
        vel = QPointF(-speed, 0);
        break;
    case Direction::RIGHT:
        vel = QPointF(speed, 0);
        break;
    case Direction::UP:
        vel = QPointF(0, -speed);
        break;
    case Direction::DOWN:
        vel = QPointF(0, speed);
        break;
    case Direction::STOP:
        vel = QPointF(0, 0);
        break;
    default:
        qDebug() << "Unhandled direction" << dir;
    }
}

bool Dino::collide(QPointF center, qreal radius)
{
    QRectF we(position.x(),position.y(),width, width);
    qreal hr = radius/2.0;
    QRectF them(center.x()-hr, center.y()-hr, radius, radius);
    return we.intersects(them);
}

void Dino::setCoolDown(bool cd)
{
    coolDown = cd;
}
