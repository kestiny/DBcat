#include "appconfig.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QFontDatabase>

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

QString AppConfig::configFile(const QString &name)
{
    return QString("%1/%2").arg(_configDir, name);
}

QString AppConfig::documentFile(const QString &name)
{
    return QString("%1/%2").arg(_docDir, name);
}

QFont AppConfig::iconFont(int size)
{
    _iconFont.setPointSize(size);
    return _iconFont;
}

AppConfig::AppConfig()
    : _configDir{QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).append("/DBcat")}
    , _docDir{QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).append("/DBcat")}
{
    int fontId = QFontDatabase::addApplicationFont(":/image/fontawesome-webfont.ttf");
    _iconFont.setFamily(QFontDatabase::applicationFontFamilies(fontId).front());

    QDir dir;
    if(!dir.exists(_configDir))
    {
        dir.mkpath(_configDir);
    }
    if(!dir.exists(_docDir))
    {
        dir.mkpath(_docDir);
    }
}

AppConfig::~AppConfig()
{

}

QSqlQuery AppConfig::getQuery()
{
    if(!QSqlDatabase::contains(configFile("app.cfg")))
    {
        initDB();
    }
    return QSqlQuery{QSqlDatabase::database(configFile("app.cfg"))};
}

void AppConfig::initDB()
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(configFile("app.cfg"));
    db.open();
    auto result =  QSqlQuery{db};
    result.exec("CREATE TABLE IF NOT EXISTS hostInfo(id integer primary key autoincrement,"
                "name varchar(255),host varchar(64),port varchar(12),"
                "userName varchar(255),password varchar(256),sqlType varchar(12))");
}
