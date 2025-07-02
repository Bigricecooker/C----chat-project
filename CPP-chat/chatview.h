#ifndef CHATVIEW_H
#define CHATVIEW_H
#include <QWidget>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>



/******************************************************************************
 *
 * @file       chatview.h
 * @brief      滚动聊天布局
 *
 * @author     Bigricecooker
 * @date       2025/02/28
 * @history
 *****************************************************************************/
class ChatView:public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget * parent = Q_NULLPTR);

    // 添加item到聊天背景
    void appendChatItem(QWidget *item);                 //尾插
    void prependChatItem(QWidget *item);                //头插
    void insertChatItem(QWidget *before, QWidget *item);//中间插
    void removeAllItem();// 移除所有信息
protected:
    // 与右侧滚动条有关
    bool eventFilter(QObject *o, QEvent *e) override;// 安装事件过滤器


    void paintEvent(QPaintEvent *event) override;// 确保 Qt 样式表生效
private slots:
    void onVScrollBarMoved(int min, int max);// 滚动条移动
private:
    void initStyleSheet();// 初始化样式表
private:
    //QWidget *m_pCenterWidget;
    QVBoxLayout *m_pVl;// 垂直布局
    QScrollArea *m_pScrollArea;
    bool isAppended;// 是否正在加载

};

#endif // CHATVIEW_H
