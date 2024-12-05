#include "resetdialog.h"
#include "ui_resetdialog.h"
#include <QDebug>
#include <QRegularExpression>
#include "httpmgr.h"


ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);

    // 连接输入框错误处理
    connect(ui->user_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });
    connect(ui->email_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkEmailValid();
    });
    connect(ui->ver_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkVerifyValid();
    });
    connect(ui->newpwd_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkNewPassValid();
    });


    //连接reset相关信号和注册处理回调
    initHttpHandlers();
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_rest_mod_finish,this,&ResetDialog::slot_reset_mod_finish);

}

ResetDialog::~ResetDialog()
{
    delete ui;
}

// 错误信息显示
void ResetDialog::showTip(QString str,bool b_ok)
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

// 添加错误
void ResetDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te]=tips;
    showTip(tips,false);
}

// 删除错误
void ResetDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty())
    {
        ui->err_tip->clear();
        return;
    }

    showTip(_tip_errs.first(),false);
}

// 输入框错误提示
bool ResetDialog::checkUserValid()// 用户名
{
    if(ui->user_lineEdit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}
bool ResetDialog::checkNewPassValid()// 新密码
{
    auto pass = ui->newpwd_lineEdit->text();
    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("新密码长度应为6~15"));
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
bool ResetDialog::checkEmailValid()// 邮箱
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
bool ResetDialog::checkVerifyValid()// 验证码
{
    auto pass = ui->ver_lineEdit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

// 注册消息处理或其他完成回调
void ResetDialog::initHttpHandlers()
{
    // 获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }

        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is " << email ;
    });

    // 确认重置按钮回包逻辑
    _handlers.insert(ReqId::ID_RESET_PWD,[this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }

        auto email = jsonObj["email"].toString();
        showTip(tr("重置成功,点击返回登录"), true);
        qDebug()<< "email is " << email ;
        qDebug()<< "user uuid is " <<  jsonObj["uuid"].toString();
    });
}

// 点击取消按钮事件
void ResetDialog::on_cancal_pushButton_clicked()
{
    emit switchLogin();
}

// 点击确认按钮事件
void ResetDialog::on_confirm_pushButton_clicked()
{
    bool valid = checkUserValid();
    if(!valid){
        return;
    }
    valid=checkEmailValid();
    if(!valid){
        return;
    }
    valid=checkVerifyValid();
    if(!valid){
        return;
    }
    valid=checkNewPassValid();
    if(!valid){
        return;
    }

    // 发送修改密码的http请求
    QJsonObject json_obj;
    json_obj["user"] = ui->email_lineEdit->text();
    json_obj["email"] = ui->email_lineEdit->text();
    json_obj["verifycode"] = ui->email_lineEdit->text();
    json_obj["newpwd"] = ui->email_lineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/reset_pwd"),
                                        json_obj,ReqId::ID_RESET_PWD,Modules::RESETMOD);
}

// 点击获取验证码按钮事件
void ResetDialog::on_get_code_clicked()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_lineEdit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match)
    {
        // 发送获取验证码的http请求
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_varifycode"),json_obj,ReqId::ID_GET_VARIFY_CODE,Modules::RESETMOD);
    }
    else
    {
        showTip("邮箱地址不正确",false);
    }
}

// 收到重置完成或其他完成事件
void ResetDialog::slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err)
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
    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());
    return;
}

