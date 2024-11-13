#ifndef REGISTERGDIALOG_H
#define REGISTERGDIALOG_H

#include <QDialog>

namespace Ui {
class RegistergDialog;
}

class RegistergDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistergDialog(QWidget *parent = nullptr);
    ~RegistergDialog();

private:
    Ui::RegistergDialog *ui;
};

#endif // REGISTERGDIALOG_H
