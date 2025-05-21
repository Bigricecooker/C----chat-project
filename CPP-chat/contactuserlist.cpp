#include "contactuserlist.h"
#include "tcpmgr.h"
#include "conuseritem.h"
#include "grouptipitem.h"
#include <QRandomGenerator>


ContactUserList::ContactUserList(QWidget *parent):QListWidget(parent),_add_friend_item(nullptr)
{
    // 关闭滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);

    //模拟从数据库或者后端传输过来的数据,进行列表加载
    addContactUserList();

    // 连接点击信号和槽
    connect(this,&QListWidget::itemClicked,this,&ContactUserList::slot_item_clicked);

}

void ContactUserList::ShowRedPoint(bool bshow)
{
    _add_friend_item->ShowRedPoint(bshow);
}

bool ContactUserList::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==this->viewport())
    {
        // 鼠标进入和离开
        if(event->type()==QEvent::Enter)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if(event->type()==QEvent::Leave)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查是否为滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数
        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量
        if (maxScrollValue - currentValue <= 0) {
            // 滚动到底部，加载新的联系人
            qDebug()<<"load more contact user";
            //发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_contact_user();
        }
        return true; // 停止事件传递
    }
    return QListWidget::eventFilter(watched, event);

}

void ContactUserList::addContactUserList()
{
    // “新的朋友”组条目
    auto grouptip=new GroupTipItem();
    QListWidgetItem* item=new QListWidgetItem();
    item->setSizeHint(grouptip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item,grouptip);// 如何在QListWidgetItem中定义自定义条目的方法
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);// 设置item为不可选中

    // “新的朋友”条目
    _add_friend_item=new ConUserItem();
    _add_friend_item->setObjectName("new_friend_item");
    _add_friend_item->SetInfo(0,tr("新的朋友"),":/res/add_friend.png");
    _add_friend_item->SetItemType(APPLY_FRIEND_ITEM);
    QListWidgetItem* add_item=new QListWidgetItem();
    add_item->setSizeHint(_add_friend_item->sizeHint());
    this->addItem(add_item);
    this->setItemWidget(add_item,_add_friend_item);

    this->setCurrentItem(add_item);// 设置该条目默认选中


    // “联系人”组条目
    auto groupcon=new GroupTipItem();
    groupcon->SetGroupTip(tr("联系人"));
    QListWidgetItem* item2=new QListWidgetItem();
    item2->setSizeHint(groupcon->sizeHint());
    this->addItem(item2);
    this->setItemWidget(item2,groupcon);



    // 创建QListWidgetItem，并设置自定义的widget（模拟数据）
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();
        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(0,names[name_i], heads[head_i]);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }
}

void ContactUserList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* widget=this->itemWidget(item);// 获取自定义item
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 转换为自定义基类
    ListItemBase* base=qobject_cast<ListItemBase*>(widget);

    if(!base)
    {
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto type=base->GetItemType();

    if(type==ListItemType::INVALID_ITEM||ListItemType::GROUP_TIP_ITEM)
    {
        qDebug()<< "slot invalid item clicked ";
        return;
    }
    else if(type == ListItemType::APPLY_FRIEND_ITEM)
    {
        // 创建对话框，提示用户
        qDebug()<< "apply friend item clicked ";
        //跳转到好友申请界面
        emit sig_switch_apply_friend_page();
        return;
    }
    else if(type ==ListItemType::CONTACT_USER_ITEM)
    {
        // 创建对话框，提示用户
        qDebug()<< "contact user item clicked ";

        // auto con_item = qobject_cast<ConUserItem*>(base);
        // auto user_info = con_item->GetInfo();
        // //跳转到好友申请界面
        // emit sig_switch_friend_info_page(user_info);
        return;
    }
}
