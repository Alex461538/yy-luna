from tokens import Token, get_from_source, TokenName, tokenEntries
from typing import List, Dict

def report_error(source: str, readPosition: int, message: str = "", file: str = "<unknown>", line: int = 0, column: int = 0):
    windowLength = 64
    windowBegin = max(0, readPosition - max(windowLength, column))
    windowEnd = min(len(source), readPosition + 2)

    what = "Error: " + message + "\nAt: " + file + " - line: " + str(line) + " - column: " + str(column) + "\n...\n" + source[windowBegin:windowEnd-windowBegin] + " ...\n" + ' ' * (column - 2) + "^^^^"
    raise Exception(what)

def is_char_blank(code: str):
    return code == ' ' or code == '\n' or code == '\t' or code == '\r'

def is_char_digit(code: str):
    return code >= '0' and code <= '9'

def is_char_alpha(code: str):
    return (code >= 'A' and code <= 'Z') or (code >= 'a' and code <= 'z')

def is_char_word(code: str):
    return is_char_alpha(code) or is_char_digit(code)

class Tokenizer:
    tokens: List[Token] = []
    bracketStack: List[str] = []
    line = 1
    column = 1

    def __init__(self):
        pass
    
    def reset(self):
        self.line = 1
        self.column = 1
        self.tokens = []
        self.bracketStack = []
    
    def read_from(self, source: str, name: str = "<unknown>"):
        self.reset()
        position: int = 0
        while (position < len(source)):
            delta: int = position
            # consumes whitespace
            while (position < len(source) and is_char_blank(source[position])):
                if (source[position] == '\n'):
                    self.line += 1
                    self.column = 1
                    delta = position + 1
                position += 1
            if (position >= len(source)):
                break
            if (is_char_alpha(source[position])):
                begin: int = position
                while (position < len(source) and (is_char_word(source[position]) or source[position] == '_')):
                    position += 1
                end: int = position
                length: int = end - begin
                self.column += position - delta
                name = source[begin:length+begin]
                self.tokens.append(Token(name, get_from_source(source, begin, length), begin, length, self.line, self.column - length))
            elif (is_char_digit(source[position])):
                begin: int = position
                if (source[position] == '0' and position < len(source) - 1 and (source[position + 1] == 'b' or source[position + 1] == 'x' or source[position + 1] == 'o')):
                    position += 1
                    if (position >= len(source)):
                        report_error(source, position, "Malformed number literal at end", name);
                    elif (source[position] == 'x'):
                        position += 1
                        while (position < len(source) and ((source[position] >= 'a' and source[position] <= 'f' or source[position] >= 'A' and source[position] <= 'F') or is_char_digit(source[position]) or source[position] == '_')):
                            position += 1
                        if (position - begin <= 2):
                            report_error(source, position, "Malformed hex literal, missing body", name, self.line, self.column + position - delta)
                        end: int = position
                        length: int = end - begin
                        self.column += position - delta
                        name = source[begin:length+begin]
                        self.tokens.append(Token( name, TokenName.HEX_LITERAL, begin, length, self.line, self.column - length))
                    elif (source[position] == 'b'):
                        position += 1
                        while (position < len(source) and ((source[position] == '0' or source[position] == '1') or source[position] == '_')):
                            position += 1
                        if (position - begin <= 2):
                            report_error(source, position, "Malformed bin literal, missing body", name, self.line, self.column + position - delta)
                        end: int = position
                        length: int = end - begin
                        self.column += position - delta
                        name = source[begin:length+begin]
                        self.tokens.append(Token( name, TokenName.BIN_LITERAL, begin, length, self.line, self.column - length))
                else:
                    begin: int = position
                    decimalPoint: int = 0
                    while (position < len(source)):
                        if (source[position] == '.'):
                            if (decimalPoint):
                                report_error(source, position, "Malformed number literal, misplaced decimal points", name, self.line, self.column + position - delta)
                            decimalPoint = position
                            position += 1
                        elif (is_char_digit(source[position]) or source[position] == '_'):
                            position += 1
                        else:
                            break
                    if (position - decimalPoint <= 1):
                        report_error(source, position, "Malformed number literal, misplaced decimal points", name, self.line, self.column + position - delta)
                    end: int = position
                    length: int = end - begin
                    self.column += position - delta
                    name = source[begin:length+begin]
                    self.tokens.append(Token( name, TokenName.INT_LITERAL if decimalPoint else TokenName.FLOAT_LITERAL , begin, length, self.line, self.column - length))
            elif (source[position] == '{'):
                self.bracketStack.append({
                    "name": "{",
                    "position": position,
                    "token-index": len(self.tokens)
                })
                self.tokens.append(Token( "{", TokenName.BRACKET_CURLY_OPEN, position, 1, self.line, self.column - 1))
                position += 1
            elif (source[position] == '('):
                self.bracketStack.append({
                    "name": "(",
                    "position": position,
                    "token-index": len(self.tokens)
                })
                self.tokens.append(Token( "(", TokenName.BRACKET_PAREN_OPEN, position, 1, self.line, self.column - 1))
                position += 1
            elif (source[position] == '['):
                self.bracketStack.append({
                    "name": "[",
                    "position": position,
                    "token-index": len(self.tokens)
                })
                self.tokens.append(Token( "[", TokenName.BRACKET_SQUARE_OPEN, position, 1, self.line, self.column - 1))
                position += 1
            elif (source[position] == '}'):
                if (len(self.bracketStack) == 0 or self.bracketStack[ len(self.bracketStack) - 1 ]["name"] != '{'):
                    report_error(source, position, "Unclosed curly brace", name, self.line, self.column)
                last_bracket: int = self.bracketStack[ len(self.bracketStack) -1 ]
                self.tokens[ last_bracket["token-index"] ].pairClosingToken = len(self.tokens)
                self.tokens.append(Token( "}", TokenName.BRACKET_CURLY_CLOSE, position, 1, self.line, self.column - 1))
                self.bracketStack.pop()
                position += 1
            elif (source[position] == ')'):
                if (len(self.bracketStack) == 0 or self.bracketStack[ len(self.bracketStack) - 1 ]["name"] != '('):
                    report_error(source, position, "Unclosed paren", name, self.line, self.column)
                last_bracket: int = self.bracketStack[ len(self.bracketStack) -1 ]
                self.tokens[ last_bracket["token-index"] ].pairClosingToken = len(self.tokens)
                self.tokens.append(Token( ")", TokenName.BRACKET_PAREN_CLOSE, position, 1, self.line, self.column - 1))
                self.bracketStack.pop()
                position += 1
            elif (source[position] == ']'):
                if (len(self.bracketStack) == 0 or self.bracketStack[ len(self.bracketStack) - 1 ]["name"] != '['):
                    report_error(source, position, "Unclosed paren", name, self.line, self.column)
                last_bracket: int = self.bracketStack[ len(self.bracketStack) -1 ]
                self.tokens[ last_bracket["token-index"] ].pairClosingToken = len(self.tokens)
                self.tokens.append(Token( "]", TokenName.BRACKET_SQUARE_CLOSE, position, 1, self.line, self.column - 1))
                self.bracketStack.pop()
                position += 1
            elif (source[position] == '"'):
                position += 1
                begin: int = position
                while (position < len(source) and source[position] != '"'):
                    if (source[position] == '\n'):
                        self.line += 1
                        self.column = 1
                        delta = position + 1
                    elif (source[position] == '\\'):
                        position += 1
                    position += 1
                if (len(source) - position < 1):
                    report_error(source, position, "Unclosed string at end of file", name, self.line, self.column)
                end: int = position
                length: int = end - begin
                name = source[begin:length+begin]
                self.tokens.append(Token( name, TokenName.STRING_LITERAL, begin, length, self.line, self.column - length))
                self.column += position - delta
                position += 1
            elif (source[position] == '/' and position < len(source) - 1 and source[position + 1] == '*'):
                while ( position < len(source) and not (source[position] == '*' and position < len(source) - 1 and source[position + 1] == '/')):
                    if (source[position] == '\n'):
                        self.line += 1
                        self.column = 1
                        delta = position + 1
                    position += 1
                if (len(source) - position < 2):
                    report_error(source, position, "Unclosed comment at end of file", name, self.line, self.column)
                position += 2
                self.column += position - delta
            else:
                begin: int = position
                while (position < len(source) and not (is_char_word(source[position]) or source[position] == '_' or is_char_blank(source[position]) or source[position] == '{' or source[position] == '}' or source[position] == '(' or source[position] == ')' or source[position] == '[' or source[position] == ']')):
                    position += 1
                end: int = position
                length: int = end - begin
                self.column += position - delta
                name = source[begin:length+begin]
                # Esto tiene un bug (Pero eso es lo irónico) no nos afecta, pero simplifica todo
                # Se bugea si hay operadores no consecutivos ej: + -> +=- Con un operador += intermedio que no existiría en esta teoría, mordería solo +
                oprPos = 0
                chunk = name[0]
                lastPos = 0
                while True:
                    if oprPos >= len(name):
                        break
                    # Puede morder más
                    elif oprPos < len(name) - 1 and (chunk + name[oprPos + 1]) in tokenEntries:
                        chunk += name[oprPos + 1]
                        oprPos += 1
                    # Acabamos de morder
                    elif chunk in tokenEntries:
                        self.tokens.append(Token( "" + chunk, get_from_source(source, begin + lastPos, len(chunk)), begin + lastPos, len(chunk), self.line, self.column - length + oprPos))
                        oprPos += 1
                        lastPos = oprPos
                        if oprPos < len(name):
                            chunk = name[oprPos]
                    else:
                        raise Exception(f"Illegal character sequence: {chunk} {chunk + name[oprPos]}")
        self.tokens.append(Token("", TokenName.TK_EOF, 0, 0))
        if (len(self.bracketStack) > 0):
            raise "There are unclosed parens"