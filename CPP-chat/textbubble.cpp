#include "textbubble.h"
#include <QTextBlock>



TextBubble::TextBubble(ChatRole role, const QString &text,QWidget *parent):BubbleFrame(role,parent)
{
    m_pTextEdit = new QTextEdit();
    m_pTextEdit->setReadOnly(true);
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pTextEdit->installEventFilter(this);

    QFont font("Microsoft YaHei");
    font.setPointSize(12);
    m_pTextEdit->setFont(font);

    // // 让 QTextEdit 允许根据内容调整大小
    // m_pTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    setPlainText(text);


    setWidget(m_pTextEdit);// 基类的
    initStyleSheet();
}

// 之前这里因为函数名字写错导致textedit的高度一直不正确
bool TextBubble::eventFilter(QObject *o, QEvent *e)
{
    // 当左右拉伸时
    if(m_pTextEdit == o && e->type() == QEvent::Paint)
    {
        adjustTextHeight(); //PaintEvent中设置
    }
    return BubbleFrame::eventFilter(o, e);
}

void TextBubble::adjustTextHeight()
{
    qreal doc_margin = m_pTextEdit->document()->documentMargin();    //字体到边框的距离默认为4
    QTextDocument *doc = m_pTextEdit->document();
    qreal text_height = 0;
    //把每一段的高度相加=文本高
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        QTextLayout *pLayout = it.layout();
        QRectF text_rect = pLayout->boundingRect();                             //这段的rect
        text_height += text_rect.height();
    }
    int vMargin = this->layout()->contentsMargins().top();
    //设置这个气泡需要的高度 文本高+文本边距+TextEdit边框到气泡边框的距离
    setFixedHeight(text_height + doc_margin *2 + vMargin*2 );
}

void TextBubble::setPlainText(const QString &text)
{
    m_pTextEdit->setPlainText(text);
    //m_pTextEdit->setHtml(text);
    //找到段落中最大宽度
    qreal doc_margin = m_pTextEdit->document()->documentMargin();
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();
    QFontMetricsF fm(m_pTextEdit->font());
    QTextDocument *doc = m_pTextEdit->document();
    int max_width = 0;
    //int total_height = 0;
    //遍历每一段找到 最宽的那一段
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())    //字体总长
    {
        int txtW = int(fm.horizontalAdvance(it.text()));
        //int txtH = int(fm.height());// 每一行的高度
        max_width = max_width < txtW ? txtW : max_width;                 //找到最长的那段
        //total_height += txtH; // 计算总高度
    }
    //设置这个气泡的最大宽度 只需要设置一次
    setMaximumWidth(max_width + doc_margin * 2 + (margin_left + margin_right)+1);        //设置最大宽度

    // 设置textedit的高度使其能够刚好包裹文本
    //int doc_height = int(doc->size().height());
    //m_pTextEdit->setFixedHeight(doc_height + doc_margin * 2); // 设置合适高度
}

void TextBubble::initStyleSheet()
{
    m_pTextEdit->setStyleSheet("QTextEdit{background:transparent;border:none}");
}
