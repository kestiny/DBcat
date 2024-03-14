#include "newhost.h"
#include "ui_newhost.h"
#include <QAction>
#include "appconfig.h"

NewHost::NewHost(QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::NewHost)
    ,hostInfo(nullptr)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);

    ui->pushButtonOk->setFocus();
    ui->pushButtonOk->setDefault(true);

    ui->labelTitle->setText(tr("new cat wizard"));
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButtonOk->hide();
    ui->pushButtonCancel->hide();

    ui->pushButtonClose->setFont(AppConfig::instance().iconFont(16));
    ui->pushButtonClose->setText(QChar(0xf00d));

    ui->checkBoxRemember->hide();
    connect(ui->pushButtonClose, &QPushButton::clicked, this, &NewHost::slotClose);
    connect(ui->pushButtonMysql, &QPushButton::clicked, this, &NewHost::slotCatMysql);
    connect(ui->pushButtonSqlite, &QPushButton::clicked, this, &NewHost::slotCatSqlite);

    connect(ui->pushButtonOk, &QPushButton::clicked, this, &NewHost::slotSubmit);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &NewHost::slotCancel);
}

void NewHost::setHostInfo(HostInfo info)
{
    hostInfo = new HostInfo();
    hostInfo->id = info.id;
    hostInfo->name = info.name;
    hostInfo->host = info.host;
    hostInfo->port = info.port;
    hostInfo->userName = info.userName;
    hostInfo->password = info.password;
    hostInfo->sqlType = info.sqlType;

    ui->lineEditName->setText(info.name);
    ui->lineEditHost->setText(info.host);
    ui->lineEditPort->setText(info.port);
    ui->lineEditUserName->setText(info.userName);
    ui->lineEditPassword->setText(info.sqlType);

    slotCatMysql();
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
        info.name = ui->lineEditName->text();
        info.host = ui->lineEditHost->text();
        info.port = ui->lineEditPort->text();
        info.userName = ui->lineEditUserName->text();
        info.password = ui->lineEditPassword->text();
        if(hostInfo != nullptr) {
            info.id = hostInfo->id;
            info.sqlType = hostInfo->sqlType;

            AppConfig::instance().updateHost(info);
        } else {
            info.id = "999";
            info.sqlType = "MYSQL";

            AppConfig::instance().addHost(info);
        }
    }
    }
    delete hostInfo;
    hostInfo = nullptr;
    accept();
}

void NewHost::slotCancel()
{
    reject();
}
