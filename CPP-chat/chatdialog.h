#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
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


    // 测试用函数
    void addChatUserList();
    //--------
private:
    void ShowSearch(bool bsearch = false);// 选择展示哪个list

    Ui::ChatDialog *ui;
    ChatUIMode _mode;// 模式，side_bar
    ChatUIMode _state;// 状态，chat_user_wid
    bool _b_loading;
private slots:
    void slot_loading_chat_user();// 滑动滚轮后加载更多联系人
};

#endif // CHATDIALOG_H
