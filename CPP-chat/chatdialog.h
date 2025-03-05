#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "statewidget.h"
#include <QList>
#include <QMouseEvent>>
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
    void ClearLabelState(StateWidget *lb);// 清除lb的状态


    // 测试用函数
    void addChatUserList();
    //--------

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;// 事件过滤器
private:
    void ShowSearch(bool bsearch = false);// 选择展示哪个list
    void AddLBGroup(StateWidget* lb);// 添加至按钮组
    void handleGlobalMousePress(QMouseEvent *event);// 处理全局鼠标按下

    Ui::ChatDialog *ui;
    ChatUIMode _mode;// 模式，side_bar
    ChatUIMode _state;// 状态，chat_user_wid
    bool _b_loading;
    QList<StateWidget*> _lb_list;
private slots:
    void slot_loading_chat_user();// 滑动滚轮后加载更多联系人
    // 侧边栏切换
    void slot_side_chat();
    void slot_side_contact();

    void slot_text_changed(const QString &str);
};

#endif // CHATDIALOG_H
