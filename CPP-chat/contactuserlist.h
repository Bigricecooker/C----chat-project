#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H
#include <QListWidget>
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>
#include <memory.h>
#include "userdata.h"

class ConUserItem;

/******************************************************************************
 *
 * @file       contactuserlist.h
 * @brief      联系人列表
 *
 * @author     Bigricecooker
 * @date       2025/03/10
 * @history
 *****************************************************************************/
class ContactUserList:public QListWidget
{
    Q_OBJECT;
public:
    ContactUserList(QWidget *parent = nullptr);
    void ShowRedPoint(bool bshow = true);// 展示红点
protected:
    bool eventFilter(QObject *watched, QEvent *event) override ;
private:
    void addContactUserList();// 添加好友list
public slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);// 对方同意后将其加入联系人列表
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
signals:
    void sig_loading_contact_user();// 加载联系好友
    void sig_switch_apply_friend_page();// 切换到申请好友
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo> user_info);// 切换好友
private:
    ConUserItem* _add_friend_item;
    QListWidgetItem * _groupitem;
    bool _load_pending;
};

#endif // CONTACTUSERLIST_H
