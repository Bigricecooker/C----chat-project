#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H
#include <QLineEdit>
#include <QDebug>


/******************************************************************************
 *
 * @file       customizeedit.h
 * @brief      自定义edit(聊天界面搜索框)
 *
 * @author     Bigricecooker
 * @date       2025/02/27
 * @history
 *****************************************************************************/
class CustomizeEdit:public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget *parent=nullptr);
    void SetMaxLength(int maxLen);
protected:
    void focusOutEvent(QFocusEvent *event) override;// 发送失去焦点信号的函数
private:
    void limitTextLength(QString text);// 限制最大长度

    int _max_len;
signals:
    void sig_foucus_out();// 失去焦点信号
};

#endif // CUSTOMIZEEDIT_H
