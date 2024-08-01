# -*- coding: utf-8 -*-
from pathlib import Path
from PyQt5 import QtCore

from DBCat.texteditor import text_editor
from DBCat import resource as res


class SqlEditor:
    def __init__(self, sqlEditor):
        self.sqlEditor = sqlEditor
        self.sqlEditor.setTabsClosable(True)
        self.sqlEditor.tabCloseRequested.connect(self.tabClose)

        self.initSqlEdit()
        # 每隔5分钟，自动保存一次文件
        self.timer = QtCore.QTimer()
        self.timer.start(5 * 60 * 1000)
        self.timer.timeout.connect(self.saveFiles)

    def selections(self):
        return self.sqlEditor.currentWidget().selections()

    def loadFiles(self):
        directory_path = res.sql_dir()
        files_list = [file for file in directory_path.glob('**/*') if file.is_file()]
        return [file for file in files_list if file.suffix == '.sql']

    def initSqlEdit(self):
        files = self.loadFiles()
        if len(files) == 0:
            sqlCode = text_editor.TextEditor()
            self.sqlEditor.addTab(sqlCode, "新建查询")
        else:
            for file in files:
                sqlCode = text_editor.TextEditor()
                with open(file, 'r') as fileHandler:
                    sqlCode.setPlainText(fileHandler.read())
                self.sqlEditor.addTab(sqlCode, Path(file).stem)

    def saveFiles(self):
        directory_path = res.sql_dir()
        # 获取所有tab页
        for i in range(self.sqlEditor.count()):
            with open(directory_path / (self.sqlEditor.tabText(i) + '.sql'), 'w') as file:
                file.write(self.sqlEditor.widget(i).wholeText())

    def newSqlEdit(self, name):
        tab_index = self.findText(name)
        if tab_index != -1:
            self.sqlEditor.setCurrentIndex(tab_index)
        else:
            sqlCode = text_editor.TextEditor()
            self.sqlEditor.addTab(sqlCode, name)
            self.sqlEditor.setCurrentWidget(sqlCode)

    def tabClose(self, index: int):
        name = self.sqlEditor.tabText(index)
        self.sqlEditor.removeTab(index)
        file = res.sql_dir() / (name + '.sql')
        # 使用unlink()方法删除文件， unlink(missing_ok=True) 可以避免 FileNotFoundError 异常
        try:
            file.unlink(missing_ok=True)
        except Exception as e:
            print(f"删除文件时发生错误: {e}")

    def findText(self, tab_name):
        """根据tab名称查找并返回其索引，如果未找到则返回-1"""
        for index in range(self.sqlEditor.count()):
            if self.sqlEditor.tabText(index) == tab_name:
                return index
        return -1
