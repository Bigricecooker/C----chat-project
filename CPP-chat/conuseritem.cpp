#include "conuseritem.h"
#include "ui_conuseritem.h"
#include <QPixmap>

ConUserItem::ConUserItem(QWidget *parent)
    : ui(new Ui::ConUserItem),
    ListItemBase(parent)
{
    ui->setupUi(this);
    this->SetItemType(CONTACT_USER_ITEM);
    ui->red_point->raise();// 将组件移动到最上层
    ShowRedPoint(false);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

void ConUserItem::ShowRedPoint(bool show)
{
    if(show)
    {
        ui->red_point->show();
    }
    else
    {
        ui->red_point->hide();
    }
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250,70);
}

void ConUserItem::SetInfo(std::shared_ptr<AuthInfo> auth)
{
    _info = std::make_shared<UserInfo>(auth);
    // 设置图片
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::SetInfo(std::shared_ptr<AuthRsp> auth)
{
    _info = std::make_shared<UserInfo>(auth);
    // 设置图片
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
    _info = std::make_shared<UserInfo>(uid,name,icon);
    // 设置图片
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_info->_name);
}
