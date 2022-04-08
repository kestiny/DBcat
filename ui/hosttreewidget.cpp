#include "hosttreewidget.h"
#include "appconfig.h"
#include "newhost.h"
#include "operation/idboperator.h"
#include "sqlresultcontroll.h"
#include <QSqlQuery>
#include "messagebox.h"

HostTreeWidget::HostTreeWidget(QWidget *parent/* = nullptr*/)
    :QTreeWidget{parent}
    , _currentHostId{-1}
    , _currentDatabase{""}
{
    setIndentation(indentation() / 2);
    setExpandsOnDoubleClick(true);
    setColumnCount(1);

    initHostTree();

    // enable right click menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &HostTreeWidget::customContextMenuRequested,
            this, &HostTreeWidget::showContextMenu);

    _contextMenu = new QMenu();
    _actOpenCat = _contextMenu->addAction(QIcon(":/image/default/server.svg"), tr("open Connection"));
    _actCloseCat = _contextMenu->addAction(QIcon(":/image/default/server.svg"), tr("close Connection"));
    _actNewCat = _contextMenu->addAction(QIcon(":/image/default/server.svg"), tr("new Connection"));
    _actDelCat = _contextMenu->addAction(QIcon(":/image/default/server.svg"), tr("del Connection"));

    _contextMenu->addSeparator();
    _actOpenDB = _contextMenu->addAction(QIcon(":/image/default/database_open.png"), tr("open Database"));
    _actCloseDB = _contextMenu->addAction(QIcon(":/image/default/database_open.png"), tr("close Database"));
    _actCreateDB = _contextMenu->addAction(QIcon(":/image/default/database_open.png"), tr("create Database"));
    _actDropDB = _contextMenu->addAction(QIcon(":/image/default/database_open.png"), tr("drop Database"));

    _contextMenu->addSeparator();
    _actCreateTable = _contextMenu->addAction(QIcon(":/image/default/query.svg"), tr("create table"));
    _actDropTable = _contextMenu->addAction(QIcon(":/image/default/query.svg"), tr("drop table"));
    _actDeleteTable = _contextMenu->addAction(QIcon(":/image/default/query.svg"), tr("delete table"));

    connect(this, &QTreeWidget::itemDoubleClicked, this, &HostTreeWidget::slotDoubleClicked);
    connect(this, &QTreeWidget::itemChanged, this, &HostTreeWidget::slotItemChanged);

    connect(_actOpenCat, &QAction::triggered, this, &HostTreeWidget::slotOpenCat);
    connect(_actCloseCat, &QAction::triggered, this, &HostTreeWidget::slotCloseCat);
    connect(_actNewCat, &QAction::triggered, this, &HostTreeWidget::slotNewCat);
    connect(_actDelCat, &QAction::triggered, this, &HostTreeWidget::slotDelCat);

    connect(_actOpenDB, &QAction::triggered, this, &HostTreeWidget::slotOpenDB);
    connect(_actCloseDB, &QAction::triggered, this, &HostTreeWidget::slotCloseDB);
    connect(_actCreateDB, &QAction::triggered, this, &HostTreeWidget::slotCreateDB);
    connect(_actDropDB, &QAction::triggered, this, &HostTreeWidget::slotDropDB);

    connect(_actCreateTable, &QAction::triggered, this, &HostTreeWidget::slotCreateTable);
    connect(_actDropTable, &QAction::triggered, this, &HostTreeWidget::slotDropTable);
    connect(_actDeleteTable, &QAction::triggered, this, &HostTreeWidget::slotDeleteTable);
}

void HostTreeWidget::initDatabseOperator(IDBOperator *op, SqlResultControll *resultControll, SqlEditor *sqlEditor)
{
    _dbOperator = op;
    _resultControll = resultControll;
    _sqlEditor = sqlEditor;
}

void HostTreeWidget::addItemCommand(const QString &name)
{
    auto sqlCmd = name.toLower();
    if(sqlCmd.indexOf("database") != -1)
    {
        for(int i = 0; i < topLevelItemCount(); ++i)
        {
            auto pItem = topLevelItem(i);
            if(_currentHostId == getItemHostId(pItem))
            {
                auto cmdName = name.toLower();
                cmdName.replace("create", "");
                cmdName.replace("database", "");
                cmdName = cmdName.trimmed();
                auto item = new QTreeWidgetItem();
                item->setText(0, cmdName);
                setItemDatabaseInfo(item, {cmdName});
                setItemHostId(item, _currentHostId);
                setItemType(item, NodeType::DATABASE);
                setItemIcon(item, NodeType::DATABASE, "close");
                pItem->addChild(item);
                return;
            }
        }
    }
    else if(sqlCmd.indexOf("table") != -1)
    {
        for(int i = 0; i < topLevelItemCount(); ++i)
        {
            auto pItem = topLevelItem(i);
            if(_currentHostId == getItemHostId(pItem))
            {
                int count = pItem->childCount();
                for(int i = 0; i < count; ++i)
                {
                    auto cItem = pItem->child(i);
                    if(_currentDatabase.name.compare(getItemDatabaseInfo(cItem).name) == 0)
                    {
                        cItem->takeChildren();
                        // init table
                        initTableTree(cItem, _currentHostId, _currentDatabase, true);
                        setItemIcon(pItem, NodeType::DATABASE, "open");
                        cItem->setExpanded(true);
                        return;
                    }
                }
            }
        }
    }
}

void HostTreeWidget::showContextMenu(const QPoint &pos)
{
    auto item = itemAt(pos);
    if(item != nullptr)
    {
        NodeType nodeType = (NodeType)getItemType(item);
        switch (nodeType) {
        case NodeType::HOST:
        {
            _actOpenCat->setVisible(true);
            _actCloseCat->setVisible(true);
            _actNewCat->setVisible(true);
            _actDelCat->setVisible(true);
            _actOpenCat->setDisabled(false);
            _actCloseCat->setDisabled(false);
            _actNewCat->setDisabled(false);
            _actDelCat->setDisabled(false);

            _actOpenDB->setDisabled(true);
            _actCloseDB->setDisabled(true);
            _actCreateDB->setDisabled(true);
            _actDropDB->setDisabled(true);

            _actCreateTable->setVisible(false);
            _actDropTable->setVisible(false);
            _actDeleteTable->setVisible(false);
        }
            break;
        case NodeType::DATABASE:
        {
            _actOpenCat->setDisabled(true);
            _actCloseCat->setDisabled(true);
            _actNewCat->setVisible(true);
            _actNewCat->setDisabled(false);
            _actDelCat->setDisabled(true);

            _actOpenDB->setVisible(true);
            _actCloseDB->setVisible(true);
            _actCreateDB->setVisible(true);
            _actDropDB->setVisible(true);
            _actOpenDB->setDisabled(false);
            _actCloseDB->setDisabled(false);
            _actCreateDB->setDisabled(false);
            _actDropDB->setDisabled(false);

            _actCreateTable->setVisible(false);
            _actDropTable->setVisible(false);
            _actDeleteTable->setVisible(false);
        }
            break;
        case NodeType::TABLE:
        case NodeType::TABLE_CONTAINER:
        {
            _actOpenCat->setDisabled(true);
            _actCloseCat->setDisabled(true);
            _actNewCat->setVisible(true);
            _actDelCat->setDisabled(true);

            _actOpenDB->setDisabled(true);
            _actCloseDB->setDisabled(true);
            _actCreateDB->setDisabled(true);
            _actDropDB->setDisabled(true);

            _actCreateTable->setVisible(true);
            _actDropTable->setVisible(true);
            _actDeleteTable->setVisible(true);
            _actCreateTable->setDisabled(false);
            _actDropTable->setDisabled(false);
            _actDeleteTable->setDisabled(false);
        }
            break;
        case NodeType::NIL:
            return;
        }
    }
    else
    {
        _actOpenCat->setDisabled(true);
        _actCloseCat->setDisabled(true);
        _actNewCat->setVisible(true);
        _actNewCat->setDisabled(false);
        _actDelCat->setDisabled(true);

        _actOpenDB->setDisabled(true);
        _actCloseDB->setDisabled(true);
        _actCreateDB->setDisabled(true);
        _actDropDB->setDisabled(true);

        _actCreateTable->setVisible(false);
        _actDropTable->setVisible(false);
        _actDeleteTable->setVisible(false);
    }
    _contextMenu->exec(QCursor::pos());
}

void HostTreeWidget::slotDoubleClicked(QTreeWidgetItem *item, int)
{
    if(item == nullptr) return;
    NodeType nodeType = (NodeType)getItemType(item);
    switch (nodeType) {
    case NodeType::HOST:
        slotOpenCat(true);
        break;
    case NodeType::DATABASE:
        slotOpenDB(true);
        break;
    case NodeType::TABLE:
    {
        auto tableName = item->text(0);
        _resultControll->setMessage(tr("[OPEN TABLE] %1").arg(tableName));
        _resultControll->setTableName(getItemHostId(item->parent()), getItemDatabaseInfo(item->parent()), tableName);
    }
        break;
    case NodeType::TABLE_CONTAINER:
    case NodeType::NIL:
        break;
    }
}

void HostTreeWidget::slotItemChanged(QTreeWidgetItem *item, int)
{
    if(item == nullptr) return;
    NodeType nodeType = (NodeType)getItemType(item);
    switch (nodeType) {
    case NodeType::HOST:
        _currentHostId = getItemHostId(item);
        break;
    case NodeType::DATABASE:
    {
        _currentHostId = getItemHostId(item->parent());
        _currentDatabase = getItemDatabaseInfo(item);
    }
        break;
    case NodeType::TABLE:
        case NodeType::TABLE_CONTAINER:
    case NodeType::NIL:
        break;
    }
}

void HostTreeWidget::slotOpenCat(bool)
{
    auto pItem = currentItem();
    if (pItem && getItemType(pItem) == NodeType::HOST && pItem->childCount() == 0)
    {
        _currentHostId = getItemHostId(pItem);
        auto databases = _dbOperator->database(_currentHostId);
        if(databases.size() == 0)
        {
            auto strError = _dbOperator->lastError();
            if(!strError.isEmpty())
            {
                _resultControll->setMessage(strError);
            }
            return;
        }

        for(auto info : databases) {
            auto item = new QTreeWidgetItem();
            item->setText(0, info.name);
            setItemDatabaseInfo(item, info);
            setItemHostId(item, _currentHostId);
            setItemType(item, NodeType::DATABASE);
            setItemIcon(item, NodeType::DATABASE, "close");
            pItem->addChild(item);
        }
        _resultControll->setMessage(tr("connect server:%1").arg(pItem->text(0)));
    }
}

void HostTreeWidget::slotCloseCat(bool)
{
    closeHost(currentItem());
}

void HostTreeWidget::slotNewCat(bool)
{
    NewHost* newHost = new NewHost(this);
    if(QDialog::Accepted == newHost->exec())
    {
        updateHostTree();
    }
}

void HostTreeWidget::slotDelCat(bool)
{
   if(AppConfig::instance().deleteHost(getItemHostId(currentItem())))
   {
       updateHostTree();
       return;
   }

   _resultControll->setMessage(tr("[DELETE HOST] %1").arg(AppConfig::instance().lastError()));
}

void HostTreeWidget::slotOpenDB(bool)
{
    auto pItem = currentItem();
    if (pItem && getItemType(pItem) == NodeType::DATABASE && pItem->childCount() == 0)
    {
        _currentHostId = getItemHostId(pItem);
        _currentDatabase = getItemDatabaseInfo(pItem);
        // init table
        initTableTree(pItem, _currentHostId, _currentDatabase);
        setItemIcon(pItem, NodeType::DATABASE, "open");
        _resultControll->setMessage(_dbOperator->lastExecMsg());
    }
}

void HostTreeWidget::slotCloseDB(bool)
{
    auto pItem = currentItem();
    if (pItem && getItemType(pItem) == NodeType::DATABASE)
    {
        auto items = pItem->takeChildren();
        //delete[] items;
        setItemIcon(pItem, NodeType::DATABASE, "close");
    }
}

void HostTreeWidget::slotCreateDB(bool)
{
    _sqlEditor->appendText("\r\nCREATE DATABASE [name] DEFAULT CHARACTER SET = 'utf8mb4'");
}

void HostTreeWidget::slotDropDB(bool)
{
    auto pItem = currentItem();
    if (pItem && getItemType(pItem) == NodeType::DATABASE)
    {
        if(QDialog::Rejected == MessageBox::messageWarning(this, tr("warning"), tr("confirm drop database:").append(pItem->text(0))))
        {
            return;
        }
        if(_dbOperator->getQuery(currentHostId(), currentDatabase())
                ->exec(QString("DROP DATABASE %1").arg(pItem->text(0))))
        {
            pItem->takeChildren();
            pItem->parent()->removeChild(pItem);
        }
        else
        {
            _resultControll->setMessage(tr("[DROP DATABASE] %1").arg(_dbOperator->lastError()));
        }
    }
}

void HostTreeWidget::slotCreateTable(bool)
{
    _sqlEditor->appendText("\n#CREATE TABLE Template\n"
                "CREATE TABLE name(\n"
                    "id int NOT NULL PRIMARY KEY AUTO_INCREMENT COMMENT 'Primary Key',\n"
                    "create_time DATETIME COMMENT 'Create Time',\n"
                    "column VARCHAR(255) COMMENT ''\n"
                ") DEFAULT CHARSET UTF8 COMMENT ''");
}

void HostTreeWidget::slotDropTable(bool)
{
    auto pItem = currentItem();
    if (pItem && getItemType(pItem) == NodeType::TABLE)
    {
        if(QDialog::Rejected == MessageBox::messageWarning(this, tr("warning"), tr("confirm drop table:").append(pItem->text(0))))
        {
            return;
        }
        if(_dbOperator->getQuery(currentHostId(), currentDatabase())
                ->exec(QString("DROP TABLE %1").arg(pItem->text(0))))
        {
            pItem->parent()->removeChild(pItem);
        }
        else
        {
            _resultControll->setMessage(tr("[DROP TABLE] %1").arg(_dbOperator->lastError()));
        }
    }
}

void HostTreeWidget::slotDeleteTable(bool)
{
    auto pItem = currentItem();
    if (pItem && getItemType(pItem) == NodeType::TABLE)
    {
        if(QDialog::Rejected == MessageBox::messageWarning(this, tr("warning"), tr("confirm delete table's all data:").append(pItem->text(0))))
        {
            return;
        }
        if(!_dbOperator->getQuery(currentHostId(), currentDatabase())
                ->exec(QString("DELETE FROM %1").arg(pItem->text(0))))
        {
            _resultControll->setMessage(tr("[DELETE TABLE] %1").arg(_dbOperator->lastError()));
        }
    }
}

void HostTreeWidget::initHostTree()
{
    auto hostInfos = AppConfig::instance().listHosts();
    for(auto info : hostInfos) {
        addTopLevelItem(createItem(info));
    }
}

void HostTreeWidget::updateHostTree()
{
    std::map<int, QTreeWidgetItem*> indexs;
    for(int i = 0; i < topLevelItemCount(); ++i)
    {
        auto item = topLevelItem(i);
        indexs[getItemHostId(item)] = item;
    }
    auto hostInfos = AppConfig::instance().listHosts();
    QList<QTreeWidgetItem*> listItemAdd;
    for(auto info : hostInfos) {
        auto iter = indexs.find(info.id);
        if(iter != std::end(indexs))
        {
            listItemAdd.push_back(iter->second);
            indexs.erase(iter);
            continue;
        }
        listItemAdd.push_back(createItem(info));
    }
    for(auto& [key, value]: indexs)
    {
        closeHost(value);
        auto item = takeTopLevelItem(indexOfTopLevelItem(value));
        item = nullptr;
    }
    if(listItemAdd.size() > 0)
    {
        // before add to treewidget, it should must be remove do,
        // because add will broken the top item's index.
        addTopLevelItems(listItemAdd);
    }
}

QTreeWidgetItem *HostTreeWidget::createItem(HostInfo info)
{
    auto item = new QTreeWidgetItem();
    item->setText(0, info.name);
    setItemHostId(item, info.id);
    setItemType(item, NodeType::HOST);
    setItemIcon(item, NodeType::HOST, info.sqlType);
    return item;
}

int HostTreeWidget::getItemHostId(QTreeWidgetItem *pItem)
{
    return pItem->data(0, Qt::UserRole + 998).toInt();
}

void HostTreeWidget::setItemHostId(QTreeWidgetItem *pItem, int id)
{
    if(pItem == nullptr)
    {
        return;
    }
    pItem->setData(0, Qt::UserRole + 998, id);
}

Database HostTreeWidget::getItemDatabaseInfo(QTreeWidgetItem *pItem)
{
    return {pItem->data(0, Qt::UserRole + 997).toString()};
}

void HostTreeWidget::setItemDatabaseInfo(QTreeWidgetItem *pItem, Database info)
{
    pItem->setData(0, Qt::UserRole + 997, info.name);
}

void HostTreeWidget::setItemType(QTreeWidgetItem *pItem, HostTreeWidget::NodeType type)
{
    if(pItem == nullptr)
    {
        return;
    }
    pItem->setData(0, Qt::UserRole + 999, type);
}

int HostTreeWidget::getItemType(QTreeWidgetItem *pItem)
{
    if(pItem == nullptr)
    {
        return HostTreeWidget::NodeType::NIL;
    }

    return pItem->data(0, Qt::UserRole + 999).toInt();
}

void HostTreeWidget::initTableTree(QTreeWidgetItem *pItem, int id, const Database &database, bool expand)
{
    if(pItem == NULL
            || id == 0
            || database.name.isEmpty())
    {
        return;
    }

    auto parentItem = new QTreeWidgetItem;
    parentItem->setText(0, "Table");
    setItemType(parentItem, NodeType::TABLE_CONTAINER);
    setItemHostId(parentItem, id);
    setItemDatabaseInfo(parentItem, database);
    pItem->addChild(parentItem);

    // search for all tables
    auto tables = _dbOperator->tables(id, database);
    if(tables.size() == 0)
    {
        return;
    }
    for(auto table : tables) {
        auto item = new QTreeWidgetItem();
        item->setText(0, table.name);
        item->setData(0, Qt::UserRole + 1, database.name);
        setItemType(item, NodeType::TABLE);
        setItemIcon(item, NodeType::TABLE, "");
        parentItem->addChild(item);
    }
    parentItem->setExpanded(expand);
}

void HostTreeWidget::setItemIcon(QTreeWidgetItem *pItem, NodeType type, const QString &name)
{
    if(pItem == nullptr)
    {
        return;
    }

    switch (type) {
    case NodeType::HOST:
    {
        if(name.compare("MYSQL") == 0)
        {
            pItem->setIcon(0, QIcon(":/image/default/mysql.svg"));
        }
        else if(name.compare("SQLITE") == 0)
        {
            pItem->setIcon(0, QIcon(":/image/default/sqlite-icon.svg"));
        }
    }
        break;
    case NodeType::DATABASE:
    {
        if(name.compare("close") == 0)
        {
            pItem->setIcon(0, QIcon(":/image/default/database_close.svg"));
        }
        else if(name.compare("open") == 0)
        {
            pItem->setIcon(0, QIcon(":/image/default/database_open.png"));
        }
    }
        break;
    case NodeType::TABLE_CONTAINER:
    case NodeType::TABLE:
        pItem->setIcon(0, QIcon(":/image/default/table.svg"));
        break;
    case NodeType::NIL:
        break;
    }
}

void HostTreeWidget::closeHost(QTreeWidgetItem *pItem)
{
    if (pItem && getItemType(pItem) == NodeType::HOST)
    {
        _dbOperator->closeConnection(getItemHostId(pItem));
        auto items = pItem->takeChildren();
        //delete[] items;
    }
}
