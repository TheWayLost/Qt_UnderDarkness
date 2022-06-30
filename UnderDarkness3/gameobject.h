#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QRectF>
#include <QPixmap>

class QPainter;

class GameObject : public QObject
{
    Q_OBJECT
public:
    explicit GameObject(QObject *parent = nullptr);
    virtual ~GameObject();

    double rotation() const { return mRotation; }
    double width() const { return mRect.width(); }
    double height() const { return mRect.height(); }
    QPointF pos() const { return mPos; }
    QPointF scenePos() const;
    QRectF rect() const { return mRect; }
    QPixmap background() const { return mBackground; }

    void setParentObject(GameObject *parent);
    void setRotation(double rotation);
    void setPos(const QPointF &pos);
    void setPos(double x, double y) { setPos(QPointF(x, y)); }
    void setRect(const QRectF &rect);
    void setBackground(const QPixmap &background);
    void addChild(GameObject *child);

    void copyOther(const GameObject &other);

    virtual void update();
    virtual void draw(QPainter *painter);
    virtual GameObject *duplicate() const;
    virtual bool selectTest(const QPointF &pos);
    virtual bool selectTestRect(const QRectF &rect);

protected:
    GameObject *mParent;
    double mRotation;
    QPointF mPos;
    QRectF mRect;
    QPixmap mBackground;
    QList<GameObject *> mChildren;
};

#endif // GAMEOBJECT_H
