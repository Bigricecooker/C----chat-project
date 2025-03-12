#ifndef TCPMGR_H
#define TCPMGR_H

#include <QTcpsocket.h>
#include <functional>
#include <QObject>
#include "singleton.h"
#include "global.h"
#include "userdata.h"

/******************************************************************************
 *
 * @file       tcpmgr.h
 * @brief      客户端TCP连接管理类（单例）
 *
 * @author     Bigricecooker
 * @date       2024/12/10
 * @history
 *****************************************************************************/
class TcpMgr:public QObject, public Singleton<TcpMgr>,
               public std::enable_shared_from_this<TcpMgr>
{
    friend class Singleton<TcpMgr>;
    Q_OBJECT
public:
    ~TcpMgr();

private:
    TcpMgr();

    void initHttpHandlers();// 注册消息处理回调
    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;// 消息处理函数集合
    void handleMsg(ReqId id, int len, QByteArray data);// 处理读到的数据并调用对应的逻辑

    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;// 数据是否收全
    quint16 _message_id;// 数据id - 2个字节
    quint16 _message_len;// 数据长度 - 2个字节

public slots:
    void slot_tcp_connect(ServerInfo info);// 执行TCP连接（信号由登录界面发送）
    void slot_send_data(ReqId reqId, QByteArray data);// 执行数据发送

signals:
    void sig_con_success(bool bsuccess);// socket连接建立成功信号（不是登录成功）
    void sig_send_data(ReqId reqId, QByteArray data);// 发送数据信号
    void sig_switch_chatdlg();// 跳转到聊天界面信号
    void sig_login_failed(int err);// 登录失败信号

    void sig_user_search(std::shared_ptr<SearchInfo> info);// 搜索用户信号
    void sig_auth_rsp(std::shared_ptr<AuthRsp> auth);// 我同意对方的申请
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);// 同意对方的申请
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);// 收到对方的同意 //zhe几个可能有问题
};

#endif // TCPMGR_H
