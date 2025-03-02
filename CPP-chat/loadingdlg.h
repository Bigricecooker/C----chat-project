#ifndef LOADINGDLG_H
#define LOADINGDLG_H

#include <QDialog>

/******************************************************************************
 *
 * @file       loadingdlg.h
 * @brief      动态加载
 *
 * @author     Bigricecooker
 * @date       2025/02/28
 * @history
 *****************************************************************************/
namespace Ui {
class LoadingDlg;
}

class LoadingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingDlg(QWidget *parent = nullptr);
    ~LoadingDlg();

private:
    Ui::LoadingDlg *ui;
};

#endif // LOADINGDLG_H
