//
//  PokerKlondike.cpp
//  KillTime
//
//  Created by keltonxian on 5/12/14.
//
//

#include "PokerKlondike.h"
#include "GameTool.h"

void PokerKlondike::getPack(Vector<PokerKlondike *> *list)
{
    for (int i = POKERKLONDIKE_TYPE::KLO_SPADE_A; i <= POKERKLONDIKE_TYPE::KLO_DIAMOND_K; i++) {
        (*list).pushBack(PokerKlondike::create(0, i));
    }
    randomPack(list);
}

void PokerKlondike::randomPack(Vector<PokerKlondike *> *list)
{
    // need random seed
    srand((int)time(NULL));
    for (int i = 0; i < 100; i++) {
        ssize_t index1 = rand() % (*list).size();
        ssize_t index2 = rand() % (*list).size();
        (*list).swap(index1, index2);
    }
}

PokerKlondike* PokerKlondike::create(int style, int ptype)
{
    PokerKlondike *poker = new PokerKlondike(ptype);
//    poker->setScale(0.30);
//    char path[500];
//    KUtil::getPath(path, 500, F_KLO, "cover_card_1.png");
    std::string path = KUtil::getPath(F_KLO, "cover_card_1.png");
    if (poker && poker->initWithFile(path)) {
        poker->setSize();
//        poker->setFront();
        poker->autorelease();
        return poker;
    }
    CC_SAFE_DELETE(poker);
    return NULL;
}

PokerKlondike::PokerKlondike(int ptype):_ptype(ptype),_num(0),_face(0),_width(0),_height(0),_pos(Point::ZERO),_status(POKER_BACK)
{
//    CCLOG("Poker num[%d],face[%d]", (ptype-1)%13+1, (ptype-1)/13+1);
    _num = (ptype-1)%13+1;
    _face = (ptype-1)/13+1;
}

PokerKlondike::~PokerKlondike()
{
//    CCLOG("~Poker num[%d],face[%d]", _num, _face);
}

void PokerKlondike::setSize()
{
    _width = this->getContentSize().width;
    _height = this->getContentSize().height;
}

Rect PokerKlondike::getRect()
{
//    auto position = this->getPosition();
//    // assume anchorpoint is ANCHOR_CENTER_CENTER
//    auto rect = Rect(position.x-_width / 2, position.y-_height / 2, _width, _height);
//    CCLOG("rect[%f][%f]", rect.origin.x, rect.origin.y);
//    return rect;
    return Rect(-_width / 2, -_height / 2, _width, _height);
}

int PokerKlondike::getPtype()
{
    return _ptype;
}

int PokerKlondike::getFace()
{
    return _face;
}

int PokerKlondike::getNum()
{
    return _num;
}

bool PokerKlondike::isBack()
{
    return _status == POKER_BACK;
}

void PokerKlondike::turnFront()
{
    if (_status == POKER_FRONT) {
        return;
    }
    this->removeAllChildren();
    char path[500];
    KUtil::getPath(path, 500, F_KLO, "bg_card_1.png");
    this->setTexture(path);
    this->setFront();
    _status = POKER_FRONT;
}

void PokerKlondike::cbTurnFront(Node *pSender)
{
    this->turnFront();
}

void PokerKlondike::addActionFlipFront(Vector<FiniteTimeAction *> *actionList, float time)
{
    Vector<FiniteTimeAction *> list;
    FiniteTimeAction *action;
    action = OrbitCamera::create(time/2, 1, 0, 0, -90, 0, 0);
    list.pushBack(action);
    action = CallFuncN::create(CC_CALLBACK_1(PokerKlondike::cbTurnFront, this));
    list.pushBack(action);
    action = OrbitCamera::create(time/2, 1, 0, 90, -90, 0, 0);
    list.pushBack(action);
    action = Sequence::create(list);
    (*actionList).pushBack(action);
}

void PokerKlondike::flipFront(float delay, float time)
{
    if (_status == POKER_FRONT) {
        return;
    }
    
    Vector<FiniteTimeAction *> list;
    FiniteTimeAction *action;
    action = DelayTime::create(delay);
    list.pushBack(action);
    addActionFlipFront(&list, time);
//    action = OrbitCamera::create(time/2, 1, 0, 0, -90, 0, 0);
//    list.pushBack(action);
//    action = CallFuncN::create(CC_CALLBACK_1(Poker::cbTurnFront, this));
//    list.pushBack(action);
//    action = OrbitCamera::create(time/2, 1, 0, 90, -90, 0, 0);
//    list.pushBack(action);
    action = Sequence::create(list);
    this->runAction(action);
}

void PokerKlondike::turnBack()
{
    if (_status == POKER_BACK) {
        return;
    }
    this->removeAllChildren();
    char path[500];
    KUtil::getPath(path, 500, F_KLO, "cover_card_1.png");
    this->setTexture(path);
    _status = POKER_BACK;
}

void PokerKlondike::cbTurnBack(Node *pSender)
{
    this->turnBack();
}

void PokerKlondike::addActionFlipBack(Vector<FiniteTimeAction *> *actionList, float time)
{
    Vector<FiniteTimeAction *> list;
    FiniteTimeAction *action;
    action = OrbitCamera::create(time/2, 1, 0, 0, -90, 0, 0);
    list.pushBack(action);
    action = CallFuncN::create(CC_CALLBACK_1(PokerKlondike::cbTurnBack, this));
    list.pushBack(action);
    action = OrbitCamera::create(time/2, 1, 0, 90, -90, 0, 0);
    list.pushBack(action);
    action = Sequence::create(list);
    (*actionList).pushBack(action);
}

void PokerKlondike::flipBack(float delay, float time)
{
    if (_status == POKER_BACK) {
        return;
    }
    
    Vector<FiniteTimeAction *> list;
    FiniteTimeAction *action;
    action = DelayTime::create(delay);
    list.pushBack(action);
    addActionFlipBack(&list, time);
//    action = OrbitCamera::create(time/2, 1, 0, 0, -90, 0, 0);
//    list.pushBack(action);
//    action = CallFuncN::create(CC_CALLBACK_1(Poker::cbTurnBack, this));
//    list.pushBack(action);
//    action = OrbitCamera::create(time/2, 1, 0, 90, -90, 0, 0);
//    list.pushBack(action);
    action = Sequence::create(list);
    this->runAction(action);
}

int PokerKlondike::getStatus()
{
    return _status;
}

void PokerKlondike::printDetail()
{
//    char numStr[5] = {0};
//    memset(numStr, 0, 5);
//    sprintf(numStr, "%d", _num);
    
    char faceStr[5] = {0};
    getFaceString(faceStr, 5);
    
    CCLOG("Poker[num][%d][face][%s]", _num, faceStr);
}

void PokerKlondike::setFront()
{
    Color4B color;
    if (_face == POKERKLONDIKE_FACE::KLO_SPADE || _face == POKERKLONDIKE_FACE::KLO_CLUB) {
        color = Color4B(0, 0, 0, 255);
    } else {
        color = Color4B(255, 0, 0, 255);
    }
    
    char numStr[5] = {0};
    memset(numStr, 0, 5);
    if (_num == POKERKLONDIKE_NUM::KLO_JACK) {
        sprintf(numStr, "%s", "J");
    } else if (_num == POKERKLONDIKE_NUM::KLO_QUEEN) {
        sprintf(numStr, "%s", "Q");
    } else if (_num == POKERKLONDIKE_NUM::KLO_KING) {
        sprintf(numStr, "%s", "K");
    } else if (_num == POKERKLONDIKE_NUM::KLO_ACE) {
        sprintf(numStr, "%s", "A");
    } else {
        sprintf(numStr, "%d", _num);
    }
    GameTool::addLabelDefault(this, numStr, 30, Point(10, _height-10), color, ANCHOR_LEFT_UP, 10);
    
//    char faceStr[5] = {0};
//    getFaceString(faceStr, 5);
//    KUtil::addLabel(this, faceStr, 8, color, Point(_width-5, _height-5), ANCHOR_RIGHT_UP, 10);
    
    this->setFace();
    
//    char path[500] = {0};
//    memset(path, 0, 500);
//    KUtil::getPath(path, 500, F_POK, "pattern_1.png");
//    auto pattern = Sprite::create(path);
//    pattern->setPosition(_width-10, 10);
//    pattern->setAnchorPoint(Vec2(1, 0));
//    this->addChild(pattern);
}

int PokerKlondike::getFaceString(char *string, int len)
{
    memset(string, 0, len);
    switch (_face) {
        case POKERKLONDIKE_FACE::KLO_SPADE:
            sprintf(string, "%s", "SPADE");
            break;
        case POKERKLONDIKE_FACE::KLO_HEART:
            sprintf(string, "%s", "HEART");
            break;
        case POKERKLONDIKE_FACE::KLO_DIAMOND:
            sprintf(string, "%s", "DIAMOND");
            break;
        case POKERKLONDIKE_FACE::KLO_CLUB:
            sprintf(string, "%s", "CLUB");
            break;
            
        default:
            sprintf(string, "%s", "UNKNOWN");
            break;
    }
    return 0;
}

void PokerKlondike::setFace()
{
    char filename[20] = {0};
    memset(filename, 0, 20);
    switch (_face) {
        case POKERKLONDIKE_FACE::KLO_SPADE:
            sprintf(filename, "%s", "spade_1.png");
            break;
        case POKERKLONDIKE_FACE::KLO_HEART:
            sprintf(filename, "%s", "heart_1.png");
            break;
        case POKERKLONDIKE_FACE::KLO_DIAMOND:
            sprintf(filename, "%s", "diamond_1.png");
            break;
        case POKERKLONDIKE_FACE::KLO_CLUB:
            sprintf(filename, "%s", "club_1.png");
            break;
            
        default:
            return;
    }
    char path[500] = {0};
    memset(path, 0, 500);
    KUtil::getPath(path, 500, F_KLO, filename);
    auto face = Sprite::create(path);
    face->setPosition(10, _height-50);
    face->setAnchorPoint(ANCHOR_LEFT_UP);
    this->addChild(face);
}

void PokerKlondike::onEnter()
{
    Sprite::onEnter();
    
//    auto listener = EventListenerTouchOneByOne::create();
//    listener->setSwallowTouches(true);
//    
//    listener->onTouchBegan = CC_CALLBACK_2(Poker::onTouchBegan, this);
//    listener->onTouchMoved = CC_CALLBACK_2(Poker::onTouchMoved, this);
//    listener->onTouchEnded = CC_CALLBACK_2(Poker::onTouchEnded, this);
//    
//    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void PokerKlondike::onExit()
{
//    _eventDispatcher->removeEventListenersForTarget(this);
    Sprite::onExit();
}

bool PokerKlondike::isInRect(Touch* touch)
{
//    printDetail();
    if (getRect().containsPoint(convertTouchToNodeSpaceAR(touch))) {
        return true;
    }
    return false;
}

bool PokerKlondike::onTouchBegan(Touch* touch, Event* event)
{
    CCLOG("Poker::onTouchBegan id = %d, x = %f, y = %f", touch->getID(), touch->getLocation().x, touch->getLocation().y);
    _pos = Point::ZERO;
    if (!getRect().containsPoint(convertTouchToNodeSpaceAR(touch))) {
//        printDetail();
        return false;
    }
    this->setLocalZOrder(PK_ZORDER_SHOW);
    _pos = this->getPosition();
    printDetail();
    return true;
}

void PokerKlondike::onTouchMoved(Touch* touch, Event* event)
{
    if (_pos.isZero()) {
        return;
    }
    
    auto pos = Point(touch->getLocation().x, touch->getLocation().y);
    this->setPosition(pos);
}

void PokerKlondike::onTouchEnded(Touch* touch, Event* event)
{
    if (_pos.isZero()) {
        return;
    }
    this->setPosition(_pos);
    _pos = Point::ZERO;
    this->setLocalZOrder(PK_ZORDER_NORMAL);
}



