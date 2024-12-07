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
#include <QDir>
#include <QSettings>
#include <QTimer>

/******************************************************************************
 *
 * @file       global.h
 * @brief      各种枚举值和全局变量
 *
 * @author     Bigricecooker
 * @date       2024/11/14
 * @history
 *****************************************************************************/
// 用于刷新qss
extern std::function<void(QWidget*)> repolish;

// md5用于加密密码
extern std::function<QString(QString)> md5Encrypt;

// 功能
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002, // 注册用户
    ID_RESET_PWD = 1003,// 重置密码
    ID_LOGIN_USER = 1004,// 用户登录
};

// 模块
enum Modules{
    REGISTERMOD = 0, //注册模块
    RESETMOD = 1,// 重置密码模块
    LOGINMOD = 2,// 登录模块
};

// 返回码
enum ErrorCodes{
    SUCCESS = 0, // 成功
    ERR_JSON = 1, // Json解析失败
    ERR_NETWORK = 2,// 网络错误
};

// 注册界面输入框错误
enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

// Label的两种主要状态
enum ClickLbState{
    Normal = 0,
    Selected = 1
};
extern QString gate_url_prefix;

#endif // GLOBAL_H
