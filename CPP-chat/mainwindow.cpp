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
    //_login_dlg->show();

    // 创建和注册消息链接
    connect(_login_dlg, &LoginDialog::switchRegister,this, &MainWindow::SlotSwitchReg);// 连接登录界面发出的进入注册界面信号和进入注册界面槽函数
    _reg_dlg=new RegistergDialog(this);

    // 用于父窗口隐藏？
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _reg_dlg->hide();
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

void MainWindow::SlotSwitchReg()
{
    // 注册界面
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}
