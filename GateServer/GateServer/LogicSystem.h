#pragma once
#include "singleton.h"
#include "const.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;// Ö´ÐÐº¯Êý
class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton;
public:
	~LogicSystem();
	bool HandleGet(std::string path, std::shared_ptr<HttpConnection> connection);
	void RegGet(std::string url, HttpHandler handler);// ×¢²áGETÇëÇó

	void RegPost(std::string url, HttpHandler handler);// ×¢²áPOSTÇëÇó
	bool HandlePost(std::string path, std::shared_ptr<HttpConnection> connection);

private:
	LogicSystem();
	std::map<std::string, HttpHandler> _post_handlers;
	std::map<std::string, HttpHandler> _get_handlers;
};

