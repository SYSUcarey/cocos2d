#include "LoginRegisterScene.h"
#include "ui\CocosGUI.h"
#include "json\document.h"
#include "Utils.h"
#include "json\writer.h"
#include "json\stringbuffer.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace rapidjson;

cocos2d::Scene * LoginRegisterScene::createScene() {
  return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
  if (loginButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + loginButton->getContentSize().height / 2;
    loginButton->setPosition(Vec2(x, y));
  }

  auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
  if (registerButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + registerButton->getContentSize().height / 2 + 100;
    registerButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [] (Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  usernameInput = TextField::create("username", "arial", 24);
  if (usernameInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    usernameInput->setPosition(Vec2(x, y));
    this->addChild(usernameInput, 1);
  }

  passwordInput = TextField::create("password", "arial", 24);
  if (passwordInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 130.0f;
    passwordInput->setPosition(Vec2(x, y));
    this->addChild(passwordInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 200.0f;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
	// 获得用户输入的username和password	
	std::string username = usernameInput->getString();
	std::string password = passwordInput->getString();
	
	// 发出网络请求
	// type: POST
	// url: /auth
	// 配置post请求
	HttpRequest* request = new HttpRequest();
	request->setUrl("http://127.0.0.1:8000/auth");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRequestCompleted, this));
	
	// write the post json data
	std::string postData = analysisJson(username, password);
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	// send the request to server
	HttpClient::getInstance()->enableCookies(nullptr);
	HttpClient::getInstance()->send(request);
	request->release();
}

void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
	// 获得用户输入的username和password	
	std::string username = usernameInput->getString();
	std::string password = passwordInput->getString();

	// 发出网络请求
	// type: POST
	// url: /users
	// 配置post请求
	HttpRequest* request = new HttpRequest();
	request->setUrl("http://127.0.0.1:8000/users");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRequestCompleted, this));

	// write the post json data
	std::string postData = analysisJson(username, password);
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	// send the request to server
	HttpClient::getInstance()->send(request);
	request->release();
}

void LoginRegisterScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response ) {
	// login_response_json_format:
	// take an example: {"status":true, "msg": "login succeed!"}
	//	if succeed，status = true；else，status = false
	//	login the same username，status = true

	// register_response_json_format:
	// take an example: {"status":true, "msg": "register succeed!"}
	// if succeed，status = true；else，status = false
	// register the same username，status = false
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
	bool status;
	if (document.IsObject()) {
		if (document.HasMember("msg")) msg = document["msg"].GetString();
		if (document.HasMember("status")) status = document["status"].GetBool();
	}

	messageBox->setString(msg);

}


// 根据用户输入的username("cbb"), password("cbb")生成json
// request-json-format:
// {"username":"cbb","password":"cbb"}
std::string LoginRegisterScene::analysisJson(std::string username, std::string password) {
	
	// 用rapidjson获得发送请求的json
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	//rapidjson::Value array(rapidjson::kArrayType);
	//rapidjson::Value object(rapidjson::kObjectType);
	//object.AddMember("username", username, allocator);
	//object.AddMember("password", password, allocator);
	//array.PushBack(object,allocator);

	document.AddMember("username", rapidjson::Value(username.c_str(),allocator), allocator);
	document.AddMember("password", rapidjson::Value(password.c_str(), allocator), allocator);

	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	document.Accept(writer);

	return buffer.GetString();
}
