#pragma once

#include <string>
#include "const.h"
#include <iostream>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
class LogicSystem;

class MsgNode
{
public:
	MsgNode(short max_len);
	virtual ~MsgNode();

	void Clear();


	short _cur_len;
	short _total_len;
	char* _data;
};

// ���ڵ�
class RecvNode :public MsgNode
{
	friend class LogicSystem;
public:
	RecvNode(short max_len,short msg_id);

private:
	short _msg_id;
};

// д�ڵ�
class SendNode :public MsgNode 
{
	friend class LogicSystem;
public:
	SendNode(const char* msg, short max_len, short msg_id);
private:
	short _msg_id;
};