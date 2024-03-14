#ifndef APPCONFIG_H
#define APPCONFIG_H
#include <list>
#include <QSqlDatabase>
#include <QFont>
#include "dbtype.h"

class AppConfig
{
public:
    static AppConfig &instance(){
        static AppConfig appConfig;
        return appConfig;
    }

    bool addHost(HostInfo);
    bool updateHost(HostInfo);
    std::list<HostInfo> listHosts();
    HostInfo findHost(QString id);
    bool deleteHost(QString id);
    QString lastError() { return _strError; }
    QByteArray encrypt(const QString& text);
    QString decrypt(const QByteArray& text);
    QString configFile(const QString &name);
    QString documentFile(const QString &name);

    QFont iconFont(int size = 12);
private:
    AppConfig();
    ~AppConfig();
    QSqlQuery getQuery();
    void initDB();

private:
    QString _strError;
    QString _configDir;
    QString _docDir;
    QFont _iconFont;
};

#endif // APPCONFIG_H
