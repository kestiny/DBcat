#include "messagebox.h"
#include "ui_messagebox.h"

int MessageBox::messageWarning(QWidget *parent, const QString &title, const QString &text)
{
    MessageBox box{title, text, parent};
    return box.exec();
}

MessageBox::MessageBox(const QString &title, const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(new Ui::MessageBox)
{
    setWindowFlag(Qt::FramelessWindowHint);
    ui->setupUi(this);

    ui->labelTitle->setText(title);
    ui->labelContent->setText(text);

    connect(ui->pushButtonOk, &QPushButton::clicked, this, &MessageBox::slotOk);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &MessageBox::slotCancel);
    connect(ui->pushButtonClose, &QPushButton::clicked, this, &MessageBox::slotCancel);
}
