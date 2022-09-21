#include "messagebox.h"
#include "ui_messagebox.h"
#include "appconfig.h"

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

    ui->labelIcon->setFont(AppConfig::instance().iconFont(24));
    ui->labelIcon->setText(QChar(0xf0f3));

    ui->pushButtonClose->setFont(AppConfig::instance().iconFont(14));
    ui->pushButtonClose->setText(QChar(0xf00d));

    connect(ui->pushButtonOk, &QPushButton::clicked, this, &MessageBox::slotOk);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &MessageBox::slotCancel);
    connect(ui->pushButtonClose, &QPushButton::clicked, this, &MessageBox::slotCancel);
}
