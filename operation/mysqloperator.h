#ifndef MYSQLOPERATOR_H
#define MYSQLOPERATOR_H
#include "appconfig.h"
#include <map>
#include "idboperator.h"

class MysqlOperator : public IDBOperator
{
public:
    MysqlOperator();
    ~MysqlOperator();

    bool connection(HostInfo info) override;
    bool closeConnection(int id) override;

    std::list<Database> database(int id) override;
    std::list<Table> tables(int id, Database database) override;
    QSqlQuery* getQuery(int id, Database database) override;
    QSqlDatabase getDatabase(int id) override;

    QString lastExecMsg() override;
    QString lastError() override;

private:
    QSqlDatabase getDBByHost(int id);

private:
    std::map<int, QSqlDatabase> _mapDatabase;
    QString _lastError;
    QString _lastExecMsg;
};

#endif // MYSQLOPERATOR_H
