#ifndef TIMBERBTN_H
#define TIMBERBTN_H

#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>

/******************************************************************************
 *
 * @file       timberbtn.h
 * @brief      定时按钮
 *
 * @author     Bigricecooker
 * @date       2024/11/29
 * @history
 *****************************************************************************/
class TimberBtn:public QPushButton
{
public:
    TimberBtn(QWidget *parent = nullptr);
    ~TimberBtn();
    // 重写鼠标抬起事件
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QTimer *_timer;
    int _counter;
};

#endif // TIMBERBTN_H
