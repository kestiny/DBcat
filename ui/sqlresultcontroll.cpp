#include "sqlresultcontroll.h"
#include <QTabBar>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>
#include <QTime>
#include <QSortFilterProxyModel>
#include "regexpconstexpr.h"
#include "operation/syntaxparser.h"
#include "ui/messagebox.h"

SqlResultControll::SqlResultControll(QWidget *parent)
    :QTabWidget{parent},_dbOperator{nullptr},
      _messageEdit{nullptr}, _sqlEdit{nullptr},
      _resultModel{nullptr}
{
    connect(this, &QTabWidget::tabCloseRequested,
            this, &SqlResultControll::slotCloseTab);
}

SqlResultControll::~SqlResultControll()
{

}

void SqlResultControll::initUi(IDBOperator *dbOperator, SqlEditor *sqlEdit, QTextEdit *message)
{
    _dbOperator = dbOperator;
    _sqlEdit = sqlEdit;
    _messageEdit = message;

    setTabsClosable(true);
    static_cast<QTabBar*>(tabBar())->setTabButton(0, QTabBar::RightSide, nullptr);
    setCurrentIndex(0);
    setTabText(0, tr("Message"));
    setTabIcon(0, QIcon("://image/default/log.svg"));

    removeTab(1);
}

void SqlResultControll::setMessage(const QString &msg)
{
    _messageEdit->setPlainText(QTime::currentTime().toString("hh:mm:ss.zzz  ").append(msg));
    setCurrentIndex(0);
}

void SqlResultControll::addMessage(const QString &msg)
{
    _messageEdit->append(QTime::currentTime().toString("hh:mm:ss.zzz  ").append(QString("\r\n\r\n")).append(msg));
}

void SqlResultControll::setTableName(QString id, Database, const QString &name)
{
    auto iter = _mapTable.find(name);
    if(iter != std::end(_mapTable))
    {
        setCurrentIndex(iter->second.index);
        return;
    }

    auto model = new QSqlTableModel(parent(), _dbOperator->getDatabase(id));
    model->setTable(name);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    addResultTab(name, model);
}

bool SqlResultControll::doExec(QString id, Database database)
{
    QString strSql = _sqlEdit->currentSelectionSqlStatement();
    if(strSql.isEmpty())
    {
        setMessage(tr("[Selection] %1").arg(_sqlEdit->currentSelection()));
        addMessage(tr("[Query] please input a valid sql statement."));
        return false;
    }
    if(regExpFactory().isMatch(DANGEROUSCOMMAND, strSql))
    {
        if(QDialog::Rejected == MessageBox::messageWarning(this, tr("warning"), tr("exec dangerous command!!!")))
        {
            return false;
        }
    }

    auto query = _dbOperator->getQuery(id, database);
    if(query == nullptr)
    {
        setMessage(tr("[query] %1").arg(_sqlEdit->currentSelection()));
        addMessage(tr("[Error] %1").arg(_dbOperator->lastError()));
        return false;
    }

    if(!strSql.isEmpty() && query->exec(strSql))
    {
        if(query->isSelect())
        {
            if(_resultModel == nullptr)
            {
                _resultModel = new QSqlQueryModel();
                QSortFilterProxyModel *sqlproxy = new QSortFilterProxyModel(this);
                sqlproxy->setSourceModel(_resultModel);
                auto view = getView();
                view->setModel(sqlproxy);
                insertTab(1, view, QIcon("://image/default/query.svg"), "Result");
                static_cast<QTabBar*>(tabBar())->setTabButton(1, QTabBar::RightSide, nullptr);
            }

            setCurrentIndex(1);
            _resultModel->setQuery(*query);
        }
        else
        {
            SyntaxParser parser;
            parser.parse(strSql);
            setMessage(tr("execute %1 command, affected %2 rows.")
                       .arg(parser.sqlCommand(), QString::number(query->numRowsAffected())));
            setCurrentIndex(0);
        }
        return true;
    }
    else
    {
        setMessage(tr("[Selection] %1").arg(_sqlEdit->currentSelection()));
        addMessage(tr("[Query] %1").arg(strSql));
        addMessage(tr("[Error] %1").arg(query->lastError().text()));
        return false;
    }
}

QStringList SqlResultControll::queryFields(QString id, const QString &name)
{
    QStringList fields;
    auto query = _dbOperator->getQuery(id, {"information_schema"});
    if(query != nullptr)
    {
        query->exec(QString("select DISTINCT COLUMN_NAME from COLUMNS where table_name='%1'").arg(name));
        int fieldNo = query->record().indexOf("COLUMN_NAME");
        while (query->next())
        {
            fields.append(query->value(fieldNo).toString());
        }
    }
    return fields;
}

void SqlResultControll::slotCloseTab(int index)
{
    auto iter = _mapTable.find(tabText(index));
    if(iter != std::end(_mapTable))
    {
        iter->second.model->revertAll();
        delete iter->second.model;
        _mapTable.erase(iter);
        removeTab(index);
        return;
    }
}

void SqlResultControll::addResultTab(const QString &name, QSqlTableModel *model)
{
    auto view = getView();
    view->setModel(model);
    int index = addTab(view, QIcon("://image/default/query.svg"), name);
    _mapTable[name] = {index, model};
    setCurrentIndex(index);
}

QTableView *SqlResultControll::getView()
{
    auto view = new QTableView;
    view->setSortingEnabled(true);

    return view;
}
