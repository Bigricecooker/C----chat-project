#include "MySqlPool.h"
#include "ConfigMgr.h"

MySqlPool::MySqlPool(std::string url, std::string user, std::string passwd, std::string schema, std::size_t poolSize) :_url(url),
_user(user), _passwd(passwd), _schema(schema), _poolSize(poolSize), _b_stop(false)
{
	try
	{
		for (std::size_t i = 0; i < _poolSize; i++)
		{
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			std::unique_ptr<sql::Connection> con(driver->connect(_url, _user, _passwd));
			con->setSchema(_schema);
			_connections.push(std::move(con));
		}
	}
	catch (sql::SQLException& e) {
		// 处理异常
		std::cout << "mysql pool init failed" << std::endl;
	}
}

MySqlPool::~MySqlPool()
{
	std::unique_lock<std::mutex> lock(_mutex);
	while (!_connections.empty()) {
		_connections.pop();// unique_ptr会自动释放
	}
}

std::unique_ptr<sql::Connection> MySqlPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]() {
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

	auto connection = std::move(_connections.front());
	_connections.pop();
	return connection;
}

void MySqlPool::returnConnection(std::unique_ptr<sql::Connection> connection)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}
	_connections.push(std::move(connection));
	_cond.notify_one();
}

void MySqlPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}




//---------------------------------------------------------------------------------------------------------------


MysqlDao::MysqlDao()
{
	ConfigMgr& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["Mysql"]["Host"];
	std::string port = gCfgMgr["Mysql"]["Port"];
	std::string user = gCfgMgr["Mysql"]["User"];
	std::string passwd = gCfgMgr["Mysql"]["Passwd"];
	std::string schema = gCfgMgr["Mysql"]["Schema"];

	_pool.reset(new MySqlPool(host + ":" + port, user, passwd, schema, 5));
}

MysqlDao::~MysqlDao()
{
	_pool->Close();
}

int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	// 获取一个连接
	auto con = _pool->getConnection();

	try
	{
		if (con == nullptr) {
			//_pool->returnConnection(std::move(con));// 把空指针放入池也会有bug
			return false;
		}
		// 准备调用存储过程
		std::unique_ptr<sql::PreparedStatement>stmt(con->prepareStatement("CALL reg_user(?,?,?,@result)"));
		// 设置输入参数
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		// 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
		  // 执行存储过程
		stmt->execute();
		// 如果存储过程设置了会话变量或有其他方式获取输出参数的值，你可以在这里执行SELECT查询来获取它们
	   
		// 例如，如果存储过程设置了一个会话变量@result来存储输出结果，可以这样获取：
		std::unique_ptr<sql::Statement> stmtResult(con->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
		if (res->next()) {
			int result = res->getInt("result");
			std::cout << "Result: " << result << std::endl;
			_pool->returnConnection(std::move(con));
			return result;
		}
		_pool->returnConnection(std::move(con));
		return -1;
	}
	catch (sql::SQLException& e) {
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return -1;
	}
}

// 判断用户与邮箱是否对应操作
bool MysqlDao::CheckEmail(const std::string& name, const std::string& email)
{
	// 获取一个连接
	auto con = _pool->getConnection();
	try
	{
		if (con == nullptr)
		{
			return false;
		}

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT email FROM user WHERE name = ?"));
		
		// 绑定参数
		pstmt->setString(1, name);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		// 遍历结果集并判断
		while (res->next())
		{
			std::cout << "Check Email: " << res->getString("email") << std::endl;
			if (email != res->getString("email")) {
				_pool->returnConnection(std::move(con));
				return false;
			}
			_pool->returnConnection(std::move(con));
			return true;
		}
		return true;
	}
	catch (sql::SQLException& e)
	{
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& pwd)
{
	// 获取一个连接
	auto con = _pool->getConnection();

	try
	{
		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("Update user SET pwd = ? WHERE name = ?"));

		// 绑定参数
		pstmt->setString(2, name);
		pstmt->setString(1, pwd);

		// 执行更新
		int updateCount = pstmt->executeUpdate();

		std::cout << "Updated rows: " << updateCount << std::endl;
		_pool->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& e)
	{
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::Checkpwd(const std::string& name, const std::string& pwd, UserInfo& userinfo)
{
	// 获取一个连接
	auto con = _pool->getConnection();
	try
	{
		if (con == nullptr)
		{
			return false;
		}

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT * FROM user WHERE name = ?"));

		// 绑定参数
		pstmt->setString(1, name);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		std::string origin_pwd = "";
		std::string email = "";
		int uid = 0;
		// 遍历结果集并判断
		while (res->next())
		{
			origin_pwd = res->getString("pwd");
			email = res->getString("email");
			uid = res->getInt("uid");
			std::cout << "Check pwd: " << res->getString("pwd") << std::endl;
			break;
		}

		if (res->next())
		{
			origin_pwd = res->getString("pwd");
			std::cout << "Check pwd: " << origin_pwd << std::endl;

			if (pwd != origin_pwd) {
				_pool->returnConnection(std::move(con));
				return false;
			}
		}

		// 只有再有数据是才填充
		userinfo.name = name;
		userinfo.email = email;
		userinfo.uid = uid;
		userinfo.pwd = pwd;
		_pool->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& e)
	{
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(const int& uid)
{
	auto con = _pool->getConnection();

	try
	{
		if (con == nullptr)
		{
			return nullptr;
		}
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT * FROM user WHERE uid = ?"));
		pstmt->setInt(1, uid); // 将uid替换为你要查询的uid

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		// 遍历结果集
		if(res->next()) 
		{
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			// 后面再加
			user_ptr->nick = res->getString("nick");
			user_ptr->desc = res->getString("desc");
			user_ptr->sex = res->getInt("sex");
			user_ptr->icon = res->getString("icon");
			user_ptr->uid = uid;
		}
		_pool->returnConnection(std::move(con));
		return user_ptr;
	}
	catch (sql::SQLException& e)
	{
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(const std::string& name)
{
	auto con = _pool->getConnection();

	try
	{
		if (con == nullptr)
		{
			return nullptr;
		}
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT * FROM user WHERE name = ?"));
		pstmt->setString(1, name); // 将name替换为你要查询的name

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		// 遍历结果集
		if (res->next())
		{
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			// 后面再加
			/*user_ptr->nick = res->getString("nick");
			user_ptr->desc = res->getString("desc");
			user_ptr->sex = res->getInt("sex");
			user_ptr->icon = res->getString("icon");*/
			user_ptr->uid = res->getInt("uid");
		}
		_pool->returnConnection(std::move(con));
		return user_ptr;
	}
	catch (sql::SQLException& e)
	{
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}

bool MysqlDao::AddFriendApply(const int& from, const int& to)
{
	auto con = _pool->getConnection();
	try 
	{
		if (con == nullptr)
		{
			return false;
		}

		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO friend_apply (from_uid,to_uid) values(?,?)"
			"ON DUPLICATE KEY UPDATE from_uid = from_uid,to_uid = to_uid"));
		pstmt->setInt(1, from);
		pstmt->setInt(2, to);
		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0) {
			return false;
		}
		_pool->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& e)
	{
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
	return true;
}

bool MysqlDao::GetApplyList(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list, int begin, int limit)
{
	auto con = _pool->getConnection();
	if (con == nullptr) {
		return false;
	}

	try {
		// 准备SQL语句, 根据起始id和限制条数返回列表
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("select apply.from_uid, apply.status, user.name, "
			"user.nick, user.sex from friend_apply as apply join user on apply.from_uid = user.uid where apply.to_uid = ? "
			"and apply.id > ? order by apply.id ASC LIMIT ? "));

		pstmt->setInt(1, to_uid); // 将uid替换为你要查询的uid
		pstmt->setInt(2, begin); // 起始id
		pstmt->setInt(3, limit); //偏移量
		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		// 遍历结果集
		while (res->next()) {
			auto name = res->getString("name");
			auto uid = res->getInt("from_uid");
			auto status = res->getInt("status");
			auto nick = res->getString("nick");
			auto sex = res->getInt("sex");
			auto apply_ptr = std::make_shared<ApplyInfo>(uid, name, "", "", nick, sex, status);
			list.push_back(apply_ptr);
		}
		_pool->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& e) {
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::AuthFriendApply(int uid, int touid)
{
	auto con = _pool->getConnection();
	if (con == nullptr) {
		return false;
	}

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE friend_apply SET status = 1 "
			"WHERE from_uid = ? AND to_uid = ?"));
		//反过来的申请时from，验证时to
		pstmt->setInt(1, touid); // from id
		pstmt->setInt(2, uid);
		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		_pool->returnConnection(std::move(con));
		if (rowAffected < 0) {
			return false;
		}
		return true;
	}
	catch (sql::SQLException& e) {
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::AddFriend(int uid, int touid, std::string back_name)
{
	auto con = _pool->getConnection();
	if (con == nullptr) {
		return false;
	}

	try {

		//开始事务
		con->setAutoCommit(false);

		// 准备第一个SQL语句, 插入认证方好友数据
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) "
			"VALUES (?, ?, ?) "
		));
		//反过来的申请时from，验证时to
		pstmt->setInt(1, uid); // from id
		pstmt->setInt(2, touid);
		pstmt->setString(3, back_name);
		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0) {
			con->rollback();
			return false;
		}

		//准备第二个SQL语句，插入申请方好友数据
		std::unique_ptr<sql::PreparedStatement> pstmt2(con->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) "
			"VALUES (?, ?, ?) "
		));
		//反过来的申请时from，验证时to
		pstmt2->setInt(1, touid); // from id
		pstmt2->setInt(2, uid);
		pstmt2->setString(3, "");
		// 执行更新
		int rowAffected2 = pstmt2->executeUpdate();
		if (rowAffected2 < 0) {
			con->rollback();
			return false;
		}

		// 提交事务
		con->commit();
		std::cout << "addfriend insert friends success" << std::endl;

		_pool->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& e) {
		// 如果发生错误，回滚事务
		if (con) {
			con->rollback();
		}
		_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}
