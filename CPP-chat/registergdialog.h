#ifndef REGISTERGDIALOG_H
#define REGISTERGDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class RegistergDialog;
}

class RegistergDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistergDialog(QWidget *parent = nullptr);
    ~RegistergDialog();

private slots:
    void on_get_code_clicked();

private:
    void showTip(QString str, bool b_ok);
    Ui::RegistergDialog *ui;
};

#endif // REGISTERGDIALOG_H
