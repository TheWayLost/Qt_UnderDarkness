#include "mainwindow.h"
#include "mypushbutton.h"
#include "mainscene.h"
#include <QPainter>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mStartButton(new MyPushButton(":/Test/startbutton.png", "", this))
    , mMainScene(new MainScene)
{
    setWindowIcon(QIcon(":/Test/WindowIcon.png")); // 设置图标
    setWindowTitle(tr("UnderDarkness")); // 设置标题
    mBackground = QPixmap(":/Test/startscene.png");  // 背景

    setFixedSize(1920, 1000);

    // 点击开始按钮，隐藏当前窗口，新建游戏窗口
    connect(mStartButton, &QPushButton::clicked, this, &QWidget::hide);
    connect(mStartButton, &QPushButton::clicked, this, &MainWindow::StartButtonClicked);
}

void MainWindow::StartButtonClicked()
{
    mMainScene->deleteLater();
    mMainScene = new MainScene;
    mMainScene->loadScene();
    mMainScene->show();
}

MainWindow::~MainWindow()
{
    delete mMainScene;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), mBackground);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    mStartButton->move((width() - mStartButton->width()) * 0.5, (height() - mStartButton->height()));
}
