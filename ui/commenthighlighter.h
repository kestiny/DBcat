#ifndef COMMENTHIGHLIGHTER
#define COMMENTHIGHLIGHTER
#include <vector>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextCharFormat;
QT_END_NAMESPACE

class CommentHighlighter : public QSyntaxHighlighter
{
public:
    CommentHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule
    {
        QRegularExpression exp;
        QTextCharFormat format;
    };
    std::vector<HighlightRule> _highlightingRules;
    QRegularExpression commentExpression;
    QTextCharFormat syntaxFormat;
    QTextCharFormat commentFormat;
};

#endif // COMMENTHIGHLIGHTER
