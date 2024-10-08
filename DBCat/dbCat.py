# -*- coding: utf-8 -*-
from PyQt5 import QtWidgets, QtCore, QtGui

from DBCat import sql_editor
from DBCat.hosts import host_tree
from DBCat.hosts import host_edit_dialog
from DBCat import sql_control_edit
from DBCat.dboperator import mysql_operator
from DBCat import resource as res
from DBCat.component import sqlTableView

class DBCat(QtWidgets.QMainWindow):
    """主窗口"""

    def __init__(self, parent=None):
        super(DBCat, self).__init__(parent)
        self.setupUi(self)

    def setupUi(self, DBcat):
        DBcat.setObjectName("DBcat")
        DBcat.resize(1187, 668)
        DBcat.setWindowTitle("DBCat")
        DBcat.setWindowIcon(QtGui.QIcon(res.resource_path('image/title.svg')))

        self.centralwidget = QtWidgets.QWidget(DBcat)
        self.centralwidget.setObjectName("centralwidget")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.splitter_2 = QtWidgets.QSplitter(self.centralwidget)
        self.splitter_2.setOrientation(QtCore.Qt.Horizontal)
        self.splitter_2.setObjectName("splitter_2")
        self.layoutWidget = QtWidgets.QWidget(self.splitter_2)
        self.layoutWidget.setObjectName("layoutWidget")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.layoutWidget)
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_2.setSpacing(0)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.widgetSearch = QtWidgets.QWidget(self.layoutWidget)
        self.widgetSearch.setMinimumSize(QtCore.QSize(0, 36))
        self.widgetSearch.setMaximumSize(QtCore.QSize(16777215, 36))
        self.widgetSearch.setObjectName("widgetSearch")
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout(self.widgetSearch)
        self.horizontalLayout_2.setContentsMargins(2, 2, 2, 2)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.lineEdit = QtWidgets.QLineEdit(self.widgetSearch)
        self.lineEdit.setMinimumSize(QtCore.QSize(0, 24))
        self.lineEdit.setMaximumSize(QtCore.QSize(16777215, 24))
        self.lineEdit.setObjectName("lineEdit")
        self.horizontalLayout_2.addWidget(self.lineEdit)
        self.label = QtWidgets.QLabel(self.widgetSearch)
        self.label.setMinimumSize(QtCore.QSize(0, 32))
        self.label.setObjectName("label")
        self.horizontalLayout_2.addWidget(self.label)
        self.verticalLayout_2.addWidget(self.widgetSearch)
        self.widgetSearch.hide()
        self.hostWidget = QtWidgets.QTreeWidget(self.layoutWidget)
        self.hostWidget.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.hostWidget.setStyleSheet("")
        self.hostWidget.setColumnCount(1)
        self.hostWidget.setObjectName("hostWidget")
        self.hostWidget.setFocus()
        self.verticalLayout_2.addWidget(self.hostWidget)
        self.splitter = QtWidgets.QSplitter(self.splitter_2)
        self.splitter.setOrientation(QtCore.Qt.Vertical)
        self.splitter.setObjectName("splitter")
        self.sqlEdit = QtWidgets.QTabWidget(self.splitter)
        self.sqlEdit.setObjectName("sqlEdit")
        self.sqlEdit.setTabsClosable(True)
        self.sqlControll = QtWidgets.QTabWidget(self.splitter)
        self.sqlControll.setTabsClosable(True)
        self.sqlControll.setObjectName("sqlControll")
        self.tabMessage = QtWidgets.QWidget()
        self.tabMessage.setObjectName("tabMessage")
        self.verticalLayout_4 = QtWidgets.QVBoxLayout(self.tabMessage)
        self.verticalLayout_4.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_4.setSpacing(0)
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.textEditMessage = QtWidgets.QTextEdit(self.tabMessage)
        self.textEditMessage.setReadOnly(True)
        self.textEditMessage.setObjectName("textEditMessage")
        self.verticalLayout_4.addWidget(self.textEditMessage)
        self.sqlControll.addTab(self.tabMessage, "")
        self.tab_2 = sqlTableView.SqlTableView()
        self.tab_2.setObjectName("tab_2")
        self.sqlControll.addTab(self.tab_2, "")
        self.horizontalLayout.addWidget(self.splitter_2)
        DBcat.setCentralWidget(self.centralwidget)

        self.splitter_2.setStretchFactor(0, 1)
        self.splitter_2.setStretchFactor(1, 3)

        # action init
        newConnect = QtWidgets.QAction(QtGui.QIcon(res.resource_path('image/connect.svg')), 'New Connect',
                                       self)
        newSql = QtWidgets.QAction(QtGui.QIcon(res.resource_path('image/newsql.svg')), 'New Sql', self)
        execSql = QtWidgets.QAction(QtGui.QIcon(res.resource_path('image/run.svg')), 'Run Sql', self)
        execSql.setToolTip("运行(F9)")
        execSql.setShortcut("F9")

        newSql.triggered.connect(self.newSqlEdit)
        newConnect.triggered.connect(self.new_conn_edit)
        execSql.triggered.connect(self.do_exec_sql)

        # toolBar
        self.toolBar = self.addToolBar("New")
        self.toolBar.addAction(newConnect)
        self.toolBar.addAction(newSql)
        self.toolBar.addAction(execSql)

        self.retranslateUi(DBcat)
        self.sqlControll.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(DBcat)

        # 初始化sql代码编辑器
        self.sqlTabEdit = sql_editor.SqlEditor(self.sqlEdit)
        self.sqlControllEdit = sql_control_edit.SqlControlEdit(self.sqlControll)
        self.sqlControllEdit.init_message(self.textEditMessage)
        self.sqlHostTreeWidget = host_tree.HostTree(self.hostWidget, self.sqlControllEdit)

    def do_exec_sql(self):
        """执行SQL语句查询"""
        sql_statement, high_risk_operator = self.sqlTabEdit.selections()
        self.sqlHostTreeWidget.exec_sql(sql_statement, high_risk_operator)

    def retranslateUi(self, DBcat):
        _translate = QtCore.QCoreApplication.translate
        DBcat.setWindowTitle(_translate("DBcat", "DBcat"))
        self.label.setText(_translate("DBcat", "No results"))
        self.sqlControll.setTabText(self.sqlControll.indexOf(self.tabMessage), _translate("DBcat", "Message"))
        self.sqlControll.setTabText(self.sqlControll.indexOf(self.tab_2), _translate("DBcat", "Result"))

    def closeEvent(self, event):
        self.sqlTabEdit.saveFiles()
        mysql_operator.MysqlOperator().release_connections()

    def newSqlEdit(self):
        text, ok = QtWidgets.QInputDialog.getText(self, '新建查询', '请输入新查询名称:')
        # 如果用户点击OK
        if ok:
            self.sqlTabEdit.newSqlEdit(text)

    def new_conn_edit(self):
        dialog = host_edit_dialog.HostEditDialog()
        if QtWidgets.QDialog.Accepted == dialog.exec():
            self.sqlHostTreeWidget.add_host(dialog.get_host())