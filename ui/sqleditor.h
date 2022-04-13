#ifndef SQLEDITOR_H
#define SQLEDITOR_H
#include "codeeditor.h"
#include "commenthighlighter.h"
QT_BEGIN_NAMESPACE
class QCompleter;
QT_END_NAMESPACE

class SqlEditor : public CodeEditor
{
    Q_OBJECT
public:
    SqlEditor(QWidget *parent = nullptr);
    ~SqlEditor();

    void initEditorText();
    void appendText(const QString&);
    QString currentSelection();
    QString currentSelectionSqlStatement();
    void addTableNames(const QString& database, QStringList tables);
    void removeTableNames(const QString& database);
    void addTableFields(QStringList tables);

protected:
    void keyPressEvent(QKeyEvent*) override;

private slots:
    void slotSaveEditorText();
    void slotTextChanged();
    void slotCompleterActivated(const QString&);

private:
    QString preprocessingSql();
    QString currentWord();
    void updateCompleterMode(QStringList words = {});

private:
    CommentHighlighter *_highligter;
    QCompleter* _completer;
    std::map<QString, QStringList> _mapTables;
};

#endif // SQLEDITOR_H
