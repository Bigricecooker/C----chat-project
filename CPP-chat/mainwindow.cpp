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

    // 连接进入注册界面信号
    connect(_login_dlg, &LoginDialog::switchRegister,this, &MainWindow::SlotSwitchReg);// 连接登录界面发出的进入注册界面信号和进入注册界面槽函数

    // 连接进入重置密码界面信号
    connect(_login_dlg,&LoginDialog::switchReset,this,&MainWindow::SlotSwitchReset);
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

// 进入注册界面
void MainWindow::SlotSwitchReg()
{
    _reg_dlg = new RegistergDialog(this);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    connect(_reg_dlg, &RegistergDialog::switchLogin,this, &MainWindow::SlotSwitchLog);// 连接返回登录界面槽函数

    // 进入注册界面
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}

// 进入重置密码界面
void MainWindow::SlotSwitchReset()
{
    _reset_dlg= new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    connect(_reset_dlg,&ResetDialog::switchLogin,this,&MainWindow::SlotSwitchLog2);// 连接返回登录界面槽函数

    // 进入重置密码界面
    setCentralWidget(_reset_dlg);
    _login_dlg->hide();
    _reset_dlg->show();
}



// 注册界面返回登陆界面
void MainWindow::SlotSwitchLog()
{
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);


    connect(_login_dlg, &LoginDialog::switchRegister,this, &MainWindow::SlotSwitchReg);// 连接登录界面发出的进入注册界面信号和进入注册界面槽函数（如上面说的，之前的_login_dlg被销毁了，所以之前的连接也没了）
    connect(_login_dlg,&LoginDialog::switchReset,this,&MainWindow::SlotSwitchReset);// 连接进入重置密码界面

    // 登录界面
    setCentralWidget(_login_dlg);
    _reg_dlg->hide();
    _login_dlg->show();
}

// 重置密码界面返回登录界面
void MainWindow::SlotSwitchLog2()
{
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    connect(_login_dlg, &LoginDialog::switchRegister,this, &MainWindow::SlotSwitchReg);// 连接登录界面发出的进入注册界面信号和进入注册界面槽函数（如上面说的，之前的_login_dlg被销毁了，所以之前的连接也没了）
    connect(_login_dlg,&LoginDialog::switchReset,this,&MainWindow::SlotSwitchReset);// 连接进入重置密码界面

    // 重置密码界面
    setCentralWidget(_login_dlg);
    _reset_dlg->hide();

    _login_dlg->show();
}


