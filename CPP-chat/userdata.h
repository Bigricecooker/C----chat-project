#ifndef USERDATA_H
#define USERDATA_H
#include <QString>

/******************************************************************************
 *
 * @file       userdata.h
 * @brief      搜索信息
 *
 * @author     Bigricecooker
 * @date       2025/03/04
 * @history
 *****************************************************************************/
class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _sex;
};

#endif // USERDATA_H
