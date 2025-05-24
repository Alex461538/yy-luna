import json
import dataclasses
from typing import List, Dict, Any
from packages import Package

CONFIG_JSON_FILE = "config.json"

def get_input(message: str, default: str | None = None):

    raw = input(message)
    while raw == "" and default == None:
        print("This is a required field")
        raw = input(message)
    return raw if raw else default

@dataclasses.dataclass
class Config:
    name: str = ""
    version: str = ""
    description: str = ""
    keywords: List[str] = dataclasses.field(default_factory=list)
    license: str = ""
    author: str = ""
    main: str = ""
    packages: List[Package] = dataclasses.field(default_factory=list)
    dev_packages: List[Package] = dataclasses.field(default_factory=list)
    build: Dict = dataclasses.field(default_factory=dict)

    @classmethod
    def from_file(cls, file_path: str) -> "Config":
        with open(file_path, "r", encoding="utf-8") as f:
            data = json.load(f)
        return cls(**data)

    def to_file(self, file_path: str):
        with open(file_path, "w", encoding="utf-8") as f:
            json.dump(dataclasses.asdict(self), f, indent=4, ensure_ascii=False)

    def __repr__(self):
        return json.dumps(self.__dict__, indent=4, ensure_ascii=False)
    
    @staticmethod
    def get_for_project(directory: str) -> "Config":
        config = Config()
        try:
            config = Config.from_file(f"{directory}/{CONFIG_JSON_FILE}")
        except TypeError:
            raise Exception(f"config.json is not well formed for: {directory}")
        except FileNotFoundError:
            raise Exception(f"Project config not found: {directory}")
        return config
    
    @classmethod
    def from_console(cls):
        config = Config()
        config.name = get_input("Project name: ")
        config.version = get_input("Version: (1.0.0) ", default="1.0.0")
        config.description = get_input("Description: (none) ", default="")
        config.keywords = get_input("Keywords: (none) ", default="").split(",")
        config.license = get_input("LICENSE: (unknown) ", default="unknown")
        config.author = get_input("Author: (unknown) ", default="")
        config.main = get_input("Entry point: (main.lyn) ", default="main.lyn")
        return config