#ifndef CONUSERITEM_H
#define CONUSERITEM_H

#include <QWidget>
#include <listitembase.h>
#include "userdata.h"
/******************************************************************************
 *
 * @file       conuseritem.h
 * @brief      联系人item
 *
 * @author     Bigricecooker
 * @date       2025/03/10
 * @history
 *****************************************************************************/
namespace Ui {
class ConUserItem;
}

class ConUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem();

    // 是否展示红点
    void ShowRedPoint(bool show= false);

    QSize sizeHint()const override;

    void SetInfo(std::shared_ptr<AuthInfo> auth);
    void SetInfo(std::shared_ptr<AuthRsp> auth);
    void SetInfo(int uid,QString name,QString icon);
    std::shared_ptr<UserInfo> GetInfo();
private:
    Ui::ConUserItem *ui;
    std::shared_ptr<UserInfo> _info;
};

#endif // CONUSERITEM_H
