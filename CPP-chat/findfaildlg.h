#ifndef FINDFAILDLG_H
#define FINDFAILDLG_H

#include <QDialog>

/******************************************************************************
 *
 * @file       findfaildlg.h
 * @brief      查找失败对话框
 *
 * @author     Bigricecooker
 * @date       2025/04/28
 * @history
 *****************************************************************************/
namespace Ui {
class FindFailDlg;
}

class FindFailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindFailDlg(QWidget *parent = nullptr);
    ~FindFailDlg();

private slots:
    void on_fail_sure_btn_clicked();

private:
    Ui::FindFailDlg *ui;
};

#endif // FINDFAILDLG_H
