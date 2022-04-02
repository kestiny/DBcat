#ifndef APPCONFIG_H
#define APPCONFIG_H
#include <list>
#include <QSqlDatabase>
#include "dbtype.h"

class AppConfig
{
public:
    static AppConfig &instance(){
        static AppConfig appConfig;
        return appConfig;
    }

    bool addHost(HostInfo);
    std::list<HostInfo> listHosts();
    HostInfo findHost(int id);
    bool deleteHost(int id);
    QString lastError() { return _strError; }
    QByteArray encrypt(const QString& text);
    QString decrypt(const QByteArray& text);

private:
    AppConfig();
    ~AppConfig();
    QSqlQuery getQuery();

private:
    QString _strError;
};

#endif // APPCONFIG_H
