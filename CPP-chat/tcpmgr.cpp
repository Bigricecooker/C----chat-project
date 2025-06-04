#include "tcpmgr.h"
#include <QAbstractSocket>
#include <QJsonDocument>
#include "usermgr.h"

TcpMgr::~TcpMgr()
{

}

TcpMgr::TcpMgr():_host(""),_port(0),_b_recv_pending(false),_message_id(0),_message_len(0)
{
    // 读写socket都是和聊天服务器



    // 连接socket建立成功
    connect(&_socket, &QTcpSocket::connected, this, [&]() {
        qDebug() << "Connected to server!";
        // 连接建立后发送消息
        emit sig_con_success(true);
    });

    // 连接套接字有可读数据（QT里非阻塞）
    connect(&_socket,&QTcpSocket::readyRead, this, [&](){
        // 当有数据可读时，读取所有数据
        // 读取所有数据并追加到缓冲区
        _buffer.append(_socket.readAll());// 存储读到的数据

        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);

        forever
        {
            // 先解析头部
            if(!_b_recv_pending)
            {
                // 未解析完true
                if(_buffer.size()<static_cast<int>(sizeof(quint16)*2))
                {
                    return;// 数据不够
                }

                // 预读取消息ID和消息长度，但不从缓冲区中移除
                stream>>_message_id>>_message_len;

                // 移除前4个字节的数据
                _buffer = _buffer.mid(sizeof(quint16)*2);

                // 输出读取的数据
                qDebug() << "Message ID:" << _message_id << ", Length:" << _message_len;
            }


            // 开始读取消息体
            if(_buffer.size()<_message_len)
            {
                _b_recv_pending = true;
                return;// 数据不够
            }

            _b_recv_pending = false;// 本次读取完成

            // 读取消息体
            QByteArray messageBody = _buffer.mid(0, _message_len);
            _buffer=_buffer.mid(_message_len);
            qDebug() << "receive body msg is " << messageBody ;

            // 处理读到的数据并调用对应的逻辑(相比前几个模块需要对数据做一些处理再调用对应逻辑，因为不一定有发过来的id的处理函数    _handlers[ReqId(_message_id)](ReqId(_message_id),_message_len,messageBody);  )
            handleMsg(ReqId(_message_id),_message_len,messageBody);


        }
    });

        // 连接TCP错误处理
        connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
            Q_UNUSED(socketError)
            qDebug() << "Error:" << _socket.errorString();
            // 这里要发一个信号让按钮可以点击
        });

        // 连接断开连接处理
        connect(&_socket,&QTcpSocket::disconnected,this,[&](){
            qDebug() << "Disconnected from server.";
        });

        // 连接发送信号用来执行数据发送
        QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);

        // 注册消息处理或其他完成回调
        initHttpHandlers();
}

// 注册消息处理回调
void TcpMgr::initHttpHandlers()
{
    // 聊天服务器发来的登录请求回包
    _handlers.insert(ReqId::ID_CHAT_LOGIN_RSP,[this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<< "handle id is "<< id ;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        // json格式错误
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, err is Json Parse Err" << err ;
            emit sig_login_failed(err);
            return;
        }

        // 失败
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Login Failed, err is " << err ;
            emit sig_login_failed(err);
            return;
        }

        // 设置获取各种登录信息 todo...

        // 获取申请列表
        if(jsonObj.contains("apply_list"))
        {
            UserMgr::GetInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
        }

        // 获取基本信息
        auto uid = jsonObj["uid"].toInt();
        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);
        UserMgr::GetInstance()->SetUserInfo(user_info);
        // 获取token
        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());

        // 登录成功操作
        // 发送信号进入聊天界面
        emit sig_switch_chatdlg();
    });

    // 聊天服务器发来的搜索用户回包
    _handlers.insert(ReqId::ID_SEARCH_USER_RSP,[this](ReqId id, int len, QByteArray data){
        qDebug()<< "handle id is "<< id ;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        // json格式错误
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Search Failed, err is Json Parse Err" << err ;
            emit sig_user_search(nullptr);
            return;
        }

        // 失败
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Search Failed, err is " << err ;
            emit sig_user_search(nullptr);
            return;
        }

        // 搜索到的信息
        auto search_info =  std::make_shared<SearchInfo>(jsonObj["uid"].toInt(), jsonObj["name"].toString(),
                                                        jsonObj["nick"].toString(), jsonObj["desc"].toString(),
                                                        jsonObj["sex"].toInt(), jsonObj["icon"].toString());
        emit sig_user_search(search_info);
    });

    // 聊天服务器发来的申请好友回包
    _handlers.insert(ReqId::ID_ADD_FRIEND_RSP,[this](ReqId id, int len, QByteArray data){
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        // json格式错误
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Add Friend Failed, err is Json Parse Err" << err ;
            emit sig_user_search(nullptr);
            return;
        }

        // 失败
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Add Friend Failed, err is " << err ;
            emit sig_user_search(nullptr);
            return;
        }

        // 这里本来就没有其他处理
        qDebug()<<"Add Friend REQ Success";

    });

    // 聊天服务器发来的通知用户添加好友申请
    _handlers.insert(ReqId::ID_NOTIFY_ADD_FRIEND_REQ,[this](ReqId id, int len, QByteArray data){
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        // json格式错误
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Notify Apply ADD Friend Failed, err is Json Parse Err" << err ;
            //emit sig_user_search(nullptr);
            return;
        }

        // 失败
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Notify Apply ADD Friend Failed, err is " << err ;
            //emit sig_user_search(nullptr);
            return;
        }

        // 获取对方（申请添加好友的人）的信息
        int from_uid = jsonObj["applyuid"].toInt();// 问题在这,fromuid改为applyuid（服务器发的是applyuid）
        QString name = jsonObj["name"].toString();
        QString desc = jsonObj["desc"].toString();
        QString nick = jsonObj["nick"].toString();
        QString icon = jsonObj["icon"].toString();
        int sex = jsonObj["sex"].toInt();

        // 对方（申请方）的信息
        auto apply_info = std::make_shared<AddFriendApply>(from_uid,name,desc,icon,nick,sex);

        emit sig_friend_apply(apply_info);
        qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ Success";
    });

    // 聊天服务器发来的好友认证回包
    _handlers.insert(ReqId::ID_AUTH_FRIEND_RSP,[this](ReqId id, int len, QByteArray data){
        qDebug()<<"handle id is "<< id<<" data is "<< data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj ;

        // json格式错误
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed, err is Json Parse Err" << err ;
            //emit sig_user_search(nullptr);
            return;
        }

        // 失败
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Auth Friend Failed, err is " << err ;
            //emit sig_user_search(nullptr);
            return;
        }

        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto uid = jsonObj["uid"].toInt();
        auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        emit sig_auth_rsp(rsp);
        qDebug() << "Auth Friend Success " ;
    });

    // 聊天服务器发来的通知用户对方同意的申请
    _handlers.insert(ReqId::ID_NOTIFY_AUTH_FRIEND_REQ,[this](ReqId id, int len, QByteArray data){
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }
        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }
        int from_uid = jsonObj["fromuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString nick = jsonObj["nick"].toString();
        QString icon = jsonObj["icon"].toString();
        int sex = jsonObj["sex"].toInt();
        auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                    nick, icon, sex);
        emit sig_add_auth_friend(auth_info);
    });
}

// 处理读到的数据并调用对应的逻辑
void TcpMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter =  _handlers.find(id);
    if(find_iter == _handlers.end()){
        qDebug()<< "not found id ["<< id << "] to handle";
        return ;
    }

    find_iter.value()(id,len,data);
}

// 执行TCP连接
void TcpMgr::slot_tcp_connect(ServerInfo info)
{
    qDebug()<< "receive tcp connect signal";
    // 尝试连接到聊天服务器
    qDebug() << "Connecting to server...";
    _host = info.Host;
    _port = static_cast<quint16>(info.Port.toUInt());
    qDebug()<< _host<<_port;
    _socket.connectToHost(_host,_port);
}

// 执行数据发送
void TcpMgr::slot_send_data(ReqId reqId, QByteArray data)
{
    uint16_t id = reqId;

    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(data.size());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // 设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);

    // 写入ID和长度(序列化这些数据到block)
    out << id << len;

    // 添加字符串数据
    block.append(data);

    // 向对端发送数据
    _socket.write(block);
    qDebug() << "tcp mgr send byte data is " << block ;
}
