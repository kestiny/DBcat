# -*- coding: utf-8 -*-
import sys
from PyQt5 import QtWidgets

from DBCat import dbCat


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)

    window = dbCat.DBCat()
    window.show()

    sys.exit(app.exec_())