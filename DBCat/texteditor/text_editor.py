"""
https://doc.qt.io/qt-5/qtwidgets-widgets-PlainTextEditor-example.html#the-linenumberarea-class
https://doc.qt.io/qtforpython/examples/example_widgets__PlainTextEditor.html
"""
import unicodedata
from PyQt5 import QtCore, QtGui, QtWidgets

from DBCat.texteditor import sql_highlighter


class LineNumberArea(QtWidgets.QWidget):
    def __init__(self, editor):
        super(LineNumberArea, self).__init__(editor)
        self._code_editor = editor

    def sizeHint(self):
        return QtCore.QSize(self._code_editor.line_number_area_width(), 0)

    def paintEvent(self, event):
        self._code_editor.lineNumberAreaPaintEvent(event)


class CodeTextEdit(QtWidgets.QPlainTextEdit):
    is_first = False
    pressed_keys = list()

    indented = QtCore.pyqtSignal(object)
    unindented = QtCore.pyqtSignal(object)
    commented = QtCore.pyqtSignal(object)
    uncommented = QtCore.pyqtSignal(object)

    def __init__(self):
        super(CodeTextEdit, self).__init__()

        self.indented.connect(self.do_indent)
        self.unindented.connect(self.undo_indent)
        self.commented.connect(self.do_comment)
        self.uncommented.connect(self.undo_comment)

    def clear_selection(self):
        """
        Clear text selection on cursor
        """
        pos = self.textCursor().selectionEnd()
        self.textCursor().movePosition(pos)

    def get_selection_range(self):
        """
        Get text selection line range from cursor
        Note: currently only support continuous selection

        :return: (int, int). start line number and end line number
        """
        cursor = self.textCursor()
        if not cursor.hasSelection():
            return 0, 0

        start_pos = cursor.selectionStart()
        end_pos = cursor.selectionEnd()

        cursor.setPosition(start_pos)
        start_line = cursor.blockNumber()
        cursor.setPosition(end_pos)
        end_line = cursor.blockNumber()

        return start_line, end_line

    def remove_line_start(self, string, line_number):
        """
        Remove certain string occurrence on line start

        :param string: str. string pattern to remove
        :param line_number: int. line number
        """
        cursor = QtGui.QTextCursor(
            self.document().findBlockByLineNumber(line_number))
        cursor.select(QtGui.QTextCursor.LineUnderCursor)
        text = cursor.selectedText()
        if text.startswith(string):
            cursor.removeSelectedText()
            cursor.insertText(text.split(string, 1)[-1])

    def insert_line_start(self, string, line_number):
        """
        Insert certain string pattern on line start

        :param string: str. string pattern to insert
        :param line_number: int. line number
        """
        cursor = QtGui.QTextCursor(
            self.document().findBlockByLineNumber(line_number))
        self.setTextCursor(cursor)
        self.textCursor().insertText(string)

    def keyPressEvent(self, event):
        """
        Extend the key press event to create key shortcuts
        """
        self.is_first = True
        self.pressed_keys.append(event.key())
        start_line, end_line = self.get_selection_range()

        # indent event
        if event.key() == QtCore.Qt.Key_Tab and \
                (end_line - start_line):
            lines = range(start_line, end_line + 1)
            self.indented.emit(lines)
            return

        # un-indent event
        elif event.key() == QtCore.Qt.Key_Backtab:
            lines = range(start_line, end_line + 1)
            self.unindented.emit(lines)
            return

        super(CodeTextEdit, self).keyPressEvent(event)

    def keyReleaseEvent(self, event):
        """
        Extend the key release event to catch key combos
        """
        if self.is_first:
            self.process_multi_keys(self.pressed_keys)

        self.is_first = False
        self.pressed_keys.pop()
        super(CodeTextEdit, self).keyReleaseEvent(event)

    def process_multi_keys(self, keys):
        """
        Placeholder for processing multiple key combo events

        :param keys: [QtCore.Qt.Key]. key combos
        """
        # toggle comments indent event
        if keys == [QtCore.Qt.Key_Control, QtCore.Qt.Key_Slash]:
            pass

    def do_indent(self, lines):
        """
        Indent lines

        :param lines: [int]. line numbers
        """
        for line in lines:
            self.insert_line_start('\t', line)

    def undo_indent(self, lines):
        """
        Un-indent lines

        :param lines: [int]. line numbers
        """
        for line in lines:
            self.remove_line_start('\t', line)

    def do_comment(self, lines):
        """
        Comment out lines

        :param lines: [int]. line numbers
        """
        for line in lines:
            pass

    def undo_comment(self, lines):
        """
        Un-comment lines

        :param lines: [int]. line numbers
        """
        for line in lines:
            pass


class TextEditor(CodeTextEdit):
    """
    text editor Impl
    """
    def __init__(self):
        super(TextEditor, self).__init__()
        self.sqlHighlighter = sql_highlighter.SqlHighlighter(self.document())
        self.line_number_area = LineNumberArea(self)

        self.font = QtGui.QFont()
        self.font.setFamily("Courier New")
        self.font.setStyleHint(QtGui.QFont.Monospace)
        self.font.setPointSize(12)
        self.setFont(self.font)

        self.tab_size = 4
        self.setTabStopWidth(self.tab_size * self.fontMetrics().width(' '))

        self.blockCountChanged.connect(self.update_line_number_area_width)
        self.updateRequest.connect(self.update_line_number_area)
        self.cursorPositionChanged.connect(self.highlightCurrentLine)

        self.update_line_number_area_width(0)
        self.highlightCurrentLine()

    def line_number_area_width(self):
        digits = 1
        max_num = max(1, self.blockCount())
        while max_num >= 10:
            max_num *= 0.1
            digits += 1

        space = 30 + self.fontMetrics().width('9') * digits
        return space

    def resizeEvent(self, e):
        super(TextEditor, self).resizeEvent(e)
        cr = self.contentsRect()
        width = self.line_number_area_width()
        rect = QtCore.QRect(cr.left(), cr.top(), width, cr.height())
        self.line_number_area.setGeometry(rect)

    def highlightCurrentLine(self):
        extra_selections = list()

        if not self.isReadOnly():
            selection = QtWidgets.QTextEdit.ExtraSelection()
            lineColor = QtGui.QColor(QtCore.Qt.lightGray).lighter(160)
            selection.format.setBackground(lineColor)
            selection.format.setProperty(QtGui.QTextFormat.FullWidthSelection, True)
            selection.cursor = self.textCursor()
            selection.cursor.clearSelection()
            extra_selections.append(selection)

        self.setExtraSelections(extra_selections)

    def lineNumberAreaPaintEvent(self, event):
        painter = QtGui.QPainter(self.line_number_area)
        block = self.firstVisibleBlock()
        block_number = block.blockNumber()
        offset = self.contentOffset()
        top = self.blockBoundingGeometry(block).translated(offset).top()
        bottom = top + self.blockBoundingRect(block).height()

        while block.isValid() and top <= event.rect().bottom():
            if block.isVisible() and bottom >= event.rect().top():
                number = str(block_number + 1)
                painter.setPen(QtGui.QColor(118, 150, 185))
                width = self.line_number_area.width() - 10
                height = self.fontMetrics().height()
                painter.drawText(0, int(top), width, height, QtCore.Qt.AlignmentFlag.AlignRight, number)

            block = block.next()
            top = bottom
            bottom = top + self.blockBoundingRect(block).height()
            block_number += 1

    def update_line_number_area_width(self, newBlockCount):
        self.setViewportMargins(self.line_number_area_width(), 0, 0, 0)

    def update_line_number_area(self, rect, dy):
        if dy:
            self.line_number_area.scroll(0, dy)
        else:
            width = self.line_number_area.width()
            self.line_number_area.update(0, rect.y(), width, rect.height())

        if rect.contains(self.viewport().rect()):
            self.update_line_number_area_width(0)

    def wholeText(self):
        return self.toPlainText()

    def selections(self):
        selectionText = self.textCursor().selectedText()
        contentText = []

        # 段落分隔符 (Zp) 替换为换行符
        result = ""
        for char in selectionText:
            if unicodedata.category(char) == 'Zp':
                result += '\n'
            else:
                result += char

        for text in result.split('\n'):
            text = text.strip()
            if not text or self.sqlHighlighter.is_comment(text):
                continue
            else:
                contentText.append(text)
        return ' '.join(contentText)
