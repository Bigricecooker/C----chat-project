#include "registergdialog.h"
#include "ui_registergdialog.h"


RegistergDialog::RegistergDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegistergDialog)
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

    initHttpHandlers();
}

RegistergDialog::~RegistergDialog()
{
    delete ui;
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

// 收到注册完成事件
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
        showTip(tr("用户注册成功"), true);
        qDebug()<< "email is " << email ;
    });
}

// 点击确认注册按钮事件
void RegistergDialog::on_confirm_pushButton_clicked()
{
    if(ui->user_lineEdit->text()=="")
    {
        showTip(tr("用户名不能为空"), false);
        return;
    }

    if(ui->email_lineEdit->text()=="")
    {
        showTip(tr("邮箱不能为空"), false);
        return;
    }

    if(ui->pass_lineEdit->text()=="")
    {
        showTip(tr("密码不能为空"), false);
        return;
    }

    if(ui->confirm_lineEdit->text()=="")
    {
        showTip(tr("确认密码不能为空"), false);
        return;
    }

    if(ui->confirm_lineEdit->text()!=ui->pass_lineEdit->text())
    {
        showTip(tr("确认密码和密码不匹配"), false);
        return;
    }

    if(ui->ver_lineEdit->text()=="")
    {
        showTip(tr("验证码不能为空"), false);
        return;
    }

    // 发送http请求注册账号
    QJsonObject json_obj;
    json_obj["user"]=ui->user_lineEdit->text();
    json_obj["email"] = ui->email_lineEdit->text();
    json_obj["passwd"] = ui->pass_lineEdit->text();
    json_obj["confirm"] = ui->confirm_lineEdit->text();
    json_obj["verifycode"] = ui->ver_lineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}

