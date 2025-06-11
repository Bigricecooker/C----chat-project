#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include "listItembase.h"
#include "userdata.h"

/******************************************************************************
 *
 * @file       chatuserwid.h
 * @brief      聊天列表的聊天用户item
 *
 * @author     Bigricecooker
 * @date       2025/02/27
 * @history
 *****************************************************************************/
namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();
    QSize sizeHint() const override {
        return QSize(250, 70); // 返回自定义的尺寸
    }

    void SetInfo(std::shared_ptr<UserInfo> user_info);
    void SetInfo(QString name, QString head, QString msg);
    std::shared_ptr<UserInfo> GetUserInfo();
private:
    Ui::ChatUserWid *ui;
    QString _name;
    QString _head;
    QString _msg;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHATUSERWID_H
