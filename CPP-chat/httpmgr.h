#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "global.h"
#include "singleton.h"

/******************************************************************************
 *
 * @file       httpmgr.h
 * @brief      http请求类
 *
 * @author     Bigricecooker
 * @date       2024/11/14
 * @history
 *****************************************************************************/

class HttpMgr:public QObject, public Singleton<HttpMgr>,public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

public:
    ~HttpMgr();
    // 发送http请求
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager _manager;



private slots:
    // http请求完后处理
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
signals:
    // http请求完成信号
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    // 注册完成信号
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    // 确认重置按钮完成信号
    void sig_rest_mod_finish(ReqId id, QString res, ErrorCodes err);
    // 点击登录按钮完成信号
    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // HTTPMGR_H
