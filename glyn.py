import os
import argparse
from pathlib import Path

from local_config import Config

from build_tree import TreeNode, NodeType

from parser import parse

GLYN_HOME = os.getenv("GLYN_HOME")

def build_project(args):
    project = TreeNode(NodeType.NODE_PROJECT)
    project.project_config_from_path(args.d)
    project.preprocess()

    parse(project)

    print(" *" * 20, "\n", project, "\n", "* " * 20)

def init_project(args):
    root = os.path.abspath(args.d) if args.d else os.getcwd()
    config = Config.from_console()
    project_root = (Path(root) / config.name).absolute()
    project_root.mkdir(parents=True, exist_ok=True)
    config.to_file(project_root / "config.json")

def main():
    argParser = argparse.ArgumentParser(
                    prog='ProgramName',
                    description='What the program does',
                    epilog='Text at the bottom of help')
    argParser.add_argument("action", help="specifies what to do :D")
    argParser.add_argument("--d", nargs="?", help="specifies what to do :D")
    argParser.add_argument("--f", nargs="?", help="specifies what to do :D")
    args = argParser.parse_args()
    if args.action == "build" and args.d:
        args.d = os.path.abspath(args.d)
        build_project(args)
    elif args.action == "init":
        init_project(args)

if __name__ == "__main__":
    main()