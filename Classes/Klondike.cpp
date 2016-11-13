//
//  Klondike.cpp
//  KillTime
//
//  Created by keltonxian on 5/12/14.
//
//

#include "Klondike.h"

#include "LayerMenu.h"
#include "GameTool.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <platform/android/jni/JniHelper.h>
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
#include "KUtilIOS.h"
#endif

#define   LIST1  0
#define   LIST2  1
#define   LIST3  2
#define   LIST4  3
#define   LIST5  4
#define   LIST6  5
#define   LIST7  6
#define   DECK   7
#define   SLOT1  8
#define   SLOT2  9
#define   SLOT3  10
#define   SLOT4  11
#define   FLAG_CHANGE_POS   1
#define   FLAG_GATHER       2

#define   BAR_HEIGHT        50

#define   ACTION_NEW_POKER  80

#define   KEY_LIST_SIZE      "list_size"
#define   KEY_QUEUE          "queue"
#define   KEY_SCORE          "score"
#define   KEY_TIME           "time"
#define   KEY_RECORD_SCORE_1 "record_score_1"
#define   KEY_RECORD_SCORE_2 "record_score_2"
#define   KEY_RECORD_SCORE_3 "record_score_3"
#define   KEY_RECORD_TIME_1  "record_time_1"
#define   KEY_RECORD_TIME_2  "record_time_2"
#define   KEY_RECORD_TIME_3  "record_time_3"
#define   FILE_NAME          "KlondikeSaveData.kx"
#define   FILE_NAME_SCORE    "KlondikeScoreData.kx"

Scene* Klondike::createScene()
{
    auto scene = Scene::create();
    auto layer = Klondike::create();
    scene->addChild(layer, 1, 0);
    auto layerMenu = LayerMenu::create();
    layerMenu->setCallbackRestart(CC_CALLBACK_1(Klondike::callbackRestart, layer));
    layerMenu->setCallbackSave(CC_CALLBACK_1(Klondike::callbackSave, layer));
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    layerMenu->setCallbackRank(CC_CALLBACK_1(Klondike::callbackRank, layer));
#endif
    scene->addChild(layerMenu, 10, 0);
    return scene;
}

bool Klondike::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    canTouch = true;
    isGameOver = false;
    _lastDeckPoker = nullptr;
    _isLoop = false;
    
    auto bg = KUtil::addSprite(this, KUtil::getPath(F_KLO, "bg_klondike.png"), Point(HALF_WIDTH, HALF_HEIGHT), ANCHOR_CENTER_CENTER, 0);
    bg->setScale(KUtil::getScale(FULL_WIDTH, FULL_HEIGHT, bg->getContentSize(), true));
    
    auto layerColor = LayerGradient::create(Color4B(0, 0, 0, 0), Color4B(0, 0, 150, 100), Vec2(FULL_WIDTH, FULL_HEIGHT));
//    auto layerColor = LayerColor::create(Color4B(0, 0, 0, 100), FULL_WIDTH, FULL_HEIGHT);
    layerColor->setPosition(Point(0, 0));
    this->addChild(layerColor, 1);
    
    for (int i = SLOT1; i <= SLOT4; i++) {
        Point pf = getPokerPosition(i, 0);
        KUtil::addSprite(this, KUtil::getPath(F_KLO, "frame.png"), pf, ANCHOR_CENTER_CENTER, 1);
//        KUtil::addLabel(this, "X", 50, Color3B(255, 0, 0), pf, ANCHOR_CENTER_CENTER, PK_ZORDER_NORMAL-10);
    }
    
    Sprite *bgNum;
    bgNum = KUtil::addSprite(this, KUtil::getPath(F_KLO, "bg_num.png"), Point(FULL_WIDTH-50, FULL_HEIGHT-10), ANCHOR_RIGHT_UP, 200);
    KUtil::addSprite(bgNum, KUtil::getPath(F_KLO, "star.png"), Point(0, 23), ANCHOR_LEFT_CENTER, 1);
    bgNum = KUtil::addSprite(this, KUtil::getPath(F_KLO, "bg_num.png"), Point(FULL_WIDTH-300, FULL_HEIGHT-10), ANCHOR_RIGHT_UP, 200);
    KUtil::addSprite(bgNum, KUtil::getPath(F_KLO, "time.png"), Point(0, 23), ANCHOR_LEFT_CENTER, 1);
    
    this->_score = 0;
    this->_time = 0;
    char scorePath[500];
    KUtil::getPath(scorePath, 500, F_FONT, "font_replay.fnt");
    this->labelScore = KUtil::addLabelBmf(this, "0", scorePath, Point(FULL_WIDTH-60, FULL_HEIGHT-22), ANCHOR_RIGHT_UP, 250);
    
    this->labelTime = KUtil::addLabelBmf(this, "0", scorePath, Point(FULL_WIDTH-310, FULL_HEIGHT-22), ANCHOR_RIGHT_UP, 250);
    
    touchPoker = NULL;
    pokerPos = Point::ZERO;
    if (true != this->loadGame()) {
        PokerKlondike::getPack(&pack);
        setupPoker();
    }
    this->setScore(this->_score);
    setTime(this->_time);
    
    schedule(schedule_selector(Klondike::updateTime), 1.0f);
    
//    Vector<MenuItem *> itemArray;
//    
//    KUtil::addItemSpriteScale(&itemArray, F_IMAGE, "btn_pause.png", Point(10, FULL_HEIGHT-10), ANCHOR_LEFT_UP, CC_CALLBACK_1(Klondike::pauseGame, this));
//    
//    KUtil::addMenu(this, &itemArray, 30);
    
    return true;
}

void Klondike::pauseGame(cocos2d::Ref* pSender)
{
    auto scene = Director::getInstance()->getRunningScene();
    auto layerMenu = LayerMenu::create();
    layerMenu->setCallbackRestart(CC_CALLBACK_1(Klondike::callbackRestart, this));
    layerMenu->setCallbackSave(CC_CALLBACK_1(Klondike::callbackSave, this));
    layerMenu->setCallbackRank(CC_CALLBACK_1(Klondike::callbackRank, this));
    scene->addChild(layerMenu, 10, 0);
}

void Klondike::updateTime(float delta)
{
    if (isGameOver) {
        return;
    }
    setTime(++_time);
    if (_time % 10 == 0 && this->_score > 0) {
        this->_score -= 1;
        this->setScore(this->_score);
    }
}

void Klondike::setTime(int timeNum)
{
    _time = timeNum;
    int sec = timeNum%60;
    int min = ceil(timeNum/60);
    int hour = ceil(min/60);
    char scoreStr[10] = {0};
    memset(scoreStr, 0, 10);
    if (hour > 0) {
        min = min%60;
        sprintf(scoreStr, "%d%d:%d", hour, min, sec);
    }
    else if (min > 0) {
        sprintf(scoreStr, "%d:%d", min, sec);
    }
    else {
        sprintf(scoreStr, "%d", sec);
    }
    this->labelTime->setString(scoreStr);
}

void Klondike::setupPoker()
{
//    for_each(pack.begin(), pack.end(), CC_CALLBACK_1(Klondike::menuCloseCallback, this));
    ssize_t total = pack.size();
    auto defaultPos = getPokerPosition(DECK, 0);
    defaultPos.x += 100;
    
    for (int i = 0; i < total; i++) {
        auto poker = pack.at(i);
        poker->setPosition(defaultPos);
        poker->setAnchorPoint(ANCHOR_CENTER_CENTER);
        poker->setTag((int)total-i);
        this->addChild(poker, PK_ZORDER_NORMAL-1);
    }
    
    this->dealPoker();
}

void Klondike::dealPoker()
{
    canTouch = false;
    pokerList.clear();
    
    int total = (int)pack.size();
    
    if (total != POKERKLONDIKE_TYPE::KLO_DIAMOND_K) {
        return;
    }
    
    int index = 1;
    auto defaultPos = getPokerPosition(DECK, 0);
    defaultPos.x += 100;
    
    for (int i = 0; i < total; i++) {
        auto poker = pack.at(i);
        poker->turnBack();
        poker->setPosition(defaultPos);
    }
    
    float delay = 0;
    
    for (int i = LIST1; i <= LIST7; i++) {
        Vector<PokerKlondike *> list;
        for (int j = 0; j < i+1; j++, index++) {
            auto poker = pack.at(total-index);
            list.pushBack(poker);
            if (j == i) {
                poker->flipFront(delay, 0.2);
            }
            delay = this->moveAnimation(poker, getPokerPosition(i, j), delay, 0.1);
            delay -= 0.05;
            
        }
        // LIST1 ~ LIST7 : 0 ~ 6
        pokerList.push_back(list);
    }
    
    Vector<PokerKlondike *> list;
    for (int i = total-index; i >= 0; i--) {
        auto poker = pack.at(i);
        list.pushBack(poker);
        delay = this->moveAnimation(poker, getPokerPosition(DECK, 0), delay, 0.05);
    }
    this->scheduleOnce(schedule_selector(Klondike::callbackEnable), delay);
    // DECK   7
    pokerList.push_back(list);
    
    for (int i = 0; i < 4; i++) {
        Vector<PokerKlondike *> list;
        pokerList.push_back(list);
    }
}

void Klondike::callbackEnable(float delta)
{
    canTouch = true;
}

float Klondike::moveAnimation(Sprite *sprite, Point targetPos, float delay, float time)
{
    Vector<FiniteTimeAction *> list;
    FiniteTimeAction *action;
    action = DelayTime::create(delay);
    list.pushBack(action);
    action = CallFuncN::create(CC_CALLBACK_1(Klondike::callback_move, this));
    list.pushBack(action);
    action = MoveTo::create(time, targetPos);
    list.pushBack(action);
    action = Sequence::create(list);
    sprite->runAction(action);
    
    return delay + time;
}

void Klondike::callback_move(Node* pSender)
{
    int tag = pSender->getTag();
    this->reorderChild(pSender, PK_ZORDER_NORMAL+tag);
}

void Klondike::callbackRestart(Ref* pSender)
{
    reStart();
//    this->setPositionY(this->getPositionY() + 10);
}

void Klondike::callbackSave(Ref* pSender)
{
    this->saveGame();
}

void Klondike::callbackRank(Ref* pSender)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    KUtilIOS::showRank(IOS_KEY_SCORE_RANK);
#endif
}

void Klondike::reStart()
{
    this->isGameOver = false;
    this->_score = 0;
    this->setScore(this->_score);
    this->setTime(0);
    this->_lastDeckPoker = nullptr;
    this->_isLoop = false;
    PokerKlondike::randomPack(&pack);
    
    ssize_t total = pack.size();
    
    for (int i = 0; i < total; i++) {
        auto poker = pack.at(i);
        poker->stopAllActions();
        poker->setTag((int)total-i);
        this->reorderChild(poker, i+1);
    }
    
    this->dealPoker();
}

Point Klondike::getPokerPosition(int listType, int index)
{
    Size size = Director::getInstance()->getVisibleSize();
    Point pos = Point::ZERO;
    switch (listType) {
        case LIST1:
        case LIST2:
        case LIST3:
        case LIST4:
        case LIST5:
        case LIST6:
        case LIST7:
        {
            // TODO
            float gapFront = 35;
            float gapBack = 10;
            float x = size.width/7/2 + size.width/7 * listType;
            float y = size.height - 240 - BAR_HEIGHT;
            if (index <= 0) {
                return Point(x, y);
            }
            if (pokerList.size() < listType+1) { // in case out of bound
                y -= gapBack * index;
                return Point(x, y);
            }
            auto list = pokerList.at(listType);
            /////
            int countHas = index;
            if (list.size() < index + 1) {
                countHas = (int)list.size();
            }
            for (int i = 1; i <= countHas; i++) {
                auto p = list.at(i-1);
                if (true == p->isBack()) {
                    y -= gapBack;
                    continue;
                }
                y -= gapFront;
            }
            y -= (index - countHas) * gapBack;
            return Point(x, y);
        }
            break;
            
        case SLOT1:
        case SLOT2:
        case SLOT3:
        case SLOT4:
        {
            float x = size.width/7/2 + size.width/7 * (listType-5);
            float y = size.height - 80 - BAR_HEIGHT;
            pos = Point(x, y);
        }
            break;
            
        case DECK:
        {
            float x = size.width/7/2 + size.width/7;
            float y = size.height - 80 - BAR_HEIGHT;
            pos = Point(x, y);
        }
            break;
            
        default:
            break;
    }
    return pos;
}

void Klondike::resetChildren(PokerKlondike *poker, bool isFollow)
{
    for (int i = 0; i < pokerList.size(); i++) {
        auto list = pokerList.at(i);
        if (0 == list.size()) {
            continue;
        }
        int lastIndex = 100; // set a default max value
        for (int j = 0; j < list.size(); j++) {
            auto p = list.at(j);
            if (p == poker) {
                reorderChild(p, PK_ZORDER_SHOW);
                lastIndex = j;
            } else if (true == isFollow && j > lastIndex) {
                reorderChild(p, PK_ZORDER_SHOW+j);
            } else {
                reorderChild(p, PK_ZORDER_NORMAL+j);
            }
        }
    }
}

void Klondike::onEnter()
{
    Layer::onEnter();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(Klondike::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(Klondike::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(Klondike::onTouchEnded, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto keyPadListener = EventListenerKeyboard::create();
    keyPadListener->onKeyPressed = CC_CALLBACK_2(Klondike::onKeyPressed, this);
    keyPadListener->onKeyReleased = CC_CALLBACK_2(Klondike::onKeyReleased, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyPadListener, this);
}

void Klondike::onExit()
{
//    _eventDispatcher->removeEventListenersForTarget(this);
    Layer::onExit();
}

void Klondike::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    //    CCLOG("onKeyPressed");
    //    CCLOG("onKeyPressed keyCode[%d]", keyCode);
}

void Klondike::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    if (EventKeyboard::KeyCode::KEY_BACK == keyCode) {
        JniMethodInfo t;
        
        if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/cpp/AppActivity", "exitGame", "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
#endif
    //    CCLOG("onKeyReleased keyCode[%d]", keyCode);
}

void Klondike::nextPokers()
{
//    CCLOG("nextPokers");
    auto &list = pokerList.at(DECK);
    if (0 == list.size()) {
        return;
    }
    for (int i = 0; i < list.size(); i++) {
        auto poker = list.back();
        auto action = poker->getActionByTag(ACTION_NEW_POKER);
        if (nullptr != action) {
            return;
        }
    }
    int size = (int)list.size();
    if (size > 3) {
        size = 3;
    }
    for (int i = 0; i < size; i++) {
        auto poker = list.back();
        list.popBack();
        list.insert(0, poker);
    }
    if (list.size() > 0) {
        auto poker = list.back();
        if (poker == _lastDeckPoker) { // has been a loop
            CCLOG("has been a loop");
            _isLoop = true;
        } else if (nullptr == _lastDeckPoker) {
            _lastDeckPoker = list.back();
            _isLoop = false;
            CCLOG("===== _lastDeckPoker ptype[%d] face[%d] num[%d]", _lastDeckPoker->getPtype(), _lastDeckPoker->getFace(), _lastDeckPoker->getNum());
        }
    }
    float delay = 0;
    float moveTime = 0.4;
    for (int i = 0; i < list.size(); i++) {
        auto poker = list.at(i);
        auto pos = getPokerPosition(DECK, i);
        poker->turnBack();
        if (i <= list.size() - 4) {
            poker->setPosition(pos);
            continue;
        }
        poker->setTag(100+i);
        this->reorderChild(poker, PK_ZORDER_SHOW+100-i);
        pos.x += 80+30*(i-(list.size()-4));
        
        FiniteTimeAction *action;
        Vector<FiniteTimeAction *> list;
        action = DelayTime::create(delay);
        list.pushBack(action);
        Vector<FiniteTimeAction *> listSpawn;
        action = MoveTo::create(moveTime, pos);
        listSpawn.pushBack(action);
        poker->addActionFlipFront(&listSpawn, moveTime);
        Vector<FiniteTimeAction *> list2;
        action = ScaleTo::create(moveTime/2, 1.2);
        list2.pushBack(action);
        action = ScaleTo::create(moveTime/2, 1);
        list2.pushBack(action);
        action = Sequence::create(list2);
        listSpawn.pushBack(action);
        action = Spawn::create(listSpawn);
        list.pushBack(action);
        
        action = CallFuncN::create(CC_CALLBACK_1(Klondike::callback_move, this));
        list.pushBack(action);
        action = Sequence::create(list);
        action->setTag(ACTION_NEW_POKER);
        poker->runAction(action);
        
        delay += moveTime/2;
    }
}

bool Klondike::onTouchBegan(Touch* touch, Event* event)
{
//    CCLOG("Klondike::onTouchBegan id = %d, x = %f, y = %f", touch->getID(), touch->getLocation().x, touch->getLocation().y);
    offset = Point(0, 0);
    touchPoker = NULL;
    pokerPos = Point::ZERO;
    if (true == isGameOver) {
        reStart();
        return true;
    }
    if (false == canTouch) {
        return true;
    }
    for (int i = 0; i < pokerList.size(); i++) {
        auto list = pokerList.at(i);
        if (0 == list.size()) {
            continue;
        }
        if (i >= LIST1 && i <= LIST7) {
            int lastNum = -1;
            for (int j = (int)list.size()-1; j >= 0; j--) {
                auto poker = list.at(j);
                if (poker->isBack()) {
                    break;
                }
                int num = poker->getNum();
                if (-1 != lastNum && 1 != (num - lastNum)) {
                    break;
                }
                if (!poker->isInRect(touch)) {
                    lastNum = num;
                    continue;
                }
                touchPoker = poker;
                pokerPos = touchPoker->getPosition();
                break;
            }
            if (NULL != touchPoker) {
                break;
            }
        } else {
            if (i == DECK) {
                auto deckPos = getPokerPosition(DECK, 0);
                auto size = pack.at(0)->getContentSize();
                Rect deckRect = Rect(deckPos.x-size.width/2 ,deckPos.y-size.height/2, size.width, size.height);
                if (deckRect.containsPoint(Point(touch->getLocation().x, touch->getLocation().y))) {
                    nextPokers();
                    if (true == checkLose()) {
                        gameLoseAnimate();
                    }
                    break;
                }
            }
            auto poker = list.back();
            if (poker->isInRect(touch)) {
                touchPoker = poker;
                pokerPos = touchPoker->getPosition();
                break;
            }
        }
    }
    if (NULL != touchPoker) {
        auto pos = touchPoker->getPosition();
        offset.x = pos.x - touch->getLocation().x;
        offset.y = pos.y - touch->getLocation().y;
        resetChildren(touchPoker, true);
    }
    
    return true;
}

void Klondike::onTouchMoved(Touch* touch, Event* event)
{
    if (NULL == touchPoker) {
        return;
    }
    auto pos = Point(touch->getLocation().x+offset.x, touch->getLocation().y+offset.y);
    touchPoker->setPosition(pos);
    // set follow team pos
    for (int i = LIST1; i <= LIST7; i++) {
        auto list = pokerList.at(i);
        int index = (int)list.getIndex(touchPoker);
        if (0 > index) {
            continue;
        }
        auto ipos = getPokerPosition(i, index);
        index += 1;
        for (int j = index; j < list.size(); j++) {
            auto poker = list.at(j);
            auto opos = getPokerPosition(i, j);
            auto npos = Point(pos.x, opos.y-ipos.y+pos.y);
            poker->setPosition(npos);
        }
        break;
    }
}

void Klondike::onTouchEnded(Touch* touch, Event* event)
{
    if (NULL == touchPoker) {
        return;
    }
    Point pos = Point(touch->getLocation().x, touch->getLocation().y);
    int listType1 = canMoveToList(touchPoker, pos);
    int listType2 = canMoveToSlot(touchPoker, pos);
    if (-1 != listType1) {
        for (int i = 0; i < pokerList.size(); i++) {
            auto list = pokerList.at(i);
            int index = (int)list.getIndex(touchPoker);
            if (0 > index) {
                continue;
            }
            if (index > 0 && list.size() > 1 && i != DECK) {
                auto p = list.at(index - 1);
                p->flipFront(0, 0.2);
            }
            if (i == DECK) {
                _lastDeckPoker = nullptr;
            }
            for (int j = index; j < list.size(); j++) {
                auto poker = list.at(j);
                removePoker(poker);
                insertPoker(poker, listType1);
                poker->setPosition(getPokerPosition(listType1, (int)(pokerList.at(listType1)).size()-1));
            }
            this->increaseScore(touchPoker, listType1);
            break;
        }
    } else if (-1 != listType2) {
        for (int i = 0; i < pokerList.size(); i++) {
            auto list = pokerList.at(i);
            int index = (int)list.getIndex(touchPoker);
            if (0 > index) {
                continue;
            }
            if (i != DECK && list.size() > 1) {
                auto p = list.at(index - 1);
                p->flipFront(0, 0.2);
            }
            if (index < list.size()-1) {
                break;
            }
            if (i == DECK) {
                _lastDeckPoker = nullptr;
            }
            for (int j = index; j < list.size(); j++) {
                auto poker = list.at(j);
                removePoker(poker);
                insertPoker(poker, listType2);
                poker->setPosition(getPokerPosition(listType2, (int)(pokerList.at(listType2)).size()-1));
            }
            this->increaseScore(touchPoker, listType2);
            break;
        }
    } else {
        for (int i = 0; i < pokerList.size(); i++) {
            auto list = pokerList.at(i);
            int index = (int)list.getIndex(touchPoker);
            if (0 > index) {
                continue;
            }
            if ((i >= LIST1 && i <= LIST7)) {
                for (int j = index; j < list.size(); j++) {
                    auto poker = list.at(j);
                    poker->setPosition(getPokerPosition(i, j));
                }
            } else {
                touchPoker->setPosition(pokerPos);
            }
//            if (i == DECK) {
//                auto pos = getPokerPosition(DECK, index);
//                if (index > list.size() - 4) {
//                    pos.x += 80+30*(index-(list.size()-4));
//                }
//                touchPoker->setPosition(pos);
//            } else { // (i >= LIST1 && i <= LIST7) || (i >= SLOT1 && i <= SLOT4)
//                for (int j = index; j < list.size(); j++) {
//                    auto poker = list.at(j);
//                    poker->setPosition(getPokerPosition(i, j));
//                }
//            }
            
            break;
        }
    }
    
    pokerPos = Point::ZERO;
    touchPoker = NULL;
    resetChildren(NULL, true);
    
    if (-1 != listType1 || -1 != listType2) {
        if (true == checkWin()) {
            gameWinAnimate();
            return;
        }
    }
}

void Klondike::insertPoker(PokerKlondike *poker, int listType)
{
    auto &list = pokerList.at(listType);
    list.pushBack(poker);
}

void Klondike::removePoker(PokerKlondike *poker)
{
    for (int i = 0; i < pokerList.size(); i++) {
        auto &list = pokerList.at(i);
        if (list.contains(poker)) {
            list.eraseObject(poker);
            return;
        }
    }
}

bool Klondike::canFollow(PokerKlondike *lastPoker, PokerKlondike *poker, int actionType)
{
    int lastFace = lastPoker->getFace();
    int face = poker->getFace();
    bool condition1 = false;
    bool condition2 = false;
    if (actionType == FLAG_CHANGE_POS && 0 != abs(lastFace - face)%2) {
        condition1 = true;
//    } else if (actionType == FLAG_GATHER && 0 == abs(lastFace - face)%2) {
    } else if (actionType == FLAG_GATHER && lastFace == face) {
        condition1 = true;
    }
    int lastNum = lastPoker->getNum();
    int num = poker->getNum();
    if (actionType == FLAG_CHANGE_POS && 1 == (lastNum - num)) {
        condition2 = true;
    } else if (actionType == FLAG_GATHER && 1 == (num - lastNum)) {
        condition2 = true;
    }
    if (true == condition1 && true == condition2) {
        return true;
    }
    return false;
}

int Klondike::canMoveToList(PokerKlondike *poker, Point pos)
{
    Size pokerSize = poker->getContentSize();
    pokerSize.width += 15;
    pokerSize.height += 30;
    Point pokerPos = poker->getPosition();
    Rect pokerRect = Rect(pokerPos.x-pokerSize.width/2, pokerPos.y-pokerSize.height/2, pokerSize.width, pokerSize.height);
    for (int i = LIST1; i <= LIST7; i++) {
        auto list = pokerList.at(i);
        int index = (int)list.size() - 1; //get the last one
        if (index < 0) {
            index = 0;
        }
        Point p = getPokerPosition(i, index);
        if (!pokerRect.containsPoint(p)) {
            continue;
        }
        if (0 == list.size()) {
            if (POKERKLONDIKE_NUM::KLO_KING != poker->getNum()) {
                continue;
            }
            return i;
        }
        auto lastPoker = list.back();
        if (canFollow(lastPoker, poker, FLAG_CHANGE_POS)) {
            return i;
        }
    }
    return -1;
}

int Klondike::canMoveToSlot(PokerKlondike *poker, Point pos)
{
    Size pokerSize = poker->getContentSize();
    pokerSize.width += 15;
    pokerSize.height += 30;
    Point pokerPos = poker->getPosition();
    Rect pokerRect = Rect(pokerPos.x-pokerSize.width/2, pokerPos.y-pokerSize.height/2, pokerSize.width, pokerSize.height);
    for (int i = SLOT1; i <= SLOT4; i++) {
        auto list = pokerList.at(i);
        int index = (int)list.size() - 1;
        if (index < 0) {
            index = 0;
        }
        Point p = getPokerPosition(i, index);
        if (!pokerRect.containsPoint(p)) {
            continue;
        }
        if (0 == list.size()) {
            if (POKERKLONDIKE_NUM::KLO_ACE != poker->getNum()) {
                continue;
            }
            return i;
        }
        auto lastPoker = list.back();
        if (canFollow(lastPoker, poker, FLAG_GATHER)) {
            return i;
        }
    }
    return -1;
}

void Klondike::increaseScore(PokerKlondike *poker, int listType)
{
    if (scoreList.contains(poker)) {
        return;
    }
    switch (listType) {
        case LIST1:
        case LIST2:
        case LIST3:
        case LIST4:
        case LIST5:
        case LIST6:
        case LIST7:
        {
            this->_score += 5;
        }
            break;
            
        case SLOT1:
        case SLOT2:
        case SLOT3:
        case SLOT4:
        {
            this->_score += 10;
        }
            break;
            
        default:
            return;
    }
    
    this->setScore(this->_score);
}

void Klondike::setScore(int score)
{
    char scoreStr[10] = {0};
    memset(scoreStr, 0, 10);
    sprintf(scoreStr, "%d", score);
    this->labelScore->setString(scoreStr);
}

void Klondike::decreaseScore()
{
    
}

void Klondike::saveGame()
{
    if (pokerList.size() != SLOT4+1) {
//        KUtil::saveDataBool("klondike_game", false);
        return;
    }
    ValueMap saveData;
    for (int i = LIST1; i <= SLOT4; i++) {
        auto list = pokerList.at(i);
        
        char keySize[20] = {0};
        memset(keySize, 0, 20);
        sprintf(keySize, "%s_%d", KEY_LIST_SIZE, i);
        Value keySizeValue = Value((int)list.size());
        saveData.insert(make_pair(keySize, keySizeValue));
        for (int j = 0; j < list.size(); j++) {
            auto poker = list.at(j);
            int ptype = poker->getPtype();
            
            char keyPoker[20] = {0};
            memset(keyPoker, 0, 20);
            sprintf(keyPoker, "%s_%d_%d", KEY_QUEUE, i, j);
            char value[20] = {0};
            memset(value, 0, 20);
            sprintf(value, "%d_%d", ptype, (true == poker->isBack())?1:0);
            Value pokerInfo = Value(value);
            saveData.insert(make_pair(keyPoker, pokerInfo));
        }
    }
    
    saveData.insert(make_pair(KEY_SCORE, Value(this->_score)));
    saveData.insert(make_pair(KEY_TIME, Value(this->_time)));
    auto path = FileUtils::getInstance()->getWritablePath() + FILE_NAME;
    FileUtils::getInstance()->writeToFile(saveData, path);
    CCLOG("======>>>> klondike saved");
}

bool Klondike::loadGame()
{
    auto path = FileUtils::getInstance()->getWritablePath() + FILE_NAME;
    ValueMap saveData = FileUtils::getInstance()->getValueMapFromFile(path);
    if (0 == saveData.size()) {
        return false;
    }
    pokerList.clear();
    for (int i = LIST1; i <= SLOT4; i++) {
        char listSizeKey[30] = {0};
        memset(listSizeKey, 0, 30);
        sprintf(listSizeKey, "%s_%d", KEY_LIST_SIZE, i);
        Value lsv = saveData[listSizeKey];
        int size = saveData[listSizeKey].asInt();
        Vector<PokerKlondike *> list;
        for (int j = 0; j < size; j++) {
            char keyPoker[20] = {0};
            memset(keyPoker, 0, 20);
            sprintf(keyPoker, "%s_%d_%d", KEY_QUEUE, i, j);
            const char *pokerInfo = saveData[keyPoker].asString().c_str();
            int ret;
            int ptype;
            int isBack;
            ret = sscanf(pokerInfo, "%d_%d", &ptype, &isBack);
            if (2 != ret) {
                pack.clear();
                pokerList.clear();
                return false;
            }
            auto poker = PokerKlondike::create(0, ptype);
            if (1 == isBack) {
                poker->turnBack();
            } else {
                poker->turnFront();
            }
            pack.pushBack(poker);
            list.pushBack(poker);
            
            poker->setAnchorPoint(ANCHOR_CENTER_CENTER);
            auto pos = Point(getPokerPosition(i, j));
            if (i == DECK && poker->isBack() == false) {
                pos.x += 80+30*(j-(size-4));
            }
            poker->setPosition(pos);
            this->addChild(poker, PK_ZORDER_NORMAL+j);
        }
        pokerList.push_back(list);
    }
    for (int i = LIST1; i <= LIST7; i++) {
        auto list = pokerList.at(i);
        for (int j = 0; j < list.size(); j++) {
            auto poker = list.at(j);
            poker->setPosition(getPokerPosition(i, j));
        }
    }
    this->_score = saveData[KEY_SCORE].asInt();
    this->_time = saveData[KEY_TIME].asInt();
    
    CCLOG("======>>>> klondike loaded");
    return true;
}

bool Klondike::checkLose()
{
    if (pokerList.at(DECK).size() == 0) {
        return false;
    }
    if (nullptr == _lastDeckPoker) {
        return false;
    }
    auto lastPoker = pokerList.at(DECK).back();
    for (int i = LIST1; i <= LIST7; i++) {
        auto list = pokerList.at(i);
        // deck to slot
        if (list.size() == 0) {
            if (POKERKLONDIKE_NUM::KLO_KING == lastPoker->getNum()) {
                _lastDeckPoker = nullptr;
                return false;
            }
            continue;
        } else {
            auto poker = list.back();
            // check can move to list
            if (canFollow(poker, lastPoker, FLAG_CHANGE_POS)) {
                _lastDeckPoker = nullptr;
                return false;
            }
        }
        
        // TODO not work right now
        auto lastListPoker = list.back();
        // list to slot
        for (int j = SLOT1; j <= SLOT4; j++) {
            auto listSlot = pokerList.at(j);
            if (0 == listSlot.size()) {
                if (POKERKLONDIKE_NUM::KLO_ACE == lastListPoker->getNum()) {
                    _lastDeckPoker = nullptr;
                    return false;
                }
                continue;
            }
            auto poker = listSlot.back();
            // check can move to slot
            if (canFollow(poker, lastListPoker, FLAG_GATHER)) {
                _lastDeckPoker = nullptr;
                return false;
            }
        }
    }
    for (int i = SLOT1; i <= SLOT4; i++) {
        auto list = pokerList.at(i);
        if (0 == list.size()) {
            if (POKERKLONDIKE_NUM::KLO_ACE == lastPoker->getNum()) {
                _lastDeckPoker = nullptr;
                return false;
            }
            continue;
        }
        auto poker = list.back();
        // check can move to slot
        if (canFollow(poker, lastPoker, FLAG_GATHER)) {
            _lastDeckPoker = nullptr;
            return false;
        }
    }
    if (false == _isLoop) {
        return false;
    }
    return true;
}

bool Klondike::checkWin()
{
    if (pokerList.at(DECK).size() != 0) {
        return false;
    }
    for (int i = LIST1; i <= LIST7; i++) {
        auto list = pokerList.at(i);
        for (int j = 0; j < list.size(); j++) {
            auto poker = list.at(j);
            if (poker->isBack()) {
                return false;
            }
        }
    }
    return true;
}

void Klondike::callback_move2(Node* pSender)
{
    int tag = pSender->getTag();
    this->reorderChild(pSender, PK_ZORDER_SHOW+tag);
}

void Klondike::gameWinAnimate()
{
    canTouch = false;
    bool hasCard = true;
    float delay = 0;
    float gap = 0.6;
    while (hasCard == true) {
        hasCard = false;
        for (int i = LIST1; i <= LIST7; i++) {
            auto &list = pokerList.at(i);
            if (0 == list.size()) {
                continue;
            }
            auto poker = list.back();
            auto face = poker->getFace();
            auto num = poker->getNum();
            int markSlot = -1;
            for (int j = SLOT1; j <= SLOT4; j++) {
                auto &list2 = pokerList.at(j);
                if (0 == list2.size()) {
                    if (POKERKLONDIKE_NUM::KLO_ACE == num) {
                        markSlot = j;
                    }
                    continue;
                }
                auto poker2 = list2.back();
                auto face2 = poker2->getFace();
                auto num2 = poker2->getNum();
                if (face == face2 && num == num2 + 1) {
                    markSlot = j;
                    break;
                }
            }
            if (-1 == markSlot) {
                continue;
            }
            hasCard = true;
            list.eraseObject(poker);
            auto &listSlot = pokerList.at(markSlot);
            listSlot.pushBack(poker);
            
            poker->setTag(PK_ZORDER_SHOW+(int)listSlot.size());
            Vector<FiniteTimeAction *> listAction;
            FiniteTimeAction *action;
            action = DelayTime::create(delay);
            listAction.pushBack(action);
            action = CallFuncN::create(CC_CALLBACK_1(Klondike::callback_move2, this));
            listAction.pushBack(action);
            action = MoveTo::create(0.5, getPokerPosition(markSlot, 0));
            listAction.pushBack(action);
            action = Sequence::create(listAction);
            poker->runAction(action);
            gap *= 0.9;
            delay += gap;
        }
    }
    delay += 0.5;
    this->scheduleOnce(schedule_selector(Klondike::callbackEnable), delay);
    this->scheduleOnce(schedule_selector(Klondike::showResult), delay);
}

void Klondike::gameLoseAnimate()
{
    auto layer = KlondikeResult::create();
    layer->setLose(CC_CALLBACK_1(Klondike::callbackRestart, this));
    KUtil::addLayer(layer, 20, 0);
    isGameOver = true;
}

void Klondike::showResult(float delta)
{
    auto layer = KlondikeResult::create();
    layer->setWin(_time, _score, CC_CALLBACK_1(Klondike::callbackRestart, this));
    KUtil::addLayer(layer, 20, 0);
    isGameOver = true;
}

int KlondikeResult::saveResult(int time, int score, std::vector<int> *results)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    KUtilIOS::insertRank(IOS_KEY_SCORE_RANK, score);
#endif
    int s1, s2, s3 = 0;
    int t1, t2, t3 = 0;
    int pos = 0;
    auto savePath = FileUtils::getInstance()->getWritablePath() + FILE_NAME_SCORE;
    ValueMap readData = FileUtils::getInstance()->getValueMapFromFile(savePath);
    s1 = readData[KEY_RECORD_SCORE_1].asInt();
    t1 = readData[KEY_RECORD_TIME_1].asInt();
    s2 = readData[KEY_RECORD_SCORE_2].asInt();
    t2 = readData[KEY_RECORD_TIME_2].asInt();
    s3 = readData[KEY_RECORD_SCORE_3].asInt();
    t3 = readData[KEY_RECORD_TIME_3].asInt();
    ValueMap saveData;
    if (score > s1) {
        s3 = s2;
        t3 = t2;
        s2 = s1;
        t2 = t1;
        s1 = score;
        t1 = time;
        pos = 1;
    } else if (score > s2) {
        s3 = s2;
        t3 = t2;
        s2 = score;
        t2 = time;
        pos = 2;
    } else if (score > s3) {
        s3 = score;
        t3 = time;
        pos = 3;
    }
    saveData.insert(make_pair(KEY_RECORD_SCORE_1, Value(s1)));
    saveData.insert(make_pair(KEY_RECORD_TIME_1, Value(t1)));
    saveData.insert(make_pair(KEY_RECORD_SCORE_2, Value(s2)));
    saveData.insert(make_pair(KEY_RECORD_TIME_2, Value(t2)));
    saveData.insert(make_pair(KEY_RECORD_SCORE_3, Value(s3)));
    saveData.insert(make_pair(KEY_RECORD_TIME_3, Value(t3)));
    
    auto path = FileUtils::getInstance()->getWritablePath() + FILE_NAME_SCORE;
    FileUtils::getInstance()->writeToFile(saveData, path);
    
    (*results).push_back(s1);
    (*results).push_back(t1);
    (*results).push_back(s2);
    (*results).push_back(t2);
    (*results).push_back(s3);
    (*results).push_back(t3);
    
    return pos;
}

void KlondikeResult::setWin(int time, int score, const ccMenuCallback& callbackRestart)
{
    _callbackRestart = callbackRestart;
    auto bg = KUtil::addSprite(this, KUtil::getPath(F_KLO, "bg_win.png"), Point(HALF_WIDTH, HALF_HEIGHT), ANCHOR_CENTER_CENTER, 1);
    auto bgSize = bg->getContentSize();
    auto title = KUtil::addSprite(bg, KUtil::getPath(F_KLO, "title_win.png"), Point(bgSize.width/2, bgSize.height-10), ANCHOR_CENTER_CENTER, 1);
    auto titleSize = title->getContentSize();
    GameTool::addLabelDefault(this, "You Win", 50, Point(titleSize.width/2, titleSize.height/2), Color4B(255, 255, 0, 255), ANCHOR_CENTER_DOWN, PK_ZORDER_NORMAL-10);
    
    //
    std::vector<int> rs;
    int pos = this->saveResult(time, score, &rs);
    //
    
    char timeStr[100];
    memset(timeStr, 0, 100);
    sprintf(timeStr, "time: %d", time);
    GameTool::addLabelDefault(bg, timeStr, 50, Point(bgSize.width/2, bgSize.height/2+90), Color4B(100, 200, 50, 255), ANCHOR_CENTER_DOWN, PK_ZORDER_NORMAL-10);
    
    char starStr[100];
    memset(starStr, 0, 100);
    sprintf(starStr, "star: %d", score);
    GameTool::addLabelDefault(bg, starStr, 50, Point(bgSize.width/2, bgSize.height/2+70), Color4B(100, 200, 50, 255), ANCHOR_CENTER_UP, PK_ZORDER_NORMAL-10);
    
    if (rs.size() > 0) {
        auto c1 = Color4B(100, 200, 50, 255);
        auto c2 = Color4B(200, 100, 50, 255);
        char str1[100];
        memset(str1, 0, 100);
        sprintf(str1, "1. score[%d] time[%d]", rs.at(0), rs.at(1));
        GameTool::addLabelDefault(bg, str1, 40, Point(bgSize.width/2, bgSize.height/2+10), (1==pos?c2:c1), ANCHOR_CENTER_UP, PK_ZORDER_NORMAL-10);
        
        char str2[100];
        memset(str2, 0, 100);
        sprintf(str2, "2. score[%d] time[%d]", rs.at(2), rs.at(3));
        GameTool::addLabelDefault(bg, str2, 40, Point(bgSize.width/2, bgSize.height/2-30), (2==pos?c2:c1), ANCHOR_CENTER_UP, PK_ZORDER_NORMAL-10);
        
        char str3[100];
        memset(str3, 0, 100);
        sprintf(str3, "3. score[%d] time[%d]", rs.at(4), rs.at(5));
        GameTool::addLabelDefault(bg, str3, 40, Point(bgSize.width/2, bgSize.height/2-70), (3==pos?c2:c1), ANCHOR_CENTER_UP, PK_ZORDER_NORMAL-10);
        
        if (pos != 0) {
            GameTool::addLabelDefault(bg, "new record!!!", 40, Point(bgSize.width/2, bgSize.height/2-110), c2, ANCHOR_CENTER_UP, PK_ZORDER_NORMAL-10);
        }
    }
    
    char path[500];
    KUtil::getPath(path, 500, F_KLO, "eff_win.plist");
    auto emitter = ParticleSystemQuad::create(path);
    auto batch = ParticleBatchNode::createWithTexture(emitter->getTexture());
    batch->setPosition(Point(HALF_WIDTH, HALF_HEIGHT));
    batch->addChild(emitter);
    this->addChild(batch);
    
    Vector<MenuItem *> itemArray;
    
    GameTool::addItemSpriteScale(&itemArray, KUtil::getPath(F_KLO, "btn_next.png"), Point(bgSize.width/2, 0), ANCHOR_CENTER_CENTER, CC_CALLBACK_0(KlondikeResult::doNext, this));
    
    KUtil::addMenu(bg, &itemArray, 30);
}

void KlondikeResult::setLose(const ccMenuCallback& callbackRestart)
{
    _callbackRestart = callbackRestart;
    auto bg = KUtil::addSprite(this, KUtil::getPath(F_KLO, "bg_win.png"), Point(HALF_WIDTH, HALF_HEIGHT), ANCHOR_CENTER_CENTER, 1);
    auto bgSize = bg->getContentSize();
    auto title = KUtil::addSprite(bg, KUtil::getPath(F_KLO, "title_win.png"), Point(bgSize.width/2, bgSize.height-10), ANCHOR_CENTER_CENTER, 1);
    auto titleSize = title->getContentSize();
    GameTool::addLabelDefault(title, "You Lose", 50, Point(titleSize.width/2, titleSize.height/2), Color4B(255, 255, 0, 255), ANCHOR_CENTER_DOWN, PK_ZORDER_NORMAL-10);
    
    GameTool::addLabelDefault(bg, "tap to restart", 50, Point(bgSize.width/2, bgSize.height/2+30), Color4B(100, 200, 50, 255), ANCHOR_CENTER_DOWN, PK_ZORDER_NORMAL-10);
    
    Vector<MenuItem *> itemArray;
    
    GameTool::addItemSpriteScale(&itemArray, KUtil::getPath(F_KLO, "btn_next.png"), Point(bgSize.width/2, 0), ANCHOR_CENTER_CENTER, CC_CALLBACK_0(KlondikeResult::doNext, this));
    
    KUtil::addMenu(bg, &itemArray, 30);
}

bool KlondikeResult::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    return true;
}

void KlondikeResult::doNext()
{
    removeFromParentAndCleanup(true);
    if (nullptr == _callbackRestart) {
        return;
    }
    _callbackRestart(nullptr);
}

void KlondikeResult::onEnter()
{
    Layer::onEnter();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(KlondikeResult::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(KlondikeResult::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(KlondikeResult::onTouchEnded, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void KlondikeResult::onExit()
{
    Layer::onExit();
}

bool KlondikeResult::onTouchBegan(Touch* touch, Event* event)
{
    return true;
}

void KlondikeResult::onTouchMoved(Touch* touch, Event* event)
{
    
}

void KlondikeResult::onTouchEnded(Touch* touch, Event* event)
{
    
}
