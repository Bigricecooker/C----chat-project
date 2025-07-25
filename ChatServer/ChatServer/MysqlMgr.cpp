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

bool MysqlMgr::AddFriendApply(const int& from, const int& to)
{
	return _dao.AddFriendApply(from, to);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(const int& uid)
{
	return _dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(const string& name)
{
	return _dao.GetUser(name);
}

bool MysqlMgr::GetApplyList(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list, int begin, int limit)
{
	return _dao.GetApplyList(to_uid,list,begin,limit);
}

bool MysqlMgr::AuthFriendApply(int uid, int touid)
{
	return _dao.AuthFriendApply(uid,touid);
}

bool MysqlMgr::AddFriend(int uid, int touid, std::string back_name)
{
	return _dao.AddFriend(uid,touid,back_name);
}

bool MysqlMgr::GetFriendList(int uid, std::vector<std::shared_ptr<UserInfo>>& friend_list)
{
	return _dao.GetFriendList(uid, friend_list);
}
