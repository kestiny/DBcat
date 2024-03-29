#ifndef HOSTTREEWIDGET_H
#define HOSTTREEWIDGET_H

#include <QTreeWidget>
#include <QMenu>
#include "dbtype.h"

QT_BEGIN_NAMESPACE
class SqlResultControll;
class IDBOperator;
class SqlEditor;
QT_END_NAMESPACE

class HostTreeWidget : public QTreeWidget
{
    Q_OBJECT

    enum NodeType
    {
        HOST,
        DATABASE,
        TABLE,
        TABLE_CONTAINER,
        NIL
    };

public:
    HostTreeWidget(QWidget *parent = nullptr);
    void initDatabseOperator(IDBOperator *op,
                             SqlResultControll *resultControll,
                             SqlEditor *sqlEditor);
    QString currentHostId(){ return _currentHostId; }
    Database currentDatabase() { return _currentDatabase; }
    void addItemCommand(const QString& name);
    void addFileMenuAction(QMenu *meun);

private slots:
   void showContextMenu(const QPoint &);
   void slotDoubleClicked(QTreeWidgetItem *, int);
   void slotItemChanged(QTreeWidgetItem *currItem, QTreeWidgetItem*);

   // connection operator
   void slotOpenCat(bool);
   void slotCloseCat(bool);
   void slotEditCat(bool);
   void slotNewCat(bool);
   void slotDelCat(bool);

   // database operator
   void slotOpenDB(bool);
   void slotCloseDB(bool);
   void slotCreateDB(bool);
   void slotDropDB(bool);

   // table operator
   void slotCreateTable(bool);
   void slotDropTable(bool);
   void slotDeleteTable(bool);

private:
   void initHostTree();
   void updateHostTree();
   QTreeWidgetItem *createItem(HostInfo info);
   QString getItemHostId(QTreeWidgetItem *pItem);
   void setItemHostId(QTreeWidgetItem *pItem, QString id);
   Database getItemDatabaseInfo(QTreeWidgetItem *pItem);
   void setItemDatabaseInfo(QTreeWidgetItem *pItem, Database info);
   void setItemType(QTreeWidgetItem *pItem, NodeType type);
   int getItemType(QTreeWidgetItem *pItem);
   void initTableTree(QTreeWidgetItem *pItem, QString id, const Database& database, bool expand = false);
   void setItemIcon(QTreeWidgetItem *pItem, NodeType type, const QString& name);
   void closeHost(QTreeWidgetItem *pItem);

private:
   QMenu *_contextMenu;
   QAction *_actOpenCat;
   QAction *_actCloseCat;
   QAction *_actEditCat;
   QAction *_actNewCat;
   QAction *_actDelCat;

   QAction *_actOpenDB;
   QAction *_actCloseDB;
   QAction *_actCreateDB;
   QAction *_actDropDB;

   QAction *_actCreateTable;
   QAction *_actDropTable;
   QAction *_actDeleteTable;

   IDBOperator *_dbOperator;
   SqlResultControll *_resultControll;
   SqlEditor *_sqlEditor;
   QString _currentHostId;
   Database _currentDatabase;
};

#endif // HOSTTREEWIDGET_H
