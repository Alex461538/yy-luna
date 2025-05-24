from pathlib import Path
import uuid

from local_config import Config
from tokenizer import Tokenizer
from tokens import Token, TokenInfo, TokenName

def expect(token: Token, type: TokenInfo):
    if token.info != type:
        raise "Expected"

GENERAL_PACKAGE_FOLDER = "@packages"

class NodeType:
    NODE_DIRECTORY = "directory"
    NODE_SOURCE = "source"
    NODE_PROJECT = "project"

class TreeNode:
    def __init__(self, type: str):
        self.id = str( uuid.uuid4() )
        self.type = type
        self.sources = {}
        self.subNodes = {}
        self.path = ""
        self.name = ""
        self.topProject = None
        self.root = None
        self.preprocessed = False
        self.parsed = False
        self.dependencies = []
    
    def cd(self, name):
        if (name in self.subNodes):
            return self.subNodes[name]
        newDir = TreeNode(NodeType.NODE_DIRECTORY)
        newDir.name = name
        if self.type == NodeType.NODE_PROJECT:
            newDir.topProject = self
        elif self.topProject:
            newDir.topProject = self.topProject
        if not self.root:
            newDir.root = self
        else:
            newDir.root = self.root
        self.subNodes[name] = newDir
        return newDir
    
    def __repr__(self, level = 0):
        rep = ""
        if self.type == NodeType.NODE_DIRECTORY:
            rep += ". " * level + ( "📂 " if self.path != "" else  "🌿 ") + self.name + f" \"{self.path}\""
        elif self.type == NodeType.NODE_PROJECT:
            rep += ". " * level + "🔑 " + self.name + f" \"{self.path}\""
        else:
            rep += ". " * level + "🌙 " + self.name + f" \"{self.path}\""
        if len(self.sources):
            rep += "\n" + "\n".join([ node.__repr__(level + 1) for node in self.sources.values() ])
        if len(self.subNodes):
            rep += "\n" + "\n".join([ node.__repr__(level + 1) for node in self.subNodes.values() ])
        return rep
    
    def project_config_from_path(self, root):
        self.path = Path(root).resolve()
        self.config = Config.get_for_project(self.path)
    
    def addSource(self, path):
        newSource = TreeNode(NodeType.NODE_SOURCE)
        newSource.path = path
        self.sources[ newSource.id ] = newSource
        if self.type == NodeType.NODE_PROJECT:
            newSource.topProject = self
        elif self.topProject:
            newSource.topProject = self.topProject
        if not self.root:
            newSource.root = self
        else:
            newSource.root = self.root
        return newSource
    
    def append(self, query: str, CAN_USE_PATHS = True, CAN_USE_INCLUDE = True, ALLOWED_PACKAGES = None):
        # print("appending: ", query, " from ", self.path)

        relativePath = (self.path.parent / query).resolve() if self.path else ""

        if self.path and CAN_USE_PATHS and relativePath.exists():
            if relativePath.is_relative_to(self.topProject.path) and CAN_USE_PATHS:
                subPath = Path(relativePath.parent).relative_to(self.topProject.path)

                rootDir = self.topProject
                rootPath = self.topProject.path

                for i in subPath.parts:
                    rootDir = rootDir.cd(i)
                    rootPath = Path(rootPath) / i
                    rootDir.path = rootPath
                    
                newSource = rootDir.addSource(rootPath / relativePath.name)
                newSource.name = relativePath.name
                newSource.preprocess()

                return newSource
            else:
                raise Exception("Can't access files outside of the project's scope")
        elif CAN_USE_INCLUDE and self.root:
            for package in self.root.config.packages:
                if package["name"] == query:
                    packagesDir = self.root.cd(GENERAL_PACKAGE_FOLDER)
                    packageDir = packagesDir.cd(package["name"])
                    packageDir.path = Path(package["path"]) if Path(package["path"]).is_absolute() else (Path(self.topProject.path) / package["path"]).resolve()
                    packageDir.type = NodeType.NODE_PROJECT
                    packageDir.project_config_from_path(packageDir.path)
                    packageDir.preprocess()

                    # print("Found it! ", query, packageDir.path)
                    return packageDir
        else:
            raise Exception("Sources not found for this query: ", query, " from ", self.path)
    
    def preprocess(self):
        if self.preprocessed:
            return
        self.preprocessed = True
        if self.type == NodeType.NODE_PROJECT:
            if self.config:
                self.main = self.addSource(Path(self.path) / self.config.main)
                self.main.name = Path(self.config.main).name
                self.main.preprocess()
            else:
                raise Exception("Can't have a project without a config.json file")
        elif self.type == NodeType.NODE_SOURCE:
            self.source_load()
            self.source_preprocess()
            # print("source", [token.name for token in self.tokens])
    
    def source_load(self):
        self.text = open(self.path, "r").read()
        t = Tokenizer()
        t.read_from(self.text, self.name)
        self.tokens = t.tokens
    
    def source_preprocess(self):
        tokens = []
        index = 0
        while index < len(self.tokens):
            token = self.tokens[index]
            if token.info == TokenName.LIB_IMPORT:
                index += 1
                token = self.tokens[index]
                expect(token, TokenName.STRING_LITERAL)

                dependency = self.append(token.name)

                if not (dependency in self.dependencies):
                    self.dependencies.append(dependency)

                if (self.tokens[index + 1].info == TokenName.TK_EOL):
                    index += 1
            else:
                tokens.append(self.tokens[index])
            index += 1
        self.tokens = tokens