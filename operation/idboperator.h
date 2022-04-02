#ifndef IDBOPERATOR_H
#define IDBOPERATOR_H
#include "dbtype.h"
#include "appconfig.h"

class IDBOperator
{
public:
    virtual bool connection(HostInfo info) = 0;
    virtual bool closeConnection(int id) = 0;

    virtual std::list<Database> database(int id) = 0;
    virtual std::list<Table> tables(int id, Database database) = 0;

    virtual QSqlQuery* getQuery(int id, Database database) = 0;
    virtual QSqlDatabase getDatabase(int id) =0;

    virtual QString lastExecMsg() = 0;
    virtual QString lastError() = 0;
};

#endif // IDBOPERATOR_H
