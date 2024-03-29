#ifndef SQLRESULTCONTROLL_H
#define SQLRESULTCONTROLL_H
#include <QTableWidget>
#include <QTextEdit>
#include <QSqlTableModel>
#include "operation/idboperator.h"
#include "sqleditor.h"

class SqlResultControll : public QTabWidget
{
    Q_OBJECT
    struct SqlTablePair
    {
        int index;
        QSqlTableModel* model;
    };

public:
    SqlResultControll(QWidget *parent = nullptr);
    ~SqlResultControll();

    void initUi(IDBOperator *dbOperator, SqlEditor *sqlEdit, QTextEdit *message);
    void setMessage(const QString& msg);
    void addMessage(const QString& msg);

    void setTableName(QString id, Database database, const QString& name);

    bool doExec(QString id, Database database);
    QStringList queryFields(QString id, const QString& name);

private slots:
    void slotCloseTab(int);

private:
    void addResultTab(const QString& name, QSqlTableModel* model);
    QTableView* getView();

private:
    IDBOperator *_dbOperator;
    QTextEdit *_messageEdit;
    SqlEditor *_sqlEdit;
    std::map<QString, SqlTablePair> _mapTable;
    QSqlQueryModel *_resultModel;
};

#endif // SQLRESULTCONTROLL_H
