#include "Thunder.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Thunder::init() {
	if (!Layer::init()) {
		return false;
	}
	stoneType = 0;
	isMove = false;  // 是否点击飞船
	visibleSize = Director::getInstance()->getVisibleSize();

	// 创建背景
	auto bgsprite = Sprite::create("bg.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	// 创建飞船
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	player->setName("player");
	this->addChild(player, 1);

	// 显示陨石和子弹数量
	enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
	enemysNum->setColor(Color3B(255, 255, 255));
	enemysNum->setPosition(50, 60);
	this->addChild(enemysNum, 3);
	bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
	bulletsNum->setColor(Color3B(255, 255, 255));
	bulletsNum->setPosition(50, 30);
	this->addChild(bulletsNum, 3);

	addEnemy(5);        // 初始化陨石
	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);
	
	return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
	auto audioengine = SimpleAudioEngine::getInstance();
	audioengine->preloadBackgroundMusic("music/bgm.mp3");
	audioengine->preloadEffect("music/fire.wav");
	audioengine->preloadEffect("music/explore.wav");
}

//播放背景音乐
void Thunder::playBgm() {
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化陨石
void Thunder::addEnemy(int n) {
	enemys.clear();
	for (unsigned i = 0; i < 3; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemys.push_back(enemy);
			addChild(enemy, 1);
		}
	}
}

// 陨石向下移动并生成新的一行(加分项)
void Thunder::newEnemy() {
	// 旧的陨石往下移动一行
	for (auto i : enemys) {
		i->setPosition(Vec2(i->getPositionX(), i->getPositionY()-50));
	}
	//产生新的一行陨石
	char enemyPath[20];
	sprintf(enemyPath, "stone%d.png", stoneType+1);
	for (int i = 0; i < 5; i++) {
		auto enemy = Sprite::create(enemyPath);
		enemy->setAnchorPoint(Vec2(0.5, 0.5));
		enemy->setScale(0.5, 0.5);
		double width = visibleSize.width / (6.0),
			height = visibleSize.height - 50;
		enemy->setPosition(width * (i + 0.5), height);
		enemys.push_back(enemy);
		addChild(enemy, 1);
	}
	
}

// 移动飞船
void Thunder::movePlane(char c) {
	//根据movekey的值（'A'、'D'）决定向左还是向右移动
	//不能移出整个屏幕之外
	if (c == 'A' && (player->getPositionX() - player->getContentSize().width/2 >= 0)) {
		auto moveToLeft = MoveBy::create(0.04f, Vec2(-25, 0));
		player->runAction(moveToLeft);
	}
	else if (c == 'D' && (player->getPositionX() + player->getContentSize().width/2 <= visibleSize.width)) {
		auto moveToRight = MoveBy::create(0.04f, Vec2(25, 0));
		player->runAction(moveToRight);
	}
}

//发射子弹
void Thunder::fire() {
	
	//创建子弹
	auto bullet = Sprite::create("bullet.png");
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);

}

// 切割爆炸动画帧
void Thunder::explosion() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	blast.reserve(8);
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(193 * i + 1, 0, 142, 142)));
		blast.pushBack(frame);
	}
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(193 * i + 1, 193, 142, 142)));
		blast.pushBack(frame);
	}
	auto blastAnimation = Animation::createWithSpriteFrames(blast, 0.1f);
	blastAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(blastAnimation, "blast");
}

// 更新调度器，0.04f秒更新一次
void Thunder::update(float f) {
	// 实时更新页面内陨石和子弹数量(不得删除)
	// 要求数量显示正确(加分项)
	char str[15];
	sprintf(str, "enemys: %d", enemys.size());
	enemysNum->setString(str);
	sprintf(str, "bullets: %d", bullets.size());
	bulletsNum->setString(str);

	// 飞船移动
	if (isMove)
		this->movePlane(movekey);

	static int ct = 0;
	static int dir = 4;
	++ct;
	if (ct == 120)
		ct = 40, dir = -dir;
	else if (ct == 80) {
		dir = -dir;
		newEnemy();  // 陨石向下移动并生成新的一行(加分项)
		stoneType = (stoneType + 1) % 3;
	}
	else if (ct == 20)
		ct = 40, dir = -dir;

	//陨石左右移动
	for (Sprite* s : enemys) {
		if (s != NULL) {
			s->setPosition(s->getPosition() + Vec2(dir, 0));
		}
	}

	//发射子弹后子弹的轨迹问题
	//1.子弹往上飞
	//2.飞出界要从list中移除这颗子弹
	if (bullets.size() > 0) {
		for (Sprite* b : bullets) {
			auto moveUp = MoveBy::create(0.04f, Vec2(0, 10));
			b->runAction(moveUp);
		}
		list<Sprite*>::iterator it = bullets.begin();
		for (it = bullets.begin(); it != bullets.end();) {
			float y = (*it)->getPositionY();
			if (y > visibleSize.height) {
				(*it)->removeFromParent();
				it = bullets.erase(it);
			}
			else it++;
		}
	}

	// 分发自定义事件
	EventCustom e("meet");
	_eventDispatcher->dispatchEvent(&e);
	
	// 检测是否机毁人亡
	for (auto i : enemys) {
		if (i->getPosition().getDistance(player->getPosition()) < 40 || i->getPositionY() - 50 < 0) {
			stopAc();
		}
	}
}

// 自定义碰撞事件
void Thunder::meet(EventCustom * event) {
	// 判断子弹是否打中陨石并执行对应操作
	// 遍历两个list
	list<Sprite*>::iterator it = bullets.begin();
	list<Sprite*>::iterator it2 = enemys.begin();
	for (it = bullets.begin(); it != bullets.end(); ) {
		for (it2 = enemys.begin(); it2 != enemys.end(); ) {
			//二者距离小于25视为子弹打到陨石
			bool judge = ((*it)->getPosition().getDistance((*it2)->getPosition())< 25);
			if (judge) {
				//1.移除子弹
				(*it)->removeFromParent();
				it = bullets.erase(it);
				if (it != bullets.begin()) it--;
				//2.陨石播放爆炸动画，伴随爆炸声响，爆炸完后移除
				Animate* explode = Animate::create(AnimationCache::getInstance()->getAnimation("blast"));
				Sprite* temp = (*it2);
				auto removeEnemy = CallFunc::create([temp]() {
					temp->removeFromParentAndCleanup(true);
				});
				auto sequence = Sequence::create(explode, removeEnemy, NULL);
				SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
				(*it2)->runAction(sequence);
				it2 = enemys.erase(it2);
				break;
			}
			else it2++;
		}
		if(it!=bullets.end())it++;
	}
}

// 结束游戏
void Thunder::stopAc() {
	// 1.飞机爆炸+音效
	Animate* explode = Animate::create(AnimationCache::getInstance()->getAnimation("blast"));
	Sprite* temp = player;
	auto removeEnemy = CallFunc::create([temp]() {
		temp->removeFromParentAndCleanup(true);
	});
	auto sequence = Sequence::create(explode, removeEnemy, NULL);
	SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
	player->runAction(sequence);
	// 2.停止所有调度器和监听器
	unschedule(schedule_selector(Thunder::update));
	_eventDispatcher->removeAllEventListeners();
	// 3.显示提示GameOver
	Sprite* tip = Sprite::create("gameOver.png");
	tip->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(tip);
}

// 添加自定义监听器
void Thunder::addCustomListener() {
	// 添加对子弹打到敌人的meet事件监听
	auto listenCustom = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenCustom, this);
}

// 添加键盘事件监听器
void Thunder::addKeyboardListener() {
	//添加当键盘按下和松开的两个回调函数
	auto listenerKeyboard = EventListenerKeyboard::create();
	listenerKeyboard->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	listenerKeyboard->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerKeyboard, this);
}

// 键盘按下事件回调函数
void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;
	}
}

// 键盘松开事件回调函数
void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
		if (movekey == 'A') isMove = false;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		if(movekey == 'D') isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void Thunder::addTouchListener() {
	auto listenerTouch = EventListenerTouchOneByOne::create();
	listenerTouch->setSwallowTouches(true);
	listenerTouch->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	listenerTouch->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	listenerTouch->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerTouch, this);
}	

// 鼠标点击发射炮弹
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	// 当鼠标点击到飞机处，是拖动飞机移动，不发射子弹
	if (touch->getLocation().getDistance(player->getPosition()) <= 30) {
		isClick = true;
	}
	else fire();
	return true;

}

// 鼠标点击松开的响应
void Thunder::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;	
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	// 用鼠标偏移量来控制飞船移动
	// 还要确保飞船不会飞出屏幕
	if (isClick) {
		Vec2 delta = touch->getDelta();
		Vec2 deltaX = Vec2(delta.x, 0);
		if (player->getPositionX() + delta.x >= 0 && player->getPositionX()+ delta.x <= visibleSize.width) {
			player->setPosition(Vec2(player->getPositionX() + delta.x, player->getPositionY()));
		}	
	}
}




