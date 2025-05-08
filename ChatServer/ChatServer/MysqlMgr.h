#pragma once
#include "const.h"
#include "MySqlPool.h"
#include "singleton.h"

// 数据库连接管理类
class MysqlMgr :public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);// 注册用户
    bool CheckEmail(const std::string& name, const std::string& email);// 判断用户与邮箱是否对应
    bool UpdatePwd(const std::string& name, const std::string& pwd);// 重置密码
    bool Checkpwd(const std::string& name, const std::string& pwd, UserInfo& userinfo);// 判断用户与密码是否对应
    bool AddFriendApply(const int& from, const int& to);// 将请求好友数据写入数据库
    std::shared_ptr<UserInfo> GetUser(const int& uid);// uid查询用户信息
    std::shared_ptr<UserInfo> GetUser(const string& name);// name查询用户信息
private:
    MysqlMgr();
    MysqlDao  _dao;
};

