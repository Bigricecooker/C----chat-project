#include "resetdialog.h"
#include "ui_resetdialog.h"
#include <QDebug>
#include <QRegularExpression>


ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);

    // 输入框错误处理
    connect(ui->user_lineEdit,&QLineEdit::editingFinished,this,[this](){

    });
    connect(ui->email_lineEdit,&QLineEdit::editingFinished,this,[this](){

    });
    connect(ui->ver_lineEdit,&QLineEdit::editingFinished,this,[this](){

    });
    connect(ui->newpwd_lineEdit,&QLineEdit::editingFinished,this,[this](){

    });

}

ResetDialog::~ResetDialog()
{
    delete ui;
}

// 错误信息显示
void ResetDialog::showTip()
{

}

// 添加错误
void ResetDialog::AddTipErr(TipErr te, QString tips)
{

}

// 删除错误
void ResetDialog::DelTipErr(TipErr te)
{

}
