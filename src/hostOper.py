from pathlib import Path
import json

from src.hostInfo import HostInfo

dbcat_setting_file = Path.home() / "Documents" / "DBCat" / ".conn"

class Singleton(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]
    
# 数组的序列化和反序列化
def hosts_to_json(hosts):
    """将HostInfo对象数组转换为字典数组"""
    return [p.to_json() for p in hosts]

def hosts_from_json(hosts_json):
    """从字典数组构建HostInfo对象数组"""
    return [HostInfo.from_json(host) for host in hosts_json]

class HostOper(metaclass=Singleton):
    def __init__(self) -> None:
        self.hosts = []
        with open(dbcat_setting_file, 'r') as file:
            try:
                hosts = hosts_from_json(json.load(file))
                self.hosts = sorted(hosts, key=lambda x: x.id)
            except json.JSONDecodeError as e:
                self.hosts = []

    def get_hosts(self):
        return self.hosts
    
    def find_host(self, id):
        matching_hosts = [host for host in self.hosts if host.id == id]
        return matching_hosts[0] if matching_hosts else None

    def add_host(self, new_host):
        print("add host by {}".format(new_host.to_json()))
        id = self.hosts[-1].id if len(self.hosts) > 0 else 100
        new_host.id = id + 1
        self.hosts.append(new_host)
        self.hosts = sorted(self.hosts, key=lambda x: x.id)

        self.save_hosts_to_file()
    
    def update_host(self, new_host):
        matching_hosts = [host for host in self.hosts if host.id == new_host.id]
        target_host = matching_hosts[0] if matching_hosts else None
        if target_host is not None:
            # update
            print("update host by {}".format(new_host.to_json()))
            target_host.name = new_host.name
            target_host.host = new_host.host
            target_host.port = new_host.port
            target_host.userName = new_host.userName
            target_host.password = new_host.password
            target_host.type = new_host.type
            target_host.environment = new_host.environment
            self.save_hosts_to_file()
        else:
            # add
            self.add_host(new_host)

    def del_host(self, id):
        print("delete host by id {}".format(id))
        self.hosts = [host for host in self.hosts if host.id != id]
        self.save_hosts_to_file()

    def save_hosts_to_file(self):
        print("save_hosts_to_file")
        with open(dbcat_setting_file, 'w') as file:
            json.dump(hosts_to_json(self.hosts), file)