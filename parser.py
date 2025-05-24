from tokens import Token, TokenName, TokenInfo
from tokenizer import Tokenizer
from typing import List

from build_tree import TreeNode, NodeType

import uuid

def parseSource(node: TreeNode):
    for dep in node.dependencies:
        parse(dep)
    print("Parsing source: ", node.name, node.path)

def parse(node: TreeNode):
    if node.parsed: # Evita duplicación y e inclusión circular
        return
    node.parsed = True
    if node.type == NodeType.NODE_PROJECT:
        parse(node.main) # Parse entry point
    elif node.type == NodeType.NODE_SOURCE:
        parseSource(node)
    else:
        raise Exception("For some strange reason, i'm parsing a folder")