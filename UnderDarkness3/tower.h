#ifndef TOWER_H
#define TOWER_H

#include "gameobject.h"

class Bullet;
class Blood;

class Tower : public GameObject
{
    Q_OBJECT
public:
    explicit Tower(QObject *parent = nullptr);
    ~Tower();
    bool isAdvanced() const { return mIsAdvance; }
    int AdvanceAble() const { return mAdvanceAble; }
    int Gold() const { return mGold; }
    int type() const { return mType; }
    double life();
    bool checkCooling();
    void setType(int type);
    void enableBlood(bool enable);
    void copyOther(const Tower &other);
    void takeDamage(int damage);
    virtual Tower *duplicate() const;
    virtual void update(QPointF pos);

signals:
    void death(Tower *tower);

protected:
    bool mIsAdvance;
    // 是否是高级炮塔
    int mAdvanceAble;
    // 升级对应类型，-1代表不可升级
    int mType;
    // 炮塔类型
    double mCoolTime;
    // 炮塔发射子弹冷却时间
    double mCoolTimer;
    // 炮塔发射子弹计时器
    int mGold;
    // 价格
    Bullet *mBullet;
    // 子弹
    Blood *mBlood;
    // 血条
};

class Card : public GameObject
{
    Q_OBJECT
public:
    explicit Card(QObject *parent = nullptr);

    int gold() const { return mGold; }
    void setGold(int gold);
    Tower *tower() const { return mTower; }

    virtual void draw(QPainter *painter);
protected:
    Tower *mTower;
    int mGold;
};

class Road : public GameObject
{
    Q_OBJECT
public:
    explicit Road(QObject *parent = nullptr);
protected:
    int mCost;
};

#endif // TOWER_H

/*
 * 防御塔文档：
 * ----类型（mType）
 * 1 - 代表箭塔
 * 2 - 代表双子塔
 * 3 - 代表巨石
 * 4 - 代表高墙(高级)
 * 5 - 代表引力塔
 */
