#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
#include <QWidget>
#include "global.h"
/******************************************************************************
 *
 * @file       listitembase.h
 * @brief      item格式基类
 *
 * @author     Bigricecooker
 * @date       2025/02/27
 * @history
 *****************************************************************************/
class ListItemBase:public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget *parent =nullptr);

    void SetItemType(ListItemType itemType);
    ListItemType GetItemType();
private:
    ListItemType _itemType;
public slots:
signals:
};

#endif // LISTITEMBASE_H
