#include "mainscene.h"
#include "tower.h"
#include "gameobject.h"
#include "gamehelper.h"
#include "bullet.h"
#include "monster.h"
#include "mainscene.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QTime>
#include <QMenu>
#include <QMessageBox>
#include <QRandomGenerator>

MainScene::MainScene(QWidget *parent)
    : QWidget(parent)
    , mLeftBg(new GameObject(this))
    , mRightBg(new GameObject(this))
    , mTowerSit(new GameObject(mLeftBg))
    , mDragTower(nullptr)
    , mFPSTimer(new QTimer(this))
    , mMonsterTimer(new QTimer(this))
    , mLoadingTimer(new QTimer(this))
    , mPlayerHealth(10)
    , mPlayerGold(100)
    , mCardGold(0)
    , mMaxTowers(5)
    , mTarget(12)
    , mGeneTarget(12)
{
    setWindowIcon(QIcon(":/Test/WindowIcon.png")); // 设置图标
    setWindowTitle("UnderDarkness");

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    hide();
    setFixedSize(1875, 1000);
    mBuffer = QPixmap(1875, 1000);
    mBuffer.fill(Qt::transparent);

    // 背景图片
    QPixmap left = QPixmap(":/Test/forest.jpg"); // -------------TODO-------------
    QPixmap right = QPixmap(":/Test/forest_bar.png");// -------------TODO-------------
    left = left.copy(0, 0, left.width(), left.height());
    left = left.scaled(left.width(), 1000);
    right = right.scaled(1875 - left.width(), left.height());
    mLeftBg->setBackground(left);
    mRightBg->setBackground(right);
    mRightBg->setPos(QPointF(left.width(), 0));

    // 炮塔底座
    QPixmap towerSit = QPixmap(":/Test/towersit.jpg");
    towerSit = towerSit.scaled(0.8*left.width(),0.8*left.height());
    mTowerSit->setBackground(towerSit);
    mTowerSit->setRect(QRectF(0, 0, towerSit.width(), towerSit.height()));
    mTowerSit->setPos(QPointF((left.width() - towerSit.width()) * 0.5, (left.height() - towerSit.height()) * 0.5));

    // 裁切矩形，即怪物、子弹、炮塔都裁切进炮塔底座的范围之内
    mClipRect = QRectF(mTowerSit->scenePos() + QPointF(0, 10), towerSit.size() - QSizeF(0, 20));

    // 计算炮塔底座位置（每个底座对应的矩形）
    const int TowerRows = 8;
    const int TowerColumns = 12;

    double x = mTowerSit->scenePos().x();
    double y = mTowerSit->scenePos().y();
    double w = towerSit.width() / TowerColumns;
    double h = towerSit.height() / TowerRows;
    mTowerSitRect.resize(TowerRows * TowerColumns);
    mTowers.resize(TowerRows * TowerColumns);
    mRoads.resize(TowerRows * TowerColumns);

    for (int i = 0; i < TowerRows; ++i) {
        for (int j = 0; j < TowerColumns; ++j)
            mTowerSitRect[i * TowerColumns + j] = QRectF(x + j * w, y + i * h, w, h);
    }

    //tower的卡片位置
    x = 40;
    y = 28;
    for(int i=0;i<mMaxTowers;++i) {
        Card *card = new Card(mRightBg);
        int type = i+1;
        card->tower()->setType(type);
        card->setGold(card->tower()->Gold());
        mCards.append(card);
        if(i!=0) y += card->height()+1;
        card->setPos(QPointF(x,y));
    }

    //定时
    connect(mFPSTimer, &QTimer::timeout, this, &MainScene::updateObjects);//刷新地图物品
    connect(mMonsterTimer, &QTimer::timeout, this, &MainScene::generateMonster);//生成怪物
    connect(mLoadingTimer, &QTimer::timeout, this, [this]() {
        mLoading--;
        if (mLoading <= 0) {          // 倒计时完毕，开始游戏
            mLoadingTimer->stop();
            startGame();
        } else drawToBuffer();
    });
    //音乐（暂不做）
}

MainScene::~MainScene()
{
    clearAll();
}

void MainScene::setGameState(MainScene::GameState state)
{
    mGameState = state;

    switch (mGameState) {
    case PauseState:
        stopTimer();
        break;
    case LoadingState:
        break;
    case FailedState:
        stopTimer();
        QMessageBox::information(this, "提示", "很遗憾，游戏失败了，请再接再厉吧！");
        break;
    case GamingState:
        if (!mFPSTimer->isActive() || !mMonsterTimer->isActive()) {        // 如果计时器停止了，就重新开启它
            Time::lastTime = QTime();
            mFPSTimer->start();
            mMonsterTimer->start();
        }
        break;
    case SuccessState:
        stopTimer();
        QMessageBox::information(this, "提示", "恭喜你！赢得游戏了！");
        break;
    }
}

void MainScene::loadScene()
{
    // 替换左边的背景
    // int pix=QRandomGenerator::global()->bounded(1, 10);
    // QString path = QString(":/resources/map/map%1.jpg").arg(pix);
    // QPixmap leftBg = QPixmap(path);
    // mLeftBg->setBackground(leftBg.copy(0, 0, leftBg.width(), leftBg.height() - 200));

    stopTimer();
    clearAll();

    setGameState(LoadingState);
    mLoading = 2;
    mLoadingTimer->start(1000);
    drawToBuffer();
}

void MainScene::updateObjects()
{
    if (Time::lastTime.isNull()) {
        Time::lastTime = QTime::currentTime();
        setGameState(GamingState);
        return;
    }

    // 首先判断游戏是否结束
    if (checkGameOver()) {
        setGameState(FailedState);
        return;
    }

    // 之后判断当前场景是否结束（胜利）
    if (checkSceneSuccess()) {
        setGameState(SuccessState);
        return;
    }

    // 每帧所耗费的时间
    Time::deltaTime = Time::lastTime.msecsTo(QTime::currentTime()) / 1000.0;
    Time::lastTime = QTime::currentTime();

    // 处理碰撞
    collidingObjects();

    // 更新物体信息
    for (int i = 0; i < mTowers.size(); ++i) {
        if (!mTowers[i])
            continue;

        if(mTowers[i]->type()<=2&&check_surr(i)!=QPointF(-1,-1)) {
            mTowers[i]->update(check_surr(i));
        }
        if(mTowers[i]->type()==5&&check_surr(i)!=QPointF(-1,-1))
        {
            if(mTowers[i]->checkCooling()) continue;
            else
            {
                qDebug("tower5 prepared!!!!!!!!!!!\n");
                int tx = i/12, ty = i%12;
                for(Monster *mon : mMonsters)
                {
                    if(mon->type()==1) continue;
                    int nx = mon->stX, ny = mon->stY;
                    if((ny==tx+1)&&((nx==ty)||(nx==ty+1))) // 乐，由于是两个人写的，我们的x和y轴颠倒了
                    {
                        qDebug("Tower5 FIRE??????????????\n");
                        qDebug("%d                   nx==ty\n",nx==ty);
                        mon->stX = (i+11)%12;
                        mon->stY = (i+11)/12;
                        mon->MonsterState = Monster::Crossing;
                        mon->OriginRoadRect = mTowerSitRect.at(i+11);
                        QRectF rect = mTowerSitRect.at(i+11);
                        mon->setPos(rect.x() + (rect.width() - mon->width()) * 0.5, rect.y() - mon->rect().height() + rect.height());
                    }
                }
            }
        }
    }

    for (Bullet *bullet : GameManager::TotalBullet) {
        bullet->update();
        //qDebug("WHAT THE HELL!!!!!");
    }

    for (Monster *monster : mMonsters)
        monster->update();

    // 绘制到缓冲区
    drawToBuffer();
}

QPointF MainScene::check_surr(int ind)
{
    if(!mTowers[ind]) return QPointF(-1,-1);
    else
    {
        if(mTowers[ind]->type()==1||mTowers[ind]->type()==2||mTowers[ind]->type()==5)
        {
            Monster * tmp = new Monster(this);
            double resxx, resyy;
            double minlength = 100000000;
            double dis=0;
            for(Monster * mons : mMonsters)
            {
                int xx=mons->pos().x()+mons->rect().width()*0.5;
                int yy=mons->pos().y()+mons->rect().height()*0.5;
                dis = Math::abs(xx-mTowers[ind]->pos().x()) + Math::abs(yy-mTowers[ind]->pos().y());
                // qDebug("distance:     %f\n",dis);
                if( dis < minlength ) {
                    minlength = dis;
                    tmp = mons;
                    resxx = xx, resyy = yy;
                }
            }
            if(minlength < 1000) return QPointF(resxx, resyy);
            if(minlength >=1000) return QPointF(-1,-1);
        }
    }
}

void MainScene::generateMonster()
{
    if(mGeneTarget<=0) return;
    int index = qrand() % 8;         // 在哪条路径生成

    // 放个怪物测试一下
    Monster *monster = new Monster;

    QRectF rect = mTowerSitRect.at(index*12);
    monster->setType(1);
    monster->stX = -1;
    monster->stY = -1;
    monster->MonsterState = Monster::Crossing;
    monster->OriginRoadRect = mTowerSitRect.at(index*12);
    monster->setPos(0, rect.y() - monster->rect().height() + rect.height());
    mMonsters.append(monster);

    // 连接怪物死亡时的信号，将怪物删除掉，并且给玩家增加金币
    connect(monster, &Monster::death, [this](Monster *obj){
        mPlayerGold += obj->gold();
        mMonsters.removeOne(obj);
        obj->deleteLater();
        mTarget=mTarget-1;
        qDebug("mTarget is:         %d\n",mTarget);
    });

    mGeneTarget = mGeneTarget - 1;
    // 放个怪物测试一下

}


void MainScene::clearAll()
{
    // 清除所有炮塔
    qDeleteAll(mTowers);
    qDeleteAll(mRoads);
    for (int i = 0; i < mTowers.size(); ++i)
        mTowers[i] = nullptr;
    for (int i = 0; i < mRoads.size(); ++i)
        mRoads[i] = nullptr;

    // 清除所有怪物
    qDeleteAll(mMonsters);
    mMonsters.clear();

    // 清除所有子弹
    qDeleteAll(GameManager::TotalBullet);
    GameManager::TotalBullet.clear();
}

void MainScene::startGame()
{
    // 重新计时
    Time::lastTime = QTime();
    mFPSTimer->start(1000 / Time::FPS);
    mMonsterTimer->start(2000);

}

void MainScene::mousePressEvent(QMouseEvent *event)
{
    if (mGameState != GamingState)
        return;

    if (event->button() == Qt::LeftButton) {
        for (Card *card : mCards) {
            if (card->selectTest(event->pos()) && canBuyCard(card)) {   // 点击的卡片可以购买
                mDragTower = card->tower()->duplicate();                      // 复制一个炮塔
                GameBorder::centerRect(mDragTower);                             // 将其原点设置在中心
                mDragTower->setPos(event->pos());                                 // 将其位置设置为鼠标的位置
                mCardGold = card->gold();                                                // 保存当前卡片需要消耗的金币值
                break;
            }
        }
    }
}

void MainScene::mouseMoveEvent(QMouseEvent *event)
{
    if (mDragTower) {
        mDragTower->setPos(event->pos());
        int index = selectTestSitrect(event->pos());
        if(index==-1 || (check_tower_avail(mDragTower->type(),index)==false))
        {
            //TODO
        }
        update();
        return;
    }

    // 鼠标形状变化
    bool cursorChanged = false;
    for (Card *card : mCards) {
        if (card->selectTest(event->pos())) {
            cursorChanged = true;
            setCursor(Qt::PointingHandCursor);
            break;
        }
    }

    if (!cursorChanged)
        unsetCursor();
}

void MainScene::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && mDragTower) {
        int index = selectTestSitrect(event->pos());

        // 点击的不是炮塔底座或者当前底座已经存在炮塔了
        if(index==-1 || (check_tower_avail(mDragTower->type(),index)==false)) return;

        mPlayerGold -= mCardGold; // 减少玩家金币
        mCardGold = 0;
        mDragTower->enableBlood(true);  // 开启塔的血条显示
        mDragTower->setPos(mTowerSitRect.at(index).center());  // 设置塔位置

        Road* road1 = new Road(this);
        Road* road2 = new Road(this);
        Road* road3 = new Road(this);
        GameBorder::centerRect(road1);
        GameBorder::centerRect(road2);
        GameBorder::centerRect(road3);
        if(mDragTower->type()==1)
        {
            road1->setPos(mTowerSitRect.at(index-12).center());
            mRoads[index-12] = road1;
            road2->setPos(mTowerSitRect.at(index-11).center());
            mRoads[index-11] = road2;

            /*// 放个怪物测试一下

            Monster *monster = new Monster;

            QRectF rect = mTowerSitRect.at(index-11);
            monster->setType(1);
            monster->stX = -1;
            monster->stY = -1;
            monster->MonsterState = Monster::Crossing;
            monster->originRoad = road1;
            monster->OriginRoadRect = mTowerSitRect.at(index-11);
            monster->setPos(0, rect.y() - monster->rect().height() + rect.height());
            mMonsters.append(monster);

            // 连接怪物死亡时的信号，将怪物删除掉，并且给玩家增加金币
            connect(monster, &Monster::death, [this](Monster *obj){
                mPlayerGold += obj->gold();
                mMonsters.removeOne(obj);
                obj->deleteLater();
            });

            // 放个怪物测试一下*/
        }
        if(mDragTower->type()==2)
        {
            road1->setPos(mTowerSitRect.at(index-12).center());
            mRoads[index-12] = road1;
            road2->setPos(mTowerSitRect.at(index+1).center());
            mRoads[index+1] = road2;
            Tower * mDragTower2 = mDragTower->duplicate();
            mDragTower2->setPos(mTowerSitRect.at(index-11).center());
            mDragTower2->enableBlood(true);  // 开启塔的血条显示
            mTowers[index-11] = mDragTower2;
            mDragTower2 = nullptr;
            // 当塔死亡的时候销毁塔
            connect(mTowers[index-11], &Tower::death, [this, index](Tower *tower) {
                tower->deleteLater();
                mTowers[index-11] = nullptr;
            });
        }
        if(mDragTower->type()==3)
        {
            road1->setPos(mTowerSitRect.at(index+1).center());
            mRoads[index+1] = road1;
        }
        if(mDragTower->type()==5)
        {
            road1->setPos(mTowerSitRect.at(index+11).center());
            mRoads[index+11] = road1;
            road2->setPos(mTowerSitRect.at(index+12).center());
            mRoads[index+12] = road2;
            road3->setPos(mTowerSitRect.at(index+13).center());
            mRoads[index+13] = road3;
        }
        road1 = nullptr;road2=nullptr;road3=nullptr;

        mTowers[index] = mDragTower;
        mDragTower = nullptr;

        // 当塔死亡的时候销毁塔
        connect(mTowers[index], &Tower::death, [this, index](Tower *tower) {
            tower->deleteLater();
            mTowers[index] = nullptr;
        });

        update();
    }
}

bool MainScene::check_tower_avail(int typ, int ind)
{
    if(mTowers[ind]||mRoads[ind]) return false;
    int x = ind/12 ;
    int y = ind%12 ;
    switch (typ) {
    case 1:
        if(x==0||y==11) return false;
        if((!mTowers[ind-12])&&(!mTowers[ind-11])) return true;
        else return false;
    case 2:
        if(x==0||y==11) return false;
        if((!mTowers[ind-12])&&(!mTowers[ind-11])&&(!mTowers[ind+1])) return true;
        else return false;
    case 3:
        if(y==11) return false;
        if(!mTowers[ind+1]) return true;
        else return false;
    case 5:
        if(x==7||y==11||y==0) return false;
        if((!mTowers[ind+12])&&(!mTowers[ind+13])&&(!mTowers[ind+11])) return true;
        else return false;
    default:
        return true;
    }
}

void MainScene::contextMenuEvent(QContextMenuEvent *event)
{
    //TODO-------改成旋转方向，以及合并

    if (mDragTower) {   // 取消购买卡片
        mCardGold = 0;
        mDragTower->deleteLater();
        mDragTower = nullptr;
        update();
        return;
    }

    // 弹出菜单
    int index = selectTestSitrect(event->pos());
    if (index == -1 || !mTowers[index])
        return;

    bool advanceTower = mTowers[index]->isAdvanced();

    mMenu->clear();
    mMenu->addAction(mDeleteAction);

    // 如果不是高级塔的话可以进行升级
    if (!advanceTower) {
        mMenu->addAction(mUpgradeAction);
        Card *card = findCard(mTowers[index]->type() + 5);  // 对应的高级塔的卡片
        mUpgradeAction->setEnabled(canBuyCard(card));     // 当前金币是否能够升级，不能升级的话，升级功能被禁用
    }

    mMenu->move(QCursor::pos());
    mMenu->show();
}

void MainScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), mBuffer);
}

void MainScene::closeEvent(QCloseEvent *)
{

    stopTimer();
    clearAll();
}

void MainScene::draw(QPainter *painter, GameObject *object)
{
    if (!object) return;

    painter->save();
    painter->translate(object->scenePos());
    painter->rotate(object->rotation());
    object->draw(painter);
    painter->restore();
}

//绘制玩家信息
void MainScene::drawPlayerInfo(QPainter *painter)
{
    QRectF textRect = QRectF(width() - mRightBg->width(), 700, mRightBg->width() - 50, 300);

    //qDebug("here draw\n");

    QString text = QString("LIFE：%1\nGOLD：%2").arg(mPlayerHealth).arg(mPlayerGold);
    QFont font("Microsoft YaHei", 24);
    font.setBold(true);
    painter->setPen(Qt::yellow);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignTop | Qt::AlignLeft, text);
    painter->setPen(Qt::black);
}

void MainScene::drawToBuffer()
{
    mBuffer.fill(Qt::transparent);
    QPainter painter(&mBuffer);

    draw(&painter, mLeftBg);

    painter.save();
    painter.setClipRect(mClipRect); // 将绘制范围限制在底座范围之内

    // 绘制塔
    for (Tower *tower : mTowers)
        draw(&painter, tower);

    for(Road *roa : mRoads)
        draw(&painter, roa);

    // 绘制子弹
    for (Bullet *bullet : GameManager::TotalBullet){
        draw(&painter, bullet);
        //qDebug("GODDAMN ITTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n");
    }

    // 绘制怪物
    for (Monster *monster : mMonsters)
        draw(&painter, monster);

    painter.restore();

    // 绘制右边的背景
    draw(&painter, mRightBg);

    // 绘制玩家信息
    drawPlayerInfo(&painter);
    // 绘制鼠标拖动时的塔
    draw(&painter, mDragTower);

    // 绘制过场动画
    if (mGameState == LoadingState) {
        QString loadingText = QString("准备\n%1").arg(mLoading);
        QFont font("Microsoft YaHei", 24);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(mLeftBg->rect(), Qt::AlignCenter,  loadingText);
    }

    update();
}

int MainScene::findBestPathBfs(int nx, int ny)
{
    return QRandomGenerator::global()->bounded(1, 100);
}

void MainScene::collidingObjects()
{
    // 子弹飞出场景之外，删除子弹
    for (int i = GameManager::TotalBullet.size() - 1; i >= 0; --i) {
        Bullet *bullet = GameManager::TotalBullet.at(i);
        if (!bullet->selectTestRect(mClipRect)) {
            //qDebug("why here???? COLLIDING TESTTTTTTTTTTT? %d\n",GameManager::TotalBullet.size());
            //qDebug("%f %f\n",bullet->pos().x(),bullet->pos().y());
            //qDebug("%f %f %f %f\n",mClipRect.x(),mClipRect.y(),mClipRect.width(),mClipRect.height());
            GameManager::TotalBullet.removeAt(i);
            bullet->deleteLater();
        }
    }

    // 怪物与子弹的碰撞以及怪物与塔的碰撞
    for (int i = mMonsters.size() - 1; i >= 0; --i) {
        Monster *monster = mMonsters.at(i);

        // 怪物超出了可视范围，玩家承受伤害
        if (monster->scenePos().x() > mClipRect.right()) {
            mPlayerHealth -= monster->atk();    //减少玩家生命值
            if (mPlayerHealth < 0) mPlayerHealth = 0;
            monster->deleteLater();
            mMonsters.removeAt(i);
            continue;
        }

        QRectF rect = QRectF(monster->scenePos(), monster->rect().size()) & mClipRect;

        if(monster->mType == 0) // 地面类怪物
        {
            if(monster->MonsterState == Monster::Crossing)
            {
                monster->OriginRoadRect = mTowerSitRect.at(monster->stY * 12 + monster->stX); // 将新到达的格子设为origin
                monster->originRoad = mRoads[monster->stY * 12 + monster->stX];
                int dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0};
                for(int edirc = 0; edirc < 4; ++edirc)
                {
                    int nx = monster->stX + dx[edirc], ny = monster->stY + dy[edirc];
                    if(!(nx >= 0 && nx < 12 && ny >= 0 && ny < 8))
                        continue;
                    if(mTowers[nx + ny * 12]) // 周围有Tower
                    {
                        monster->MonsterState = Monster::Attack;
                        //qDebug("begin attack");
                        break;
                    }
                }
                if(monster->MonsterState == Monster::Attack) // 攻击塔
                {
                    if(!monster->checkCooling()){
                        //qDebug("attack");
                        for(int edirc = 0; edirc < 4; ++edirc)
                        {
                            int nx = monster->stX + dx[edirc], ny = monster->stY + dy[edirc];
                            if(!(nx >= 0 && nx < 12 && ny >= 0 && ny < 8))
                                continue;
                            if(mTowers[nx + ny * 12]) // 周围有Tower
                            {
                                mTowers[nx + ny * 12]->takeDamage(monster->atk());
                                break;
                            }
                        }
                    }
                    monster->MonsterState = Monster::Crossing;
                   /* for (int j = mTowers.size() - 1; j >= 0; --j) {
                        if (!mTowers[j]) continue;


                        // 塔承受伤害
                        if (rect.intersects(mTowerSitRect.at(j))) {
                            mTowers[j]->takeDamage(monster->atk());

                            monster->deleteLater();
                            //m_NormalDeath->play();
                            monster = nullptr;
                            mMonsters.removeAt(i);

                            break;
                        }
                    }*/
                }
                else // 周围没有塔，沿着路寻找最近的塔
                {
                    int bestdirc = 0, minCost = 1e5;
                    for(int edirc = 0; edirc < 4; ++edirc)
                    {
                        int nx = monster->stX + dx[edirc], ny = monster->stY + dy[edirc];
                        if(!(nx >= 0 && nx < 12 && ny >= 0 && ny < 8 && mRoads[nx + ny * 12]))
                            continue;
                        int res = findBestPathBfs(nx, ny);
                        //qDebug("%d", res);
                        if(res < minCost)
                        {
                            minCost = res;
                            bestdirc = edirc;
                        }
                    }
                    monster->MonsterState = Monster::Moving;
                    monster->moveDirc = bestdirc;
                }
            }
        }
        if(monster->mType == 1) // 飞行类怪物
        {
            // 路过Road时生成怪物
            double centerX = monster->pos().x() + monster->rect().width() * 0.5,
                   centerY = monster->pos().y() + monster->rect().height() * 0.5;
            for(int j = mRoads.size() - 1; j >= 0; --j)
            {
                if(mRoads[j] && mTowerSitRect.at(j).contains(centerX, centerY))
                {
                    if(!(monster->stX == (j % 12) && monster->stY == (j / 12)))
                    {
                        monster->stX = j % 12;
                        monster->stY = j / 12;
                        // 放个怪物测试一下

                        Monster *newmonster = new Monster;
                        QRectF rect = mTowerSitRect.at(j);
                        newmonster->setType(0);
                        newmonster->stX = j % 12;
                        newmonster->stY = j / 12;
                        newmonster->MonsterState = Monster::Crossing;
                        newmonster->originRoad = mRoads[j];
                        newmonster->OriginRoadRect = mTowerSitRect.at(j);
                        newmonster->setPos(rect.x() + (rect.width() - newmonster->width()) * 0.5, rect.y() - newmonster->rect().height() + rect.height());
                        mMonsters.append(newmonster);

                        connect(newmonster, &Monster::death, [this](Monster *obj){
                            mPlayerGold += obj->gold();
                            mMonsters.removeOne(obj);
                            obj->deleteLater();
                        });
                        // 放个怪物测试一下
                    }
                }
            }
        }

/*        // 与塔的碰撞
        for (int j = mTowers.size() - 1; j >= 0; --j) {
            if (!mTowers[j]) continue;

            // 塔承受伤害
            if (rect.intersects(mTowerSitRect.at(j))) {
                mTowers[j]->takeDamage(monster->atk());

                monster->deleteLater();
                //m_NormalDeath->play();
                monster = nullptr;
                mMonsters.removeAt(i);

                break;
            }
        }

        if (!monster)
            continue;*/

        // 与子弹的碰撞
        for (int j = GameManager::TotalBullet.size() - 1; j >= 0; --j) {
            Bullet *bullet = GameManager::TotalBullet.at(j);
            if (bullet->selectTestRect(rect)) {
                monster->takeDamage(bullet->damageValue);
                monster->addGold(bullet->addmoney);
                GameManager::TotalBullet.removeAt(j);
                bullet->deleteLater();
            }
        }
    }
}

void MainScene::stopTimer()
{
    mFPSTimer->stop();
    mMonsterTimer->stop();
}

//返回点击的是哪个炮塔底座
int MainScene::selectTestSitrect(const QPointF &pos)
{
    for (int i = mTowerSitRect.size() - 1; i >= 0; --i) {
        QRectF rect = mTowerSitRect.at(i);
        if (rect.contains(pos))
            return i;
    }
    return -1;
}

//根据type查找对应的卡片
Card *MainScene::findCard(int type)
{
    for (Card *card : mCards) {
        if (card->tower()->type() == type)
            return card;
    }
    return nullptr;
}

//是否可以购买卡片
bool MainScene::canBuyCard(Card *card)
{
    return (mPlayerGold >= card->gold());
}

//检查当前场景是否结束，结束条件是 boss 死亡，并且怪物列表为空 TODO

bool MainScene::checkSceneSuccess()
{
    return ((mTarget==0)&&(mPlayerHealth>0));
}

//检测游戏是否结束
bool MainScene::checkGameOver()
{
    return (mPlayerHealth <= 0);
}

