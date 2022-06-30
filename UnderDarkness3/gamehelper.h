#ifndef GAMEHELPER_H
#define GAMEHELPER_H

#include <QTime>

class GameObject;
class Bullet;

class GameBorder
{
public:
    static void centerRect(GameObject *object);
    static void topLeftRect(GameObject *object);
};

class GameManager
{
public:
   static QList<Bullet *> TotalBullet;
};

class Time
{
public:
    static const double FPS;
    // 帧率
    static double deltaTime;
    // 每帧所用的时间
    static QTime lastTime;
};

class Math
{
public:
    static double abs(double a);
};

#endif // GAMEHELPER_H
