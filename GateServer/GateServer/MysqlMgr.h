#pragma once
#include "const.h"
#include "MySqlPool.h"
#include "singleton.h"

class MysqlMgr :public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);// 注册用户
    bool CheckEmail(const std::string& name, const std::string& email);// 判断用户与邮箱是否对应
    bool UpdatePwd(const std::string& name, const std::string& pwd);// 重置密码
private:
    MysqlMgr();
    MysqlDao  _dao;
};

