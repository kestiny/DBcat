# -*- coding: utf-8 -*-


class HostInfo:
    def __init__(self, id, name, host, port, user_name, password, type, environment):
        self.id = id
        self.name = name
        self.host = host
        self.port = port
        self.user_name = user_name
        self.password = password
        self.type = type
        self.environment = environment

    @classmethod
    def from_json(cls, data):
        return cls(data["id"], data["name"], data["host"], data["port"], data["userName"], data["password"],
                   data["type"], data["environment"])

    def to_json(self):
        return {
            "id": self.id,
            "name": self.name,
            "host": self.host,
            "port": self.port,
            "userName": self.user_name,
            "password": self.password,
            "type": self.type,
            "environment": self.environment
        }
