#ifndef FRIENDLABEL_H
#define FRIENDLABEL_H

#include <QWidget>
#include <QFrame>
#include "clickedlabel.h"

namespace Ui {
class FriendLabel;
}

/******************************************************************************
 *
 * @file       friendlabel.h
 * @brief      好友标签
 *
 * @author     Bigricecooker
 * @date       2025/03/06
 * @history
 *****************************************************************************/
class FriendLabel : public QFrame
{
    Q_OBJECT

public:
    explicit FriendLabel(QWidget *parent = nullptr);
    ~FriendLabel();

    void SetText(QString text);// 设置文本
    int Width();
    int Height();// 获取宽高
    QString Text();

private:
    Ui::FriendLabel *ui;
    QString _text;
    int _width;
    int _height;
public slots:
    void slot_close();
signals:
    void sig_close(QString);
};

#endif // FRIENDLABEL_H
