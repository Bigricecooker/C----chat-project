#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"
#include <QDir>
#include <QPixmap>
#include <QDebug>
#include "applyfrined.h"

FindSuccessDlg::FindSuccessDlg(QWidget *parent)
    : QDialog(parent),_parent(parent)
    , ui(new Ui::FindSuccessDlg)
{
    ui->setupUi(this);
    // 设置对话框标题
    setWindowTitle("添加");
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path = QDir::toNativeSeparators(app_path +
                                                QDir::separator() + "static"+QDir::separator()+"head_1.jpg");

    // 获取图片(缩放，保持宽高比，高质量缩放)
    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_lb->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);

    // 设置状态
    ui->add_friend_btn->SetState("normal","hover","press");
    this->setModal(true);// 设为模态

}

FindSuccessDlg::~FindSuccessDlg()
{
    delete ui;
}

void FindSuccessDlg::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->name_lb->setText(si->_name);
    qDebug()<<"name is "<<si->_name;
    _si=si;
}

void FindSuccessDlg::on_add_friend_btn_clicked()
{
    // 添加好友界面
    this->hide();
    auto applyfrined = new Applyfrined(_parent);
    applyfrined->SetSearchInfo(_si);
    applyfrined->setModal(true);
    applyfrined->show();

}
