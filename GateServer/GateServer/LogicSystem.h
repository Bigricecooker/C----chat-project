#pragma once
#include "singleton.h"
#include "const.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)>HttpHandler;
class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton;
public:
	~LogicSystem();
	bool HandleGet(std::string, std::shared_ptr<HttpConnection>);
	void RegGet(std::string url, HttpHandler handler);

private:
	LogicSystem();
	std::map<std::string, HttpHandler> _post_handlers;
	std::map<std::string, HttpHandler> _get_handlers;
};

