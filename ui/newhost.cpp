#include "newhost.h"
#include "ui_newhost.h"
#include <QAction>
#include "appconfig.h"

NewHost::NewHost(QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::NewHost)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);

    ui->pushButtonOk->setFocus();
    ui->pushButtonOk->setDefault(true);

    ui->labelTitle->setText(tr("new cat wizard"));
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButtonOk->hide();
    ui->pushButtonCancel->hide();

    connect(ui->pushButtonClose, &QPushButton::clicked, this, &NewHost::slotClose);
    connect(ui->pushButtonMysql, &QPushButton::clicked, this, &NewHost::slotCatMysql);
    connect(ui->pushButtonSqlite, &QPushButton::clicked, this, &NewHost::slotCatSqlite);

    connect(ui->pushButtonOk, &QPushButton::clicked, this, &NewHost::slotSubmit);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &NewHost::slotCancel);
}

void NewHost::slotCatMysql()
{
    ui->labelTitle->setText(tr("new mysql"));
    ui->pushButtonOk->show();
    ui->pushButtonCancel->show();
    ui->stackedWidget->setCurrentIndex(1);
}

void NewHost::slotCatSqlite()
{
    accept();
}

void NewHost::slotSubmit()
{
    switch (ui->stackedWidget->currentIndex()) {
    case 1: // mysql
    {
        HostInfo info;
        info.id = 999;
        info.name = ui->lineEditName->text();
        info.host = ui->lineEditHost->text();
        info.port = ui->lineEditPort->text();
        info.userName = ui->lineEditUserName->text();
        info.password = ui->lineEditPassword->text();
        info.sqlType = "MYSQL";

        AppConfig::instance().addHost(info);
    }
    }
    accept();
}

void NewHost::slotCancel()
{
    reject();
}
