#include "monster.h"
#include "gamehelper.h"

#include <QDebug>
#include <QPainter>
#include <QRandomGenerator>

Monster::Monster(QObject *parent)
    : GameObject(parent)
    , mGold(0)
    , mAtk(1)
    , mSpeed(30)
    , add(false)
    , slow(false)
    , mBlood(new Blood(this))
    , MonsterState(Crossing)
    , mCoolTime(2)
    , mCoolTimer(0)
{

}

void Monster::setSpeed(double speed)
{
    mSpeed = speed;
}

void Monster::setType(int type)
{
    mType = type;
    if(type == 0)
    {
        QPixmap pixmap(":/Test/monster/1.png");
        mCachePixmap = pixmap.copy(10, 15, pixmap.width() - 20, pixmap.height() - 30);   // 裁剪掉多余的空白部分
        mCachePixmap = mCachePixmap.scaled(mCachePixmap.width() * 0.7, mCachePixmap.height() * 0.7, Qt::KeepAspectRatio);
    }
    else
    {
        QPixmap pixmap(":/Test/monster/4.png");
        mCachePixmap = pixmap.copy(10, 15, pixmap.width() - 20, pixmap.height() - 30);   // 裁剪掉多余的空白部分
    }
    setBackground(mCachePixmap);
    mBlood->setRect(QRectF(0, 0, mCachePixmap.width(), 5));
    mBlood->maxHealth = 50;
    mBlood->currentHealth = mBlood->maxHealth;
}

//每帧更新怪物的位置
void Monster::update()
{
    int ex[4] = {0, 1, 0, -1}, ey[4] = {-1, 0, 1, 0}; // 依次为上右下左
    if(mType == 0) // 行走类怪物
    {
        if(MonsterState == Moving)
        {
            double newx = pos().x() + ex[moveDirc] * mSpeed * Time::deltaTime,
                   newy = pos().y() + ey[moveDirc] * mSpeed * Time::deltaTime;
            setPos(newx, newy);
            QRectF monsterCollidingRect = QRectF(QPointF(
                                                     newx + (width() - OriginRoadRect.width()) * 0.5,
                                                     newy + rect().height() - OriginRoadRect.height()),
                                                 OriginRoadRect.size());
            //QRectF originRect = QRectF(originRoad->scenePos(), originRoad->rect().size());
            if(!OriginRoadRect.intersects(monsterCollidingRect)) // 走出了原来的那个格子说明来到了新的格子（路）
            {
                MonsterState = Crossing;
                stX += ex[moveDirc];
                stY += ey[moveDirc];
            }
            return;
        }
        double newx = OriginRoadRect.x() + (OriginRoadRect.width() - width()) * 0.5,
               newy = OriginRoadRect.y() - rect().height() + OriginRoadRect.height(); // 除了正在移动的其它状态，将位置牢牢固定在当前的Road上
        setPos(newx, newy);
        /*if(MonsterState == Attack)
        {
            // 技能还在冷却当中
            if (mCoolTimer > 0) {
                mCoolTimer -= Time::deltaTime;
                //qDebug("MonsterCooling with %f\n",mCoolTimer);
                return;
            } else mCoolTimer = mCoolTime;
        }*/
    }
    if(mType == 1)
    {
        setPos(pos().x() + 2 * mSpeed * Time::deltaTime, pos().y());
    }
}

//吸收伤害
void Monster::takeDamage(int damage)
{
    //qDebug("monster ddddddddddddddddddddd blood %f\n",mBlood->currentHealth);
    mBlood->currentHealth -= damage;

    if (mBlood->currentHealth <= 0) {
        mBlood->currentHealth = 0;
        //qDebug("monster aaaaaaaaaaaaaaaaaaaaaa blood %f\n",mBlood->currentHealth);
        emit death(this);
    }
}

bool Monster::checkCooling()
{
    if (mCoolTimer > 0) {
        mCoolTimer -= Time::deltaTime;
        //qDebug("mcooltimer   %f\n",mCoolTimer);
        return true;
    } else {mCoolTimer = mCoolTime; return false;}
}

void Monster::slowSpeed(double slowSpeed)
{
    if(slow||slowSpeed==1){}
    else{
    mSpeed*=slowSpeed;
    if(slowSpeed==-1)slow = true;
    }
}

void Monster::addGold(int gold)
{
    if(add){}
    else{
        mGold*=gold;
        add = true;
    }
}

bool Monster::ifslow()
{
    return slow;
}
