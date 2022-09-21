#ifndef MYSQLOPERATOR_H
#define MYSQLOPERATOR_H
#include "appconfig.h"
#include <map>
#include <QDateTime>
#include "idboperator.h"

class MysqlOperator : public IDBOperator
{
    struct ConnectOption{
        QSqlDatabase db;
        QDateTime createTime;

        ConnectOption(QSqlDatabase d, QDateTime time):db{d}, createTime{time} {}
    };

public:
    MysqlOperator();
    ~MysqlOperator();

    bool connection(HostInfo info) override;
    bool closeConnection(QString id) override;

    std::list<Database> database(QString id) override;
    std::list<Table> tables(QString id, Database database) override;
    QSqlQuery* getQuery(QString id, Database database) override;
    QSqlDatabase getDatabase(QString id) override;

    QString lastExecMsg() override;
    QString lastError() override;

private:
    QSqlDatabase getDBByHost(QString id);

private:
    std::map<QString, ConnectOption*> _mapDatabase;
    QString _lastError;
    QString _lastExecMsg;
};

#endif // MYSQLOPERATOR_H
