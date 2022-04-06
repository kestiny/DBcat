#include "appconfig.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

bool AppConfig::addHost(HostInfo info)
{
    auto sqlQuery = getQuery();
    sqlQuery.prepare("INSERT INTO hostInfo(name,host,port,userName,password,sqlType) "
                "VALUES (?, ?, ?, ?, ?,?)");
    sqlQuery.bindValue(0, info.name);
    sqlQuery.bindValue(1, info.host);
    sqlQuery.bindValue(2, info.port);
    sqlQuery.bindValue(3, info.userName);
    sqlQuery.bindValue(4, encrypt(info.password));
    sqlQuery.bindValue(5, info.sqlType);

    bool ret =  sqlQuery.exec();
    _strError = sqlQuery.lastError().text();
    return ret;
}

std::list<HostInfo> AppConfig::listHosts()
{
    auto sqlQuery = getQuery();
    std::list<HostInfo> results;
    sqlQuery.exec("select id,name,host,port,userName,password,sqlType from hostInfo");
    while (sqlQuery.next()) {
        results.emplace_back(
                    HostInfo{
                        sqlQuery.value(0).toInt(),
                        sqlQuery.value(1).toString(),
                        sqlQuery.value(2).toString(),
                        sqlQuery.value(3).toString(),
                        sqlQuery.value(4).toString(),
                        decrypt(sqlQuery.value(5).toByteArray()),
                        sqlQuery.value(6).toString()
                    });
    }
    _strError = sqlQuery.lastError().text();
    return results;
}

HostInfo AppConfig::findHost(int id)
{
    auto sqlQuery = getQuery();
    auto str = QString("select name,host,port,userName,password,sqlType "
                       "from hostInfo where id=%1").arg(id);
    sqlQuery.exec(str);
    if (sqlQuery.next()) {
        return HostInfo{
            id,
            sqlQuery.value(0).toString(),
            sqlQuery.value(1).toString(),
            sqlQuery.value(2).toString(),
            sqlQuery.value(3).toString(),
            decrypt(sqlQuery.value(4).toByteArray()),
            sqlQuery.value(5).toString()
        };
    }
    _strError = sqlQuery.lastError().text();
    return {};
}

bool AppConfig::deleteHost(int id)
{
    auto sqlQuery = getQuery();
    bool ret = sqlQuery.exec(QString("delete from hostInfo where id=%1").arg(QString::number(id)));
    _strError = sqlQuery.lastError().text();
    return ret;
}

QByteArray AppConfig::encrypt(const QString &text)
{
    QByteArray bytes = text.toLocal8Bit();
    bytes = bytes.toBase64();
    for(int i = 0; i < bytes.size(); ++i)
    {
        bytes[i] = bytes[i] + 24;
    }
    return bytes;
}

QString AppConfig::decrypt(const QByteArray &text)
{
    QByteArray bytes = text;
    for(int i = 0; i < bytes.size(); ++i)
    {
        bytes[i] = bytes[i] - 24;
    }
    return  QByteArray::fromBase64(bytes).data();
}

AppConfig::AppConfig()
{

}

AppConfig::~AppConfig()
{

}

QSqlQuery AppConfig::getQuery()
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("app.cfg");
    db.open();
    return QSqlQuery{db};
}