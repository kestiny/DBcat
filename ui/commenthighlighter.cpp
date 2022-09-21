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

    for (auto &pattern : regExpFactory().keywords()) {
        HighlightRule rule;
        rule.exp = QRegularExpression(QString("\\b%1\\b").arg(pattern), QRegularExpression::CaseInsensitiveOption);
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
