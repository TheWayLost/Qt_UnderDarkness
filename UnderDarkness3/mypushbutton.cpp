#include "mypushbutton.h"

#include <QPainter>
#include <QBitmap>

MyPushButton::MyPushButton(const QString &normal, const QString &pressed, QWidget *parent)
    : QPushButton(parent),
      mNormalPixmap(normal),
      mPressedPixmap(pressed)
{
    setCursor(Qt::PointingHandCursor);        // 鼠标形状
    setFixedSize(mNormalPixmap.size());    // 大小为图片的大小
}

void MyPushButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPixmap background = mNormalPixmap;

    if (isDown()) {
        painter.translate(0, 2);   // 稍微向下偏移
        if (!mPressedPixmap.isNull())
            background = mPressedPixmap;
    }

    painter.drawPixmap(rect(), background);
}
