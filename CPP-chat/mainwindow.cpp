#include "mainwindow.h"
#include "ui_mainwindow.h"
/******************************************************************************
 *
 * @file       mainwindow.cpp
 * @brief      主窗口
 *
 * @author     Bigricecooker
 * @date       2024/11/13
 * @history
 *****************************************************************************/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _login_dlg = new LoginDialog(this);
    setCentralWidget(_login_dlg);

    // 用于父窗口隐藏？
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    // 创建和注册消息链接
    connect(_login_dlg, &LoginDialog::switchRegister,this, &MainWindow::SlotSwitchReg);// 连接登录界面发出的进入注册界面信号和进入注册界面槽函数


}

MainWindow::~MainWindow()
{
    delete ui;
    // if(_login_dlg)
    // {
    //     delete _login_dlg;
    //     _login_dlg=nullptr;
    // }
    // if(_reg_dlg)
    // {
    //     delete _reg_dlg;
    //     _reg_dlg=nullptr;
    // } // 设置父窗口后不需要单独清理动态内存了
}

//----------------------------------------槽函数-----------------------------------------------//
// 注意setCentralWidget()会触发之前的对象的析构，所以这里都要重新分配和连接信号与槽
void MainWindow::SlotSwitchReg()
{
    _reg_dlg = new RegistergDialog(this);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    connect(_reg_dlg, &RegistergDialog::switchLogin,this, &MainWindow::SlotSwitchLog);// 连接注册界面发出的进入登录界面信号和进入登录界面槽函数

    // 进入注册界面
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}

void MainWindow::SlotSwitchLog()
{
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    connect(_login_dlg, &LoginDialog::switchRegister,this, &MainWindow::SlotSwitchReg);// 连接登录界面发出的进入注册界面信号和进入注册界面槽函数
    // 登录界面
    setCentralWidget(_login_dlg);
    _reg_dlg->hide();
    _login_dlg->show();
}
