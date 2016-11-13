//
//  LayerMenu.h
//  KillTime
//
//  Created by keltonxian on 8/29/14.
//
//

#ifndef __KillTime__LayerMenu__
#define __KillTime__LayerMenu__

#include "KHeader.h"

class LayerMenu : public Layer {
public:
    virtual bool init() override;
    void setCallbackRestart(const ccMenuCallback& callbackRestart);
    void setCallbackSave(const ccMenuCallback& callbackSave);
    void setCallbackRank(const ccMenuCallback& callbackRank);
    CREATE_FUNC(LayerMenu);
private:
    ccMenuCallback _callbackRestart;
    ccMenuCallback _callbackSave;
    ccMenuCallback _callbackRank;
    MenuItemToggle *btnMore;
    MenuItemSprite *btnRestart;
    MenuItemSprite *btnSave;
    MenuItemSprite *btnRank;
    LayerColor *layerColor;
    Sprite *bgMore;
    void callbackMore(cocos2d::Ref* pSender);
    void callbackRestart(cocos2d::Ref* pSender);
    void callbackSave(cocos2d::Ref* pSender);
    void callbackRank(cocos2d::Ref* pSender);
    virtual void onEnter() override;
    virtual void onExit() override;
    bool onTouchBegan(Touch* touch, Event* event) override;
    void onTouchMoved(Touch* touch, Event* event) override;
    void onTouchEnded(Touch* touch, Event* event) override;
};

#endif /* defined(__KillTime__LayerMenu__) */
