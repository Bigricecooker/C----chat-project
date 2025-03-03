#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H
#include <QTextEdit>
#include "bubbleframe.h"
#include <QHBoxLayout>
#include <QEvent>
/******************************************************************************
 *
 * @file       textbubble.h
 * @brief      文本气泡
 *
 * @author     Bigricecooker
 * @date       2025/03/03
 * @history
 *****************************************************************************/
class TextBubble:public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(ChatRole role,const QString &text, QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject *o,QEvent *e);
private:
    void adjustTextHeight();// 调整气泡框高度
    void setPlainText(const QString &text);// 设置文本
    void initStyleSheet();// 初始化样式

    QTextEdit *m_pTextEdit;
};

#endif // TEXTBUBBLE_H
