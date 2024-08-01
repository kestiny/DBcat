# -*- coding: utf-8 -*-
import mysql.connector
from mysql.connector import Error


class Singleton(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]


class MysqlOperator(metaclass=Singleton):
    def __init__(self):
        self.__conns = {}
        self.data = "Hello, World!"

    def release_connections(self):
        for connection in self.__conns.values():
            if connection.is_connected():
                connection.close()

    def release_connection(self, id):
        connection = self.__conns.pop(id)
        if connection is not None and connection.is_connected():
            connection.close()

    def do_exec_statement(self, id, database, sql):
        connection = self.__conns.get(id)
        if connection is None:
            return None, "error: 未找到数据库链接"
        if connection.is_connected():
            try:
                cursor = connection.cursor()
                if database:
                    cursor.execute("USE {}".format(database))
                cursor.execute(sql)
                if cursor.with_rows:
                    # 处理查询结果
                    records = cursor.fetchall()
                    description = cursor.description
                    return records, [desc[0] for desc in description]
                else:
                    # 如果不是 SELECT 查询，则获取受影响的行数
                    affected_rows = cursor.rowcount
                    return None, f"Query executed successfully. Affected rows: {affected_rows}"
            finally:
                cursor.close()
        else:
            return None, "error: 数据库链接未打开"

    def database(self, host_info):
        connection = self.__conns.get(host_info.id, None)
        if connection is None or connection.is_connected() == False:
            connection, msg = self.connection(host_info)
            if connection is None:
                return None, msg

        if connection is not None:
            try:
                cursor = connection.cursor()
                cursor.execute("SHOW DATABASES")
                records = cursor.fetchall()
                return [record[0] for record in records], ''
            finally:
                cursor.close()
        return None, 'failure'

    def tables(self, id, database):
        connection = self.__conns.get(id, None)
        if connection is None or connection.is_connected() == False:
            return None, 'error: 数据库链接未打开'

        if connection is not None:
            try:
                cursor = connection.cursor()
                cursor.execute("USE {}".format(database))
                cursor.execute('SHOW TABLES')
                records = cursor.fetchall()
                return [record[0] for record in records], ''
            finally:
                cursor.close()
        return None, 'error: 数据库链接未打开'

    def connection(self, host_info):
        if host_info is not None:
            try:
                # 创建连接
                connection = mysql.connector.connect(
                    host=host_info.host,
                    port=host_info.port,
                    user=host_info.user_name,
                    password=host_info.password,
                    ssl_disabled=True
                )

                if connection.is_connected():
                    self.__conns[host_info.id] = connection
                    return connection, '[Connected to MySQL Server version:{}] {}:{}'.format(
                        connection.get_server_info(), host_info.host, host_info.port)
                else:
                    return None, 'Error while connecting to MySQL:{}, open failure'.format(host_info.host)
            except Error as e:
                msg = "Error while connecting to MySQL:{}, {}".format(host_info.host, e)
                return None, msg
