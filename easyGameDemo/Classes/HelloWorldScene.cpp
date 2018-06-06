#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

	schedule(schedule_selector(HelloWorld::UpdateTime), 1.0f, kRepeatForever, 0);
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	attackAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(attackAnimation, "attack");
	//attackAnimate->setTag(1);
	//执行动作
	//player->runAction(RepeatForever::create(attackAnimate));

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	//从贴图中以像素单位切割，创建关键帧
	//auto frame2 = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 90, 90)));
	// 死亡动画制作
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
	deadAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "dead");
	//执行动作
	//player->runAction(RepeatForever::create(deadAnimate));

	
	// Todo

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	auto frame3 = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 68, 101)));
	//运动动画制作
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.1f);
	runAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(runAnimation, "run");
	//执行动作
	//player->runAction(RepeatForever::create(runAnimate));
	// Todo
	time = Label::createWithTTF("200", "fonts/arial.ttf", 36);
	time->setPosition(visibleSize.width / 2, visibleSize.height - 100);
	this->addChild(time, 1);

	
	auto w_text = Label::createWithTTF("W", "fonts/arial.ttf", 36);
	auto w_menuItem = MenuItemLabel::create(w_text, CC_CALLBACK_0(HelloWorld::wCallBack, this));
	w_menuItem->setPosition(100, 150);
	
	auto s_text = Label::createWithTTF("S", "fonts/arial.ttf", 36);
	auto s_menuItem = MenuItemLabel::create(s_text, CC_CALLBACK_0(HelloWorld::sCallBack, this));
	s_menuItem->setPosition(100, 61);
	
	auto a_text = Label::createWithTTF("A", "fonts/arial.ttf", 36);
	auto a_menuItem = MenuItemLabel::create(a_text, CC_CALLBACK_0(HelloWorld::aCallBack, this));
	a_menuItem->setPosition(51, 100);
	
	auto d_text = Label::createWithTTF("D", "fonts/arial.ttf", 36);
	auto d_menuItem = MenuItemLabel::create(d_text, CC_CALLBACK_0(HelloWorld::dCallBack, this));
	d_menuItem->setPosition(150, 100);
	
	auto x_text = Label::create("X", "fonts/arial.ttf", 36);
	auto x_menuItem = MenuItemLabel::create(x_text, CC_CALLBACK_0(HelloWorld::Attack, this));
	x_menuItem->setPosition(visibleSize.width - 18, 100);
	
	auto y_text = Label::create("Y", "fonts/arial.ttf", 36);
	auto y_menuItem = MenuItemLabel::create(y_text, CC_CALLBACK_0(HelloWorld::Dead, this));
	y_menuItem->setPosition(visibleSize.width - 50, 50);
	
	
	auto menu = Menu::create(x_menuItem, y_menuItem, w_menuItem, s_menuItem, a_menuItem, d_menuItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);
	
    return true;
}

void HelloWorld::wCallBack() {
	//player->runAction(runAnimate);
	if (done) {
		auto move = MoveBy::create(1.0f, Vec2(0, 50));
		auto move2 = MoveTo::create(1.0f, Vec2(player->getPositionX(), visibleSize.height - 50));
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		if (player->getPositionY() + 50 >= visibleSize.height) {
			player->runAction(move2);
		}
		else player->runAction(move);
		player->runAction(sequence);
	}
}
void HelloWorld::sCallBack() {
	//player->runAction(runAnimate);
	if (done) {
		auto move = MoveBy::create(1.0f, Vec2(0, -50));
		auto move2 = MoveTo::create(1.0f, Vec2(player->getPositionX(), 50));
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		if (player->getPositionY() - 50 <= 0) {
			player->runAction(move2);
		}
		else player->runAction(move);
		player->runAction(sequence);
	}
	
}
void HelloWorld::aCallBack() {
	//player->runAction(runAnimate);
	if (done == true) {
		done = false;
		auto move = MoveBy::create(1.0f, Vec2(-50, 0));
		auto move2 = MoveTo::create(1.0f, Vec2(20, player->getPositionY()));
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		if (player->getPositionX() - 50 <= 0) {
			player->runAction(move2);
		}
		else player->runAction(move);
		player->runAction(sequence);
	}
	
}
void HelloWorld::dCallBack() {
	//player->runAction(runAnimate);
	if (done == true) {
		done = false;
		auto move = MoveBy::create(1.0f, Vec2(50, 0));
		auto move2 = MoveTo::create(1.0f, Vec2(visibleSize.width - 20, player->getPositionY()));
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		if (player->getPositionX() + 50 >= visibleSize.width) {
			player->runAction(move2);
		}
		else player->runAction(move);
		player->runAction(sequence);
	}
	
}
void HelloWorld::Attack() {
	if (done) {
		auto attackAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("attack"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(attackAnimate, setDoneToTrue, NULL);
		done = false;
		player->runAction(sequence);
		auto HPPlus20 = ProgressTo::create(1.0f, pT->getPercentage() + 20);
		auto HPPlusTo100 = ProgressTo::create(1.0f, 100.0f);
		if (pT->getPercentage() <= 80) {
			pT->runAction(HPPlus20);
		}
		else{
			pT->runAction(HPPlusTo100);
		}
	}
	//auto animation = AnimationCache::getInstance()->addAnimation("attack");	
}
void HelloWorld::Dead() {
	if (done) {
		auto deadAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("dead"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(deadAnimate, setDoneToTrue, NULL);
		done = false;
		player->runAction(sequence);
		auto HPReduce20 = ProgressTo::create(1.0f, pT->getPercentage() - 20);
		auto HPReduceTo0 = ProgressTo::create(1.0f, 0.0f);
		if (pT->getPercentage() >= 20) {
			
			pT->runAction(HPReduce20);
		}
		
		else {
			pT->runAction(HPReduceTo0);
		}
	}
}
void HelloWorld::UpdateTime(float dt) {
	dtime--;
	if (dtime == 0) unschedule(schedule_selector(HelloWorld::UpdateTime));
	char str[5];
	sprintf(str, "%d", dtime);
	time->setString(str);
}


