#include "chatuserlist.h"


ChatUserList::ChatUserList(QWidget *parent):QListWidget(parent)
{
    // 关闭自带的滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);// 左右
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);// 上下

    // 安装事件过滤器
    this->viewport()->installEventFilter(this);// 随后重写eventFilter
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否鼠标悬浮进入或离开
    if(watched==this->viewport())// 检测当前对象是否是视口
    {
        if(event->type()==QEvent::Enter)
        {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if(event->type()==QEvent::Leave)
        {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if(watched==this->viewport()&&event->type()==QEvent::Wheel)
    {
        QWheelEvent *wheelEvent=static_cast<QWheelEvent*> (event);
        int numDegress = wheelEvent->angleDelta().y()/8;
        int numSteps = numDegress / 15;// 计算滚动步数

        // 设置滚动幅度（滚动条向上和向下运动）
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量
        if (maxScrollValue - currentValue <= 0) {
            // 滚动到底部，加载新的联系人
            qDebug()<<"load more chat user";
            //发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_chat_user();
        }
        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched,event);
}
