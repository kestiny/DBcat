#include "mysqloperator.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include "regexpconstexpr.h"

MysqlOperator::MysqlOperator()
{

}

MysqlOperator::~MysqlOperator()
{
    for(auto& [key, value] : _mapDatabase)
    {
        value->db.close();
    }

    _mapDatabase.clear();
}

bool MysqlOperator::connection(HostInfo info)
{
    if(_mapDatabase.find(info.id) != _mapDatabase.end())
    {
        return true;
    }

    // open mysql database
    auto db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(info.host);
    db.setPort(info.port.toInt());
    db.setUserName(info.userName);
    db.setPassword(info.password);
    if(db.open())
    {
        _mapDatabase[info.id] = new ConnectOption(db, QDateTime::currentDateTime());
        return true;
    }
    _lastExecMsg = QString("[Open database] %1:%2").arg(info.host, info.port);
    _lastError = db.lastError().text();
    return false;
}

std::list<Database> MysqlOperator::database(QString id)
{
    auto db = getDBByHost(id);
    if(!db.isOpen())
    {
        return {};
    }

    std::list<Database> results;
    QSqlQuery query("SHOW DATABASES", db);
    while (query.next()) {
        results.emplace_back(
                    Database{
                        query.value(0).toString()
                    });
    }
    _lastExecMsg = QString("SHOW DATABASES");
    _lastError = query.lastError().text();
    return results;
}

std::list<Table> MysqlOperator::tables(QString id, Database database)
{
    auto db = getDBByHost(id);
    std::list<Table> results;

    QSqlQuery query(db);
    if(!query.exec(QString("USE `%1`").arg(database.name)))
    {
        _lastError = query.lastError().text();
    }
    query.exec("SHOW TABLES");
    while (query.next()) {
        results.emplace_back(
                    Table{
                        query.value(0).toString()
                    });
    }
    _lastExecMsg = QString("[%1] SHOW TABLES").arg(database.name);
    _lastError = query.lastError().text();
    return results;
}

QSqlQuery* MysqlOperator::getQuery(QString id, Database database)
{
    auto db = getDBByHost(id);
    auto query = new QSqlQuery(db);
    if(!database.name.isEmpty() && !query->exec(QString("USE `%1`").arg(database.name)))
    {
        _lastError = query->lastError().text();
    }

    return query;
}

QSqlDatabase MysqlOperator::getDatabase(QString id)
{
    return getDBByHost(id);
}

QString MysqlOperator::lastExecMsg()
{
    return _lastExecMsg;
}

QString MysqlOperator::lastError()
{
    return _lastError;
}

QSqlDatabase MysqlOperator::getDBByHost(QString id)
{
    auto iter = _mapDatabase.find(id);
    if(iter == _mapDatabase.end())
    {
        if(!connection(AppConfig::instance().findHost(id)))
        {
            return {};
        }
        iter = _mapDatabase.find(id);
    }

    if(iter->second->createTime.secsTo(QDateTime::currentDateTime()) > 4 * 60 * 60) {
        auto db = iter->second->db;
        db.close();
        db.open();
        _mapDatabase[id] = new ConnectOption(db, QDateTime::currentDateTime());
        return db;
    } else {
       return iter->second->db;
    }
}

bool MysqlOperator::closeConnection(QString id)
{
    auto iter = _mapDatabase.find(id);
    if(iter != _mapDatabase.end())
    {
        iter->second->db.close();
        _mapDatabase.erase(iter);
    }
    return true;
}
