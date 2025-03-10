#include "searchlist.h"
#include <QScrollBar>
#include <tcpmgr.h>
#include <adduseritem.h>
#include "findsuccessdlg.h"


SearchList::SearchList(QWidget *parent):QListWidget(parent),_search_edit(nullptr),_send_pending(false)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);// 列表中的item被点击就触发
    //添加条目
    addTipItem();
    //连接搜索条目
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);

    this->setContentsMargins(0,0,0,0);
}

void SearchList::CloseFindDlg()
{
    if(_find_dlg)
    {
        _find_dlg->hide();
        _find_dlg=nullptr;

    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{

}

bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数
        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        return true; // 停止事件传递
    }
    return QListWidget::eventFilter(watched, event);
}

void SearchList::waitPending(bool pending)
{

}

void SearchList::addTipItem()
{
    // 创建
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;

    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    // 设置大小并添加
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);

    invalid_item->setObjectName("invalid_item");
    // 将QWidget绑定到QListWidgetItem上
    this->setItemWidget(item_tmp, invalid_item);

    // 设置该项不可选中
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);


    auto *add_user_item = new AddUserItem();// AddUserItem的最终父类也是QWidget
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* widget =this->itemWidget(item);// 获取自定义widget对象（获取点击的条目）
    if(!widget)
    {
        qDebug()<<"slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }
    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM){// 不可点击条目
        qDebug()<< "slot invalid item clicked ";
        return;
    }
    if(itemType == ListItemType::ADD_USER_TIP_ITEM){
        // ...
        qDebug()<< "slot addusertip item clicked ";
        _find_dlg = std::make_shared<FindSuccessDlg>(this);
        auto si = std::make_shared<SearchInfo>(0,"llfc","llfc","hello , my friend!",0,"");
        qDebug()<< "中间   1"<<si->_name<<"1   ";
        (std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg))->SetSearchInfo(si);
        _find_dlg->show();
        return;
    }
    // 清除弹出框
    CloseFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{

}
