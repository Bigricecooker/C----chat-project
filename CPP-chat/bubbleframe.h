#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H
#include <QFrame>
#include <QHBoxLayout>
#include "global.h"

/******************************************************************************
 *
 * @file       bubbleframe.h
 * @brief      聊天气泡基类
 *
 * @author     Bigricecooker
 * @date       2025/03/02
 * @history
 *****************************************************************************/
class BubbleFrame:public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, QWidget *parent = nullptr);
    //inline int margin(){return margin;}
    void setWidget(QWidget *w);// 消息用单独的widget保存，用于设置
protected:
    void paintEvent(QPaintEvent *e);
private:
    QHBoxLayout *m_pHLayout;
    ChatRole m_role;
    int      m_margin;
};

#endif // BUBBLEFRAME_H
