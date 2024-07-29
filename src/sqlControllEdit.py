from datetime import datetime
from PyQt5.QtGui import QFont, QIcon
from PyQt5.QtWidgets import QTableWidget, QTabBar, QTableWidgetItem

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
        tableWidget = QTableWidget()
        tableWidget.setColumnCount(len(headers))
        tableWidget.setRowCount(len(records))
        tableWidget.setHorizontalHeaderLabels(headers)
        for row,record in enumerate(records):
            for column, value in enumerate(record):
                tableWidget.setItem(row, column, QTableWidgetItem(self.get_value(value)))
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
        tableWidget = self.tabWidget.widget(1)
        tableWidget.setColumnCount(len(headers))
        tableWidget.setRowCount(len(records))
        tableWidget.setHorizontalHeaderLabels(headers)
        for row,record in enumerate(records):
            for column, value in enumerate(record):
                tableWidget.setItem(row, column, QTableWidgetItem(self.get_value(value)))
        self.tabWidget.setCurrentIndex(1)
    