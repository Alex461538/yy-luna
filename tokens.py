from enum import Enum
from dataclasses import dataclass, field

class TokenKind(Enum):
    NONE = 0
    IDENTIFIER = 1
    CONTROL = 2
    CALLING = 3
    PACKAGE = 4
    DECLARE = 5
    ACCESS = 6
    OOP = 7
    PASSING = 8
    CONSTANT = 9
    OPERATOR = 10
    TOKEN = 11
    LITERAL = 12
    BLANK = 13
    BRACKET = 14

class OperatorPriority(Enum):
    MEMBER_ACCESS = 0
    INCREMENT = 10
    BITWISE_NOT = 20
    ARITHMETIC_MULTIPLY = 30
    ARITHMETIC_ADD = 40
    BITWISE_SHIFT = 50
    RELATION_COMPARISON = 60
    EQUALITY_COMPARISON = 70
    BITWISE_AND = 80
    BITWISE_XOR = 90
    BITWISE_OR = 100
    LOGICAL = 110
    ASSING = 120

@dataclass
class TokenInfo:
    id: int = 0
    type: TokenKind = field(default_factory=lambda: TokenKind.NONE)
    priority: int = 0

class TokenName:
    UNSET = TokenInfo(0, TokenKind.NONE)
    TK_EOF = TokenInfo(1, TokenKind.TOKEN)
    COLON = TokenInfo(2, TokenKind.TOKEN)
    TK_EOL = TokenInfo(3, TokenKind.TOKEN)
    CONTROL_IF = TokenInfo(0, TokenKind.CONTROL)
    CONTROL_ELSE = TokenInfo(1, TokenKind.CONTROL)
    CONTROL_DO = TokenInfo(2, TokenKind.CONTROL)
    CONTROL_WHILE = TokenInfo(3, TokenKind.CONTROL)
    CONTROL_FOR = TokenInfo(4, TokenKind.CONTROL)
    CONTROL_FOR_IN = TokenInfo(5, TokenKind.CONTROL)
    CONTROL_YIELD = TokenInfo(6, TokenKind.CONTROL)
    CONTROL_RETURN = TokenInfo(7, TokenKind.CONTROL)
    CONTROL_CONTINUE = TokenInfo(8, TokenKind.CONTROL)
    CONTROL_BREAK = TokenInfo(9, TokenKind.CONTROL)
    CONTROL_SWITCH = TokenInfo(10, TokenKind.CONTROL)
    CONTROL_SWITCH_CASE = TokenInfo(11, TokenKind.CONTROL)
    CONTROL_SWITCH_DEFAULT = TokenInfo(12, TokenKind.CONTROL)
    CALL_ASYNC = TokenInfo(1, TokenKind.CALLING)
    CALL_AWAIT = TokenInfo(2, TokenKind.CALLING)
    LIB_IMPORT = TokenInfo(1, TokenKind.PACKAGE)
    LIB_AS = TokenInfo(2, TokenKind.PACKAGE)
    LIB_FROM = TokenInfo(3, TokenKind.PACKAGE)
    LIB_EXTERN = TokenInfo(4, TokenKind.PACKAGE)
    LIB_EXPORT = TokenInfo(5, TokenKind.PACKAGE)
    DECL_VAR = TokenInfo(1, TokenKind.DECLARE)
    DECL_CONST = TokenInfo(2, TokenKind.DECLARE)
    DECL_PTR = TokenInfo(3, TokenKind.DECLARE)
    DECL_TYPE = TokenInfo(4, TokenKind.DECLARE)
    DECL_CLASS = TokenInfo(5, TokenKind.DECLARE)
    DECL_STRUCT = TokenInfo(6, TokenKind.DECLARE)
    DECL_ENUM = TokenInfo(7, TokenKind.DECLARE)
    DECL_FUNCTION = TokenInfo(8, TokenKind.DECLARE)
    DECL_NAMESPACE = TokenInfo(9, TokenKind.DECLARE)
    DECL_PADD = TokenInfo(10, TokenKind.DECLARE)
    DECL_TEMPLATE = TokenInfo(11, TokenKind.DECLARE)
    DECL_STATIC = TokenInfo(12, TokenKind.DECLARE)
    ACCESS_PUBLIC = TokenInfo(1, TokenKind.ACCESS)
    ACCESS_PRIVATE = TokenInfo(2, TokenKind.ACCESS)
    ACCESS_PROTECTED = TokenInfo(3, TokenKind.ACCESS)
    ACCESS_INTERNAL = TokenInfo(4, TokenKind.ACCESS)
    OOP_VIRTUAL = TokenInfo(1, TokenKind.OOP)
    OOP_OVERRIDE = TokenInfo(2, TokenKind.OOP)
    OOP_INHERITS = TokenInfo(3, TokenKind.OOP)
    OOP_THIS = TokenInfo(4, TokenKind.OOP)
    PASS_SHARE = TokenInfo(1, TokenKind.PASSING)
    PASS_TAKE = TokenInfo(2, TokenKind.PASSING)
    PASS_COPY = TokenInfo(3, TokenKind.PASSING)
    CONST_TRUE = TokenInfo(1, TokenKind.CONSTANT)
    CONST_FALSE = TokenInfo(2, TokenKind.CONSTANT)
    CONST_NULL = TokenInfo(3, TokenKind.CONSTANT)
    CONST_NAN = TokenInfo(4, TokenKind.CONSTANT)
    CONST_INFINITY = TokenInfo(5, TokenKind.CONSTANT)
    CONST_ANNOTATION = TokenInfo(6, TokenKind.CONSTANT)
    OPERATOR_NEW = TokenInfo(1, TokenKind.OPERATOR, OperatorPriority.BITWISE_NOT)
    OPERATOR_DROP = TokenInfo(2, TokenKind.OPERATOR, OperatorPriority.BITWISE_NOT)
    OPERATOR_ADD = TokenInfo(3, TokenKind.OPERATOR, OperatorPriority.ARITHMETIC_ADD)
    OPERATOR_MULTIPLY = TokenInfo(4, TokenKind.OPERATOR, OperatorPriority.ARITHMETIC_MULTIPLY)
    OPERATOR_SUB = TokenInfo(5, TokenKind.OPERATOR, OperatorPriority.ARITHMETIC_ADD)
    OPERATOR_DIVIDE = TokenInfo(6, TokenKind.OPERATOR, OperatorPriority.ARITHMETIC_MULTIPLY)
    OPERATOR_MODULE = TokenInfo(7, TokenKind.OPERATOR, OperatorPriority.ARITHMETIC_MULTIPLY)
    OPERATOR_BIT_AND = TokenInfo(8, TokenKind.OPERATOR, OperatorPriority.BITWISE_AND)
    OPERATOR_BIT_OR = TokenInfo(9, TokenKind.OPERATOR, OperatorPriority.BITWISE_OR)
    OPERATOR_BIT_XOR = TokenInfo(10, TokenKind.OPERATOR, OperatorPriority.BITWISE_XOR)
    OPERATOR_BIT_NOT = TokenInfo(11, TokenKind.OPERATOR, OperatorPriority.BITWISE_NOT)
    OPERATOR_BIT_LSHIFT = TokenInfo(12, TokenKind.OPERATOR, OperatorPriority.BITWISE_SHIFT)
    OPERATOR_BIT_RSHIFT = TokenInfo(13, TokenKind.OPERATOR, OperatorPriority.BITWISE_SHIFT)
    OPERATOR_ASSIGN = TokenInfo(14, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_ADD = TokenInfo(15, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_MULTIPLY = TokenInfo(16, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_SUB = TokenInfo(17, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_DIVIDE = TokenInfo(18, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_MODULE = TokenInfo(19, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_BIT_AND = TokenInfo(20, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_BIT_OR = TokenInfo(21, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_BIT_XOR = TokenInfo(22, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_BIT_NOT = TokenInfo(23, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_BIT_LSHIFT = TokenInfo(24, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_BIT_RSHIFT = TokenInfo(25, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_STICK_ADD = TokenInfo(26, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_STICK_MULTIPLY = TokenInfo(27, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_STICK_SUB = TokenInfo(28, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_STICK_DIVIDE = TokenInfo(29, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_SAFE_ADD = TokenInfo(30, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_SAFE_MULTIPLY = TokenInfo(31, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_SAFE_SUB = TokenInfo(32, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_EQ_SAFE_DIVIDE = TokenInfo(33, TokenKind.OPERATOR, OperatorPriority.ASSING)
    OPERATOR_LESS = TokenInfo(34, TokenKind.OPERATOR, OperatorPriority.RELATION_COMPARISON)
    OPERATOR_MORE = TokenInfo(35, TokenKind.OPERATOR, OperatorPriority.RELATION_COMPARISON)
    OPERATOR_LESS_EQ = TokenInfo(36, TokenKind.OPERATOR, OperatorPriority.RELATION_COMPARISON)
    OPERATOR_MORE_EQ = TokenInfo(37, TokenKind.OPERATOR, OperatorPriority.RELATION_COMPARISON)
    OPERATOR_EQUALS = TokenInfo(38, TokenKind.OPERATOR, OperatorPriority.EQUALITY_COMPARISON)
    OPERATOR_NOT_EQ = TokenInfo(39, TokenKind.OPERATOR, OperatorPriority.EQUALITY_COMPARISON)
    OPERATOR_OR = TokenInfo(40, TokenKind.OPERATOR, OperatorPriority.LOGICAL)
    OPERATOR_XOR = TokenInfo(41, TokenKind.OPERATOR, OperatorPriority.LOGICAL)
    OPERATOR_AND = TokenInfo(42, TokenKind.OPERATOR, OperatorPriority.LOGICAL)
    OPERATOR_NOT = TokenInfo(43, TokenKind.OPERATOR, OperatorPriority.BITWISE_NOT)
    OPERATOR_CALL = TokenInfo(44, TokenKind.OPERATOR)
    OPERATOR_INDEX = TokenInfo(45, TokenKind.OPERATOR)
    OPERATOR_MEMBER_ACCESS = TokenInfo(46, TokenKind.OPERATOR, OperatorPriority.MEMBER_ACCESS)
    OPERATOR_INCREMENT = TokenInfo(47, TokenKind.OPERATOR, OperatorPriority.INCREMENT)
    OPERATOR_DECREMENT = TokenInfo(50, TokenKind.OPERATOR, OperatorPriority.INCREMENT)
    STRING_LITERAL = TokenInfo(1, TokenKind.LITERAL)
    INT_LITERAL = TokenInfo(2, TokenKind.LITERAL)
    FLOAT_LITERAL = TokenInfo(3, TokenKind.LITERAL)
    HEX_LITERAL = TokenInfo(4, TokenKind.LITERAL)
    BIN_LITERAL = TokenInfo(5, TokenKind.LITERAL)
    COMMENT_LITERAL = TokenInfo(1, TokenKind.BLANK)
    BRACKET_PAREN_OPEN = TokenInfo(1, TokenKind.BRACKET)
    BRACKET_PAREN_CLOSE = TokenInfo(2, TokenKind.BRACKET)
    BRACKET_CURLY_OPEN = TokenInfo(3, TokenKind.BRACKET)
    BRACKET_CURLY_CLOSE = TokenInfo(4, TokenKind.BRACKET)
    BRACKET_SQUARE_OPEN = TokenInfo(5, TokenKind.BRACKET)
    BRACKET_SQUARE_CLOSE = TokenInfo(6, TokenKind.BRACKET)

"""
    Dios los bendiga a todos mis socios 😭

    DG nicky
    Arrow party
    Brei Junior
    Ete Sech
    <La quemona>
"""

tokenEntries = {
    ";": TokenName.TK_EOL,
    ":": TokenName.COLON,
    "and": TokenName.OPERATOR_AND,
    "as": TokenName.LIB_AS,
    "async": TokenName.CALL_ASYNC,
    "await": TokenName.CALL_AWAIT,
    "break": TokenName.CONTROL_BREAK,
    "case": TokenName.CONTROL_SWITCH_CASE,
    "class": TokenName.DECL_CLASS,
    "const": TokenName.DECL_CONST,
    "continue": TokenName.CONTROL_CONTINUE,
    "copy": TokenName.PASS_COPY,
    "default": TokenName.CONTROL_SWITCH_DEFAULT,
    "do": TokenName.CONTROL_DO,
    "drop": TokenName.OPERATOR_DROP,
    "else": TokenName.CONTROL_ELSE,
    "enum": TokenName.DECL_ENUM,
    "export": TokenName.LIB_EXPORT,
    "extern": TokenName.LIB_EXTERN,
    "false": TokenName.CONST_FALSE,
    "for": TokenName.CONTROL_FOR,
    "from": TokenName.LIB_FROM,
    "function": TokenName.DECL_FUNCTION,
    "if": TokenName.CONTROL_IF,
    "import": TokenName.LIB_IMPORT,
    "in": TokenName.CONTROL_FOR_IN,
    "infinity": TokenName.CONST_INFINITY,
    "inherits": TokenName.OOP_INHERITS,
    "internal": TokenName.ACCESS_INTERNAL,
    "namespace": TokenName.DECL_NAMESPACE,
    "nan": TokenName.CONST_NAN,
    "new": TokenName.OPERATOR_NEW,
    "not": TokenName.OPERATOR_NOT,
    "null": TokenName.CONST_NULL,
    "or": TokenName.OPERATOR_OR,
    "override": TokenName.OOP_OVERRIDE,
    "padd": TokenName.DECL_PADD,
    "private": TokenName.ACCESS_PRIVATE,
    "protected": TokenName.ACCESS_PROTECTED,
    "ptr": TokenName.DECL_PTR,
    "public": TokenName.ACCESS_PUBLIC,
    "return": TokenName.CONTROL_RETURN,
    "share": TokenName.PASS_SHARE,
    "static": TokenName.DECL_STATIC,
    "struct": TokenName.DECL_STRUCT,
    "switch": TokenName.CONTROL_SWITCH,
    "take": TokenName.PASS_TAKE,
    "template": TokenName.DECL_TEMPLATE,
    "this": TokenName.OOP_THIS,
    "true": TokenName.CONST_TRUE,
    "type": TokenName.DECL_TYPE,
    "var": TokenName.DECL_VAR,
    "virtual": TokenName.OOP_VIRTUAL,
    "while": TokenName.CONTROL_WHILE,
    "xor": TokenName.OPERATOR_XOR,
    "yield": TokenName.CONTROL_YIELD,
    "!=": TokenName.OPERATOR_NOT_EQ,
    "%": TokenName.OPERATOR_MODULE,
    "%=": TokenName.OPERATOR_EQ_MODULE,
    "&": TokenName.OPERATOR_BIT_AND,
    "&=": TokenName.OPERATOR_EQ_BIT_AND,
    "*": TokenName.OPERATOR_MULTIPLY,
    "*=": TokenName.OPERATOR_EQ_MODULE,
    "*?=": TokenName.OPERATOR_EQ_SAFE_MULTIPLY,
    "*|=": TokenName.OPERATOR_EQ_STICK_MULTIPLY,
    "+": TokenName.OPERATOR_ADD,
    "++": TokenName.OPERATOR_INCREMENT,
    "--": TokenName.OPERATOR_DECREMENT,
    "+=": TokenName.OPERATOR_EQ_ADD,
    "+?=": TokenName.OPERATOR_EQ_SAFE_ADD,
    "+|=": TokenName.OPERATOR_EQ_STICK_ADD,
    "-": TokenName.OPERATOR_SUB,
    "-=": TokenName.OPERATOR_EQ_SUB,
    "-?=": TokenName.OPERATOR_EQ_SAFE_SUB,
    "-|=": TokenName.OPERATOR_EQ_STICK_SUB,
    ".": TokenName.OPERATOR_MEMBER_ACCESS,
    "/": TokenName.OPERATOR_DIVIDE,
    "/=": TokenName.OPERATOR_EQ_DIVIDE,
    "/?=": TokenName.OPERATOR_EQ_SAFE_DIVIDE,
    "/|=": TokenName.OPERATOR_EQ_STICK_DIVIDE,
    "<": TokenName.OPERATOR_LESS,
    "<<": TokenName.OPERATOR_BIT_LSHIFT,
    "<<=": TokenName.OPERATOR_EQ_BIT_LSHIFT,
    "<=": TokenName.OPERATOR_LESS_EQ,
    "=": TokenName.OPERATOR_ASSIGN,
    "==": TokenName.OPERATOR_EQUALS,
    ">": TokenName.OPERATOR_MORE,
    ">=": TokenName.OPERATOR_MORE_EQ,
    ">>": TokenName.OPERATOR_BIT_RSHIFT,
    ">>=": TokenName.OPERATOR_EQ_BIT_RSHIFT,
    "@": TokenName.CONST_ANNOTATION,
    "^": TokenName.OPERATOR_BIT_XOR,
    "^=": TokenName.OPERATOR_EQ_BIT_XOR,
    "|": TokenName.OPERATOR_BIT_OR,
    "|=": TokenName.OPERATOR_EQ_BIT_OR,
    "~": TokenName.OPERATOR_BIT_NOT
}

enclosures = [
    ["\"", "\"", TokenName.STRING_LITERAL],
    ["/*", "*/", TokenName.COMMENT_LITERAL]
]

@dataclass
class Token:
    name: str = ""
    info: TokenInfo = field(default_factory=lambda: TokenInfo())
    offset: int = 0
    length: int = 0
    line: int = 0
    column: int = 0
    pairClosingToken: int = -1

def get_from_source(source: str, begin, length):
    try:
        return tokenEntries[ source[begin:length+begin] ]
    except:
        return TokenInfo(0, TokenKind.IDENTIFIER)