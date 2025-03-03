#include "bubbleframe.h"
#include <QDebug>
#include <QPainter>


//const int WIDTH_SANJIAO  = 8;  //三角宽
BubbleFrame::BubbleFrame(ChatRole role, QWidget *parent)
    :QFrame(parent)
    ,m_role(role)
    ,m_margin(3)
{
    m_pHLayout = new QHBoxLayout();
    // if(m_role == ChatRole::Self)
    //     m_pHLayout->setContentsMargins(m_margin, m_margin, m_margin, m_margin);
    // else
        m_pHLayout->setContentsMargins(m_margin, m_margin, m_margin, m_margin);

    this->setLayout(m_pHLayout);
}


void BubbleFrame::setWidget(QWidget *w)
{
    if(m_pHLayout->count() > 0)
        return ;
    else{
        m_pHLayout->addWidget(w);
    }
}

void BubbleFrame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    QColor bk_color;
    if (m_role == ChatRole::Other)
    {
        bk_color = QColor(Qt::white);  // 对方消息：白色背景
    }
    else
    {
        bk_color = QColor(0,162,255);  // 自己消息：蓝色背景
    }

    painter.setBrush(QBrush(bk_color));

    // 直接绘制整个区域的气泡，不留出小三角的空间
    QRect bk_rect = QRect(0, 0, this->width(), this->height());
    painter.drawRoundedRect(bk_rect, 5, 5);

    QFrame::paintEvent(e); // 保持父类绘制逻辑
}

// // 画聊天框
// void BubbleFrame::paintEvent(QPaintEvent *e)
// {
//     QPainter painter(this);
//     painter.setPen(Qt::NoPen);

//     if(m_role == ChatRole::Other)
//     {
//         //画气泡
//         QColor bk_color(Qt::white);
//         painter.setBrush(QBrush(bk_color));
//         QRect bk_rect = QRect(WIDTH_SANJIAO, 0, this->width()-WIDTH_SANJIAO, this->height());
//         painter.drawRoundedRect(bk_rect,5,5);
//         //画小三角
//         QPointF points[3] = {
//             QPointF(bk_rect.x(), 12),
//             QPointF(bk_rect.x(), 10+WIDTH_SANJIAO +2),
//             QPointF(bk_rect.x()-WIDTH_SANJIAO, 10+WIDTH_SANJIAO-WIDTH_SANJIAO/2),
//         };
//         painter.drawPolygon(points, 3);
//     }
//     else
//     {
//         QColor bk_color(0,162,255);
//         painter.setBrush(QBrush(bk_color));
//         //画气泡
//         QRect bk_rect = QRect(0, 0, this->width()-WIDTH_SANJIAO, this->height());
//         painter.drawRoundedRect(bk_rect,5,5);
//         //画三角
//         QPointF points[3] = {
//             QPointF(bk_rect.x()+bk_rect.width(), 12),
//             QPointF(bk_rect.x()+bk_rect.width(), 12+WIDTH_SANJIAO +2),
//             QPointF(bk_rect.x()+bk_rect.width()+WIDTH_SANJIAO, 10+WIDTH_SANJIAO-WIDTH_SANJIAO/2),
//         };
//         painter.drawPolygon(points, 3);
//     }
//     return QFrame::paintEvent(e);
// }
