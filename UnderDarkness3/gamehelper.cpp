#include "gamehelper.h"
#include "gameobject.h"

void GameBorder::centerRect(GameObject *object)
{
    if (!object) return;

    QRectF rect = object->rect();
    rect.moveCenter(rect.topLeft());
    object->setRect(rect);
}

void GameBorder::topLeftRect(GameObject *object)
{
    if (!object) return;

    QRectF rect = object->rect();
    rect.moveTopLeft(rect.center());
    object->setRect(rect);
}

double Math::abs(double a)
{
    if(a>=0) return a;
    else return -a;
}

QList<Bullet *> GameManager::TotalBullet;

const double Time::FPS = 60;
double Time::deltaTime = 0;
QTime Time::lastTime;
