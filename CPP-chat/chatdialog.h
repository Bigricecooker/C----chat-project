#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "statewidget.h"
#include <QList>
#include <QMouseEvent>>
#include "userdata.h"
#include "QListWidgetItem"
#include <QMap>

/******************************************************************************
 *
 * @file       chatdialog.h
 * @brief      聊天界面类
 *
 * @author     Bigricecooker
 * @date       2024/12/21
 * @history
 *****************************************************************************/
namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void ClearLabelState(StateWidget *lb);// 清除lb的状态


    // 测试用函数
    void addChatUserList();
    //--------

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;// 事件过滤器
private:
    void ShowSearch(bool bsearch = false);// 选择展示哪个list
    void AddLBGroup(StateWidget* lb);// 添加至按钮组
    void handleGlobalMousePress(QMouseEvent *event);// 处理全局鼠标按下
    void loadMoreConUser();
    void loadMoreChatUser();
    void SetSelectChatItem(int uid = 0);
    void SetSelectChatPage(int uid = 0);
    Ui::ChatDialog *ui;
    ChatUIMode _mode;// 模式，side_bar
    ChatUIMode _state;// 状态，chat_user_wid
    bool _b_loading;
    QList<StateWidget*> _lb_list;
    QMap<int, QListWidgetItem*>_chat_items_added;

    int _cur_chat_uid;// 当前聊天对象的uid

    QWidget* _last_widget;
private slots:
    void slot_loading_chat_user();// 滑动滚轮后加载更多聊天
    void slot_loading_contact_user();// 滑动滚轮后加载更多联系人
    // 侧边栏切换
    void slot_side_chat();
    void slot_side_contact();

    void slot_text_changed(const QString &str);// 输入框变化
    void slot_switch_apply_friend_page();
    void slot_item_clicked(QListWidgetItem *item);// 点击聊天列表item跳转对应的聊天界面

public slots:
    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply);// 收到好友申请通知
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);// 收到对方同意添加
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);// 收到自己同意添加回复
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);// 收到好友列表item被点击
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info);// 好友信息中的聊天按钮被点击
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata);// 将发送的聊天记录存储到本地
};

#endif // CHATDIALOG_H
