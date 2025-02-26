#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registergdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void SlotSwitchReg();// 进入注册界面槽函数
    void SlotSwitchReset();// 进入重置密码界面槽函数
    void SlotSwitchLog();// 注册界面返回登录界面槽函数
    void SlotSwitchLog2();// 重置密码界面返回登录界面槽函数
    void SlotSwitchChat();// 进入聊天界面槽函数


private:
    Ui::MainWindow *ui;
    LoginDialog* _login_dlg;
    RegistergDialog* _reg_dlg;
    ResetDialog* _reset_dlg;
    ChatDialog* _chat_dlg;
};
#endif // MAINWINDOW_H
