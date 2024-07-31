# -*- coding: utf-8 -*-
import sys
import os
import json
from pathlib import Path

def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = Path(sys._MEIPASS)
    except Exception:
        base_path = Path.cwd()

    return str((base_path / relative_path).resolve())

def sql_dir():
    # 获取当前用户的文档目录
    dbcat_path = Path.home() / "Documents" / "DBCat" / 'sql'
    if not os.path.exists(dbcat_path ):
        os.makedirs(dbcat_path)
    return dbcat_path

def dbcat_setting_file():
    file =  Path.home() / "Documents" / "DBCat" / ".conn"
    if not os.path.exists(file):
            with open(file, 'w') as file:
                json.dump('[]', file)
    return file