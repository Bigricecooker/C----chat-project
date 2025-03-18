#pragma once
#include "singleton.h"
#include "const.h"

class RedisConPool;
// redis���Ӵ���
// һ������ʹ��һ��������ͷ������ӳأ����٣�
class RedisMgr :public Singleton<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    //// ���ӵ�ָ���� Redis ������
    //bool Connect(const std::string& host, int port);// ���ӳ���������
    // ���ݸ����ļ���ȡֵ
    bool Get(const std::string& key, std::string& value);
    // ���ü�ֵ��
    bool Set(const std::string& key, const std::string& value);
    // ʹ��������������֤
    bool Auth(const std::string& password);
    // ��ֵ�����б�����
    bool LPush(const std::string& key, const std::string& value);
    // ���б����൯��һ��ֵ
    bool LPop(const std::string& key, std::string& value);
    // ��ֵ�����б���Ҳ�
    bool RPush(const std::string& key, const std::string& value);
    // ���б���Ҳ൯��һ��ֵ
    bool RPop(const std::string& key, std::string& value);
    // Ϊ��ϣ���е��ֶ�����ֵ
    bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
    // Ϊ��ϣ���е��ֶ�����ֵ��������ذ汾�����ַ�����ͳ��ȣ������������������Щ��
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    // ��ȡ��ϣ�����ֶε�ֵ
    std::string HGet(const std::string& key, const std::string& hkey);
    // ɾ����ϣ���е��ֶ�ֵ
    bool HDel(const std::string& key, const std::string& field);
    // ɾ��ָ���ļ�
    bool Del(const std::string& key);
    // �����Ƿ����
    bool ExistsKey(const std::string& key);
    // �ر�Redis ����
    void Close();
private:
	RedisMgr();

	//redisContext* _connect;// ����
	//redisReply* _reply;// �ظ�
    std::unique_ptr<RedisConPool> _pool;// ���ӳ�
};



// redis���ӳ�
class RedisConPool
{
public:
    RedisConPool(std::size_t poolsize, string host, int port, string pwd);
    ~RedisConPool();
    // ��ȡһ������
    redisContext* getConnection();
    // ��������
    void returnConnection(redisContext* context);
    // �ر�
    void Close();

private:
    std::atomic<bool> _b_stop;// ��ʶ�Ƿ���Ҫ����(ԭ������)
    std::size_t _poolSize;// ��С
    std::string _host;
    int _port;
    std::string _pwd;// ����
    std::queue<redisContext*> _connections;
    std::mutex _mutex;// ��
    std::condition_variable _cond;// ������
};

