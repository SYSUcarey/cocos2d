#include "MenuScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "cocos2d.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

	auto gameTitle = Sprite::create("gold-miner-text.png");
	gameTitle -> setPosition(Vec2(visibleSize.width/3*2 + origin.x, visibleSize.height/4*3 + origin.y));
	this->addChild(gameTitle, 1);

	auto stone = Sprite::create("menu-start-gold.png");
	stone -> setPosition(Vec2(origin.x + visibleSize.width-200 , origin.y + 120));
	this -> addChild(stone, 1);

	auto gameStartItem = MenuItemImage::create(
		"start-0.png",
		"start-1.png",
		CC_CALLBACK_1(MenuScene::gameStartCallBack, this));

	if (gameStartItem == nullptr ||
		gameStartItem->getContentSize().width <= 0 ||
		gameStartItem->getContentSize().height <= 0)
	{
		problemLoading("'start-0.png' and 'start-1.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width - 200;
		float y = origin.y + 150;
		gameStartItem->setPosition(Vec2(x, y));
	}

	// create menu, it's an autorelease object
	auto menu = Menu::create(gameStartItem , NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	

    return true;
}

void MenuScene::gameStartCallBack(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	//Director::getInstance()->end();
	auto gameScene = GameScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, gameScene, Color3B(0,255,255)));
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);


}

