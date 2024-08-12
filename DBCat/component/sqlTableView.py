# -*- coding: utf-8 -*-
from concurrent.futures import ThreadPoolExecutor
from PyQt5.QtWidgets import QTableView, QApplication
from PyQt5.QtCore import Qt


class SqlTableView(QTableView):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

    def keyReleaseEvent(self, event):
        if event.key() == Qt.Key.Key_C and event.modifiers() == Qt.KeyboardModifier.ControlModifier:
            with ThreadPoolExecutor(max_workers=1) as executor:
                executor.submit(self.do_copy_data).add_done_callback(SqlTableView.copy_to_clipboard)

        super().keyReleaseEvent(event)

    @staticmethod
    def copy_to_clipboard(future):
        try:
            result = future.result()
            clipboard = QApplication.clipboard()
            clipboard.setText(result)
        except Exception as e:
            print(f"Exception occurred: {e}")

    def do_copy_data(self):
        selected_ranges = self.selectedIndexes()
        indexes_dict = {}
        proxy_model = self.model()
        for index in selected_ranges:  # 遍历每个单元格
            real_index = proxy_model.mapToSource(index)
            row, column = real_index.row(), real_index.column()  # 获取单元格的行号，列号
            if row in indexes_dict.keys():
                indexes_dict[row].append(column)
            else:
                indexes_dict[row] = [column]

        text = []
        head_columns = []
        for row, columns in indexes_dict.items():
            row_data = []
            head_columns = columns
            for column in columns:
                data = self.model().sourceModel().item_data(row, column)
                row_data.append(data)

            text.append('\t'.join(row_data))

        # heads = []
        # for column in head_columns:
        #     data = self.model().sourceModel().item_head(column)
        #     heads.append(data)
        #
        # return '\t'.join(heads) + '\n' + '\n'.join(text)
        return '\n'.join(text)