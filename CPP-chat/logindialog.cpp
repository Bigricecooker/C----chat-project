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

    //忘记密码Label
    ui->forget_label->SetState("normal","hover","hover","selected","selected_hover","selected_hover");
    // 点击忘记密码按钮发出进入重置密码界面信号
    connect(ui->forget_label,&ClickedLabel::clicked,this,&LoginDialog::switchReset);
}

LoginDialog::~LoginDialog()
{
    qDebug()<<"destruct LoginDlg";
    delete ui;
}
