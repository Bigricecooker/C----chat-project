#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"

RPConPool::RPConPool(std::size_t poolsize, std::string host, std::string port):_poolSize(poolsize),_host(host),
_port(port),_b_stop(false)
{
	for (std::size_t i = 0; i < _poolSize; i++)
	{
		// ����һ�� gRPC ͨ��ͨ�� --- gRPC ͨ�ŵĻ������ͻ���ͨ����ͨ�������˽���
		std::shared_ptr<Channel> channel = grpc::CreateChannel(_host+":"+_port, grpc::InsecureChannelCredentials());// ʹ�ò���ȫ��

		// ����������󶨵�ָ����ͨ��ͨ��
		// ��������ã��ṩ�ӿڣ����ڵ���Զ�̷��񷽷������� GetVarifyCode����
		//			   �ͻ���ͨ������������󵽷�������������Ӧ��
		_connections.push(VarifyService::NewStub(channel));// �ߵ��ƶ���ֵ(VarifyService::NewStub(channel)��һ����ʱ�Ķ���)
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);// ���߳��ȴ�����
	Close();
	while (!_connections.empty()) {
		_connections.pop();
	}
}

void RPConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

std::unique_ptr<VarifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);// lock_guard��֧����ʾ�ͷ���
	_cond.wait(lock, [this]() {// ��������Ϊtrueʱ�Ż᳢�Ի�ȡ����������ִ��
		if (_b_stop)
		{
			return true;// ֹͣ
		}
		return !_connections.empty();
		});

	//���ֹͣ��ֱ�ӷ��ؿ�ָ��
	if (_b_stop) {
		return  nullptr;
	}
	
	auto context = std::move(_connections.front());
	_connections.pop();
	return context;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}
	_connections.push(std::move(context));
	_cond.notify_one();
}

//----------------------------------------------------------------------

GetVarifyRsp VerifyGrpcClient::GetVarifyCode(std::string email)
{
	ClientContext context;// ������ 
	GetVarifyRsp reply;// �ذ�
	GetVarifyReq request;// ����

	request.set_email(email);// ����email��Ա

	// ���󲢷���״̬������ RPC ���ã�
	auto stub = _pool->getConnection();
	Status status = stub->GetVarifyCode(&context, request, &reply);// ����󷵻�״̬
	if (status.ok())
	{
		_pool->returnConnection(std::move(stub));
		return reply;
	}
	else
	{
		_pool->returnConnection(std::move(stub));
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}

VerifyGrpcClient::VerifyGrpcClient()
{
	//// ����һ�� gRPC ͨ��ͨ�� --- gRPC ͨ�ŵĻ������ͻ���ͨ����ͨ�������˽���
	//std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());// ʹ�ò���ȫ��
	//// ����������󶨵�ָ����ͨ��ͨ��
	//// ��������ã��ṩ�ӿڣ����ڵ���Զ�̷��񷽷������� GetVarifyCode����
	////			   �ͻ���ͨ������������󵽷�������������Ӧ��
	//stub_ = VarifyService::NewStub(channel);

	// ��ʼ���أ��ӳ��л�ȡ�����ŵ��Ĵ��
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
	_pool.reset(new RPConPool(5, host, port));
}
