#include "CServer.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :_ioc(ioc),
_acceptor(ioc, tcp::endpoint(tcp::v4(), port))/*, _socket(ioc)*/ {
}

void CServer::Start()
{
    auto self = shared_from_this();

    auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);

    _acceptor.async_accept(/*_socket*/new_con->GetSocket(), [self, new_con](beast::error_code ec) {
        try
        {
            if (ec)
            {
                // 出错则继续监听其他连接
                self->Start();
                return;
            }

            // 启动处理
            new_con->Start();
            //继续监听
            self->Start();
        }
        catch (std::exception& ep)
        {
            std::cout << "exception is " << ep.what() << std::endl;
            self->Start();
        }
        });
}

