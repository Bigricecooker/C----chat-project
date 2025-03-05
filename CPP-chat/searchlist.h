#ifndef SEARCHLIST_H
#define SEARCHLIST_H
#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QDebug>
#include <QDialog>
#include <memory.h>
#include "loadingdlg.h"
#include "userdata.h"
/******************************************************************************
 *
 * @file       searchlist.h
 * @brief      自定义搜索列表
 *
 * @author     Bigricecooker
 * @date       2025/03/04
 * @history
 *****************************************************************************/
class SearchList: public QListWidget
{
    Q_OBJECT
public:
    SearchList(QWidget *parent = nullptr);
    void CloseFindDlg();
    void SetSearchEdit(QWidget* edit);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;// 事件过滤器
private:
    void waitPending(bool pending = true);
    bool _send_pending;
    void addTipItem();// 添加条目
    std::shared_ptr<QDialog> _find_dlg;
    QWidget* _search_edit;
    LoadingDlg * _loadingDialog;
private slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_user_search(std::shared_ptr<SearchInfo> si);
signals:
};

#endif // SEARCHLIST_H
