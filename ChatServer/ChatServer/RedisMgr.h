#pragma once
#include "singleton.h"
#include "const.h"

class RedisConPool;
// redis连接处理
// 一个连接使用一个方法后就返回连接池（销毁）
class RedisMgr :public Singleton<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    //// 连接到指定的 Redis 服务器
    //bool Connect(const std::string& host, int port);// 连接池中连接了
    // 根据给定的键获取值
    bool Get(const std::string& key, std::string& value);
    // 设置键值对
    bool Set(const std::string& key, const std::string& value);
    // 使用密码进行身份验证
    bool Auth(const std::string& password);
    // 将值推入列表的左侧
    bool LPush(const std::string& key, const std::string& value);
    // 从列表的左侧弹出一个值
    bool LPop(const std::string& key, std::string& value);
    // 将值推入列表的右侧
    bool RPush(const std::string& key, const std::string& value);
    // 从列表的右侧弹出一个值
    bool RPop(const std::string& key, std::string& value);
    // 为哈希表中的字段设置值
    bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
    // 为哈希表中的字段设置值，这个重载版本接受字符数组和长度（二进制数据用这个好些）
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    // 获取哈希表中字段的值
    std::string HGet(const std::string& key, const std::string& hkey);
    // 删除哈希表中的字段值
    bool HDel(const std::string& key, const std::string& field);
    // 删除指定的键
    bool Del(const std::string& key);
    // 检查键是否存在
    bool ExistsKey(const std::string& key);
    // 关闭Redis 连接
    void Close();
private:
	RedisMgr();

	//redisContext* _connect;// 连接
	//redisReply* _reply;// 回复
    std::unique_ptr<RedisConPool> _pool;// 连接池
};



// redis连接池
class RedisConPool
{
public:
    RedisConPool(std::size_t poolsize, string host, int port, string pwd);
    ~RedisConPool();
    // 获取一个连接
    redisContext* getConnection();
    // 返还连接
    void returnConnection(redisContext* context);
    // 关闭
    void Close();

private:
    std::atomic<bool> _b_stop;// 标识是否需要回收(原子类型)
    std::size_t _poolSize;// 大小
    std::string _host;
    int _port;
    std::string _pwd;// 密码
    std::queue<redisContext*> _connections;
    std::mutex _mutex;// 锁
    std::condition_variable _cond;// 互斥量
};

