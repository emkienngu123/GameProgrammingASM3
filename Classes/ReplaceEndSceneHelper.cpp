#include "ReplaceEndSceneHelper.h"
#include "EndScene.h"

USING_NS_CC;

// 1. createScene now creates a base Scene and adds our Helper layer to it
cocos2d::Scene* ReplaceEndSceneHelper::createScene(int coinCount)
{
    auto scene = Scene::create();
    auto layer = ReplaceEndSceneHelper::create(coinCount); // Pass count to create
    scene->addChild(layer);
    return scene;
}

// 2. This is our new create function
ReplaceEndSceneHelper* ReplaceEndSceneHelper::create(int coinCount)
{
    ReplaceEndSceneHelper* pRet = new(std::nothrow) ReplaceEndSceneHelper();
    if (pRet && pRet->initWithCoins(coinCount)) // Call our new init
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}


// 3. Renamed init() to initWithCoins()
bool ReplaceEndSceneHelper::initWithCoins(int coinCount)
{
    if (!Scene::init()) return false;

    // 4. Store the coin count
    this->finalCoinCount = coinCount;

    auto wlayer = LayerColor::create(Color4B::BLACK);
    this->addChild(wlayer);

    return true;
}

void ReplaceEndSceneHelper::onEnter()
{
    Scene::onEnter();
    makeSequence();
}

void ReplaceEndSceneHelper::onExit()
{
    Scene::onExit();
}

void ReplaceEndSceneHelper::makeSequence()
{
    auto node = Sprite::create();

    auto callback = CallFunc::create(this, callfunc_selector(ReplaceEndSceneHelper::restartFunc));
    auto seq = Sequence::create(DelayTime::create(2.1f), callback, nullptr);

    this->addChild(node);
    node->runAction(seq);
}

// 5. THIS IS THE KEY CHANGE
// restartFunc() now passes the stored count to EndScene
void ReplaceEndSceneHelper::restartFunc()
{
    _director->replaceScene(TransitionFade::create(1.0f, EndScene::createScene(this->finalCoinCount)));
}