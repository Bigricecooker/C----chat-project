#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // 设置密码模式
    ui->pass_lineEdit->setEchoMode(QLineEdit::Password);

    // 点击注册按钮发出进入注册界面信号
    connect(ui->reg_pushButton,&QPushButton::clicked,this,&LoginDialog::switchRegister);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
