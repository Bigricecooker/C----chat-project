#include <iostream>
#include <csignal>
#include "ConfigMgr.h"


int main()
{
	// 获取配置文件
	auto& cfg = ConfigMgr::Inst();
	auto server_name = cfg["SelfServer"]["Name"];

	try 
	{
		boost::asio::io_context io_context;
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		signals.async_wait([&io_context](const boost::system::error_code& error, int signal_number) {
			io_context.stop();
			});

		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}