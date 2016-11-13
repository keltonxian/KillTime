//
//  Klondike.h
//  KillTime
//
//  Created by keltonxian on 5/12/14.
//
//

#ifndef __KillTime__Klondike__
#define __KillTime__Klondike__

#include "KHeader.h"
#include "PokerKlondike.h"

class Klondike : public Layer {
public:
    static Scene* createScene();
    virtual bool init() override;
    void callbackRestart(cocos2d::Ref* pSender);
    void callbackSave(cocos2d::Ref* pSender);
    void callbackRank(cocos2d::Ref* pSender);
    CREATE_FUNC(Klondike);
private:
    void pauseGame(cocos2d::Ref* pSender);
    bool canTouch;
    bool isGameOver;
    void reStart();
    virtual void onEnter() override;
    virtual void onExit() override;
    bool onTouchBegan(Touch* touch, Event* event) override;
    void onTouchMoved(Touch* touch, Event* event) override;
    void onTouchEnded(Touch* touch, Event* event) override;
    virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) override;
    virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
    
    void setupPoker();
    void dealPoker();
    void callbackEnable(float delta);
    Vector<PokerKlondike *> pack;
    std::vector<Vector<PokerKlondike *>> pokerList;
    Vector<PokerKlondike *> scoreList;
    
    PokerKlondike *touchPoker;
    Point pokerPos;
    int _score;
    Label *labelScore;
    void setScore(int score);
    int _time;
    Label *labelTime;
    Point offset;
    void updateTime(float delta);
    void setTime(int timeNum);
    
    PokerKlondike *_lastDeckPoker;
    bool _isLoop;
    
    void insertPoker(PokerKlondike *poker, int listType);
    void removePoker(PokerKlondike *poker);
    Point getPokerPosition(int listType, int index);
    void resetChildren(PokerKlondike *poker, bool isFollow);
    bool canFollow(PokerKlondike *lastPoker, PokerKlondike *poker, int actionType);
    int canMoveToList(PokerKlondike *poker, Point pos);
    int canMoveToSlot(PokerKlondike *poker, Point pos);
    float moveAnimation(Sprite *sprite, Point targetPos, float delay, float time);
    void callback_move(Node* pSender);
    void nextPokers();
    void increaseScore(PokerKlondike *poker, int listType);
    void decreaseScore();
    void saveGame();
    bool loadGame();
    bool checkLose();
    bool checkWin();
    void callback_move2(Node* pSender);
    void gameWinAnimate();
    void gameLoseAnimate();
    void showResult(float delta);
};

class KlondikeResult : public Layer {
public:
    virtual bool init() override;
    CREATE_FUNC(KlondikeResult);
    void setWin(int time, int score, const ccMenuCallback& callbackRestart);
    void setLose(const ccMenuCallback& callbackRestart);
private:
    virtual void onEnter() override;
    virtual void onExit() override;
    bool onTouchBegan(Touch* touch, Event* event) override;
    void onTouchMoved(Touch* touch, Event* event) override;
    void onTouchEnded(Touch* touch, Event* event) override;
    
    ccMenuCallback _callbackRestart;
    void doNext();
    int saveResult(int time, int score, std::vector<int> *results);
};

#endif /* defined(__KillTime__Klondike__) */
