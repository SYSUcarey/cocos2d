#include "UsersInfoScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

using namespace cocos2d::network;
using namespace rapidjson;

cocos2d::Scene * UsersInfoScene::createScene() {
  return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
  if (getUserButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + getUserButton->getContentSize().height / 2;
    getUserButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(getUserButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  limitInput = TextField::create("limit", "arial", 24);
  if (limitInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    limitInput->setPosition(Vec2(x, y));
    this->addChild(limitInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
	// get the parameter limit
	std::string limit = limitInput->getString();

	// 发出网络请求
	// type: GET
	// url: /users?limit=$num
	// 配置GET请求
	HttpRequest* request = new HttpRequest();
	std::string url = "http://127.0.0.1:8000/users?limit=" + limit;
	request->setUrl(url);
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::onHttpRequestCompleted, this));

	HttpClient::getInstance()->send(request);
	request->release();
}


void UsersInfoScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
	// 处理response相应

	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		CCLOG("response failed!");
		CCLOG("error buffer: %s", response->getErrorBuffer());
		return;
	}

	// get the response Json
	std::vector<char> *buffer = response->getResponseData();
	std::string responseJson = "";
	for (int i = 0; i < buffer->size(); i++) {
		responseJson += (*buffer)[i];
	}
	CCLOG(responseJson.c_str());

	// use rapidjson to convert json to data we need
	rapidjson::Document document;
	document.Parse<0>(responseJson.c_str());
	// convert failed
	if (document.HasParseError()) {
		CCLOG("GetParseError: %s\n", document.GetParseError());
		return;
	}

	// convert succeed
	// get the data
	std::string msg;
	if (document["status"] == false) {
		msg = document["msg"].GetString();
		messageBox->setString(msg);
		return;
	}
	
	std::string userInfo = "";
	rapidjson::Value& userdatas = document["data"];
	for (int i = 0; i < userdatas.Size(); i++) {
		// 获得用户名
		userInfo += "Username: ";
		userInfo += userdatas[i]["username"].GetString();
		userInfo += "\n";
		// 获得用户的卡组
		userInfo += "Deck:\n";
		rapidjson::Value& userdecks = userdatas[i]["deck"];
		for (int j = 0; j < userdecks.Size(); j++) {
			rapidjson::Value& object = userdecks[j];
			for (auto iterator = object.MemberBegin(); iterator != object.MemberEnd(); ++iterator) {
				userInfo += "  ";
				userInfo += iterator->name.GetString();
				userInfo += " : ";
				userInfo += std::to_string(iterator->value.GetInt());
				userInfo += "\n";
			}
			userInfo += "  ---\n";
		}
		userInfo += "  ---\n";
	}
	messageBox->setString(userInfo);
}
