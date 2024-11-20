#pragma once
#include "singleton.h"
#include <vector>
#include "const.h"

class AsioIOServicePool:public Singleton<AsioIOServicePool>
{
	friend class Singleton<AsioIOServicePool>;
public:
	using IOService = boost::asio::io_context;
	using Work = boost::asio::io_context::work;// 作用：防止io_context没有绑定事件run()直接返回
	using WorkPtr = std::unique_ptr<Work>;

	~AsioIOServicePool();
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;

	boost::asio::io_context& GetIOService();// 返回一个io_context
	void Stop();
private:
	AsioIOServicePool(std::size_t size=2);
	std::vector<IOService> _ioServices;
	std::vector<WorkPtr> _works;
	std::vector<std::thread> _threads;
	std::size_t _nextioService;
};

