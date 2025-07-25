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

		// 遍历结果集并判断
		while (res->next())
		{
			std::cout << "Check pwd: " << res->getString("pwd") << std::endl;
			break;
		}

		if (pwd != res->getString("pwd")) {
			_pool->returnConnection(std::move(con));
			return false;
		}

		userinfo.name = name;
		userinfo.email = res->getString("email");
		userinfo.uid = res->getInt("uid");
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
