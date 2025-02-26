#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
/******************************************************************************
 *
 * @file       chatdialog.h
 * @brief      聊天界面类
 *
 * @author     Bigricecooker
 * @date       2024/12/21
 * @history
 *****************************************************************************/
namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

private:
    Ui::ChatDialog *ui;
};

#endif // CHATDIALOG_H
