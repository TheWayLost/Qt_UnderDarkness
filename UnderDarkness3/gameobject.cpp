#include "gameobject.h"

#include <QPainter>
#include <QDebug>

GameObject::GameObject(QObject *parent)
    : QObject(parent)
    , mRotation(0)
{
    mParent = qobject_cast<GameObject *>(parent);
    if (mParent)
        mParent->addChild(this);
}

GameObject::~GameObject()
{
    qDeleteAll(mChildren);
}

QPointF GameObject::scenePos() const
{
    QPointF result = pos();
    GameObject *parent = mParent;
    while (parent) {
        result += mParent->pos();
        parent = parent->mParent;
    }
    return result;
}

// 设置父对象
void GameObject::setParentObject(GameObject *parent)
{
    if (mParent)
        mParent->mChildren.removeOne(this);

    mParent = parent;
    setParent(parent);

    if (mParent)
        mParent->addChild(this);
}

//设置游戏物体旋转角度
void GameObject::setRotation(double rotation)
{
    mRotation = rotation;
}

//设置游戏物体在场景中的位置
void GameObject::setPos(const QPointF &pos)
{
    mPos = pos;
}

//设置游戏物体自身的坐标系
void GameObject::setRect(const QRectF &rect)
{
    mRect = rect;
}

//设置游戏物体背景
void GameObject::setBackground(const QPixmap &background)
{
    mBackground = background;

    if (mRect.isNull() && !background.isNull())
        setRect(QRectF(0, 0, background.width(), background.height()));
}

//添加子物体
void GameObject::addChild(GameObject *child)
{
    if (child && !mChildren.contains(child)) {
        child->setParent(this);
        mChildren.append(child);
    }
}

//复制 other
void GameObject::copyOther(const GameObject &other)
{
    setRect(other.rect());
    setPos(other.scenePos());
    setRotation(other.rotation());
    setBackground(other.background());

    mChildren.clear();
    foreach (GameObject *otherChild, other.mChildren)
        addChild(otherChild->duplicate());
}

//更新
void GameObject::update()
{

}

// 绘制游戏物体
void GameObject::draw(QPainter *painter)
{
    // 绘制自身背景
    if (!mBackground.isNull())
        painter->drawPixmap(mRect.toRect(), mBackground);

    // 绘制子物体
    foreach (GameObject *object, mChildren) {
        painter->save();
        painter->translate(object->pos());
        painter->rotate(object->rotation());
        object->draw(painter);
        painter->restore();
    }
}

// 复制一个物体
GameObject *GameObject::duplicate() const
{
    GameObject *object = new GameObject;
    object->copyOther(*this);
    return object;
}

bool GameObject::selectTest(const QPointF &pos)
{
    QPointF p = scenePos();
    QTransform transform;
    transform.translate(p.x(), p.y());
    transform.rotate(mRotation);
    QRectF mapRect = transform.mapRect(mRect);
    return mapRect.contains(pos);
}

//返回矩形 rect 是否与游戏物体相交，rect 为场景中的坐标
bool GameObject::selectTestRect(const QRectF &rect)
{
    QPointF p = scenePos();
    //qDebug("bullet scenepos %f %f\n",p.x(),p.y());
    QTransform transform;
    transform.translate(p.x(), p.y());
    transform.rotate(mRotation);
    QRectF mapRect = transform.mapRect(mRect);
    return mapRect.intersects(rect);
}
