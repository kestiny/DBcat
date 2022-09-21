#ifndef DBTYPE_H
#define DBTYPE_H
#include <QString>
/**
 * @brief The sqlType will support
 * DB2--->IBM DB2 (version 7.1 and above)
 * IBASE--->Borland InterBase
 * MYSQL--->MySQL (version 5.0 and above)
 * OCI--->Oracle Call Interface Driver
 * ODBC--->Open Database Connectivity (ODBC) - Microsoft SQL Server and other ODBC-compliant databases
 * PSQL--->PostgreSQL (versions 7.3 and above)
 * SQLITE2--->SQLite version 2
 * SQLITE--->SQLite version 3
 * TDS--->Sybase Adaptive Server
 *
 * version 0.1.0 only for QMYSQL
 */
struct HostInfo
{
    QString id;
    QString name;
    QString host;
    QString port;
    QString userName;
    QString password;
    QString sqlType;
};

struct Database
{
    QString name;
};

struct Table
{
    QString name;
};

#endif // DBTYPE_H
