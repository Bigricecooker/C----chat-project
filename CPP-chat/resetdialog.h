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

    void showTip();// 错误信息显示
    void AddTipErr(TipErr te, QString tips);// 添加错误
    void DelTipErr(TipErr te);// 删除错误


private:
    Ui::ResetDialog *ui;
    QMap<TipErr, QString> _tip_errs;// 错误缓存

signals:
    void switchLogin();
};

#endif // RESETDIALOG_H
