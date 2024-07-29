from PyQt5 import QtCore, QtGui

sql_keyWords = [
    "ADD","ALTER","ALL","AND","ANY","AS",
    "ASC","BACKUP","BETWEEN","CASE","CHECK",
    "COLUMN","CONSTRAINT","CREATE","DATABASE","DEFAULT",
    "DELETE","DESC","DISTINCT","DROP","EXEC",
    "EXISTS","FOREIGN","FROM","FULL","GROUP",
    "HAVING","IN","INDEX","INNER","INSERT",
    "IS","JOIN","LEFT","LIKE","LIMIT",
    "NOT","OR","ORDER","OUTER","PRIMARY",
    "PROCEDURE","RIGHT","ROWNUM","SELECT",
    "SET","TABLE","TOP","TRUNCATE TABLE","UNION",
    "UNION ALL","UNIQUE","UPDATE","VALUES","VIEW",
    "WHERE","SHOW","KEY","BY","INTO",
    "NULL","REPLACE"
]

class SqlHighlighter(QtGui.QSyntaxHighlighter):
    def __init__(self, parent):
        super(SqlHighlighter, self).__init__(parent)
        self.commentFormat = QtGui.QTextCharFormat()
        self.commentFormat.setForeground(QtCore.Qt.gray)

        comment = '#|--'
        self.commentExpression = QtCore.QRegularExpression("({})[^\n]*".format(comment))
        self.commentExp = QtCore.QRegExp("^\\s*({})".format(comment))
        self.syntaxFormat = QtGui.QTextCharFormat()
        self.syntaxFormat.setForeground(QtCore.Qt.blue)
        self.syntaxFormat.setFontWeight(QtGui.QFont.Bold)

    def highlightBlock(self, text):
        for key in sql_keyWords:
            expression = QtCore.QRegularExpression("\\b{}\\b".format(key), QtCore.QRegularExpression.CaseInsensitiveOption)
            matchIterator = expression.globalMatch(text)
            while (matchIterator.hasNext()):
                match = matchIterator.next()
                self.setFormat(match.capturedStart(), match.capturedLength(), self.syntaxFormat)

        matchIterator = self.commentExpression.globalMatch(text)
        while matchIterator.hasNext():
            match = matchIterator.next()
            self.setFormat(match.capturedStart(), match.capturedLength(), self.commentFormat)

    def is_comment(self, text):
        return self.commentExp.indexIn(text) != -1