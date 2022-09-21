#ifndef REGEXPCONSTEXPR_H
#define REGEXPCONSTEXPR_H
#include <vector>
#include <map>
#include <QRegExp>
#include <QStringList>

enum Constexpr{
    TABLE_NAME_FIND,
    TABLE_NAME_KEYWORD,
    COMMENT_WITH_BEGIN,
    COMMENT_SPLIT,
    DANGEROUSCOMMAND,
    SQL_DML,
    SELECT_CONTENT,
    DATABASE_OR_TABLE_CREATE
};

class RegExpConstexpr
{
public:
    RegExpConstexpr();
    std::vector<QString> captureTexts(Constexpr type, const QString& str);
    QRegExp findPattern(Constexpr type);
    bool isMatch(Constexpr type, const QString& str);
    QString commentSymbol() { return _commentSybmol; }
    QStringList keywords() {return _keywords; }

private:
    std::map<Constexpr, QRegExp> _mapRegExp;
    QString _commentSybmol;
    QStringList _keywords;
};

static RegExpConstexpr& regExpFactory()
{
    static RegExpConstexpr regExp;
    return  regExp;
}

#endif // REGEXPCONSTEXPR_H
