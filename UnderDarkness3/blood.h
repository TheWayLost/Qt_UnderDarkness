#ifndef BLOOD_H
#define BLOOD_H

#include "gameobject.h"

class Blood : public GameObject
{
public:
    explicit Blood(QObject *parent = nullptr);

    void copyOther(const Blood &other);

    virtual Blood *duplicate() const;
    virtual void draw(QPainter *painter);

    int currentHealth;
    int maxHealth;
};

#endif // BLOOD_H
