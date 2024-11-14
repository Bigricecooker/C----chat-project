#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include "QStyle"
#include <QRegularExpression>
#include <memory>
#include <mutex>
#include <iostream>
#include <QNetworkReply>
#include <memory>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include "singleton.h"

// 用于刷新qss
extern std::function<void(QWidget*)> repolish;

// 功能
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};

// 模块
enum Modules{
    REGISTERMOD = 0, //注册模块
};

// 返回码
enum ErrorCodes{
    SUCCESS = 0, //成功
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,//网络错误
};

#endif // GLOBAL_H
