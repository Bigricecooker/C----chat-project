#include "registergdialog.h"
#include "ui_registergdialog.h"
#include "clickedlabel.h"


RegistergDialog::RegistergDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegistergDialog),_countdown(5)
{
    ui->setupUi(this);

    // 设置密码模式
    ui->pass_lineEdit->setEchoMode(QLineEdit::Password);
    ui->confirm_lineEdit->setEchoMode(QLineEdit::Password);

    // 设置错误提示样式
    ui->err_tip->setProperty("state","normal");//修改状态
    repolish(ui->err_tip);

    // 连接注册完成信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegistergDialog::slot_reg_mod_finish);
    // 点击取消按钮发出进入登录界面信号
    connect(ui->cancel_pushButton,&QPushButton::clicked,this,&RegistergDialog::switchLogin);

    // 注册消息处理
    initHttpHandlers();

    // 连接输入框错误提示
    ui->err_tip->clear();
    connect(ui->user_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });
    connect(ui->pass_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkPassValid();
    });
    connect(ui->email_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkEmailValid();
    });
    connect(ui->confirm_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkPassandComfirmValid();
    });
    connect(ui->ver_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkVerifyValid();
    });


    ui->confirm_visible->SetState("unvisible","unvisible_hover","unvisible_hover","visible",
                                  "visible_hover","visible_hover");
    ui->pass_visible->SetState("unvisible","unvisible_hover","unvisible_hover","visible",
                               "visible_hover","visible_hover");// 按下和悬停一样
    // 连接这两个标签的点击事件,切换密码是否可见
    connect(ui->confirm_visible,&ClickedLabel::clicked,this,[this](){
        if(ui->confirm_visible->GetCurState()==ClickLbState::Normal)
        {
            ui->confirm_lineEdit->setEchoMode(QLineEdit::Password);
        }
        else
        {
            ui->confirm_lineEdit->setEchoMode(QLineEdit::Normal);
        }
    });
    connect(ui->pass_visible,&ClickedLabel::clicked,this,[this](){
        if(ui->pass_visible->GetCurState()==ClickLbState::Normal)
        {
            ui->pass_lineEdit->setEchoMode(QLineEdit::Password);
        }
        else
        {
            ui->pass_lineEdit->setEchoMode(QLineEdit::Normal);
        }
    });


    // 注册成功界面
    _countdown_timer=new QTimer(this);
    connect(_countdown_timer,&QTimer::timeout,this,[this](){
        if(_countdown==0)
        {
            _countdown_timer->stop();
            emit switchLogin();// 发送进入注册界面信号
            _countdown=5;
        }
        _countdown--;
        auto str = QString("注册成功，%1 s后返回登录界面").arg(_countdown);
        ui->tip1_lb->setText(str);
    });
}

RegistergDialog::~RegistergDialog()
{
    delete ui;
}

// 切换至注册成功界面
void RegistergDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    // 启动定时器，设置间隔为1000毫秒（1秒）
    _countdown_timer->start(1000);
}

// 点击获取验证码按钮事件
void RegistergDialog::on_get_code_clicked()//这里是ui界面弄的
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_lineEdit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"]=email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);

    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"),false);
    }
}

// 收到注册完成或其他完成事件
void RegistergDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }
    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }
    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();

    //调用对应的逻辑,根据id回调（initHttpHandlers中注册的）
    _handlers[id](jsonDoc.object());
    return;

}

// 错误信息显示
void RegistergDialog::showTip(QString str,bool b_ok)
{
    // 修改状态
    if(b_ok)
    {
        ui->err_tip->setProperty("state","normal");
    }
    else
    {
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

// 注册消息处理
void RegistergDialog::initHttpHandlers()
{
    // 注册获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is " << email ;
    });

    // 注册注册用户回包逻辑
    _handlers.insert(ReqId::ID_REG_USER,[this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        auto user = jsonObj["user"].toString();
        auto uid = jsonObj["uid"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug()<< "user uid is " << uid ;
        qDebug()<< "email is " << email ;

        // 切换到注册成功界面
        ChangeTipPage();
    });
}



// 点击确认注册按钮事件
void RegistergDialog::on_confirm_pushButton_clicked()
{
    // if(ui->user_lineEdit->text()=="")
    // {
    //     showTip(tr("用户名不能为空"), false);
    //     return;
    // }
    // if(ui->email_lineEdit->text()=="")
    // {
    //     showTip(tr("邮箱不能为空"), false);
    //     return;
    // }
    // if(ui->pass_lineEdit->text()=="")
    // {
    //     showTip(tr("密码不能为空"), false);
    //     return;
    // }
    // if(ui->confirm_lineEdit->text()=="")
    // {
    //     showTip(tr("确认密码不能为空"), false);
    //     return;
    // }
    // if(ui->confirm_lineEdit->text()!=ui->pass_lineEdit->text())
    // {
    //     showTip(tr("确认密码和密码不匹配"), false);
    //     return;
    // }
    // if(ui->ver_lineEdit->text()=="")
    // {
    //     showTip(tr("验证码不能为空"), false);
    //     return;
    // }

    bool valid = checkUserValid();
    if(!valid) return;

    valid=checkEmailValid();
    if(!valid) return;

    valid=checkPassValid();
    if(!valid) return;

    valid=checkPassandComfirmValid();
    if(!valid) return;

    valid=checkVerifyValid();
    if(!valid) return;



    // 发送http请求注册账号
    QJsonObject json_obj;
    json_obj["user"]=ui->user_lineEdit->text();
    json_obj["email"] = ui->email_lineEdit->text();
    // 不发送明文，加密发送
    json_obj["passwd"] = md5Encrypt(ui->pass_lineEdit->text());
    json_obj["confirm"] = md5Encrypt(ui->confirm_lineEdit->text());
    json_obj["verifycode"] = ui->ver_lineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}

// 添加错误
void RegistergDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}
// 删除错误
void RegistergDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

// 各种输入框错误提示
bool RegistergDialog::checkUserValid()// 用户名
{
    if(ui->user_lineEdit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}
bool RegistergDialog::checkPassValid()// 密码
{
    auto pass = ui->pass_lineEdit->text();
    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }
    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}
bool RegistergDialog::checkEmailValid()// 邮箱
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_lineEdit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}
bool RegistergDialog::checkVerifyValid()// 验证码
{
    auto pass = ui->ver_lineEdit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

bool RegistergDialog::checkPassandComfirmValid()// 确认密码
{
    auto confirm = ui->confirm_lineEdit->text();

    auto pass = ui->pass_lineEdit->text();
    if(confirm!=pass)
    {
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("确认密码与密码不一致"));
        return false;
    }

    DelTipErr(TipErr::TIP_PWD_CONFIRM);
    return true;
}


// 点击取消按钮事件
void RegistergDialog::on_return_btn_clicked()
{
    _countdown_timer->stop();
    emit switchLogin();// 发送返回登录界面信号
}

