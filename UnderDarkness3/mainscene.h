#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QWidget>
#include <QMediaPlayer>

class GameObject;
class QPainter;
class Card;
class Tower;
class Monster;
class QMenu;
class Bullet;
class Road;
class MainScene : public QWidget
{
    Q_OBJECT

public:
    enum GameState {
        PauseState,
        // 暂停
        LoadingState,
        // 过场动画
        FailedState,
        // 游戏失败
        GamingState,
        // 游戏中
        SuccessState,
        // 游戏胜利
    };

    explicit MainScene(QWidget *parent = nullptr);
    ~MainScene();

    void setGameState(GameState state);

public slots:
    void loadScene();
    void updateObjects();
    void generateMonster();
    void clearAll();
    //void triggeredAction(QAction *action);
    void startGame();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void draw(QPainter *painter, GameObject *object);

    void drawPlayerInfo(QPainter *painter);
    void drawToBuffer();
    void collidingObjects();
    void stopTimer();
    int selectTestSitrect(const QPointF &pos);
    Card *findCard(int type);
    bool canBuyCard(Card *card);
    bool checkSceneSuccess();
    bool checkGameOver();
    QPointF check_surr(int index);
    bool check_tower_avail(int typ,int ind);
    int findBestPathBfs(int nx, int ny);

protected:
    GameState mGameState;
    // 游戏状态
    GameObject *mLeftBg;
    // 左边的背景
    GameObject *mRightBg;
    // 右边的背景
    GameObject *mTowerSit;
    // 炮塔底座
    Tower *mDragTower;
    // 当前在拖拽的炮塔
    QVector<int> mTypeCards;
    // 选取的炮塔类型
    QList<Card *> mCards;
    // 炮塔卡片
    QVector<QRectF> mTowerSitRect;
    // 保存炮塔底座位置
    QVector<QRectF> mRoadSitRect;
    // 保存道路位置
    QVector<Tower *> mTowers;
    // 炮塔
    QVector<Road *> mRoads;
    // 道路
    QVector<Monster *> mMonsters;
    // 怪物
    QTimer *mFPSTimer;
    // FPS计时器
    QTimer *mMonsterTimer;
    // 怪物生成计时器
    QTimer *mLoadingTimer;
    // 倒计时
    QMenu *mMenu;
    // 右键菜单
    QAction *mDeleteAction;
    // 拆除
    QAction *mUpgradeAction;
    // 升级
    QPixmap mBuffer;
    // 双缓冲绘图
    QRectF mClipRect;
    // 绘制范围
    int mPlayerHealth;
    // 玩家生命值
    int mPlayerGold;
    // 玩家金币值
    int mCardGold;
    // 卡片的金币值
    int mLoading;
    // 倒计时
    int mMaxTowers;
    // 最多携带的卡牌数量
    int mTarget;
    // 需要打败的飞行类怪物数量
    int mGeneTarget;
};
#endif // MAINSCENE_H
