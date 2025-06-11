#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H
#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>

/******************************************************************************
 *
 * @file       chatuserlist.h
 * @brief      自定义聊天用户列表
 *
 * @author     Bigricecooker
 * @date       2025/02/27
 * @history
 *****************************************************************************/
class ChatUserList:public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);
protected:
    // 与鼠标悬浮和滚动条有关
    bool eventFilter(QObject *watched, QEvent *event) override;// 安装事件过滤器后，每当一个事件发生时就会调用
private:
    bool _load_pending;
signals:
    void sig_loading_chat_user();
};

#endif // CHATUSERLIST_H
