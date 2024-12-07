// StatusServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include "StatusServiceImpl.h"
#include "ConfigMgr.h"

int main()
{
    // 读取配置
    auto& cfg = ConfigMgr::Inst();
    std::string server_address(cfg["StatusServer"]["Host"] + ":" + cfg["StatusServer"]["Port"]);

    StatusServiceImpl service;

    grpc::ServerBuilder builder;// 用于配置和创建gRPC服务器

    // 监听端口和添加服务
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());// grpc::InsecureServerCredentials()创建一个不安全的凭证对象
    builder.RegisterService(&service);// service是实现了服务器端接口的类的实例

    // 构建并启动gRPC服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());// builder.BuildAndStart()方法构建并启动gRPC服务器
    std::cout << "Server listening on " << server_address << std::endl;


    // 创建Boost.Asio的io_context
    boost::asio::io_context io_context;

    // 创建signal_set用于捕获SIGINT
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    // 设置异步等待SIGINT信号
    signals.async_wait([&server](const boost::system::error_code& error, int signal_number) {
        if (!error) {
            std::cout << "Shutting down server..." << std::endl;
            server->Shutdown(); // 优雅地关闭grpc服务器
        }
        });
}


