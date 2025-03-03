#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H
#include <QPixmap>
#include "bubbleframe.h"

/******************************************************************************
 *
 * @file       picturebubble.h
 * @brief      图片气泡
 *
 * @author     Bigricecooker
 * @date       2025/03/03
 * @history
 *****************************************************************************/
class PictureBubble:public BubbleFrame
{
public:
    PictureBubble(const QPixmap &picture,ChatRole role,QWidget *parent=nullptr);
};

#endif // PICTUREBUBBLE_H
