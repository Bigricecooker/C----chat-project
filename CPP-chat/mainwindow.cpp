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
    LoginDialog* _login_dlg = new LoginDialog();
    setCentralWidget(_login_dlg);
    _login_dlg->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
