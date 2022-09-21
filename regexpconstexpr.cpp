#include "regexpconstexpr.h"

RegExpConstexpr::RegExpConstexpr()
{
    _commentSybmol = QString{"#|--"};
    _mapRegExp[TABLE_NAME_FIND] = QRegExp("(into|from|update|join)\\s*((\\w|-|\\.|`|\"|')+)?",
                             Qt::CaseInsensitive);
    _mapRegExp[TABLE_NAME_KEYWORD] = QRegExp("into|from|update|join", Qt::CaseInsensitive);
    _mapRegExp[COMMENT_WITH_BEGIN] = QRegExp(QString("^\\s*(%1)").arg(_commentSybmol));
    _mapRegExp[COMMENT_SPLIT] = QRegExp(QString("%1").arg(_commentSybmol));
    _mapRegExp[DANGEROUSCOMMAND] = QRegExp(QString("(delete|drop|alter)\\s*"), Qt::CaseInsensitive);
    _mapRegExp[SQL_DML] = QRegExp(QString("(SELECT|UPDATE|DELETE|INSERT)\\s*"), Qt::CaseInsensitive);
    _mapRegExp[SELECT_CONTENT] = QRegExp(QString("^\\s*SELECT\\s*(.*)FROM"), Qt::CaseInsensitive);
    _mapRegExp[DATABASE_OR_TABLE_CREATE] = QRegExp(QString("^\\s*CREATE\\s*(DATABASE|TABLE)\\s*(\\w)*\\s*"), Qt::CaseInsensitive);

    _keywords = QStringList{
        QStringLiteral("ADD"),QStringLiteral("ALTER"),QStringLiteral("ALL"),QStringLiteral("AND"),QStringLiteral("ANY"),QStringLiteral("AS"),
        QStringLiteral("ASC"),QStringLiteral("BACKUP"),QStringLiteral("BETWEEN"),QStringLiteral("CASE"),QStringLiteral("CHECK"),
        QStringLiteral("COLUMN"),QStringLiteral("CONSTRAINT"),QStringLiteral("CREATE"),QStringLiteral("DATABASE"),QStringLiteral("DEFAULT"),
        QStringLiteral("DELETE"),QStringLiteral("DESC"),QStringLiteral("DISTINCT"),QStringLiteral("DROP"),QStringLiteral("EXEC"),
        QStringLiteral("EXISTS"),QStringLiteral("FOREIGN"),QStringLiteral("FROM"),QStringLiteral("FULL"),QStringLiteral("GROUP"),
        QStringLiteral("HAVING"),QStringLiteral("IN"),QStringLiteral("INDEX"),QStringLiteral("INNER"),QStringLiteral("INSERT"),
        QStringLiteral("IS"),QStringLiteral("JOIN"),QStringLiteral("LEFT"),QStringLiteral("LIKE"),QStringLiteral("LIMIT"),
        QStringLiteral("NOT"),QStringLiteral("OR"),QStringLiteral("ORDER"),QStringLiteral("OUTER"),QStringLiteral("PRIMARY"),
        QStringLiteral("PROCEDURE"),QStringLiteral("RIGHT"),QStringLiteral("ROWNUM"),QStringLiteral("SELECT"),
        QStringLiteral("SET"),QStringLiteral("TABLE"),QStringLiteral("TOP"),QStringLiteral("TRUNCATE TABLE"),QStringLiteral("UNION"),
        QStringLiteral("UNION ALL"),QStringLiteral("UNIQUE"),QStringLiteral("UPDATE"),QStringLiteral("VALUES"),QStringLiteral("VIEW"),
        QStringLiteral("WHERE"),QStringLiteral("SHOW"),QStringLiteral("KEY"),QStringLiteral("BY"),QStringLiteral("INTO"),
        QStringLiteral("NULL"),QStringLiteral("REPLACE")
    };
}

std::vector<QString> RegExpConstexpr::captureTexts(Constexpr type, const QString& str)
{
    auto regExp = findPattern(type);
    if(regExp == QRegExp{})
    {
        return {};
    }
    int pos = 0;
    std::vector<QString> resultText;
    while ((pos = regExp.indexIn(str, pos)) != -1) {
        resultText.emplace_back(regExp.cap());
        pos += regExp.matchedLength();
    }
    return resultText;
}

QRegExp RegExpConstexpr::findPattern(Constexpr type)
{
    auto iter = _mapRegExp.find(type);
    return iter == std::end(_mapRegExp) ? QRegExp{} : iter->second;
}

bool RegExpConstexpr::isMatch(Constexpr type, const QString &str)
{
    auto iter = _mapRegExp.find(type);
    if(iter != std::end(_mapRegExp))
    {
        return iter->second.indexIn(str) != -1;
    }
    return iter == std::end(_mapRegExp) ? false : iter->second.exactMatch(str);
}
