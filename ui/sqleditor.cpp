#include "sqleditor.h"
#include <QFile>
#include <QTextBlock>
#include <QShortcut>
#include <QPlainTextEdit>
#include <QCompleter>
#include <QAbstractItemView>
#include <QStringListModel>
#include "regexpconstexpr.h"
#include "appconfig.h"
#include "operation/syntaxparser.h"

SqlEditor::SqlEditor(QWidget *parent)
    : CodeEditor(parent)
    , _completer{nullptr}
    , _lastTableNames{}
{
    _highligter = new CommentHighlighter(document());
    auto shortcut = QShortcut(QKeySequence("Ctrl+S"), this);

    // syntax complete
    _completer = new QCompleter(regExpFactory().keywords(), this);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);
    _completer->setWidget(this);
    connect(_completer, SIGNAL(activated(const QString&)), this, SLOT(slotCompleterActivated(const QString&)));

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

void SqlEditor::addTableNames(const QString &database, QStringList tables)
{
    if(!database.isEmpty() && tables.size() > 0)
    {
        _mapTables[database] = tables;
        updateCompleterMode();
    }
}

void SqlEditor::removeTableNames(const QString &database)
{
    auto iter = _mapTables.find(database);
    if(iter != std::end(_mapTables))
    {
        _mapTables.erase(iter);
        updateCompleterMode();
    }
}

void SqlEditor::addTableFields(QStringList fields)
{
    updateCompleterMode(fields);
}

void SqlEditor::keyPressEvent(QKeyEvent *e)
{
    auto view = _completer->popup();
    if (view && view->isVisible()) {
        switch(e->key()) {
        case Qt::Key_Escape:
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Tab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    QString word = currentWord();
    findTableNames();
    if(_completer && !word.isEmpty())
    {
        _completer->setCompletionPrefix(word);
        _completer->complete();
    }
    QRect rc = cursorRect();
    _completer->complete(QRect{rc.x() + 20, rc.y() + 6, 300, rc.height()});
    CodeEditor::keyPressEvent(e);
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

void SqlEditor::slotCompleterActivated(const QString &text)
{
    QString completionPrefix = currentWord();
    QString shouldReplaceText = text;
    QTextCursor cursor = textCursor();
    if (!shouldReplaceText.contains(completionPrefix))
    {
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, completionPrefix.size());
        cursor.removeSelectedText();
    }
    else
    {
        shouldReplaceText = shouldReplaceText.replace(
                    shouldReplaceText.indexOf(completionPrefix), completionPrefix.size(), "");
    }
    cursor.insertText(shouldReplaceText);
}

QString SqlEditor::preprocessingSql()
{
    auto selectionText = textCursor().selectedText();
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

QString SqlEditor::currentWord()
{
    auto cursor = textCursor();
    while (cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor)) {
        QString text = cursor.selectedText();
        if (text.contains(" ") || cursor.atBlockStart()) {
            break;
        }
    }
    return cursor.selectedText().simplified();
}

void SqlEditor::findTableNames()
{
    // find tabel: (curLine-5, curLine+5)
    auto cursor = textCursor();
    cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor, 5);
    QString selectedText = cursor.selectedText();

    cursor = textCursor();
    cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 5);
    selectedText.append(" ").append(cursor.selectedText());
    selectedText = selectedText.simplified();

    if(!selectedText.isEmpty())
    {
        SyntaxParser parser;
        parser.parse(selectedText);
        auto tables = parser.tableName().split(":");
        tables.removeDuplicates();
        bool hasNewTable = false;
        for(auto table : tables)
        {
            if(!_lastTableNames.contains(table))
            {
                hasNewTable = true;
                break;
            }
        }
        if(hasNewTable)
        {
            _lastTableNames = tables;
            emit signalNewTables(_lastTableNames);
        }
    }
}

void SqlEditor::updateCompleterMode(QStringList words)
{
    QStringList allWords = regExpFactory().keywords();
    for (auto& [key, value] : _mapTables) {
        allWords.append(value);
    }

    if(words.size() > 0)
    {
        allWords.append(words);
    }

    _completer->setModel(new QStringListModel{allWords});
}
