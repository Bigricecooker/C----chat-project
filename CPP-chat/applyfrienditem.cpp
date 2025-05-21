#include "applyfrienditem.h"
#include "ui_applyfrienditem.h"

ApplyFriendItem::ApplyFriendItem(QWidget *parent)
    : ListItemBase(parent),_added(false)
    , ui(new Ui::ApplyFriendItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::APPLY_FRIEND_ITEM);
    ui->addBtn->SetState("normal","hover","press");
    ui->addBtn->hide();

    // 连接点击同意按钮信号
    connect(ui->addBtn,&ClickedBtn::clicked,this,[this](){
        emit this->sig_auth_friend(_apply_info);
    });
}

ApplyFriendItem::~ApplyFriendItem()
{
    delete ui;
}

void ApplyFriendItem::SetInfo(std::shared_ptr<ApplyInfo> apply_info)
{
    _apply_info=apply_info;

    QPixmap pixmap(_apply_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));

    ui->user_name_lb->setText(_apply_info->_name);
    ui->user_chat_lb->setText(_apply_info->_desc);
}

void ApplyFriendItem::ShowAddBtn(bool bshow)
{
    if(bshow)
    {
        ui->addBtn->hide();
        ui->already_add_lb->show();
        _added=false;
    }
    else
    {
        ui->addBtn->show();
        ui->already_add_lb->hide();
        _added=true;
    }
}

int ApplyFriendItem::GetUid()
{
    return _apply_info->_uid;
}
