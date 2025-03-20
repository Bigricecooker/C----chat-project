#include "MsgNode.h"
#include "LogicSystem.h"

MsgNode::MsgNode(short max_len) :_total_len(max_len), _cur_len(0)
{
	_data = new char[_total_len + 1]();
	_data[_total_len] = '\0';// ��ʵ����Ҫ��������ȫ����ʼ��Ϊ '\0'��
}

MsgNode::~MsgNode()
{
	std::cout << "destruct MsgNode" << std::endl;
	delete[] _data;
}

void MsgNode::Clear()
{
	memset(_data, 0, _total_len);
	_cur_len = 0;
}

RecvNode::RecvNode(short max_len, short msg_id) :MsgNode(max_len), _msg_id(msg_id)
{
}

SendNode::SendNode(const char* msg, short max_len, short msg_id) :MsgNode(max_len+ HEAD_TOTAL_LEN), _msg_id(msg_id)
{
	// ��id�ͳ���ת��Ϊ�����ֽ���
	short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(_msg_id);
	short max_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);

	memcpy(_data, &msg_id_host, HEAD_ID_LEN);
	memcpy(_data+ HEAD_ID_LEN, &max_len_host, HEAD_DATA_LEN);
	memcpy(_data + HEAD_TOTAL_LEN, msg, max_len);
}
