#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "CServer.h"
#include "ConfigMgr.h"
int main()
{
    // 用于读取配置文件
    ConfigMgr& gCfgMgr = ConfigMgr::Inst();
    try 
    {
        std::string gate_port_str = gCfgMgr["GateServer"]["Port"];
        unsigned short gate_port = atoi(gate_port_str.c_str());
        net::io_context ioc{ 1 };// 用于监听连接，连接池中的用于处理连接
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code&error,int signal_number)// 这里就是那两个auto的意思
            {
                if (error)
                {
                    return;
                }

                ioc.stop();
            });

        std::make_shared<CServer>(ioc, gate_port)->Start();
        std::cout << "///////////////////////////////////" << gate_port << std::endl;
        std::cout << "Gate Server listen on port:" << gate_port << std::endl;
        std::cout << "///////////////////////////////////" << gate_port << std::endl << std::endl << std::endl;
        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}