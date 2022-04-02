#include "commenthighlighter.h"
#include "regexpconstexpr.h"

CommentHighlighter::CommentHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    commentFormat.setForeground(Qt::gray);
    commentExpression = QRegularExpression(QStringLiteral("(%1)[^\n]*")
                                           .arg(regExpFactory().commentSymbol()));

    syntaxFormat.setForeground(Qt::blue);
    syntaxFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
            QStringLiteral("\\bADD\\b"),
            QStringLiteral("\\bADD\\s+CONSTRAINT\\b"),
            QStringLiteral("\\bALTER\\b"),
            QStringLiteral("\\bALTER\\s+COLUMN\\b"),
            QStringLiteral("\\bALTER\\s+TABLE\\b"),
            QStringLiteral("\\bALL\\b"),
            QStringLiteral("\\bAND\\b"),
            QStringLiteral("\\bANY\\b"),
            QStringLiteral("\\bAS\\b"),
            QStringLiteral("\\bASC\\b"),
            QStringLiteral("\\bBACKUP\\s+DATABASE\\b"),
            QStringLiteral("\\bBETWEEN\\b"),
            QStringLiteral("\\bCASE\\b"),
            QStringLiteral("\\bCHECK\\b"),
            QStringLiteral("\\bCOLUMN\\b"),
            QStringLiteral("\\bCONSTRAINT\\b"),
            QStringLiteral("\\bCREATE\\b"),
            QStringLiteral("\\bCREATE\\s+DATABASE\\b"),
            QStringLiteral("\\bCREATE\\s+INDEX\\b"),
            QStringLiteral("\\bCREATE\\s+OR\\s+REPLACE\\s+VIEW\\b"),
            QStringLiteral("\\bCREATE\\s+TABLE\\b"),
            QStringLiteral("\\bCREATE\\s+PROCEDURE\\b"),
            QStringLiteral("\\bCREATE\\s+UNIQUE\\s+INDEX\\b"),
            QStringLiteral("\\bCREATE\\s+VIEW\\b"),
            QStringLiteral("\\bDATABASE\\b"),
            QStringLiteral("\\bDEFAULT\\b"),
            QStringLiteral("\\bDELETE\\b"),
            QStringLiteral("\\bDESC\\b"),
            QStringLiteral("\\bDISTINCT\\b"),
            QStringLiteral("\\bDROP\\b"),
            QStringLiteral("\\bDROP\\s+COLUMN\\b"),
            QStringLiteral("\\bDROP\\s+CONSTRAINT\\b"),
            QStringLiteral("\\bDROP\\s+DATABASE\\b"),
            QStringLiteral("\\bDROP\\s+DEFAULT\\b"),
            QStringLiteral("\\bDROP\\s+INDEX\\b"),
            QStringLiteral("\\bDROP\\s+TABLE\\b"),
            QStringLiteral("\\bDROP\\s+VIEW\\b"),
            QStringLiteral("\\bEXEC\\b"),
            QStringLiteral("\\bEXISTS\\b"),
            QStringLiteral("\\bFOREIGN\\s+KEY\\b"),
            QStringLiteral("\\bFROM\\b"),
            QStringLiteral("\\bFULL\\s+OUTER\\s+JOIN\\b"),
            QStringLiteral("\\bGROUP\\s+BY\\b"),
            QStringLiteral("\\bHAVING\\b"),
            QStringLiteral("\\bIN\\b"),
            QStringLiteral("\\bINDEX\\b"),
            QStringLiteral("\\bINNER\\s+JOIN\\b"),
            QStringLiteral("\\bINSERT\\s+INTO\\b"),
            QStringLiteral("\\bINSERT\\s+INTO\\s+SELECT\\b"),
            QStringLiteral("\\bIS\\s+NULL\\b"),
            QStringLiteral("\\bIS\\s+NOT\\s+NULL\\b"),
            QStringLiteral("\\bJOIN\\b"),
            QStringLiteral("\\bLEFT\\s+JOIN\\b"),
            QStringLiteral("\\bLIKE\\b"),
            QStringLiteral("\\bLIMIT\\b"),
            QStringLiteral("\\bNOT\\b"),
            QStringLiteral("\\bNOT\\s+NULL\\b"),
            QStringLiteral("\\bOR\\b"),
            QStringLiteral("\\bORDER\\s+BY\\b"),
            QStringLiteral("\\bOUTER\\s+JOIN\\b"),
            QStringLiteral("\\bPRIMARY\\s+KEY\\b"),
            QStringLiteral("\\bPROCEDURE\\b"),
            QStringLiteral("\\bRIGHT\\s+JOIN\\b"),
            QStringLiteral("\\bROWNUM\\b"),
            QStringLiteral("\\bSELECT\\b"),
            QStringLiteral("\\bSELECT\\s+DISTINCT\\b"),
            QStringLiteral("\\bSELECT\\s+INTO\\b"),
            QStringLiteral("\\bSELECT\\s+TOP\\b"),
            QStringLiteral("\\bSET\\b"),
            QStringLiteral("\\bTABLE\\b"),
            QStringLiteral("\\bTOP\\b"),
            QStringLiteral("\\bTRUNCATE TABLE\\b"),
            QStringLiteral("\\bUNION\\b"),
            QStringLiteral("\\bUNION ALL\\b"),
            QStringLiteral("\\bUNIQUE\\b"),
            QStringLiteral("\\bUPDATE\\b"),
            QStringLiteral("\\bVALUES\\b"),
            QStringLiteral("\\bVIEW\\b"),
            QStringLiteral("\\bWHERE\\b")
        };
    for (const QString &pattern : keywordPatterns) {
        HighlightRule rule;
        rule.exp = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = syntaxFormat;
        _highlightingRules.emplace_back(rule);
    }
}

void CommentHighlighter::highlightBlock(const QString &text)
{
    for(auto& value : _highlightingRules)
    {
        auto matchIterator = value.exp.globalMatch(text);
        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), value.format);
        }
    }

    auto matchIterator = commentExpression.globalMatch(text);
    while (matchIterator.hasNext()) {
        auto match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
    }
}
