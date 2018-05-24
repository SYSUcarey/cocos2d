#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
	return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	
	auto background = Sprite::create("level-background-0.jpg");
	background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(background, 0);

	stoneLayer = Layer::create();
	stoneLayer->ignoreAnchorPointForPosition(false);
	stoneLayer->setAnchorPoint(ccp(0,0));
	stoneLayer->setTag(1);
	this->addChild(stoneLayer,1);

	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(560, 480));
	stoneLayer->addChild(stone);

	mouseLayer = Layer::create();
	mouseLayer->ignoreAnchorPointForPosition(false);
	mouseLayer->setAnchorPoint(ccp(0, 0));
	mouseLayer->setPosition(origin.x + 0, visibleSize.height / 2 + origin.y);
	mouseLayer->setTag(2);
	this->addChild(mouseLayer,2);

	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	mouse->setPosition(visibleSize.width/2, 0);
	mouse->setTag(0);
	mouseLayer->addChild(mouse, 1);
	
	
	auto shoot_text = Label::createWithTTF("Shoot", "fonts/msyh.ttf", 48);
	auto shootItem = MenuItemLabel::create(shoot_text, CC_CALLBACK_0(GameScene::shootCallBack,this));
	shootItem->setPosition(Vec2(760, 480));
	auto menu = Menu::create(shootItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);
	return true;
}

bool GameScene::onTouchBegan(Touch *touch, Event *unused_event) {
	auto mouseLayer = this->getChildByTag(2);
	auto location = touch->getLocation();

	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	this->addChild(cheese,0);
	
	auto disapper = FadeOut::create(6.0f);
	cheese->runAction(disapper);

	auto moveTo = MoveTo::create(2, mouseLayer->convertToNodeSpaceAR(cheese->getPosition()));
	//auto moveTo = MoveTo::create(2, location);
	//auto delay = 2.0;

	auto seq = Sequence::create(moveTo, nullptr);
	this->getChildByTag(2)->getChildByTag(0)->runAction(seq);
	
	return true;
}

void GameScene::shootCallBack() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto mouseLayer = this->getChildByTag(2);
	auto stoneLayer = this->getChildByTag(1);
	auto newStone = Sprite::create("stone.png");

	//shoot a stone
	newStone->setPosition(Vec2(560, 480));
	stoneLayer->addChild(newStone);
	auto mouseLocation = mouseLayer->convertToWorldSpaceAR(mouseLayer->getChildByTag(0)->getPosition());
	auto moveTo = MoveTo::create(1, mouseLocation);
	auto disapper = FadeOut::create(0.5);
	auto seq = Sequence::create(moveTo, disapper, nullptr);
	newStone->runAction(seq);

	//mouse move away
	auto newLocation = mouseLayer->convertToNodeSpaceAR(Vec2(CCRANDOM_0_1()*visibleSize.width, CCRANDOM_0_1()*visibleSize.height));
	auto mouseMoveTo = MoveTo::create(0.5, newLocation);
	mouseLayer->getChildByTag(0)->runAction(mouseMoveTo);

	//left a diamond
	auto diamond = Sprite::createWithSpriteFrameName("diamond-0.png");
	Animate* diamondAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("diamondAnimation"));
	diamond->runAction(RepeatForever::create(diamondAnimate));
	diamond->setPosition(mouseLocation);
	this->addChild(diamond);
}
