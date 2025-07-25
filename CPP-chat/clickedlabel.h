#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include "global.h"
#include <QLabel>

/******************************************************************************
 *
 * @file       clickedlabel.h
 * @brief      单击标签样式
 *
 * @author     Bigricecooker
 * @date       2024/12/02
 * @history
 *****************************************************************************/
class ClickedLabel:public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget *parent=nullptr);
    ~ClickedLabel();

    // 鼠标按下事件
    virtual void mousePressEvent(QMouseEvent *ev) override;
    // 鼠标进入事件
    virtual void enterEvent(QEnterEvent* event) override;
    // 鼠标离开事件
    virtual void leaveEvent(QEvent* event) override;
    // 鼠标抬起事件
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    // 设置初始状态
    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");

    // 设置当前状态
    bool SetCurState(ClickLbState state);
    // 获取当前的状态
    ClickLbState GetCurState();
    // 重置为正常状态
    void ResetNormalState();

    void setbug();
private:

    // 未选中状态
    QString _normal;
    QString _normal_hover;
    QString _normal_press;

    // 选中状态
    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickLbState _curstate;

    bool m_bug;// 用于解决一个bug
signals:
    void clicked(QString, ClickLbState);// QLabel没有点击事件
};

#endif // CLICKEDLABEL_H
