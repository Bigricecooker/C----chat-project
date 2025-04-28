#include "MysqlMgr.h"

MysqlMgr::~MysqlMgr()
{
}

MysqlMgr::MysqlMgr()
{
}

// ע���û�
int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return _dao.RegUser(name, email, pwd);
}

// �ж��û��������Ƿ��Ӧ
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
