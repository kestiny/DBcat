from datetime import datetime
from PyQt5.QtGui import QFont, QIcon
from PyQt5.QtWidgets import QTableWidget, QTabBar, QTableView
from PyQt5.QtCore import Qt, QModelIndex, QAbstractTableModel, QVariant

from src.dbOperator import DBOperator

class SqlControllEdit():
    def __init__(self, tabWidget, sqlMessage):
        self.tabWidget = tabWidget
        self.sqlMessage = sqlMessage
        font = QFont()
        font.setFamily("Courier New")
        font.setStyleHint(QFont.Monospace)
        font.setPointSize(12)
        self.sqlMessage.setFont(font)

        self.tabWidget.setTabsClosable(True)
        self.tabWidget.tabBar().setTabButton(0, QTabBar.RightSide, None)
        self.tabWidget.tabBar().setTabButton(1, QTabBar.RightSide, None)
        self.tabWidget.setCurrentIndex(0)
        self.tabWidget.setTabIcon(0, QIcon("image/msg.svg"))
        self.tabWidget.setTabIcon(1, QIcon("image/result.svg"))
        self.tabWidget.tabCloseRequested.connect(self.tab_close)

        self.icon_table = QIcon("image/table.svg")
        self.icon_index = QIcon("image/index.svg")
    
    def tab_close(self, index):
        self.tabWidget.removeTab(index)

    def set_msg(self, msg):
        current_time = datetime.now()
        self.sqlMessage.setPlainText('{}{} {}'.format(current_time.strftime("%H:%M:%S"), ".{:03d}".format(current_time.microsecond // 1000), msg))
        self.tabWidget.setCurrentIndex(0)

    def show_index(self, id, db_name, table_name):
        records, headers = DBOperator().do_exec_statement(id, db_name, 'SHOW INDEX FROM {}'.format(table_name))
        if records is not None:
            self.add_tab(table_name, records, headers, 'INDEX')
        else:
            self.set_msg(headers)

    def open_table(self, id, db_name, table_name):
        records, headers = DBOperator().do_exec_statement(id, db_name, 'SELECT * FROM {}'.format(table_name))
        if records is not None:
            self.add_tab(table_name, records, headers, 'TABLE')
        else:
            self.set_msg(headers)

    def add_tab(self, name, records, headers, type):
        tableWidget = QTableView()
        self.fill_table_widget(tableWidget, records, headers)
        index = self.tabWidget.addTab(tableWidget, self.icon_index if type == 'INDEX' else self.icon_table, name)
        self.tabWidget.setCurrentIndex(index)
    
    def get_value(self, obj):
        if isinstance(obj, datetime):
            return '{}{}'.format(obj.strftime("%Y-%m-%d %H:%M:%S"), ".{:03d}".format(obj.microsecond // 1000))
        else:
            return '' if obj == None else str(obj)
        
    def exec_sql(self, id, db_name, sql):
        try:
            records, headers = DBOperator().do_exec_statement(id, db_name, sql)
            if records is not None:
                self.set_msg('[exec success]: {}'.format(sql))
                self.fill_result(records, headers)
            else:
                self.set_msg(headers)
        except Exception as e:
            self.set_msg(f'[ERROR]: {e}')

    def fill_result(self, records, headers):
        tableView = QTableView()
        self.fill_table_widget(tableView, records, headers) 
        self.tabWidget.removeTab(1)
        self.tabWidget.insertTab(1, tableView, self.icon_table, "Result")
        self.tabWidget.tabBar().setTabButton(1, QTabBar.RightSide, None)
        self.tabWidget.setCurrentIndex(1)

    def fill_table_widget(self, view, records, headers):
        # 创建模型
        model = MyTableModel(records, headers)
        view.setModel(model)
        view.resizeColumnsToContents()
        view.resizeRowsToContents()

class MyTableModel(QAbstractTableModel):
    def __init__(self, data, header):
        super().__init__()
        self._data = data
        self._header = header

    def rowCount(self, parent=QModelIndex()):
        return len(self._data)

    def columnCount(self, parent=QModelIndex()):
        return len(self._data[0]) if self._data else 0

    def data(self, index, role=Qt.DisplayRole):
        if role == Qt.DisplayRole:
            row = index.row()
            col = index.column()
            return str(self._data[row][col])

        return QVariant()

    def headerData(self, section, orientation, role=Qt.DisplayRole):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                return self._header[section]
            elif orientation == Qt.Vertical:
                return f"{section}"
        return QVariant()
    