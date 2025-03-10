#ifndef GROUPTIPITEM_H
#define GROUPTIPITEM_H
#include "listitembase.h"

#include <QWidget>

namespace Ui {
class GroupTipItem;
}
/******************************************************************************
 *
 * @file       grouptipitem.h
 * @brief      分组提示条目
 *
 * @author     Bigricecooker
 * @date       2025/03/10
 * @history
 *****************************************************************************/
class GroupTipItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem();

    QSize sizeHint()const override;
    void SetGroupTip(QString str);

private:
    Ui::GroupTipItem *ui;
    QString _tip;
};

#endif // GROUPTIPITEM_H
