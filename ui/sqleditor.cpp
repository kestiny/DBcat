#include "sqleditor.h"
#include <QFile>
#include <QTextBlock>
#include <QShortcut>
#include <QPlainTextEdit>
#include "regexpconstexpr.h"
#include "appconfig.h"

SqlEditor::SqlEditor(QWidget *parent)
    : CodeEditor(parent)
{
    _highligter = new CommentHighlighter(document());
    auto shortcut = QShortcut(QKeySequence("Ctrl+S"), this);
    connect(&shortcut, &QShortcut::activated, this, &SqlEditor::slotSaveEditorText);
    connect(this, &SqlEditor::textChanged, this, &SqlEditor::slotTextChanged);
}

SqlEditor::~SqlEditor()
{
    slotSaveEditorText();
}

void SqlEditor::initEditorText()
{
    auto fileName = AppConfig::instance().configFile("content");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    setPlainText(file.readAll());
    file.close();
    moveCursor(QTextCursor::End);
}

void SqlEditor::appendText(const QString &text)
{
    appendPlainText(text);
    moveCursor(QTextCursor::End);
}

QString SqlEditor::currentSelection()
{
    return textCursor().selectedText();
}

QString SqlEditor::currentSelectionSqlStatement()
{
    return preprocessingSql();
}

void SqlEditor::slotSaveEditorText()
{
    QFile file(AppConfig::instance().configFile("content"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return;

    file.write(toPlainText().toUtf8().data());
    file.close();
}

void SqlEditor::slotTextChanged()
{

}

QString SqlEditor::preprocessingSql()
{
    QString selectionText = textCursor().selectedText();
    QString result;
    for(QString str : selectionText.split(QChar::ParagraphSeparator, QString::SkipEmptyParts))
    {
        str = str.trimmed();
        if(str.isEmpty() || regExpFactory().isMatch(COMMENT_WITH_BEGIN, str))
        {
            continue;
        }

        result.append(str.split(regExpFactory().findPattern(COMMENT_SPLIT)).first())
                .append(" ");
    }
    return result;
}
