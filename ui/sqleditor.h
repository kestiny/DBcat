#ifndef SQLEDITOR_H
#define SQLEDITOR_H
#include <set>
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
    void addTableFields(QStringList fields);

protected:
    void keyPressEvent(QKeyEvent*) override;

signals:
    void signalNewTables(QStringList);

private slots:
    void slotSaveEditorText();
    void slotTextChanged();
    void slotCompleterActivated(const QString&);

private:
    QString preprocessingSql();
    QString currentWord();
    void findTableNames();
    void updateCompleterMode(QStringList words = {});

private:
    CommentHighlighter *_highligter;
    QCompleter* _completer;
    std::map<QString, QStringList> _mapTables;
    QStringList _lastTableNames;
};

#endif // SQLEDITOR_H
