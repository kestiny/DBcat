import sys
from PyQt5 import QtWidgets
from src.dbCat import DBCat

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)

    window = DBCat()
    window.setWindowTitle("DBCat")
    window.show()

    sys.exit(app.exec_())