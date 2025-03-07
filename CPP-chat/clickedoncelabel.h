#ifndef CLICKEDONCELABEL_H
#define CLICKEDONCELABEL_H
#include <QLabel>
#include <QMouseEvent>

/******************************************************************************
 *
 * @file       clickedoncelabel.h
 * @brief      单击一次的标签
 *
 * @author     Bigricecooker
 * @date       2025/03/06
 * @history
 *****************************************************************************/
class ClickedOnceLabel: public QLabel
{
    Q_OBJECT
public:
    ClickedOnceLabel(QWidget *parent=nullptr);
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;// 重写鼠标释放事件
signals:
    void clicked(QString);
};

#endif // CLICKEDONCELABEL_H
