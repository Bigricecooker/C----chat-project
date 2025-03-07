#include "clickedlabel.h"
#include <QMouseEvent>

ClickedLabel::ClickedLabel(QWidget *parent):QLabel(parent),_curstate(ClickLbState::Normal)
{

    // 为两个标签设置鼠标悬停时的光标样式,为手样式
    this->setCursor(Qt::PointingHandCursor);
}

ClickedLabel::~ClickedLabel()
{

}

// 鼠标按下事件
void ClickedLabel::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(_curstate==ClickLbState::Normal)
        {
            qDebug()<<"clicked , change to selected hover: "<< _selected_hover;
            _curstate=ClickLbState::Selected;
            setProperty("state",_selected_press);
        }
        else
        {
            qDebug()<<"clicked , change to normal hover: "<< _normal_hover;
            _curstate=ClickLbState::Normal;
            setProperty("state",_normal_press);
        }
        emit clicked(this->text(), _curstate);
        repolish(this);
        update();
    }


    QLabel::mousePressEvent(event);
}

// 鼠标进入事件
void ClickedLabel::enterEvent(QEnterEvent *event)
{
    if(_curstate==ClickLbState::Normal)
    {
        qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",_normal_hover);
    }
    else
    {
        qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_selected_hover);
    }
    repolish(this);
    update();

    QLabel::enterEvent(event);
}

// 鼠标离开事件
void ClickedLabel::leaveEvent(QEvent *event)
{

    if(_curstate == ClickLbState::Normal){
        qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
    }else{
        qDebug()<<"leave , change to normal hover: "<< _selected;
        setProperty("state",_selected);
    }
    repolish(this);
    update();

    QLabel::leaveEvent(event);
}

// 鼠标抬起事件
void ClickedLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal){
            // qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",_normal_hover);
            repolish(this);
            update();
        }else{
            //  qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(), _curstate);
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

// 设置状态
void ClickedLabel::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;
    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;

    // 初始未选中，为不可见
    setProperty("state",normal);
    repolish(this);
}

bool ClickedLabel::SetCurState(ClickLbState state)
{
    _curstate=state;
    if(state==ClickLbState::Normal)
    {
        setProperty("state", _normal);
        repolish(this);
    }
    else
    {
        setProperty("state", _selected);
        repolish(this);
    }
}

// 获取当前的状态
ClickLbState ClickedLabel::GetCurState()
{
    return _curstate;
}

