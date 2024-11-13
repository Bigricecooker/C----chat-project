#include "registergdialog.h"
#include "ui_registergdialog.h"

RegistergDialog::RegistergDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegistergDialog)
{
    ui->setupUi(this);
}

RegistergDialog::~RegistergDialog()
{
    delete ui;
}
