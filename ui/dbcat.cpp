#include "dbcat.h"
#include "ui_dbcat.h"
#include <QFile>
#include <QTreeWidgetItem>
#include "operation/mysqloperator.h"
#include "regexpconstexpr.h"

DBcat::DBcat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DBcat)
    , _lastSearchItems{}
{
    ui->setupUi(this);
    slotLoadStyleSheet("default.qss");

    ui->pushButton_2->hide();
    ui->widgetSearch->hide();
    auto oper = new MysqlOperator{};
    ui->hostWidget->initDatabseOperator(oper, ui->sqlControll, ui->sqlEdit);
    ui->sqlEdit->initEditorText();
    ui->sqlControll->initUi(oper, ui->sqlEdit, ui->textEditMessage);

    ui->pushButtonPre->setFont(AppConfig::instance().iconFont(10));
    ui->pushButtonPre->setText(QChar(0xf062));
    ui->pushButtonNext->setFont(AppConfig::instance().iconFont(10));
    ui->pushButtonNext->setText(QChar(0xf063));
    ui->pushButtonClose->setFont(AppConfig::instance().iconFont(12));
    ui->pushButtonClose->setText(QChar(0xf00d));
    ui->execButton->setFont(AppConfig::instance().iconFont(16));
    ui->execButton->setText(QChar(0xf04b));

    connect(ui->execButton, &QPushButton::clicked, this, &DBcat::slotExec);
    connect(ui->pushButtonClose, &QPushButton::clicked, this, [&](){ui->widgetSearch->hide();});
    connect(ui->lineEdit, &QLineEdit::textEdited, this, &DBcat::slotSearchText);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, [&]{this->nextSearchItem();});
    connect(ui->pushButtonPre, &QPushButton::clicked, this, [&]{this->preSearchItem();});
    connect(ui->pushButtonNext, &QPushButton::clicked, this, [&]{this->nextSearchItem();});
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

void DBcat::slotSearchText(const QString &text)
{
    searchItems(text);
}

void DBcat::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F
            &&  event->modifiers() == Qt::ControlModifier)
    {
        ui->widgetSearch->show();
        ui->lineEdit->setFocus();
    }
    QWidget::keyPressEvent(event);
}

void DBcat::searchItems(const QString &text)
{
    auto setForegroundColor = [&](const QBrush& color)
    {
        for(auto item : _lastSearchItems)
        {
            item->setForeground(0, color);
        }
    };
    if(_lastSearchItems.size() > 0)
    {
        setForegroundColor(QBrush(Qt::black));
    }
    if(text.isEmpty())
    {
        ui->label->setText("No results");
        return;
    }
    _lastSearchItems.clear();
    QTreeWidgetItemIterator it(ui->hostWidget);
    while (*it) {
        if ((*it)->text(0).contains(text, Qt::CaseInsensitive))
            _lastSearchItems.push_back(*it);
        ++it;
    }

    if(_lastSearchItems.size() > 0)
    {
        setForegroundColor(QBrush(QColor(255, 150, 50)));
        ui->label->setText(QString("1 of %1").arg(QString::number(_lastSearchItems.size())));

        _searchIndex = 0;
        nextSearchItem();
    }
    else
    {
        ui->label->setText("No results");
    }
}

void DBcat::preSearchItem()
{
    _searchIndex = --_searchIndex <= 0 ? _lastSearchItems.size() : _searchIndex;
    ui->hostWidget->setCurrentItem(_lastSearchItems.at(_searchIndex - 1));
    ui->label->setText(QString("%1 of %2").arg(QString::number(_searchIndex),
                                               QString::number(_lastSearchItems.size())));
}

void DBcat::nextSearchItem()
{
    _searchIndex = ++_searchIndex > _lastSearchItems.size() ? 1 : _searchIndex;
    ui->hostWidget->setCurrentItem(_lastSearchItems.at(_searchIndex - 1));
    ui->label->setText(QString("%1 of %2").arg(QString::number(_searchIndex),
                                               QString::number(_lastSearchItems.size())));
}
