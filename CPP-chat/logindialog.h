#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QPushButton>
#include "global.h"
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
    void switchReset();// 进入重置密码界面信号
    void sig_connect_tcp(ServerInfo info);// 通知TCP管理类发送TCP连接

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    void showTip(QString str, bool b_ok);// 错误信息显示


private slots:
    void on_login_Button_clicked();// 点击登录按钮事件
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);// 收到登录完成或其他完成事件
    void slot_tcp_con_finish(bool bsuccess);// TCP连接建立成功与否事件
    void slot_login_failed(int err);// TCP连接失败事件

private:
    Ui::LoginDialog *ui;
    QMap<TipErr, QString> _tip_errs;// 错误缓存
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;// 消息处理集合

    // 登录验证
    int _uid;
    QString _token;


    void initHttpHandlers();// 注册消息处理回调

    // 输入框错误提示
    bool checkUserValid();
    bool checkPassValid();
    void AddTipErr(TipErr te, QString tips);// 添加错误
    void DelTipErr(TipErr te);// 删除错误

    bool enableBtn(bool enabled);// 设置按钮是否可点击
};

#endif // LOGINDIALOG_H
