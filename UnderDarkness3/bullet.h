#ifndef BULLET_H
#define BULLET_H

#include "gameobject.h"

class Bullet : public GameObject
{
public:
    explicit Bullet(QObject *parent = nullptr);

    void copyOther(const Bullet &other);

    virtual Bullet *duplicate() const;
    virtual void update();

    double speed;                       // 子弹移动速度
    int damageValue;                // 子弹伤害值
    double slowspeed;              //敌人减速值
    int addmoney;
};

#endif // BULLET_H
