#pragma once
#include "cocos2d.h"

class EndScene : public cocos2d::Scene
{
public:
    // 1. Modified createScene to accept the coin count.
    static cocos2d::Scene* createScene(int coins);

    // 2. A new create function
    static EndScene* create(int coins);

    // 3. A new init method that receives the coin count.
    virtual bool initWithCoins(int coins);

private:
    int finalCoinCount;
    cocos2d::Sprite* titleSprite;
    cocos2d::Sprite* animationSprite;

    // 4. A helper function to draw the stars.
    void renderStars();
};