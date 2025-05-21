#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include <memory>
#include <singleton.h>
#include "userdata.h"
#include <vector>
#include <QJsonArray>
#include <memory>


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
    void AppendApplyList(QJsonArray array);// 设置申请列表
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();// 获取申请列表
    bool AlreadyApply(int uid);// 查看申请列表是否存在该用户的申请
    void AddApplyList(std::shared_ptr<ApplyInfo> apply);
private:
    UserMgr();
    std::shared_ptr<UserInfo> _user_info;

    QString _name;
    QString _token;
    int _uid;

    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
};

#endif // USERMGR_H
