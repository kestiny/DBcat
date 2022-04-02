#ifndef SQLEDITOR_H
#define SQLEDITOR_H
#include "codeeditor.h"
#include "commenthighlighter.h"

class SqlEditor : public CodeEditor
{
    Q_OBJECT
public:
    SqlEditor(QWidget *parent = nullptr);
    ~SqlEditor();

    void initEditorText(const QString& fileName);
    void appendText(const QString& text);
    QString currentSelection();
    QString currentSelectionSqlStatement();

private:
    void slotSaveEditorText();
    void slotTextChanged();

private:
    QString preprocessingSql();

private:
    QString _strFileName;
    CommentHighlighter *_highligter;
};

#endif // SQLEDITOR_H
