#ifndef REGISTERGDIALOG_H
#define REGISTERGDIALOG_H

#include <QDialog>
#include "global.h"
#include "httpmgr.h"

namespace Ui {
class RegistergDialog;
}

class RegistergDialog : public QDialog
{
    Q_OBJECT

signals:
    void switchLogin();// 进入登录界面信号

public:
    explicit RegistergDialog(QWidget *parent = nullptr);
    ~RegistergDialog();

private slots:
    void on_get_code_clicked();// 点击获取验证码按钮事件
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);// 收到注册完成事件

private:
    void showTip(QString str, bool b_ok);
    void initHttpHandlers();// 注册消息处理
    Ui::RegistergDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
};

#endif // REGISTERGDIALOG_H
