#ifndef APPLYFRIENDLIST_H
#define APPLYFRIENDLIST_H
#include <QListWidget>
#include <Qevent>
#include <QWidget>

/******************************************************************************
 *
 * @file       applyfriendlist.h
 * @brief      申请好友列表
 *
 * @author     Bigricecooker
 * @date       2025/03/12
 * @history
 *****************************************************************************/
class ApplyFriendList:public QListWidget
{
    Q_OBJECT
public:
    ApplyFriendList(QWidget *parent=nullptr);

protected:
    bool eventFilter(QObject *watch,QEvent *event) override;
private slots:

signals:
    void sig_show_search(bool);// 隐藏搜索框
};

#endif // APPLYFRIENDLIST_H
