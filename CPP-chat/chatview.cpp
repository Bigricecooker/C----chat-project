#include "chatview.h"
#include <QScrollBar>
#include <QEvent>
#include <QStyleOption>
#include <QPainter>


ChatView::ChatView(QWidget *parent):QWidget(parent),isAppended(false)
{
    // 添加垂直布局
    QVBoxLayout *pMainLayout = new QVBoxLayout();
    this->setLayout(pMainLayout);
    pMainLayout->setContentsMargins(0,0,0,0);// 设置布局的四个外边距

    // 向垂直布局中加入滚动区域
    m_pScrollArea =new QScrollArea();
    m_pScrollArea->setObjectName("chat_area");
    pMainLayout->addWidget(m_pScrollArea);

    // 创建显示聊天窗口
    QWidget *w = new QWidget();
    w->setObjectName("chat_bg");
    w->setAutoFillBackground(true);// 自动填充背景，充满所在的区域

    // 这里倒着理解
    QVBoxLayout* pHLayout_1=new QVBoxLayout();
    pHLayout_1->addWidget(new QWidget(),100000);//将一个窗口加入垂直布局
    w->setLayout(pHLayout_1);// 将垂直布局添加到聊天窗口中
    m_pScrollArea->setWidget(w);//将聊天窗口添加到滚动区域中并填充滚动区域

    // 先关闭滚动条，和之前的一个一样，鼠标在当前界面才显示
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar *pVSrorllBar=m_pScrollArea->verticalScrollBar();// 获取滚动区域的滚动条

    // 当滚动条范围（最小值和最大值）变化时
    connect(pVSrorllBar,&QScrollBar::rangeChanged,this,&ChatView::onVScrollBarMoved);

    // 把垂直ScrollBar放到上边 而不是原来的并排
    QHBoxLayout *pHLayout_2=new QHBoxLayout();
    pHLayout_2->addWidget(pVSrorllBar,0,Qt::AlignRight);
    pHLayout_2->setContentsMargins(0,0,0,0);
    m_pScrollArea->setLayout(pHLayout_2);
    pVSrorllBar->setHidden(true);


    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->installEventFilter(this);
    initStyleSheet();
}

void ChatView::appendChatItem(QWidget *item)
{
    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
    vl->insertWidget(vl->count()-1, item);
    isAppended = true;
}

void ChatView::prependChatItem(QWidget *item)
{
    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
    vl->insertWidget(vl->count()-1, item);
    isAppended = true;
}

void ChatView::insertChatItem(QWidget *before, QWidget *item)
{

}

void ChatView::removeAllItem()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());

    int count = layout->count();

    for (int i = 0; i < count - 1; ++i) {
        QLayoutItem *item = layout->takeAt(0); // 始终从第一个控件开始删除
        if (item) {
            if (QWidget *widget = item->widget()) {
                delete widget;
            }
            delete item;
        }
    }

}

bool ChatView::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::Enter && o == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
    }
    else if(e->type() == QEvent::Leave && o == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(o, e);
}

void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::onVScrollBarMoved(int min, int max)
{
    if(isAppended) //添加item可能调用多次
    {
        QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());// 调整滚动条的位置，将位置设置为最大值
        //500毫秒内可能调用多次
        QTimer::singleShot(500, [this]()
                           {
                               isAppended = false;
                           });
    }
}

void ChatView::initStyleSheet()
{

}
