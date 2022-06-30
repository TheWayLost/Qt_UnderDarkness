#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(const QString &normal, const QString &pressed, QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *);

protected:
    QPixmap mNormalPixmap;
    QPixmap mPressedPixmap;
};

#endif // MYPUSHBUTTON_H
