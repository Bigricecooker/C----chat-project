#include "MysqlMgr.h"

MysqlMgr::~MysqlMgr()
{
}

MysqlMgr::MysqlMgr()
{
}

// 注册用户
int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return _dao.RegUser(name, email, pwd);
}

// 判断用户与邮箱是否对应
bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email)
{
	return _dao.CheckEmail(name,email);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd)
{
	return _dao.UpdatePwd(name, pwd);
}

bool MysqlMgr::Checkpwd(const std::string& name, const std::string& pwd, UserInfo& userinfo)
{
	return _dao.Checkpwd(name, pwd, userinfo);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(const int& uid)
{
	return _dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(const string& name)
{
	return _dao.GetUser(name);
}
