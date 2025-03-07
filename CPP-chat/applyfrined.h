#ifndef APPLYFRINED_H
#define APPLYFRINED_H

#include <QDialog>
#include "clickedlabel.h"
#include "userdata.h"
#include "friendlabel.h"
#include "global.h"

namespace Ui {
class Applyfrined;
}
/******************************************************************************
 *
 * @file       applyfrined.h
 * @brief      添加好友界面
 *
 * @author     Bigricecooker
 * @date       2025/03/06
 * @history
 *****************************************************************************/
class Applyfrined : public QDialog
{
    Q_OBJECT

public:
    explicit Applyfrined(QWidget *parent = nullptr);
    ~Applyfrined();

    void InitTipLbs();// 初始化标签
    void AddTipLbs(ClickedLabel*, QPoint cur_point, QPoint &next_point, int text_width, int text_height);// 添加标签至展示框

    bool eventFilter(QObject *obj, QEvent *event);// 事件过滤器
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);// 设置搜索信息...
private:
    void resetLabels();// 重排好友标签栏...

    Ui::Applyfrined *ui;
    QMap<QString, ClickedLabel*> _add_labels;// 已经创建好的标签（展示标签）
    std::vector<QString> _add_label_keys;

    QPoint _label_point;

    // 用来在输入框显示添加新好友的标签
    QMap<QString, FriendLabel*> _friend_labels;
    std::vector<QString> _friend_label_keys;

    // 用来在好友标签编辑栏添加标签
    void addLabel(QString name);
    std::vector<QString> _tip_data;// 服务器传过来的标签内容
    QPoint _tip_cur_point;
    std::shared_ptr<SearchInfo> _si;
public slots:
    //显示更多label标签
    void ShowMoreLabel();
    //输入label按下回车触发将标签加入展示栏
    void SlotLabelEnter();
    //点击关闭，移除展示栏好友便签
    void SlotRemoveFriendLabel(QString);
    //通过点击tip实现增加和减少好友便签
    void SlotChangeFriendLabelByTip(QString, ClickLbState);
    //输入框文本变化显示不同提示
    void SlotLabelTextChange(const QString& text);
    //输入框输入完成
    void SlotLabelEditFinished();
    //输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotAddFirendLabelByClickTip(QString text);
    //处理确认回调
    void SlotApplySure();
    //处理取消回调
    void SlotApplyCancel();
};

#endif // APPLYFRINED_H
