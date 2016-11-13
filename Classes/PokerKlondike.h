//
//  PokerKlondike.h
//  KillTime
//
//  Created by keltonxian on 5/12/14.
//
//

#ifndef __KillTime__PokerKlondike__
#define __KillTime__PokerKlondike__

#include "KHeader.h"

#define PK_ZORDER_NORMAL   100
#define PK_ZORDER_SHOW     200

typedef enum {
    KLO_SPADE_A = 1, KLO_SPADE_2, KLO_SPADE_3, KLO_SPADE_4, KLO_SPADE_5, KLO_SPADE_6, KLO_SPADE_7, KLO_SPADE_8, KLO_SPADE_9, KLO_SPADE_10, KLO_SPADE_J, KLO_SPADE_Q, KLO_SPADE_K,
    KLO_HEART_A, KLO_HEART_2, KLO_HEART_3, KLO_HEART_4, KLO_HEART_5, KLO_KLO_HEART_6, KLO_HEART_7, KLO_HEART_8, KLO_HEART_9, KLO_HEART_10, KLO_HEART_J, KLO_HEART_Q, KLO_HEART_K,
    KLO_CLUB_A, KLO_CLUB_2, KLO_CLUB_3, KLO_CLUB_4, KLO_CLUB_5, KLO_CLUB_6, KLO_CLUB_7, KLO_CLUB_8, KLO_CLUB_9, KLO_CLUB_10, KLO_CLUB_J, KLO_CLUB_Q, KLO_CLUB_K,
    KLO_DIAMOND_A, KLO_DIAMOND_2, KLO_DIAMOND_3, KLO_DIAMOND_4, KLO_DIAMOND_5, KLO_DIAMOND_6, KLO_DIAMOND_7, KLO_DIAMOND_8, KLO_DIAMOND_9, KLO_DIAMOND_10, KLO_DIAMOND_J, KLO_DIAMOND_Q, KLO_DIAMOND_K,
    KLO_JOKER_BLACK, KLO_JOKER_RED,

} POKERKLONDIKE_TYPE;

typedef enum {
    KLO_ACE = 1, KLO_TWO, KLO_THREE, KLO_FOUR, KLO_FIVE, KLO_SIX, KLO_SEVEN, KLO_EIGHT, KLO_NIGHT, KLO_TEN, KLO_JACK, KLO_QUEEN, KLO_KING,
} POKERKLONDIKE_NUM;

typedef enum {
    KLO_SPADE = 1, KLO_HEART, KLO_CLUB, KLO_DIAMOND,
} POKERKLONDIKE_FACE;

class PokerKlondike : public Sprite {
public:
    static void getPack(Vector<PokerKlondike *> *list);
    static void randomPack(Vector<PokerKlondike *> *list);
    static PokerKlondike* create(int style, int ptype);
    bool isInRect(Touch* touch);
    void printDetail();
    int getPtype();
    int getFace();
    int getNum();
    bool isBack();
    void turnFront();
    void turnBack();
    void cbTurnFront(Node *pSender);
    void cbTurnBack(Node *pSender);
    void addActionFlipFront(Vector<FiniteTimeAction *> *actionList, float time);
    void addActionFlipBack(Vector<FiniteTimeAction *> *actionList, float time);
    void flipFront(float delay, float time);
    void flipBack(float delay, float time);
    int getStatus();
private:
    
    int getFaceString(char *string, int len);
    void setFace();
    void setFront();
    void setSize();
    Rect getRect();
    PokerKlondike(int ptype);
    virtual ~PokerKlondike();
    int _ptype;
    int _num;
    int _face;
    int _width;
    int _height;
    int _status;
    Point _pos;
    
    virtual void onEnter() override;
    virtual void onExit() override;
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
};

#endif /* defined(__KillTime__PokerKlondike__) */
