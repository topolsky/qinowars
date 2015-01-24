#include "bubble.h"
#include "dino.h"
#include "glwidget.h"

#include <QMouseEvent>

#include <math.h>
#include <stdlib.h>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent), lives(1), exitGame(false)
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

    animationTimer.setSingleShot(false);
    connect(&animationTimer, SIGNAL(timeout()), this, SLOT(animate()));
    animationTimer.start(25);

    timer.setInterval(250);
    timer.setSingleShot(false);
    connect(&timer, SIGNAL(timeout()), this, SLOT(stopDino()));
    clsTimer.setInterval(200);
    clsTimer.setSingleShot(false);
    connect(&clsTimer, SIGNAL(timeout()), this, SLOT(checkCollision()));
    coolDown.setInterval(5000);
    coolDown.setSingleShot(true);
    connect(&coolDown, SIGNAL(timeout()), &clsTimer, SLOT(start()));
    coolDown.start();
    overall.start();
    //setAutoFillBackground(false);
    setMinimumSize(1200, 800);
    background = QPixmap(":/data/back01.jpg");
    setWindowTitle(tr("OJO DINOWARS"));

}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    glEnable(GL_MULTISAMPLE);
}

void GLWidget::paintEvent(QPaintEvent *event)
{
    makeCurrent();

    QPainter painter(this);
    painter.drawPixmap(0, 0, background.scaled(size()));
    painter.setRenderHint(QPainter::Antialiasing);
    foreach (Bubble *bubble, bubbles) {
        if (bubble->rect().intersects(event->rect()))
            bubble->drawBubble(&painter);
    }
    dino->drawDino(&painter);
    drawInstructions(&painter);
    painter.end();
}

void GLWidget::resizeGL(int width, int height)
{
    qreal pixelRatio = devicePixelRatio();
    setupViewport(width*pixelRatio, height*pixelRatio);
}

void GLWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    QPointF position(width()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))),
                    height()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))));
    createBubbles(10 - bubbles.count());
    dino = new Dino(position, 100);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        dino->setDirection(Dino::LEFT);
        break;
    case Qt::Key_Right:
        dino->setDirection(Dino::RIGHT);
        break;
    case Qt::Key_Up:
        dino->setDirection(Dino::UP);
        break;
    case Qt::Key_Down:
        dino->setDirection(Dino::DOWN);
        break;
    }
    timer.stop();
//    if (exitGame) {
//    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *)
{
    timer.start();
}

QSize GLWidget::sizeHint() const
{
    return QSize(1200, 800);
}

void GLWidget::createBubbles(int number)
{
    for (int i = 0; i < number; ++i) {
        QPointF position(width()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))),
                        height()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))));
        qreal radius = qMin(width(), height())*(0.0125 + 0.0875*qrand()/(RAND_MAX+1.0));
        QPointF velocity(width()*0.0125*(-0.5 + qrand()/(RAND_MAX+1.0)),
                        height()*0.0125*(-0.5 + qrand()/(RAND_MAX+1.0)));

        bubbles.append(new Bubble(position, radius, velocity));
    }
}

void GLWidget::animate()
{
    QMutableListIterator<Bubble*> iter(bubbles);

    while (iter.hasNext()) {
        Bubble *bubble = iter.next();
        bubble->move(rect());
    }
    dino->move(rect());
    update();
}

void GLWidget::stopDino()
{
    timer.stop();
    dino->setDirection(Dino::STOP);
    qDebug() << "Stop dino";
}

void GLWidget::checkCollision()
{
    dino->setCoolDown(false);
    QMutableListIterator<Bubble*> iter(bubbles);
    while (iter.hasNext()) {
        Bubble *bubble = iter.next();
        if (dino->collide(bubble->position, bubble->radius)) {
            qDebug() << "Collision!!";
            lives--;
            clsTimer.stop();
            coolDown.start();
            dino->setCoolDown(true);
            if (lives <= 0) {
                exitGame = true;
                this->stopDino();
                clsTimer.stop();
                coolDown.stop();
                QMessageBox::information(this, "Koniec hry",
                                         QString("Vďaka že si si zahral/a. Vydržal/a si %1 sekúnd. To je koniec!")
                                         .arg(overall.elapsed()/1000));
                exit(1);
            }
            break;
        }
    }
}

void GLWidget::setupViewport(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES
    glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::drawInstructions(QPainter *painter)
{
    QString text;
    if (!exitGame)
        text = tr("Máš ešte %1 životov").arg(lives);
    else
        text = tr("Koniec hry!");
    QFontMetrics metrics = QFontMetrics(font());
    int border = qMax(4, metrics.leading());

    QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
                                      Qt::AlignCenter | Qt::TextWordWrap, text);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
                     QColor(0, 0, 0, 127));
    painter->setPen(Qt::white);
    painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
                      QColor(0, 0, 0, 127));
    painter->drawText((width() - rect.width())/2, border,
                      rect.width(), rect.height(),
                      Qt::AlignCenter | Qt::TextWordWrap, text);
}
