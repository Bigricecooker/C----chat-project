#ifndef STATEWIDGET_H
#define STATEWIDGET_H
#include <QWidget>
#include <QString>
#include <QLabel>
#include"global.h"
#include <QMouseEvent>
#include <QEnterEvent>
#include <QEvent>
/******************************************************************************
 *
 * @file       statewidget.h
 * @brief      侧边栏按钮
 *
 * @author     Bigricecooker
 * @date       2025/03/04
 * @history
 *****************************************************************************/
class StateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget *parent = nullptr);
    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");// 设置状态
    ClickLbState GetCurState();
    void ClearState();// 清除状态（设置为未选中状态）
    void SetSelected(bool bselected);// 是否设置为选中状态
    void AddRedPoint();// 添加红点
    void ShowRedPoint(bool show=true);// 显示红点

protected:
    void paintEvent(QPaintEvent* event);// 如果发现qss没实现就重写这个函数
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;
    ClickLbState _curstate;
    QLabel * _red_point;
signals:
    void clicked(void);
signals:
public slots:
};

#endif // STATEWIDGET_H
