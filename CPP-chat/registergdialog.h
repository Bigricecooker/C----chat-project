#ifndef REGISTERGDIALOG_H
#define REGISTERGDIALOG_H

#include <QDialog>
#include "global.h"
#include "httpmgr.h"
/******************************************************************************
 *
 * @file       registergdialog.h
 * @brief      注册界面
 *
 * @author     Bigricecooker
 * @date       2024/11/13
 * @history
 *****************************************************************************/
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

    // 切换至注册成功界面
    void ChangeTipPage();

private slots:
    void on_get_code_clicked();// 点击获取验证码按钮事件
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);// 收到注册完成事件

    void on_confirm_pushButton_clicked();

private:
    void showTip(QString str, bool b_ok);// 错误信息显示
    void initHttpHandlers();// 注册消息处理
    // 输入框错误提示
    bool checkUserValid();
    bool checkPassValid();
    bool checkEmailValid();
    bool checkVerifyValid();
    bool checkPassandComfirmValid();
    void AddTipErr(TipErr te, QString tips);// 添加错误
    void DelTipErr(TipErr te);// 删除错误

    Ui::RegistergDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;// 消息处理集合
    QMap<TipErr, QString> _tip_errs;// 错误缓存
};

#endif // REGISTERGDIALOG_H
