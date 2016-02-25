#include "bubble.h"

Bubble::Bubble(const QPointF &position, const QPointF &velocity, qreal radius)
    : Movable(position, velocity), radius(radius)
{
    innerColor = randomColor();
    outerColor = randomColor();
    updateBrush();
}

void Bubble::updateBrush()
{
    QRadialGradient gradient(QPointF(radius, radius), radius,
                             QPointF(radius*0.5, radius*0.5));

    gradient.setColorAt(0, QColor(255, 255, 255, 255));
    gradient.setColorAt(0.25, innerColor);
    gradient.setColorAt(1, outerColor);
    brush = QBrush(gradient);
}

void Bubble::draw(QPainter *painter)
{
    painter->save();
    painter->translate(position().x() - radius, position().y() - radius);
    painter->setBrush(brush);
    painter->drawEllipse(0, 0, int(2*radius), int(2*radius));
    painter->restore();
}

QColor Bubble::randomColor()
{
    int red = int(205 + 50.0*qrand()/(RAND_MAX+1.0));
    int green = int(205 + 50.0*qrand()/(RAND_MAX+1.0));
    int blue = int(205 + 50.0*qrand()/(RAND_MAX+1.0));
    int alpha = int(91 + 100.0*qrand()/(RAND_MAX+1.0));

    return QColor(red, green, blue, alpha);
}

void Bubble::move(const QRect &bbox)
{
    _position += _vel;
    qreal leftOverflow = _position.x() - radius - bbox.left();
    qreal rightOverflow = _position.x() + radius - bbox.right();
    qreal topOverflow = _position.y() - radius - bbox.top();
    qreal bottomOverflow = _position.y() + radius - bbox.bottom();

    if (leftOverflow < 0.0) {
        _position.setX(_position.x() - 2 * leftOverflow);
        _vel.setX(-_vel.x());
    } else if (rightOverflow > 0.0) {
        _position.setX(_position.x() - 2 * rightOverflow);
        _vel.setX(-_vel.x());
    }

    if (topOverflow < 0.0) {
        _position.setY(_position.y() - 2 * topOverflow);
        _vel.setY(-_vel.y());
    } else if (bottomOverflow > 0.0) {
        _position.setY(_position.y() - 2 * bottomOverflow);
        _vel.setY(-_vel.y());
    }
}

QRectF Bubble::rect()
{
    return QRectF(_position.x() - radius, _position.y() - radius,
                  2 * radius, 2 * radius);
}
