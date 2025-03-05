#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>
#include <userdata.h>
#include <memory.h>

namespace Ui {
class FindSuccessDlg;
}

/******************************************************************************
 *
 * @file       findsuccessdlg.h
 * @brief      搜索成功
 *
 * @author     Bigricecooker
 * @date       2025/03/05
 * @history
 *****************************************************************************/
class FindSuccessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);// 根据搜索到的信息设置用户信息
private slots:
    void on_add_friend_btn_clicked();
private:
    Ui::FindSuccessDlg *ui;
    QWidget * _parent;
    std::shared_ptr<SearchInfo> _si;
};

#endif // FINDSUCCESSDLG_H
