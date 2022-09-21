#ifndef SYNTAXPARSER_H
#define SYNTAXPARSER_H
#include <QString>

class SyntaxParser
{
public:
    SyntaxParser();
    void parse(const QString& sql);
    QString tableName() { return _tableName; }
    QString sqlCommand() { return _sqlDML.toUpper(); }
    bool isSingleQuery() { return _singleQuery; }

private:
    void analysing();

private:
    QString _originalInput;
    QString _tableName;
    QString _sqlDML;
    bool _singleQuery;
};

#endif // SYNTAXPARSER_H
