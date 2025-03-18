#include <iostream>
#include <csignal>
#include "ConfigMgr.h"
#include "CServer.h"
#include "RedisMgr.h"
#include "ChatServiceImpl.h"
#include "AsioIOServicePool.h"


int main()
{
	// 获取配置文件
	auto& cfg = ConfigMgr::Inst();
	auto server_name = cfg["SelfServer"]["Name"];
	auto server_port = cfg["SelfServer"]["Port"];
	try 
	{
		auto pool = AsioIOServicePool::GetInstance();

		// 设置当前登录数为0
		RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, "0");
		

		// 设置一个线程运行gRPC服务
		std::string server_address(cfg["SelfServer"]["Host"] + ":" + cfg["SelfServer"]["RPCPort"]);
		ChatServiceImpl service;
		grpc::ServerBuilder builder;
		// 监听端口和添加服务
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		// 构建并启动gRPC服务器
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
		std::cout << "RPC Server listening on " << server_address << std::endl;
		//单独启动一个线程处理grpc服务
		std::thread  grpc_server_thread([&server]() {
			server->Wait();
			});


		// 优雅退出
		boost::asio::io_context io_context;
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		signals.async_wait([&io_context](const boost::system::error_code& error, int signal_number) {
			io_context.stop();
			});

		auto port_str = cfg["SelfServer"]["Port"];
		CServer s(io_context, atoi(port_str.c_str()));

		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}