#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H
#include <QWidget>
#include <qgridlayout.h>
#include <QLabel>
#include"global.h"

class BubbleFrame;
/******************************************************************************
 *
 * @file       chatitembase.h
 * @brief      聊天信息item
 *
 * @author     Bigricecooker
 * @date       2025/03/02
 * @history
 *****************************************************************************/
class ChatItemBase: public QWidget
{
    Q_OBJECT
public:
    ChatItemBase(ChatRole role, QWidget *parent=nullptr);
    // 设置
    void setUserName(const QString &name);
    void setUserIcon(const QPixmap &icon);
    // 替换
    void setWidget(QWidget *w);

private:
    ChatRole m_role; // 角色
    QLabel *m_pNameLabel;// 名称
    QLabel *m_pIconLabel;// 头像
    QWidget *m_pBubble;// 消息
};

#endif // CHATITEMBASE_H
