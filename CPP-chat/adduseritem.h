#ifndef ADDUSERITEM_H
#define ADDUSERITEM_H

#include <QWidget>
#include "listitembase.h"

namespace Ui {
class AddUserItem;
}

/******************************************************************************
 *
 * @file       adduseritem.h
 * @brief      添加的用户
 *
 * @author     Bigricecooker
 * @date       2025/03/05
 * @history
 *****************************************************************************/
class AddUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit AddUserItem(QWidget *parent = nullptr);
    ~AddUserItem();
    QSize sizeHint() const override;
protected:
private:
    Ui::AddUserItem *ui;
};

#endif // ADDUSERITEM_H
