#include "syntaxparser.h"
#include "regexpconstexpr.h"
#include <QStringList>

SyntaxParser::SyntaxParser()
{

}

void SyntaxParser::analysing()
{
    // analyse table name
    QString result;
    for(auto text : regExpFactory().captureTexts(TABLE_NAME_FIND, _originalInput)) {
        text.replace(regExpFactory().findPattern(TABLE_NAME_KEYWORD), "");
        text.replace("`|\"|'", "");
        text.replace("^\\s*\\[(.+)]$", "$1");
        text = text.trimmed();
        if(text.isEmpty())
        {
            continue;
        }
        result.append(text).append(":");
    }
    _tableName = result.left(result.length() - 1);

    // sql DML
    auto texts = regExpFactory().captureTexts(SQL_DML, _originalInput);
    _sqlDML = texts.size() > 0 ? texts.at(0) : "";
    _sqlDML = _sqlDML.trimmed();

    // is single query
    if(_sqlDML.toUpper().compare("SELECT") == 0)
    {
        texts = regExpFactory().captureTexts(SELECT_CONTENT, _originalInput);
        if(texts.size() > 0)
        {
            QString text = texts.front();
            text.replace("SELECT", "", Qt::CaseInsensitive);
            text.replace("FROM", "", Qt::CaseInsensitive);
            text.replace("`|\"|'", "");
            text = text.trimmed();
        }
        _singleQuery = false;
    }
}

void SyntaxParser::parse(const QString &sql)
{
    if(sql.isEmpty())
    {
        return;
    }

    _originalInput = sql;
    analysing();
}
