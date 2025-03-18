#include "RedisMgr.h"
#include "ConfigMgr.h"

RedisMgr::~RedisMgr()
{
	this->Close();
}

RedisMgr::RedisMgr()
{
	auto& gCfgMgr = ConfigMgr::Inst();
	auto host = gCfgMgr["Redis"]["Host"];
	auto port = gCfgMgr["Redis"]["Port"];
	auto pwd = gCfgMgr["Redis"]["Passwd"];
	_pool.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}

//bool RedisMgr::Connect(const std::string& host, int port)
//{
//	this->connect = redisConnect(host.c_str(), port);
//	if (this->connect != NULL && this->connect->err)
//	{
//		std::cout << "connect error " << this->connect->errstr << std::endl;
//		return false;
//	}
//	return true;
//}

bool RedisMgr::Get(const std::string& key, std::string& value)
{
	auto connect = _pool->getConnection();

	auto reply = (redisReply*)redisCommand(connect, "GET %s", key.c_str());
	
	if (reply == NULL) {
		std::cout << "[ GET  " << key << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_STRING) {
		std::cout << "[ GET  " << key << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	value = reply->str;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
	return true;
}

bool RedisMgr::Set(const std::string& key, const std::string& value)
{
	auto connect = _pool->getConnection();
	// 执行redis命令行
	auto reply = (redisReply*)redisCommand(connect, "SET %s %s", key.c_str(), value.c_str());

	//如果返回NULL则说明执行失败
	if (NULL == reply)
	{
		std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	// 如果执行失败则释放连接
	if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0)))
	{
		std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	// 执行成功 释放redisCommand执行后返回的redisReply所占用的内存
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
	return true;
}

bool RedisMgr::Auth(const std::string& password)
{
	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommand(connect, "AUTH %s", password.c_str());
	
	if (reply->type == REDIS_REPLY_ERROR) {
		std::cout << "认证失败" << std::endl;
		// 执行成功 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	else {
		// 执行成功 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		std::cout << "认证成功" << std::endl;
		return true;
	}
}

bool RedisMgr::LPush(const std::string& key, const std::string& value)
{
	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());
	
	if (NULL == reply)
	{
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
	
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::LPop(const std::string& key, std::string& value)
{
	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommand(connect, "LPOP %s ", key.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
	_pool->returnConnection(connect);
	freeReplyObject(reply);
	return true;
}

bool RedisMgr::RPush(const std::string& key, const std::string& value)
{
	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());
	if (NULL == reply)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::RPop(const std::string& key, std::string& value)
{
	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommand(connect, "RPOP %s ", key.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value)// key：一级  hkey：二级  value：值
{
	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
	const char* argv[4];
	size_t argvlen[4];
	argv[0] = "HSET";
	argvlen[0] = 4;
	argv[1] = key;
	argvlen[1] = strlen(key);
	argv[2] = hkey;
	argvlen[2] = strlen(hkey);
	argv[3] = hvalue;
	argvlen[3] = hvaluelen;

	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommandArgv(connect, 4, argv, argvlen);
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
	const char* argv[3];
	size_t argvlen[3];
	argv[0] = "HGET";
	argvlen[0] = 4;
	argv[1] = key.c_str();
	argvlen[1] = key.length();
	argv[2] = hkey.c_str();
	argvlen[2] = hkey.length();

	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommandArgv(connect, 3, argv, argvlen);
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		return "";
	}
	std::string value = reply->str;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
	return value;
}

bool RedisMgr::HDel(const std::string& key, const std::string& field)
{
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}


	redisReply* reply = (redisReply*)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
	if (reply == nullptr) {
		std::cerr << "HDEL command failed" << std::endl;
		return false;
	}

	bool success = false;
	if (reply->type == REDIS_REPLY_INTEGER) {
		success = reply->integer > 0;
	}

	freeReplyObject(reply);
	return success;
}

bool RedisMgr::Del(const std::string& key)
{
	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommand(connect, "DEL %s", key.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::ExistsKey(const std::string& key)
{
	auto connect = _pool->getConnection();
	auto reply = (redisReply*)redisCommand(connect, "exists %s", key.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
		std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

void RedisMgr::Close()
{
	//redisFree(connect);
	_pool->Close();
}




//-------------------------------------------------------------------------------------------
RedisConPool::RedisConPool(std::size_t poolsize, string host, int port, string pwd) :_poolSize(poolsize),
_host(host), _port(port), _pwd(pwd), _b_stop(false)
{
	for (std::size_t i = 0; i < _poolSize; i++)
	{
		auto context= redisConnect(_host.c_str(), port);
		if (context == nullptr && context->err != 0)
		{
			if (context != nullptr)
			{
				redisFree(context);
			}
			continue;
		}
		
		auto reply = (redisReply*)redisCommand(context, "AUTH %s", _pwd.c_str());
		if (reply->type == REDIS_REPLY_ERROR)
		{
			std::cout << "认证失败" << std::endl;
			freeReplyObject(reply);
			redisFree(context);
		}

		freeReplyObject(reply);
		std::cout << "认证成功" << std::endl;
		_connections.push(context);
	}
}

RedisConPool::~RedisConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	while (!_connections.empty())
	{
		// 先回收内存再移出队列
		auto context = _connections.front();
		redisFree(context);
		_connections.pop();
	}
}

redisContext* RedisConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this](){
		if (_b_stop)
		{
			return true;
		}
		return !_connections.empty();
		});

	if (_b_stop)
	{
		return nullptr;
	}

	auto context = _connections.front();
	_connections.pop();
	return context;
}

void RedisConPool::returnConnection(redisContext* context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}
	_connections.push(context);
	_cond.notify_one();
}

void RedisConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}


