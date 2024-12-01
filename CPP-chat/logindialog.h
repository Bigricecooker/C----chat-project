#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QPushButton>
/******************************************************************************
 *
 * @file       logindialog.h
 * @brief      登陆界面
 *
 * @author     Bigricecooker
 * @date       2024/11/12
 * @history
 *****************************************************************************/
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

signals:
    void switchRegister();// 进入注册界面信号

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
