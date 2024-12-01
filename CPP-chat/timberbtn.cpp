#include "timberbtn.h"


TimberBtn::TimberBtn(QWidget *parent):QPushButton(parent),_counter(10)
{
    _timer=new QTimer(this);

    // _timeer触发一次就执行一次
    connect(_timer,&QTimer::timeout,this,[this](){
        _counter--;
        if(_counter <= 0){
            _timer->stop();
            _counter = 10;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(_counter));
    });
}

TimberBtn::~TimberBtn()
{
    _timer->stop();
}

void TimberBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)// 是否为左键
    {
        // 开始处理鼠标左键释放事件
        qDebug()<<"MyButton was released";
        this->setEnabled(false);
        this->setText(QString::number(_counter));
        _timer->start(1000);// 1s
        emit clicked();// 不是必须的，不过最好写一下
    }
    // 调用基类的mouseReleaseEvent以确保正常的事件处理（如点击效果）
    QPushButton::mouseReleaseEvent(e); // 因为基类可能还要进行部分操作，子类这里只重写了这一个功能
}
