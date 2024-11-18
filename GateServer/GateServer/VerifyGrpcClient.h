#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class VerifyGrpcClient :public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	// 获取验证码
	GetVarifyRsp GetVarifyCode(std::string email)
	{
		ClientContext context;// 上下文 
		GetVarifyRsp reply;// 回包
		GetVarifyReq request;// 请求

		request.set_email(email);// 设置email成员

		// 请求并返回状态（发起 RPC 调用）
		Status status = stub_->GetVarifyCode(&context, request, &reply);// 请求后返回状态
		if (status.ok())
		{
			return reply;
		}
		else
		{
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}
private:
	VerifyGrpcClient()
	{
		// 创建一个 gRPC 通信通道 --- gRPC 通信的基础。客户端通过此通道与服务端交互
		std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());// 使用不安全的

		// 创建存根并绑定到指定的通信通道
		// 存根的作用：提供接口，用于调用远程服务方法（例如 GetVarifyCode）。
		//			   客户端通过存根发送请求到服务器并接收响应。
		stub_ = VarifyService::NewStub(channel);
	}
	std::unique_ptr<VarifyService::Stub> stub_;
};

