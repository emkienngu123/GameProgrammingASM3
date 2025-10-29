#pragma once
#include "cocos2d.h"

class ReplaceEndSceneHelper : public cocos2d::Scene {
public:
    // 1. createScene signature is updated (cosmetic typo fix)
    static cocos2d::Scene* createScene(int coinCount = 0);

    // 2. We add a custom create function that takes the count
    static ReplaceEndSceneHelper* create(int coinCount);

    // 3. We use a custom init function
    virtual bool initWithCoins(int coinCount);

    // 4. We remove CREATE_FUNC()

    void onEnter();
    void onExit();
    void makeSequence();
    void restartFunc();

private:
    // 5. Add a variable to store the count
    int finalCoinCount;
};