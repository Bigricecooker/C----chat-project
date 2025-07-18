#include "findfaildlg.h"
#include "ui_findfaildlg.h"
#include <QDebug>

FindFailDlg::FindFailDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindFailDlg)
{
    ui->setupUi(this);

    // 设置对话框标题
    setWindowTitle("添加");

    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("FindFailDlg");
    ui->fail_sure_btn->SetState("normal","hover","press");

    this->setModal(true);
}

FindFailDlg::~FindFailDlg()
{
    qDebug()<<"FindFailDlg destruct";
    delete ui;
}

void FindFailDlg::on_fail_sure_btn_clicked()
{
    this->hide();
}
