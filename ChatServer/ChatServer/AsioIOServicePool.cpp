#include "AsioIOServicePool.h"

AsioIOServicePool::~AsioIOServicePool()
{
	std::cout << "AsioIOServicePool destruct" << endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	auto& io_context = _IOServices[_nextIOService++];
	_nextIOService = _nextIOService % _IOServices.size();
	return io_context;

}

void AsioIOServicePool::Stop()
{
	// 先停止
	for (auto& work : _works)
	{
		work->get_io_context().stop();
		work.reset();
	}

	// 等待未完成的完成
	for (auto& thread : _threads)
	{
		thread.join();
	}
}

AsioIOServicePool::AsioIOServicePool(std::size_t size):_nextIOService(0),_IOServices(size), _works(size)
{
	for (std::size_t i = 0; i < size; i++)
	{
		_works[i] = WorkPtr(new Work(_IOServices[i]));
		_threads.emplace_back([this,i]() {
			this->_IOServices[i].run();
			});
	}

}