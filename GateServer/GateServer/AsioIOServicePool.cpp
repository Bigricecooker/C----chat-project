#include "AsioIOServicePool.h"

AsioIOServicePool::~AsioIOServicePool()
{
	Stop();
	std::cout << "AsioIOServicePool destruct" << endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	auto& service = _ioServices[_nextioService++];
	if (_nextioService == _ioServices.size()) 
	{
		_nextioService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop()
{
	for (auto& work : _works)// unique_ptr,&不能去掉
	{
		work->get_io_context().stop();
		work.reset();
	}

	for (auto& t : _threads)
	{
		t.join();
	}
}

AsioIOServicePool::AsioIOServicePool(std::size_t size):_nextioService(0),_ioServices(size),_works(size)
{
	// 绑定
	for (std::size_t i = 0; i < size; ++i) {
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));
	}

	// 运行上下文
	for (std::size_t i = 0; i < _ioServices.size(); ++i)
	{
		_threads.emplace_back([this,i]() {
			_ioServices[i].run();
			});
	}
}