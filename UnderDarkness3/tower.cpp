#include "tower.h"
#include "gamehelper.h"
#include "bullet.h"
#include "blood.h"

#include <QDebug>
#include <QPainter>
#include <QtMath>

Tower::Tower(QObject *parent)
    : GameObject(parent)
    , mIsAdvance(false)
    , mType(0)
    , mCoolTime(2)
    , mCoolTimer(0)
    , mBullet(nullptr)
    , mGold(0)
    , mBlood(nullptr)
{
    setType(1);
}

Tower::~Tower()
{
    delete mBullet;
}

//设置炮塔的类型
void Tower::setType(int type)
{
    if (mType == type) return;

    QString path, bulletPath;
    int damageValue = 1;                  // 子弹攻击力

    if (type > 0 && type <= 5) {        // 基础炮塔
        mIsAdvance = false;
        mType = type;
        damageValue = type * 5;
        path = QString(":/Test/tower%1primary.png").arg(type);
        bulletPath = QString(":/Test/Bullet%1Primary.png").arg(type);
    }

    //if(type==5) mCoolTime = 3;
    // 背景图片
    if (!path.isEmpty())
        setBackground(QPixmap(path).scaled(70,70));

    // 血条
    if (mBlood) {
        mBlood->setRect(QRectF(0, 0, width() - 10, 5));
        mBlood->setPos(-mBlood->width() * 0.5, -height() * 0.5);
        mBlood->maxHealth = mBlood->currentHealth = 20;
        if(type==5) mBlood->maxHealth = mBlood->currentHealth = 100;
        if(type==3) mBlood->maxHealth = mBlood->currentHealth = 300;
    }

    // 炮塔所拥有的子弹
    if (!bulletPath.isEmpty()) {
        delete mBullet;
        mBullet = new Bullet;
        mBullet->setBackground(bulletPath);
        mBullet->damageValue = damageValue;
        /*if(type==2||type==7){
            mBullet->slowspeed=0.5;
        }
        if(type==3||type==8){
            mBullet->slowspeed=-1;
        }
        if(type==4||type==9){
            mBullet->addmoney=2;
        }*/
    }
}

//开启/关闭血条显示，默认是关闭的
void Tower::enableBlood(bool enable)
{
    delete mBlood;
    mBlood = enable ? new Blood(this) : nullptr;

    if (mBlood) {
        mBlood->setRect(QRectF(0, 0, width() - 10, 5));
        mBlood->setPos(-mBlood->width() * 0.5, -height() * 0.5);
        mBlood->maxHealth = mBlood->currentHealth = 20;
    }
}

//复制other

void Tower::copyOther(const Tower &other)
{
    GameObject::copyOther(other);

    setType(other.type());

    mCoolTime = other.mCoolTime;
    mCoolTimer = other.mCoolTimer;

    if (mBullet) {
        mBullet->deleteLater();
        mBullet = nullptr;
    }

    if (mBlood) {
        mBlood->deleteLater();
        mBlood = nullptr;
    }

    if (other.mBullet)
        mBullet = other.mBullet->duplicate();

    if (other.mBlood)
        mBlood = other.mBlood->duplicate();
}

void Tower::takeDamage(int damage)
{
    if (!mBlood) return;

    mBlood->currentHealth -= damage;
    if (mBlood->currentHealth <= 0) {
        mBlood->currentHealth = 0;
        emit death(this);
    }
}

//复制一个炮塔
Tower *Tower::duplicate() const
{
    Tower *tower = new Tower;
    tower->copyOther(*this);
    return tower;
}

double Tower::life()
{
    return mBlood->currentHealth;
}

bool Tower::checkCooling()
{
    if (mCoolTimer > 0) {
        mCoolTimer -= Time::deltaTime;
        return true;
    } else {mCoolTimer = mCoolTime; return false;}
}

//炮塔每发射一个子弹，就进入冷却时间
void Tower::update(QPointF position)
{
    // 技能还在冷却当中
    if (mCoolTimer > 0) {
        mCoolTimer -= Time::deltaTime;
        //qDebug("BUGGGGGGGGGGG!    %f\n",mCoolTimer);
        return;
    } else mCoolTimer = mCoolTime;

    //qDebug("AIMINGGGGGGGGG %f %f\n",position.x(),position.y());

    int count=0;                  // 发射子弹个数
    double rotation=0;     // 子弹旋转角度

    if(mType==1||mType==2){
        count=1;
        if(position.y() == pos().y()){
            if(position.x() < pos().x()) rotation=-90;
            else rotation = 90;
        }
        if(position.y() < pos().y())
            rotation = - qAtan((position.x()-pos().x())/(position.y()-pos().y()));
        if(position.y() > pos().y())
            rotation = 3.1415926536 - qAtan((position.x()-pos().x())/(position.y()-pos().y()));
        rotation = rotation * 180 / 3.1415926536;
        //qDebug("rotatingggggggg %f",rotation);
    }

    for (int i = 0; i < count; ++i) {
        Bullet *bullet = mBullet->duplicate();
        bullet->setPos(pos().x(), pos().y());
        bullet->setRotation(rotation);
        GameBorder::centerRect(bullet);
        GameManager::TotalBullet.append(bullet);
        //qDebug("1bullet %f %f\n",bullet->pos().x(),bullet->pos().y());
    }
}

Card::Card(QObject *parent)
    : GameObject(parent)
    , mTower(new Tower(this))
    , mGold(0)
{
    setBackground(QPixmap(":/Test/cardsitdeep.png"));
    mTower->setPos(QPointF((width() - mTower->width()) * 0.5, (height() - mTower->height()) * 0.5));
}

void Card::setGold(int gold)
{
    mGold = gold;
}

void Card::draw(QPainter *painter)
{
    GameObject::draw(painter);

    QFont font("Microsoft YaHei", 24);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(QColor(255, 215, 0));
    painter->drawText(rect(), Qt::AlignCenter, QString::number(mGold));
}

Road::Road(QObject *parent)
    : GameObject(parent)
    , mCost(0)
{
    QString path;
    path = QString(":/Test/road.png");
    setBackground(QPixmap(path));
}
