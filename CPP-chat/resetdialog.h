#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"

/******************************************************************************
 *
 * @file       resetdialog.h
 * @brief      重置密码界面
 *
 * @author     Bigricecooker
 * @date       2024/12/04
 * @history
 *****************************************************************************/
namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

    void showTip(QString str,bool b_ok);// 错误信息显示
    void AddTipErr(TipErr te, QString tips);// 添加错误
    void DelTipErr(TipErr te);// 删除错误

    // 输入框错误提示
    bool checkUserValid();
    bool checkNewPassValid();
    bool checkEmailValid();
    bool checkVerifyValid();

private:
    Ui::ResetDialog *ui;
    QMap<TipErr, QString> _tip_errs;// 错误缓存
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;// 消息处理函数集合

    void initHttpHandlers();// 注册消息处理或其他完成回调

signals:
    void switchLogin();
private slots:
    void on_cancal_pushButton_clicked();// 点击取消按钮事件
    void on_confirm_pushButton_clicked();// 点击确认按钮事件
    void on_get_code_clicked();// 点击获取验证码按钮事件

    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);// 收到重置完成或其他完成事件
};

#endif // RESETDIALOG_H
