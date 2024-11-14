#include "registergdialog.h"
#include "ui_registergdialog.h"

RegistergDialog::RegistergDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegistergDialog)
{
    ui->setupUi(this);

    // 设置密码模式
    ui->pass_lineEdit->setEchoMode(QLineEdit::Password);
    ui->confirm_lineEdit->setEchoMode(QLineEdit::Password);

    // 设置错误提示样式
    ui->err_tip->setProperty("state","normal");//修改状态
    repolish(ui->err_tip);
}

RegistergDialog::~RegistergDialog()
{
    delete ui;
}

void RegistergDialog::on_get_code_clicked()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_lineEdit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"),false);
    }
}

void RegistergDialog::showTip(QString str,bool b_ok)
{
    // 修改状态
    if(b_ok)
    {
        ui->err_tip->setProperty("state","normal");
    }
    else
    {
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

