#include "logindialog.h"
#include "ui_logindialog.h"
#include "httpmgr.h"
#include "tcpmgr.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    enableBtn(true);

    // 设置密码模式
    ui->pass_lineEdit->setEchoMode(QLineEdit::Password);

    // 点击注册按钮发出进入注册界面信号
    connect(ui->reg_pushButton,&QPushButton::clicked,this,&LoginDialog::switchRegister);

    //忘记密码Label
    ui->forget_label->SetState("normal","hover","hover","selected","selected_hover","selected_hover");
    // 点击忘记密码按钮发出进入重置密码界面信号
    connect(ui->forget_label,&ClickedLabel::clicked,this,&LoginDialog::switchReset);

    // 连接点击登录后回调
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_login_mod_finish,this,&LoginDialog::slot_login_mod_finish);

    // 连接发送TCP连接信号
    connect(this,&LoginDialog::sig_connect_tcp,TcpMgr::GetInstance().get(),&TcpMgr::slot_tcp_connect);
    // 连接TCP管理者发出的TCP连接成功与否信号
    connect(TcpMgr::GetInstance().get(),&TcpMgr::sig_con_success,this,&LoginDialog::slot_tcp_con_finish);
    // 连接TCP管理者发出的登录失败信号
    connect(TcpMgr::GetInstance().get(),&TcpMgr::sig_login_failed,this,&LoginDialog::slot_login_failed);

    initHttpHandlers();
}

LoginDialog::~LoginDialog()
{
    qDebug()<<"destruct LoginDlg";
    delete ui;
}

// 错误信息显示
void LoginDialog::showTip(QString str, bool b_ok)
{
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

// 点击登录按钮事件
void LoginDialog::on_login_Button_clicked()
{
    qDebug()<<"login btn clicked";
    if(checkUserValid() == false){
        return;
    }
    if(checkPassValid() == false){
        return ;
    }

    enableBtn(false);// 设置按钮为不可点击
    QJsonObject json;
    json["email"]=ui->user_lineEdit->text();
    json["passwd"]=ui->pass_lineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(gate_url_prefix+"/user_login",json,ReqId::ID_LOGIN_USER,Modules::LOGINMOD);
}

// 收到登录完成或其他完成事件
void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    enableBtn(true);
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


    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}

// TCP连接建立成功与否事件
void LoginDialog::slot_tcp_con_finish(bool bsuccess)
{
    if(bsuccess)
    {
        showTip(tr("聊天服务连接成功，正在登录..."),true);
        QJsonObject jsonObj;
        jsonObj["token"] = _token;
        jsonObj["uid"] = _uid;

        QJsonDocument doc(jsonObj);
        QByteArray  jsonData = doc.toJson(QJsonDocument::Indented);

        // 向聊天服务器发送登录请求
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonData);
    }
    else
    {
        showTip(tr("网络异常"),false);
        enableBtn(true);
    }
}

// 登录失败事件
void LoginDialog::slot_login_failed(int err)
{
    QString result = QString("登录失败, err is %1")
                         .arg(err);
    showTip(result,false);
    enableBtn(true);
}

// 注册消息处理回调
void LoginDialog::initHttpHandlers()
{
    // 注册点击登录回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER,[this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            enableBtn(true);
            return;
        }

        // 解析状态服务器回的密钥
        auto user = jsonObj["user"].toString();

        ServerInfo info;
        info.Host = jsonObj["host"].toString();
        info.Port = jsonObj["port"].toString();
        info.Uid = jsonObj["uid"].toInt();
        info.Token = jsonObj["token"].toString();

        _uid = info.Uid;
        _token = info.Token;

        qDebug()<< "user is " << user << " uid is " << info.Uid <<" host is "
                 << info.Host << " Port is " << info.Port << " Token is " << info.Token;

        // 发送向对端进行TCP连接的信号
        emit sig_connect_tcp(info);

        // 设置按钮为不可点击
        enableBtn(false);
    });
}

// 输入框错误提示
bool LoginDialog::checkUserValid()
{
    if(ui->user_lineEdit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}
bool LoginDialog::checkPassValid()
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

// 添加错误
void LoginDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te]=tips;
    showTip(tips,false);
}

// 删除错误
void LoginDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

// 设置按钮是否可点击
bool LoginDialog::enableBtn(bool enabled)
{
    ui->login_Button->setEnabled(enabled);
    ui->reg_pushButton->setEnabled(enabled);
    ui->forget_label->setEnabled(enabled);
    return true;
}
