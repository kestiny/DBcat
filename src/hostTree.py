from pathlib import Path
from PyQt5.QtWidgets import QTreeWidgetItem, QMenu, QDialog, QMessageBox
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon, QCursor, QFont

from src.hostOper import HostOper
from src.hostEditDialog import HostEditDialog
from src.dbOperator import DBOperator

# 获取当前用户的文档目录
host_path = Path.home() / "Documents" / "DBCat" / ".host"

class HostTree():
    '''节点类型定义(常量)'''
    HOST = 'HOST'
    DATABASE = 'DATABASE'
    TABLE = 'TABLE'
    INDEX = 'INDEX'
    
    def __init__(self, treeWidget, sqlControllWidget):
        self.sqlTreeWidget = treeWidget
        self.sqlControllWidget = sqlControllWidget
        font = QFont()
        font.setFamily("Courier New")
        font.setStyleHint(QFont.Monospace)
        font.setPointSize(12)
        self.sqlTreeWidget.setFont(font)
        self.sqlTreeWidget.setIndentation(int(self.sqlTreeWidget.indentation() / 2))
        self.sqlTreeWidget.header().setVisible(False)
        self.sqlTreeWidget.setExpandsOnDoubleClick(True)
        self.sqlTreeWidget.setColumnCount(1)

        self.sqlTreeWidget.setContextMenuPolicy(Qt.CustomContextMenu)
        self.sqlTreeWidget.customContextMenuRequested.connect(self.showContextMenu)
        self.sqlTreeWidget.itemDoubleClicked.connect(self.itemDoubleClicked)

        self.hostOper = HostOper()
        self.initHost()
        self.init_menu()

    def exec_sql(self, sql):
        item = self.sqlTreeWidget.currentItem()
        if item is not None:
            type = self.getItemType(item)
            database = ''
            if type == HostTree.DATABASE:
                database = item.text(0)
            elif type == HostTree.TABLE:
                database = item.parent().text(0)
            self.sqlControllWidget.exec_sql(self.getItemHostId(item), database, sql)

    def add_host(self, host):
        # 先存储到文件
        self.hostOper.add_host(host)
        # 新增节点到左树
        self.sqlTreeWidget.addTopLevelItem(self.createHostItem(host))

    def initHost(self):
        hosts = self.hostOper.get_hosts()
        for host in hosts:
            self.sqlTreeWidget.addTopLevelItem(self.createHostItem(host))

    def createHostItem(self, host):
        item = QTreeWidgetItem()
        item.setText(0, '{}[{}]'.format(host.name, host.environment))
        self.setItemHost(item, host)
        self.setItemTypeAndIcon(item, HostTree.HOST, host.type)
        return item
    
    def getItemHostId(self, item):
        return item.data(0, Qt.UserRole + 998)
    
    def setItemHostId(self, item, id):
        if item is not None:
            item.setData(0, Qt.UserRole + 998, id)
    
    def getItemHost(self, item):
        return item.data(0, Qt.UserRole + 997)

    def setItemHost(self, item, host):
        if item is not None:
            item.setData(0, Qt.UserRole + 998, host.id)
            item.setData(0, Qt.UserRole + 997, host)

    def setItemTypeAndIcon(self, item, node, type):
        if item is not None:
            item.setData(0, Qt.UserRole + 999, node)

            if node == HostTree.HOST:
                item.setIcon(0, QIcon('image/{}.svg'.format(type)))
            elif node == HostTree.DATABASE:
                item.setIcon(0, QIcon('image/database.svg'))
            elif node == HostTree.TABLE:
                item.setIcon(0, QIcon('image/table.svg'))

    def getItemType(self, item):
        if item is not None:
            return item.data(0, Qt.UserRole + 999)
        else:
            return 'NONE'
        
    def init_menu(self):
        self.contextMenu = QMenu()
        # connect action
        self.actEditConn = self.contextMenu.addAction(QIcon("image/edit.svg"), "edit Connection")
        self.actEditConn.triggered.connect(self.edit_conn)
        self.actDelConn = self.contextMenu.addAction(QIcon("image/delete.svg"), "del Connection")
        self.actDelConn.triggered.connect(self.del_conn)
        self.actCloseConn = self.contextMenu.addAction(QIcon("image/database_close.svg"), "close Connection")
        self.actCloseConn.triggered.connect(self.close_conn)
        # database action
        self.actCloseDB = self.contextMenu.addAction(QIcon("image/edit.svg"), "close database")
        self.actCloseDB.triggered.connect(self.close_database)
        self.actCreateDB = self.contextMenu.addAction(QIcon("image/newsql.svg"), "create database")
        self.actCreateDB.triggered.connect(self.create_database)
        self.actDropDB = self.contextMenu.addAction(QIcon("image/delete.svg"), "drop database")
        self.actDropDB.triggered.connect(self.drop_databse)
        # table action
        self.actIndexTable = self.contextMenu.addAction(QIcon("image/index.svg"), "view index")
        self.actIndexTable.triggered.connect(self.show_index)
        self.actDeleteData = self.contextMenu.addAction(QIcon("image/delete.svg"), "delete data")
        self.actDeleteData.triggered.connect(self.delete_data)
        self.actDropTable = self.contextMenu.addAction(QIcon("image/delete.svg"), "drop table")
        self.actDropTable.triggered.connect(self.drop_data)

    def edit_conn(self):
        item = self.sqlTreeWidget.currentItem()
        if self.getItemType(item) == HostTree.HOST:
            dialog = HostEditDialog()
            dialog.init_with_host(self.getItemHost(item))
            if QDialog.Accepted == dialog.exec():
                host = dialog.get_host()
                self.hostOper.update_host(host)
                item.setText(0, '{}[{}]'.format(host.name, host.environment))

    def del_conn(self):
        item = self.sqlTreeWidget.currentItem()
        if self.getItemType(item) == HostTree.HOST:
            item.takeChildren()
            self.hostOper.del_host(self.getItemHostId(item))
            self.sqlTreeWidget.takeTopLevelItem(self.sqlTreeWidget.indexOfTopLevelItem(item))

    def close_conn(self):
        item = self.sqlTreeWidget.currentItem()
        if self.getItemType(item) == HostTree.HOST:
            item.takeChildren()
            DBOperator().release_connection(self.getItemHostId(item))

    def close_database(self):
        item = self.sqlTreeWidget.currentItem()
        if item is not None and self.getItemType(item) == HostTree.DATABASE:
            item.takeChildren()

    def create_database(self):
        self.sqlControllWidget.set_msg("create database sql statement\r\nCREATE DATABASE [name] DEFAULT CHARACTER SET = 'utf8'")

    def drop_databse(self):
        item = self.sqlTreeWidget.currentItem()
        if item is not None and self.getItemType(item) == HostTree.DATABASE:
            if self.warn_message_box('确认删除DB[{}]!!!    \r\n这将清空数据并删除数据表.    '.format(item.text(0))):
                sql = "DROP DATABASE {}".format(item.text(0))
                databases, msg = DBOperator().do_exec_statement(self.getItemHostId(item), sql)
                self.sqlControllWidget.set_msg('{}: {}'.format(sql, msg))
                item.takeChildren()
                item.parent().removeChild(item)

    def show_index(self):
        item = self.sqlTreeWidget.currentItem()
        if item is not None and self.getItemType(item) == HostTree.TABLE:
            self.sqlControllWidget.show_index(self.getItemHostId(item), item.parent().text(0), item.text(0))

    def delete_data(self):
        item = self.sqlTreeWidget.currentItem()
        if item is not None and self.getItemType(item) == HostTree.TABLE:
            if self.warn_message_box('确认清空表[{}]内的所有数据？    '.format(item.text(0))):
                sql = "DELETE FROM {}".format(item.text(0))
                _, msg = DBOperator().do_exec_statement(self.getItemHostId(item), sql)
                self.sqlControllWidget.set_msg('[DELETE TABLE]{}: {}'.format(sql, msg))

    def drop_data(self):
        item = self.sqlTreeWidget.currentItem()
        if item is not None and self.getItemType(item) == HostTree.TABLE:
            if self.warn_message_box('确认清空表[{}]内的所有数据, 并删除此表？    '.format(item.text(0))):
                sql = "DELETE FROM {}".format(item.text(0))
                _, msg = DBOperator().do_exec_statement(self.getItemHostId(item), sql)
                self.sqlControllWidget.set_msg('[DROP TABLE]{}: {}'.format(sql, msg))
                item.parent().removeChild(item)

    def warn_message_box(self, content):
        reply = QMessageBox.warning(
            self.sqlTreeWidget, 'Warning', content,
            QMessageBox.Ok | QMessageBox.Cancel, QMessageBox.Ok)
        if reply == QMessageBox.Ok:
                return True
        else:
            return False
        
    def showContextMenu(self, pos):
        item = self.sqlTreeWidget.itemAt(pos)
        if item is None:
            return
        
        count = item.childCount()
        type = self.getItemType(item)
        for act in self.contextMenu.actions():
            act.setVisible(False) #先隐藏所有菜单

        if type == HostTree.HOST:
            self.actEditConn.setVisible(True)
            self.actDelConn.setVisible(True)
            if count > 0:
                self.actCloseConn.setVisible(True)
        elif type == HostTree.DATABASE:
            self.actCreateDB.setVisible(True)
            self.actDropDB.setVisible(True)
            if count > 0:
                self.actCloseDB.setVisible(True)
        elif type == HostTree.TABLE:
            self.actIndexTable.setVisible(True)
            self.actDeleteData.setVisible(True)
            self.actDropTable.setVisible(True)
        elif type == HostTree.INDEX:
            pass

        self.contextMenu.exec(QCursor.pos())

    def itemDoubleClicked(self, item):
        if item is not None:
            node_type = self.getItemType(item)
            id = self.getItemHostId(item)
            childCount = item.childCount()
            if node_type == HostTree.HOST and childCount == 0:
                # 打开连接
                databases, msg = DBOperator().database(id)
                self.sqlControllWidget.set_msg('connect to:{} {}'.format(item.text(0), msg))
                for db in databases:
                    item.addChild(self.create_clild_item(db, id, HostTree.DATABASE))
            elif node_type == HostTree.DATABASE and childCount == 0:
                # 查询数据库表
                tables, msg = DBOperator().tables(id, item.text(0))
                for table in tables:
                    item.addChild(self.create_clild_item(table, id, HostTree.TABLE))
            elif node_type == HostTree.TABLE:
                self.sqlControllWidget.open_table(id, item.parent().text(0), item.text(0))

    def create_clild_item(self, name, id, type):
        item = QTreeWidgetItem()
        item.setText(0, name)
        self.setItemTypeAndIcon(item, type, "")
        self.setItemHostId(item, id)
        return item