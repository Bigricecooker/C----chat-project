#include <iostream>
#include <csignal>
#include "ConfigMgr.h"
#include "CServer.h"
#include "RedisMgr.h"
#include "ChatServiceImpl.h"
#include "AsioIOServicePool.h"


int main()
{
	// ��ȡ�����ļ�
	auto& cfg = ConfigMgr::Inst();
	auto server_name = cfg["SelfServer"]["Name"];
	auto server_port = cfg["SelfServer"]["Port"];
	try 
	{
		auto pool = AsioIOServicePool::GetInstance();

		// ���õ�ǰ��¼��Ϊ0
		RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, "0");
		

		// ����һ���߳�����gRPC����
		std::string server_address(cfg["SelfServer"]["Host"] + ":" + cfg["SelfServer"]["RPCPort"]);
		ChatServiceImpl service;
		grpc::ServerBuilder builder;
		// �����˿ں���ӷ���
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		// ����������gRPC������
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
		std::cout << "RPC Server listening on " << server_address << std::endl;
		//��������һ���̴߳���grpc����
		std::thread  grpc_server_thread([&server]() {
			server->Wait();
			});


		// �����˳�
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