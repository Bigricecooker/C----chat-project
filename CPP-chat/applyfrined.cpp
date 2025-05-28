#include "applyfrined.h"
#include "ui_applyfrined.h"
#include <QScrollBar>
#include "tcpmgr.h"
#include "clickedlabel.h"
#include "usermgr.h"




//-------------------------------------
// gridWidget是好友标签展示
// input_tip_wid是提示框
// lb_list是标签展示框
// more_lb是展示更多标签
//-------------------------------------
Applyfrined::Applyfrined(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Applyfrined),_label_point(2,6)
{
    ui->setupUi(this);
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    this->setObjectName("Applyfrined");
    this->setModal(true);

    // 下面是测试
    ui->name_ed->setPlaceholderText(tr("恋恋风辰"));
    ui->lb_ed->setPlaceholderText("搜索、添加标签");
    ui->back_ed->setPlaceholderText("燃烧的胸毛");
    ui->lb_ed->SetMaxLength(21);
    ui->lb_ed->move(2, 2);
    ui->lb_ed->setFixedHeight(20);
    ui->lb_ed->setMaxLength(10);

    ui->input_tip_wid->hide();// 默认隐藏
    _tip_cur_point = QPoint(5, 5);
    _tip_data = { "同学","家人","菜鸟教程","C++ Primer","Rust 程序设计",
                 "父与子学Python","nodejs开发指南","go 语言开发指南",
                 "游戏伙伴","金融投资","微信读书","拼多多拼友" };// 标签

    connect(ui->more_lb, &ClickedOnceLabel::clicked, this, &Applyfrined::ShowMoreLabel);// 点击之后展示更多标签

    InitTipLbs();// 初始化自带的标签

    //链接输入标签回车事件
    connect(ui->lb_ed, &CustomizeEdit::returnPressed, this, &Applyfrined::SlotLabelEnter);// 按下回车后将标签加入展示栏
    connect(ui->lb_ed, &CustomizeEdit::textChanged, this, &Applyfrined::SlotLabelTextChange);// 文本变化后
    connect(ui->lb_ed, &CustomizeEdit::editingFinished, this, &Applyfrined::SlotLabelEditFinished);// 失去焦点后隐藏提示框
    connect(ui->tip_lb, &ClickedOnceLabel::clicked, this, &Applyfrined::SlotAddFirendLabelByClickTip);// 点击（提示框中的标签提示）之后

    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);

    // 设置按钮样式
    ui->sure_btn->SetState("normal","hover","press");
    ui->cancel_btn->SetState("normal","hover","press");

    //连接确认和取消按钮的槽函数
    connect(ui->cancel_btn, &QPushButton::clicked, this, &Applyfrined::SlotApplyCancel);
    connect(ui->sure_btn, &QPushButton::clicked, this, &Applyfrined::SlotApplySure);
}

Applyfrined::~Applyfrined()
{
    delete ui;
}

void Applyfrined::InitTipLbs()
{
    int lines = 1;

    for(int i = 0; i < _tip_data.size(); i++)
    {
        auto* lb = new ClickedLabel(ui->lb_list);
        lb->SetState("normal", "hover", "pressed", "selected_normal",
                     "selected_hover", "selected_pressed");
        lb->setObjectName("tipslb");
        lb->setText(_tip_data[i]);// 设置标签的文本

        // 点击标签实现增加或减少好友标签
        connect(lb, &ClickedLabel::clicked, this, &Applyfrined::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height(); // 获取文本的高度

        // 判断当前行的总宽度是否超过了lb_list的宽度，判断是否换行
        if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {
            lines++;
            if (lines > 2)// 只弄两行
            {
                delete lb;
                return;
            }
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }

        auto next_point = _tip_cur_point;
        AddTipLbs(lb, _tip_cur_point,next_point, textWidth, textHeight);// 添加标签

        _tip_cur_point = next_point;// 获得下次添加到的位置
    }
}

void Applyfrined::AddTipLbs(ClickedLabel * lb, QPoint cur_point, QPoint &next_point, int text_width, int text_height)
{
    lb->move(cur_point);// 设置好友标签的位置
    lb->show();

    _add_labels.insert(lb->text(), lb);
    _add_label_keys.push_back(lb->text());// 存入容器

    next_point.setX(lb->pos().x() + text_width + 15);
    next_point.setY(lb->pos().y());// 计算下次的位置
}

bool Applyfrined::eventFilter(QObject *obj, QEvent *event)
{
    // 隐藏或使用滚动条
    if(obj==ui->scrollArea&&event->type()==QEvent::Enter)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if(obj==ui->scrollArea&&event->type()==QEvent::Leave)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QDialog::eventFilter(obj,event);
}

void Applyfrined::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    _si = si;
    auto applyname = UserMgr::GetInstance()->GetName();
    auto bakname = si->_name;
    ui->name_ed->setText(applyname);
    ui->back_ed->setText(bakname);
}

void Applyfrined::resetLabels()
{
    auto max_width = ui->gridWidget->width();
    auto label_height = 0;
    for(auto iter = _friend_labels.begin(); iter != _friend_labels.end(); iter++){
        //todo... 添加宽度统计
        if( _label_point.x() + iter.value()->width() > max_width) {
            _label_point.setY(_label_point.y()+iter.value()->height()+6);
            _label_point.setX(2);
        }

        iter.value()->move(_label_point);
        iter.value()->show();

        _label_point.setX(_label_point.x()+iter.value()->width()+2);
        _label_point.setY(_label_point.y());
        label_height = iter.value()->height();
    }

    if(_friend_labels.isEmpty()){
        ui->lb_ed->move(_label_point);
        return;
    }

    if(_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->gridWidget->width()){
        ui->lb_ed->move(2,_label_point.y()+label_height+6);
    }else{
        ui->lb_ed->move(_label_point);
    }
}

void Applyfrined::addLabel(QString name)
{
    if (_friend_labels.find(name) != _friend_labels.end()) {
        ui->lb_ed->clear();
        return;
    }

    auto tmplabel = new FriendLabel(ui->gridWidget);
    tmplabel->SetText(name);
    tmplabel->setObjectName("FriendLabel");

    auto max_width = ui->gridWidget->width();
    //todo... 添加宽度统计
    if (_label_point.x() + tmplabel->width() > max_width) {
        _label_point.setY(_label_point.y() + tmplabel->height() + 6);
        _label_point.setX(2);
    }
    else {

    }


    tmplabel->move(_label_point);
    tmplabel->show();
    _friend_labels[tmplabel->Text()] = tmplabel;
    _friend_label_keys.push_back(tmplabel->Text());

    connect(tmplabel, &FriendLabel::sig_close, this, &Applyfrined::SlotRemoveFriendLabel);

    _label_point.setX(_label_point.x() + tmplabel->width() + 2);

    if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->gridWidget->width()) {
        ui->lb_ed->move(2, _label_point.y() + tmplabel->height() + 2);
    }
    else {
        ui->lb_ed->move(_label_point);
    }

    ui->lb_ed->clear();

    if (ui->gridWidget->height() < _label_point.y() + tmplabel->height() + 2) {
        ui->gridWidget->setFixedHeight(_label_point.y() + tmplabel->height() * 2 + 2);
    }
}

void Applyfrined::ShowMoreLabel()
{
    qDebug()<< "receive more label clicked";
    ui->more_lb_wid->hide();// 隐藏按钮

    ui->lb_list->setFixedWidth(325);// 扩展宽度
    _tip_cur_point = QPoint(5, 5);
    auto next_point = _tip_cur_point;
    int textWidth;
    int textHeight;
    // 重排现有的label（就是上面的初始化，不过重新排了一下）
    for(auto & added_key : _add_label_keys){
        auto added_lb = _add_labels[added_key];
        QFontMetrics fontMetrics(added_lb->font()); // 获取QLabel控件的字体信息
        textWidth = fontMetrics.horizontalAdvance(added_lb->text()); // 获取文本的宽度
        textHeight = fontMetrics.height(); // 获取文本的高度
        if(_tip_cur_point.x() +textWidth + tip_offset > ui->lb_list->width()){
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y()+textHeight+15);
        }
        added_lb->move(_tip_cur_point);
        next_point.setX(added_lb->pos().x() + textWidth + 15);
        next_point.setY(_tip_cur_point.y());
        _tip_cur_point = next_point;
    }

    // 添加之前因为限制原因没有添加的
    for(int i = 0; i < _tip_data.size(); i++){
        auto iter = _add_labels.find(_tip_data[i]);
        if(iter != _add_labels.end()){
            continue;
        }
        auto* lb = new ClickedLabel(ui->lb_list);
        lb->SetState("normal", "hover", "pressed", "selected_normal",
                     "selected_hover", "selected_pressed");
        lb->setObjectName("tipslb");
        lb->setText(_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &Applyfrined::SlotChangeFriendLabelByTip);
        QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height(); // 获取文本的高度
        if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }
        next_point = _tip_cur_point;
        AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);
        _tip_cur_point = next_point;
    }

    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();

    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);
    //qDebug()<<"after resize ui->lb_list size is " <<  ui->lb_list->size();
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height()+diff_height);// 调整lb_list和scrollcontent的高度
}

void Applyfrined::SlotLabelEnter()
{
    // 判空
    if(ui->lb_ed->text().isEmpty()){
        return;
    }

    auto text = ui->lb_ed->text();
    // 设置一个好友标签添加
    addLabel(ui->lb_ed->text());

    ui->input_tip_wid->hide();// 将提示框隐藏

    // 先在服务器传过来的标签中找
    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    // 找到了就只需设置状态为选中即可
    if (find_it == _tip_data.end()) {
        _tip_data.push_back(text);
    }

    // 判断标签展示栏是否有该标签
    auto find_add = _add_labels.find(text);
    if (find_add != _add_labels.end()) {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;// 设置为选中后返回
    }
    // 没找到则标签展示栏也增加一个标签, 并设置绿色选中
    auto* lb = new ClickedLabel(ui->lb_list);
    lb->SetState("normal", "hover", "pressed", "selected_normal",
                 "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &Applyfrined::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->lb_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();

    QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height(); // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;
    if (_tip_cur_point.x() + textWidth + tip_offset + 3 > ui->lb_list->width()) {
        _tip_cur_point.setX(5);
        _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
    }
    auto next_point = _tip_cur_point;
    AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);// 添加进展示标签
    _tip_cur_point = next_point;
    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);
    lb->SetCurState(ClickLbState::Selected);
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void Applyfrined::SlotRemoveFriendLabel(QString name)
{
    qDebug() << "receive close signal";

    _label_point.setX(2);
    _label_point.setY(6);

    auto find_iter = _friend_labels.find(name);

    if(find_iter == _friend_labels.end()){
        return;
    }

    auto find_key = _friend_label_keys.end();
    for(auto iter = _friend_label_keys.begin(); iter != _friend_label_keys.end();
         iter++){
        if(*iter == name){
            find_key = iter;
            break;
        }
    }

    if(find_key != _friend_label_keys.end()){
        _friend_label_keys.erase(find_key);
    }


    delete find_iter.value();

    _friend_labels.erase(find_iter);

    resetLabels();// 重排

    auto find_add = _add_labels.find(name);
    if(find_add == _add_labels.end()){
        return;
    }

    find_add.value()->ResetNormalState();
}

void Applyfrined::SlotChangeFriendLabelByTip(QString lbtext, ClickLbState state)
{
    auto find_iter = _add_labels.find(lbtext);
    if(find_iter == _add_labels.end()){
        return;
    }
    if(state == ClickLbState::Selected){
        //编写添加逻辑
        addLabel(lbtext);
        return;
    }
    if(state == ClickLbState::Normal){
        //编写删除逻辑
        SlotRemoveFriendLabel(lbtext);
        return;
    }
}

void Applyfrined::SlotLabelTextChange(const QString &text)
{
    // 文本变化时判断是否出现提示框
    if(text.isEmpty())
    {
        ui->tip_lb->setText("");
        ui->input_tip_wid->hide();
        return;
    }

    auto iter = std::find(_tip_data.begin(),_tip_data.end(),text);
    if (iter == _tip_data.end()) {
        auto new_text = add_prefix + text;
        ui->tip_lb->setText(new_text);
        ui->input_tip_wid->show();
        return;
    }

    ui->tip_lb->setText(text);
    ui->input_tip_wid->show();
}



void Applyfrined::SlotLabelEditFinished()
{
    ui->input_tip_wid->hide();
}

void Applyfrined::SlotAddFirendLabelByClickTip(QString text)
{
    int index = text.indexOf(add_prefix);
    if (index != -1) {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);

    // 先在服务器传过来的标签中找
    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    // 找到了就只需设置状态为选中即可
    if (find_it == _tip_data.end()) {
        _tip_data.push_back(text);
    }

    // 判断标签展示栏是否有该标签
    auto find_add = _add_labels.find(text);
    if (find_add != _add_labels.end()) {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;// 设置为选中后返回
    }
    // 没找到则标签展示栏也增加一个标签, 并设置绿色选中
    auto* lb = new ClickedLabel(ui->lb_list);
    lb->SetState("normal", "hover", "pressed", "selected_normal",
                 "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &Applyfrined::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->lb_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();

    QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height(); // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;
    if (_tip_cur_point.x() + textWidth + tip_offset + 3 > ui->lb_list->width()) {
        _tip_cur_point.setX(5);
        _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
    }
    auto next_point = _tip_cur_point;
    AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);// 添加进展示标签
    _tip_cur_point = next_point;
    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);
    lb->SetCurState(ClickLbState::Selected);
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void Applyfrined::SlotApplySure()
{
    qDebug()<<"Slot Apply Sure caller";
    QJsonObject jsonObj;
    auto uid = UserMgr::GetInstance()->GetUid();// 自己的uid
    jsonObj["uid"]=uid;
    auto name = ui->name_ed->text();// 自己的名字
    if(name.isEmpty()) name = ui->name_ed->placeholderText();
    jsonObj["applyname"]=name;

    auto bakname = ui->back_ed->text();
    if(bakname.isEmpty()) bakname = ui->back_ed->placeholderText();
    jsonObj["bakname"]=bakname;// 备注名
    jsonObj["touid"]=_si->_uid;// 对方的uid


    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    // 发送tcp请求给chatserver
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_ADD_FRIEND_REQ, jsonData);
    this->hide();
    deleteLater();
}

void Applyfrined::SlotApplyCancel()
{
    this->hide();
    deleteLater();
}
