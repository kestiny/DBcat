#ifndef IDBOPERATOR_H
#define IDBOPERATOR_H
#include "dbtype.h"
#include "appconfig.h"

class IDBOperator
{
public:
    virtual bool connection(HostInfo info) = 0;
    virtual bool closeConnection(QString id) = 0;

    virtual std::list<Database> database(QString id) = 0;
    virtual std::list<Table> tables(QString id, Database database) = 0;

    virtual QSqlQuery* getQuery(QString id, Database database) = 0;
    virtual QSqlDatabase getDatabase(QString id) =0;

    virtual QString lastExecMsg() = 0;
    virtual QString lastError() = 0;
};

#endif // IDBOPERATOR_H
