#pragma once
#include "cocos2d.h"
#include "Monster.h"
#include "sqlite3.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
        
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime = 200;
	cocos2d::ProgressTimer* pT;
	void HelloWorld::wCallBack();
	void HelloWorld::sCallBack();
	void HelloWorld::aCallBack();
	void HelloWorld::dCallBack();
	void HelloWorld::Attack();
	void HelloWorld::Dead();
	void HelloWorld::UpdateTime(float dt);
	bool done = true;
	void HelloWorld::createMonsters(float dt);
	void HelloWorld::hitByMonster(float dt);
	sqlite3* database = NULL;
	cocos2d::Label* score_text;
};
