#include "bubble.h"
#include "dino.h"
#include "glwidget.h"

#include <QMouseEvent>
#include <QApplication>

#include <math.h>
#include <stdlib.h>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

const int g_Width = 1240;
const int g_Height = 720;
const int g_HStep = 40;
const int g_WStep = 40;



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
    setMinimumSize(1200, 700);
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
    std::vector<std::unique_ptr<Bubble> >::iterator iter = bubbles.begin();
    for (; iter != bubbles.end(); iter++) {
        if ((*iter)->rect().intersects(event->rect()))
            (*iter)->draw(&painter);
    }
    dino->draw(&painter);
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
    /*QPointF position(width()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))),
                    height()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))));*/
    QPointF position(0, 0);
    createBubbles(10);
    dino = new Dino(position, 40);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    Direction dir;
    if (event->isAutoRepeat())
        return;

    qDebug() << "press" << event->key();
    dir = mapKey(event->key());
    keys[dir] = 1;
    dino->setDirection(dir);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;
    Direction dir = mapKey(event->key());
    keys.remove(dir);

    if (keys.empty()) {
        dino->setDirection(Direction::STOP);
    } else {
        dino->setDirection(keys.begin().key());
    }
}

QSize GLWidget::sizeHint() const
{
    return QSize(1200, 700);
}

void GLWidget::createBubbles(int number)
{
    for (int i = 0; i < number; ++i) {
        QPointF position(width()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))),
                        height()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))));
        qreal radius = qMin(width(), height())*(0.0125 + 0.0875*qrand()/(RAND_MAX+1.0));
        QPointF velocity(width()*0.0125*(-0.5 + qrand()/(RAND_MAX+1.0)),
                        height()*0.0125*(-0.5 + qrand()/(RAND_MAX+1.0)));
        std::unique_ptr<Bubble> newbb(new Bubble(position, velocity, radius));
        bubbles.push_back(std::move(newbb));
    }
}

Direction GLWidget::mapKey(int key)
{
    Direction dir;
    switch (key) {
    case Qt::Key_Left:
        dir = Direction::LEFT;
        break;
    case Qt::Key_Right:
        dir = Direction::RIGHT;
        break;
    case Qt::Key_Up:
        dir = Direction::UP;
        break;
    case Qt::Key_Down:
        dir = Direction::DOWN;
        break;
    }
    return dir;
}

void GLWidget::animate()
{
    std::vector<std::unique_ptr<Bubble> >::iterator iter = bubbles.begin();

    for(; iter != bubbles.end(); iter++) {
        (*iter)->move(rect());
    }
    dino->move(rect());
    update();
}

void GLWidget::stopDino()
{
    dino->setDirection(Direction::STOP);
    qDebug() << "Stop dino";
}

void GLWidget::checkCollision()
{
    dino->setCoolDown(false);
    std::vector<std::unique_ptr<Bubble> >::iterator iter = bubbles.begin();
    for(; iter != bubbles.end(); iter++) {
        if (checkBubbleCollision(**iter))
            break;
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

bool GLWidget::checkBubbleCollision(Bubble &bb)
{
    if (! dino->collide(bb.position(), bb.radius))
        return false;

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
        endGame();
    }
    return true;
}

void GLWidget::endGame()
{
    QString gameMessage = QString(tr("Thanks for playing. You played for %1 seconds. Thats end!"))
                                 .arg(overall.elapsed()/1000);
    QMessageBox::information(this, "Game Over", gameMessage);
    QApplication::quit();
}

void GLWidget::drawInstructions(QPainter *painter)
{
    QString text;
    if (!exitGame)
        text = tr("Máš ešte %1 životov, čas: %2").arg(lives).arg(overall.elapsed()/1000);
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
