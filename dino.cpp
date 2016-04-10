#include "dino.h"
#include <QDebug>
#include <QPicture>
#include <QSvgRenderer>

const int spriteTypeCnt = 4;
const int spriteW = 48;
const int spriteH = 62;
const int spriteLeft = 1;
const int spriteRight = 2;
const int spriteUp = 3;
const int spriteDown = 0;

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
    //QSvgRenderer renderer(QString(":/data/dino_sprites.png"));
    QImage img = QImage(QString(":/data/dino_sprites.png"));
    //QPainter painter(&img);
    //renderer.render(&painter);
    dinoImage = QPixmap::fromImage(img);
    spritePositions[UP] = 0;
    spritePositions[DOWN] = 0;
    spritePositions[LEFT] = 0;
    spritePositions[RIGHT] = 0;
}

void Dino::draw(QPainter *painter)
{
    QRect crop;
    painter->save();
    if(coolDown)
        painter->setOpacity(0.5);
    painter->translate(position.x(), position.y());
    crop = getSpriteCrop();
    dinoImageLast = dinoImage.copy(crop);

    painter->drawPixmap(0, 0, dinoImageLast);
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
    QRectF we(position.x(), position.y(), width, width);
    qreal hr = radius / 2.0;
    QRectF them(center.x() - hr, center.y() - hr, radius, radius);
    return we.intersects(them);
}

void Dino::setCoolDown(bool cd)
{
    coolDown = cd;
}

QRect Dino::getSpriteCrop()
{
    QRect rect;
    int xpos, ypos;
    if (cdir != Direction::STOP) {
        spritePositions[cdir]++;
    }
    if (spritePositions[cdir] >= spriteTypeCnt) {
        spritePositions[cdir] = 0;
    }
    xpos = spritePositions[cdir] * spriteW;

    switch(cdir) {
    case Direction::LEFT:
        ypos = spriteLeft * spriteH;
        break;
    case Direction::UP:
        ypos = spriteUp * spriteH;
        break;
    case Direction::RIGHT:
        ypos = spriteRight * spriteH;
        break;
    case Direction::DOWN:
        ypos = spriteDown * spriteH;
        break;
    case Direction::STOP:
    default:
        break;
    }
    rect = QRect(xpos, ypos, spriteW, spriteH);
    return rect;
}
