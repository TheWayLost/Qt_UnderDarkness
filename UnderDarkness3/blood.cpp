#include "blood.h"
#include <QPainter>

Blood::Blood(QObject *parent)
    : GameObject(parent)
    , currentHealth(10)
    , maxHealth(10)
{
}

void Blood::copyOther(const Blood &other)
{
    currentHealth = other.currentHealth;
    maxHealth = other.maxHealth;
}

Blood *Blood::duplicate() const
{
    Blood *blood = new Blood;
    blood->copyOther(*this);
    return blood;
}

void Blood::draw(QPainter *painter)
{
    painter->setPen(Qt::NoPen);
    // 绘制底色
    painter->setBrush(Qt::black);
    painter->drawRect(rect());
    // 绘制血条
    painter->setBrush(Qt::red);
    painter->drawRect(QRectF(0, 0, width() * currentHealth / maxHealth, height()));
}
