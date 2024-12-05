#include "global.h"

// 实现刷新样式
std::function<void(QWidget*)> repolish=[](QWidget* w){
    w->style()->unpolish(w);// 移除样式
    w->style()->polish(w);// 应用样式
};

// MD5加密
std::function<QString(QString)> md5Encrypt=[](QString input){
    QByteArray byteArray = input.toUtf8(); // 将输入字符串转换为字节数组
    QByteArray hash = QCryptographicHash::hash(byteArray, QCryptographicHash::Md5); // 使用 MD5 进行加密
    return QString(hash.toHex()); // 返回十六进制格式的加密结果
};

QString gate_url_prefix="";
