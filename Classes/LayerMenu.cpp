//
//  LayerMenu.cpp
//  KillTime
//
//  Created by keltonxian on 8/29/14.
//
//

#include "LayerMenu.h"
#include "GameTool.h"

bool LayerMenu::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    _callbackRestart = nullptr;
    
    layerColor = LayerColor::create(Color4B(0, 0, 0, 200), FULL_WIDTH, FULL_HEIGHT);
    layerColor->setPosition(Point(0, 0));
    this->addChild(layerColor);
    layerColor->setVisible(false);
    
    Vector<MenuItem *> itemArray;
    
    btnMore = KUtil::addItemToggle(&itemArray, KUtil::getPath(F_BTN, "btn_more.png"), KUtil::getPath(F_BTN, "btn_more_s.png"), Point(0, FULL_HEIGHT), ANCHOR_LEFT_UP, CC_CALLBACK_1(LayerMenu::callbackMore, this));
    
    btnRestart = GameTool::addBtn2(&itemArray, "restart", 30, Point(-200, FULL_HEIGHT/4*3), ANCHOR_LEFT_CENTER, CC_CALLBACK_1(LayerMenu::callbackRestart, this));
    
    btnSave = GameTool::addBtn2(&itemArray, "save", 30, Point(-200, HALF_HEIGHT), ANCHOR_LEFT_CENTER, CC_CALLBACK_1(LayerMenu::callbackSave, this));
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    btnRank = GameTool::addBtn2(&itemArray, "Rank", 30, Point(-200, HALF_HEIGHT/2), ANCHOR_LEFT_CENTER, CC_CALLBACK_1(LayerMenu::callbackRank, this));
#endif
    
    KUtil::addMenu(this, &itemArray, 30);
    
//    auto bg = KUtil::addSprite(this, F_IMAGE, "bg_pause.png", Point(HALF_WIDTH, HALF_HEIGHT), ANCHOR_CENTER_CENTER, 1);
//    KUtil::showAnim1(bg);
    
    return true;
}

void LayerMenu::setCallbackRestart(const ccMenuCallback& callbackRestart)
{
    _callbackRestart = callbackRestart;
}

void LayerMenu::setCallbackSave(const ccMenuCallback& callbackSave)
{
    _callbackSave = callbackSave;
}

void LayerMenu::setCallbackRank(const ccMenuCallback& callbackRank)
{
    _callbackRank = callbackRank;
}

void LayerMenu::callbackMore(cocos2d::Ref* pSender)
{
    MenuItemToggle *item = (MenuItemToggle *)pSender;
    int index = item->getSelectedIndex(); // 0 off,   1 on
    CCLOG("callbackMore %s", (1==index)?"is selected":"not selected");
    float x;
    if (1 == index) {
        layerColor->setVisible(true);
        x = 0;
    } else {
        layerColor->setVisible(false);
        x = -200;
    }
    Vector<MenuItemSprite *> items;
    items.pushBack(btnRestart);
    items.pushBack(btnSave);
    items.pushBack(btnRank);
    float delay = 0;
    for (int i = 0; i < items.size(); i++) {
        auto item = items.at(i);
        item->stopAllActions();
        Vector<FiniteTimeAction *> listAction;
        FiniteTimeAction *action;
        action = DelayTime::create(delay);
        listAction.pushBack(action);
        action = MoveTo::create(0.3, Point(x, item->getPositionY()));
        listAction.pushBack(action);
        action = Sequence::create(listAction);
        item->runAction(action);
        delay += 0.15;
    }
}

void LayerMenu::callbackRestart(cocos2d::Ref* pSender)
{
    if (nullptr == _callbackRestart) {
        return;
    }
    _callbackRestart(pSender);
}

void LayerMenu::callbackSave(cocos2d::Ref* pSender)
{
    if (nullptr == _callbackSave) {
        return;
    }
    _callbackSave(pSender);
}

void LayerMenu::callbackRank(cocos2d::Ref* pSender)
{
//    Director::getInstance()->replaceScene(MainScene::createScene());
    if (nullptr == _callbackRank) {
        return;
    }
    _callbackRank(pSender);
}

void LayerMenu::onEnter()
{
    Layer::onEnter();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(LayerMenu::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(LayerMenu::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(LayerMenu::onTouchEnded, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void LayerMenu::onExit()
{
    Layer::onExit();
}

bool LayerMenu::onTouchBegan(Touch* touch, Event* event)
{
    int index = btnMore->getSelectedIndex(); // 0 off,   1 on
    if (0 == index) {
        return false;
    }
    return true;
}

void LayerMenu::onTouchMoved(Touch* touch, Event* event)
{
    
}

void LayerMenu::onTouchEnded(Touch* touch, Event* event)
{
    
}


