#include "bullet.h"
#include "gamehelper.h"

#include <QDebug>

Bullet::Bullet(QObject *parent)
    : GameObject(parent)
    , speed(-200)
    , damageValue(100)
    , slowspeed(1)
    , addmoney(1)
{
}

void Bullet::copyOther(const Bullet &other)
{
    GameObject::copyOther(other);
    speed = other.speed;
    damageValue = other.damageValue;
    slowspeed = other.slowspeed;
    addmoney = other.addmoney;
}

Bullet *Bullet::duplicate() const
{
    Bullet *bullet = new Bullet;
    bullet->copyOther(*this);
    return bullet;
}

//更新子弹的位置 TODO
void Bullet::update()
{
    QTransform transfrom;
    transfrom.translate(pos().x(), pos().y());
    transfrom.rotate(rotation());
    transfrom.translate(0, speed * Time::deltaTime);
    //qDebug("bullet %f %f\n",pos().x(),pos().y());
    setPos(transfrom.map(QPointF()));
}
