#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registergdialog.h"

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

private:
    Ui::MainWindow *ui;
    LoginDialog* _login_dlg;
    RegistergDialog* _reg_dlg;
};
#endif // MAINWINDOW_H
