#ifndef MONSTER_H
#define MONSTER_H


#include "gameobject.h"
#include "blood.h"
#include "tower.h"

class Blood;

class Monster : public GameObject
{
    Q_OBJECT
public:
    explicit Monster(QObject *parent = nullptr);

    int gold() const { return mGold; }
    int atk() const { return mAtk; }
    double speed() const { return mSpeed; }
    void setSpeed(double speed);
    void setType(int type);
    int type() const { return mType; }
    void takeDamage(int damage);
    void slowSpeed(double slowSpeed);
    void addGold(int gold);
    bool ifslow();
    bool checkCooling();
    virtual void update();

    int stX, stY;
    // 怪物所在的位置
    int moveDirc;
    // 正在移动时的方向
    int mType;
    // 怪物类型
    double mCoolTime;
    // 攻击冷却时间
    double mCoolTimer;
    // 攻击冷却计时器
    Road* originRoad;
    QRectF OriginRoadRect;
    enum State {
        Moving,
        // 从一格移到另一格的过程
        Crossing,
        // 位于格子正中心的一瞬间
        Attack
        // 正在攻击
    } MonsterState;
signals:
    void death(Monster *monster);

protected:
    int mGold;
    // 杀死怪物所得的金币值
    int mAtk;
    // 怪物攻击力
    double mSpeed;
    // 怪物移动速度
    bool add;
    bool slow;
    QPixmap mCachePixmap;
    Blood *mBlood;

};


#endif // MONSTER_H

/*
 * 怪物文档：
 * ----类型（mType）
 * 0 - 代表灾厄
 * 1 - 代表爪牙
 */
