#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>

class MyPushButton;
class MainScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void StartButtonClicked();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *event);

protected:
    QPixmap mBackground;
    MyPushButton *mStartButton;
    MainScene *mMainScene;

};
#endif // MAINWINDOW_H
