#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Monster.h"
#include "sqlite3.h"
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
	schedule(schedule_selector(HelloWorld::createMonsters), 3.0f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::hitByMonster), 0.1f, kRepeatForever, 0);
	
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
	addChild(pT, 2);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 1);

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
	deadAnimation->setRestoreOriginalFrame(false);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "dead");


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

	//利用(.tmx)TiledMap文件创建背景地图
	TMXTiledMap* map = TMXTiledMap::create("map.tmx");
	//设置地图位置
	map->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	//设置地图锚点
	map->setAnchorPoint(Vec2(0.5, 0.5));
	//拉伸背景图铺满游戏屏幕
	map->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(map, 0);

	//数据库保存分数
	std::string path = "save.db";
	int result = sqlite3_open(path.c_str(), &database);
	std::string sql = "create table hero(ID int primary key not null, score int)";
	result = sqlite3_exec(database, sql.c_str(), NULL, NULL, NULL);
	sql = "insert into hero values(1,0)";
	result = sqlite3_exec(database, sql.c_str(), NULL, NULL, NULL);

	//显示储存的击杀怪物的分数
	//从数据库中获得当前的分数
	int row, col;
	char** re;
	sqlite3_get_table(database, "select * from hero", &re, &row, &col, NULL);
	sqlite3_free_table(re);
	std::string score = re[1*col+1];
	//创建Label显示
	score_text = Label::create(score, "fonts/arial.ttf", 36);
	score_text->setPosition(visibleSize.width / 2, visibleSize.height - 50);
	this->addChild(score_text, 1);

    return true;
}

void HelloWorld::wCallBack() {
	//player->runAction(runAnimate);
	if (done) {
		done = false;
		//角色移动位置
		auto move = MoveBy::create(1.0f, Vec2(0, 50));
		auto move2 = MoveTo::create(1.0f, Vec2(player->getPositionX(), visibleSize.height - 50));
		if (player->getPositionY() + 50 >= visibleSize.height) {
			player->runAction(move2);
		}
		else player->runAction(move);
		//角色移动动画
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		player->runAction(sequence);
	}
}
void HelloWorld::sCallBack() {
	//player->runAction(runAnimate);
	if (done == true) {
		//角色移动位置（判断是否出边界）
		done = false;
		auto move = MoveBy::create(1.0f, Vec2(0, -50));
		auto move2 = MoveTo::create(1.0f, Vec2(player->getPositionX(), 50));
		if (player->getPositionY() - 50 <= 0) {
			player->runAction(move2);
		}
		else player->runAction(move);
		//角色移动动画
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		player->runAction(sequence);
	}
	
}
void HelloWorld::aCallBack() {
	//player->runAction(runAnimate);
	if (done == true) {
		done = false;
		//角色移动位置
		auto move = MoveBy::create(1.0f, Vec2(-50, 0));
		auto move2 = MoveTo::create(1.0f, Vec2(20, player->getPositionY()));
		if (player->getPositionX() - 50 <= 0) {
			player->runAction(move2);
		}
		else player->runAction(move);
		//角色移动动画
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		player->runAction(sequence);
		//角色翻转朝向
		player->setFlippedX(true);
	}
	
}
void HelloWorld::dCallBack() {
	//player->runAction(runAnimate);
	if (done == true) {
		done = false;
		//角色移动位置
		auto move = MoveBy::create(1.0f, Vec2(50, 0));
		auto move2 = MoveTo::create(1.0f, Vec2(visibleSize.width - 20, player->getPositionY()));
		if (player->getPositionX() + 50 >= visibleSize.width) {
			player->runAction(move2);
		}
		else player->runAction(move);

		//角色移动动画
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		player->runAction(sequence);

		//角色翻转朝向
		player->setFlippedX(false);
	}
	
}
void HelloWorld::Attack() {
	/*
	 * 处理攻击事件
	 * 1.角色播放攻击动画
	 * 2.移除攻击范围内的怪物
	 * 3.消灭了怪物的话，角色回血
	 */
	if (done == true) {
		//角色播放攻击动画
		done = false;
		auto attackAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("attack"));
		auto setDoneToTrue = CallFunc::create([&]() {done = true; });
		auto sequence = Sequence::create(attackAnimate, setDoneToTrue, NULL);
		player->runAction(sequence);
		//获得工厂单例
		auto fac = Factory::getInstance();
		//获得攻击范围
		Rect playerRect = player->getBoundingBox();
		Rect attackRect = Rect( playerRect.getMinX() - 40, playerRect.getMinY()-20,
			playerRect.getMaxX() - playerRect.getMinX() + 80, playerRect.getMaxY() - playerRect.getMinY()+40);
		//获得攻击范围内的第一只怪物小可爱
		Sprite* collision = fac->collider(attackRect);
		//定义回血调用函数
		auto HPPlus20 = ProgressTo::create(1.0f, pT->getPercentage() + 20);
		auto HPPlusTo100 = ProgressTo::create(1.0f, 100.0f);
		if (collision != NULL) {
			//移除怪物
			fac->removeMonster(collision);

			//从数据库中获得当前的分数
			int row, col;
			char** re;
			sqlite3_get_table(database, "select * from hero", &re, &row, &col, NULL);
			sqlite3_free_table(re);
			//击杀怪物后，分数加一，写回数据库
			int next_score = std::atoi(re[1*col+1]) + 1;
			std::string nextScore = std::to_string(next_score);
			std::string sql = "update hero set score = " + nextScore + " where id = 1";
			int result = sqlite3_exec(database, sql.c_str(), NULL, NULL, NULL);
			//修改界面分数
			score_text->setString(nextScore);

			//回血
			if (pT->getPercentage() < 80) {
				pT->runAction(HPPlus20);
			}
			else {
				pT->runAction(HPPlusTo100);
			}
		}
	}
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
		if (pT->getPercentage() > 20) {
			
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
void HelloWorld::createMonsters(float dt) {
	//获得工厂单例
	auto fac = Factory::getInstance();
	//创建一个怪物，随机放到一个位置
	auto monster = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	monster->setPosition(x, y);
	this->addChild(monster, 3);
	//获得角色位置，所有怪物向角色移动
	Vec2 playerPos = player->getPosition();
	fac->moveMonster(playerPos, 3.0f);
}
void HelloWorld::hitByMonster(float dt) {
	/* 
	 * 处理碰撞事件
	 * 1.移除怪物
	 * 2.角色扣血,如果血量降到0，播放死亡动画，任何动画不再响应
	 */ 

	//获得工厂单例
	auto fac = Factory::getInstance();
	//获得碰撞范围内的第一只怪物小可爱
	Sprite* collision = fac->collider(player->getBoundingBox());
	if (collision != NULL) {
		//移除怪物
		fac->removeMonster(collision);
		auto HPReduce20 = ProgressTo::create(1.0f, pT->getPercentage() - 20);
		auto HPReduceTo0 = ProgressTo::create(1.0f, 0.0f);
		//血量扣血后不到0，未死亡
		if (pT->getPercentage() > 20) {

			pT->runAction(HPReduce20);
		}
		//血量扣血后到0，死亡
		else {
			//停止当前的所有动画
			player->stopAllActions();
			//显示提示：Game Over
			time->setString("Game over!");
			unschedule(schedule_selector(HelloWorld::UpdateTime));
			//血条减少至0
			pT->runAction(HPReduceTo0);
			//停止所有动画响应
			unschedule(schedule_selector(HelloWorld::hitByMonster));
			unschedule(schedule_selector(HelloWorld::createMonsters));
			//停止角色其他动作的响应
			done = false;
			//播放死亡动画
			auto deadAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("dead"));
			player->runAction(deadAnimate);
		}
	}
}





