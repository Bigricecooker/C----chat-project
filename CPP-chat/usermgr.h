#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include <memory>
#include <singleton.h>
#include "userdata.h"
#include <vector>
#include <QJsonArray>
#include <memory>
#include <QMap>


class UserMgr:public QObject,public Singleton<UserMgr>,
                std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
    friend class Singleton<UserMgr>;
public:
    ~UserMgr();
    void SetName(QString name);
    void SetUid(int uid);
    void SetToken(QString token);
    int GetUid();
    QString GetName();

    void SetUserInfo(std::shared_ptr<UserInfo> user_info);// 设置用户信息
    void AppendApplyList(QJsonArray array);// 设置申请列表数据
    void AppendFriendList(QJsonArray array);// 设置好友列表数据
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();// 获取申请列表
    bool AlreadyApply(int uid);// 查看申请列表是否存在该用户的申请
    void AddApplyList(std::shared_ptr<ApplyInfo> apply);// 获取申请列表
    bool CheckFriendById(int uid);// 根据uid检查是否是好友
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);// 添加为好友
    std::shared_ptr<FriendInfo> GetFriendById(int uid);// 获取好友信息

    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    bool IsLoadChatFin();// 判断是否加载完了
    void UpdateChatLoadedCount();
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();
    std::shared_ptr<UserInfo> GetUserInfo();
    //void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>);
private:
    UserMgr();
    std::shared_ptr<UserInfo> _user_info;

    QString _name;
    QString _token;
    int _uid;

    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;

    std::vector<std::shared_ptr<FriendInfo>> _friend_list;

    int _chat_loaded;
    int _contact_loaded;
};

#endif // USERMGR_H
