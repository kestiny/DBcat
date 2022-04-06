#include "dbcat.h"
#include "ui_dbcat.h"
#include <QFile>
#include "operation/mysqloperator.h"
#include "regexpconstexpr.h"

DBcat::DBcat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DBcat)
{
    ui->setupUi(this);
    slotLoadStyleSheet("default.qss");

    ui->pushButton_2->hide();
    auto oper = new MysqlOperator{};
    ui->hostWidget->initDatabseOperator(oper, ui->sqlControll, ui->sqlEdit);
    ui->sqlEdit->initEditorText();
    ui->sqlControll->initUi(oper, ui->sqlEdit, ui->textEditMessage);

    connect(ui->execButton, &QPushButton::clicked, this, &DBcat::slotExec);
}

DBcat::~DBcat()
{
    delete ui;
}

void DBcat::initWidgetSize()
{
    ui->hostWidget->resize(300, ui->hostWidget->height());
}

void DBcat::slotLoadStyleSheet(const QString &file)
{
    QFile qss(file);
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();
}

void DBcat::slotExec()
{
    if(ui->hostWidget->currentHostId() == -1)
    {
        ui->sqlControll->setMessage(tr("Please connect to the host, and select one database to use."));
        ui->sqlControll->addMessage(ui->sqlEdit->currentSelection());
        return;
    }
    if(ui->sqlEdit->currentSelection().isEmpty())
    {
        ui->sqlControll->setMessage(tr("Please select sql statement to query."));
        return;
    }

    if(ui->sqlControll->doExec(ui->hostWidget->currentHostId(), ui->hostWidget->currentDatabase()))
    {
        auto texts = regExpFactory().captureTexts(DATABASE_OR_TABLE_CREATE, ui->sqlEdit->currentSelectionSqlStatement());
        if(texts.size() > 0)
        {
            ui->hostWidget->addItemCommand(texts.front().toLower());
        }
    }
}
