#ifndef APPLYFRIENDPAGE_H
#define APPLYFRIENDPAGE_H

#include <QWidget>
#include "userdata.h"
#include <memory.h>
#include <QJsonArray>
#include <unordered_map>
#include "applyfrienditem.h"

namespace Ui {
class ApplyFriendPage;
}
/******************************************************************************
 *
 * @file       applyfriendpage.h
 * @brief      申请列表
 *
 * @author     Bigricecooker
 * @date       2025/03/11
 * @history
 *****************************************************************************/
class ApplyFriendPage : public QWidget
{
    Q_OBJECT
public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();
    void AddNewApply(std::shared_ptr<AddFriendApply> apply);// 添加新的申请
protected:
    void paintEvent(QPaintEvent *event);// 确保自定义控件能够使用qt样式，qss
private:
    void loadApplyList();// 加载后台穿过来的申请列表
    Ui::ApplyFriendPage *ui;
    std::unordered_map<int, ApplyFriendItem*> _unauth_items;
public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);// 同意之后会触发
signals:
    void sig_show_search(bool);
};

#endif // APPLYFRIENDPAGE_H
