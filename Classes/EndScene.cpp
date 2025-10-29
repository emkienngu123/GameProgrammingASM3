#include "EndScene.h"

USING_NS_CC;

// 1. Modified createScene to pass coins to create()
cocos2d::Scene* EndScene::createScene(int coins)
{
    auto scene = Scene::create();
    auto layer = EndScene::create(coins);
    scene->addChild(layer);
    return scene;
}

// 2. New create function implementation
EndScene* EndScene::create(int coins)
{
    EndScene* pRet = new(std::nothrow) EndScene();
    if (pRet && pRet->initWithCoins(coins)) // Call our new initWithCoins
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

// 3. Renamed your original init() to initWithCoins(int coins)
bool EndScene::initWithCoins(int coins)
{
    if (!Scene::init()) return false;

    // Store the coin count
    this->finalCoinCount = coins;
    log("Final coin count received: %d", this->finalCoinCount);

    // --- All your original init code ---
    titleSprite = Sprite::create("Megaman/UI/EndScene.png");
    titleSprite->setAnchorPoint(Vec2::ZERO);
    titleSprite->getTexture()->setAliasTexParameters();

    animationSprite = Sprite::create();
    animationSprite->setPosition(Vec2(128.0f, 155.0f));
    animationSprite->setScale(0.8f);

    int allSheetNum = 8;
    std::string sPath = "Megaman/Animation/Megaman_Idle.plist";
    std::string sName = "idle_";
    float frameDelay = 0.10f;

    auto cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile(sPath);

    Vector<SpriteFrame*> animFrames;
    for (int i = 0; i < allSheetNum; i++) {
        std::string _frames = StringUtils::format("%s%02d.png", sName.c_str(), i);
        SpriteFrame* frame = cache->getSpriteFrameByName(_frames);
        frame->getTexture()->setAliasTexParameters();
        animFrames.pushBack(frame);
    }

    auto animation = Animation::createWithSpriteFrames(animFrames, frameDelay);
    auto animate = Animate::create(animation);
    auto runRep = RepeatForever::create(animate);

    animationSprite->runAction(runRep);

    this->addChild(titleSprite);
    this->addChild(animationSprite);
    // --- End of original init code ---

    // 4. Call the new function to render stars
    this->renderStars();

    return true;
}

// 5. =================================================================
// START: MODIFIED renderStars() FUNCTION
// =================================================================
void EndScene::renderStars()
{
    // Get screen center
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);

    int totalStars = this->finalCoinCount;
    if (totalStars <= 0) {
        return; // Don't do anything if no stars
    }

    // --- You MUST provide your "your_star_image.png" path ---
    std::string starImage = "Megaman/Drops/star.png";

    // Use a temporary star to get dimensions for calculation
    auto tempStar = Sprite::create(starImage);
    if (tempStar == nullptr) {
        log("ERROR: 'your_star_image.png' not found. Cannot render stars.");
        return;
    }

    float starWidth = tempStar->getContentSize().width;
    float padding = 5.0f; // 5 pixels padding between stars

    // Calculate the total width of all stars
    float totalWidth = (totalStars * starWidth) + ((totalStars - 1) * padding);

    // Calculate the starting X position to center the block of stars
    float startX = center.x - (totalWidth / 2) + (starWidth / 2);

    // Set the Y position (e.g., 100 pixels from the bottom, adjust as needed)
    float yPos = 100.0f;

    // Create and position each star in a loop
    for (int i = 0; i < totalStars; i++)
    {
        auto star = Sprite::create(starImage);
        float xPos = startX + i * (starWidth + padding);
        star->setPosition(Vec2(xPos, yPos));
        this->addChild(star, 10);
    }
}
// =================================================================
// END: MODIFIED renderStars() FUNCTION
// =================================================================