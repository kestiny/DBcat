#ifndef DBCAT_H
#define DBCAT_H
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class DBcat; }
class QTreeWidgetItem;
QT_END_NAMESPACE

class DBcat : public QMainWindow
{
    Q_OBJECT

public:
    DBcat(QWidget *parent = nullptr);
    ~DBcat();

    void initWidgetSize();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    void searchItems(const QString& text);
    void preSearchItem();
    void nextSearchItem();
    void clipboardCopyData();
    void initMenuBar();

private slots:
    void slotLoadStyleSheet(const QString& file);
    void slotExec();
    void slotSearchText(const QString &text);
    void slotNewTables(QStringList);
    void slotExit();

private:
    Ui::DBcat *ui;
    QList<QTreeWidgetItem*> _lastSearchItems;
    int _searchIndex;
    QAction *_copyHeader;
};
#endif // DBCAT_H
