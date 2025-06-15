import { syntaxError } from "./errors";
import { LEGAL_EXTRA_CHARS, OPERATOR_MAP, TOKEN_MAP, Token, TokenName } from "./token";

export function tokenize(code: string, sourceFile: string = "unknown"): Token[] {
    let currentChar = 0;
    let currentColumn = 0;
    let currentLine = 0;
    let docstring = "";
    let tokens: Token[] = [];
    let text: string | undefined = "";

    let bracketStack: { type: number, index: number }[] = []

    let munchTrailing = () => {
        let munchedAnything = false;
        while (currentChar < code.length && code.charCodeAt(currentChar) <= 32) {
            munchedAnything = true;
            if (code[currentChar] == '\n') {
                currentColumn = 0;
                currentLine++;
            }
            else {
                currentColumn++;
            }
            currentChar++;
        }

        return munchedAnything;
    }

    let munchComment = (): string | undefined => {
        let munchedText = "";

        if (!(currentChar < code.length - 1 && code[currentChar] == '/' && code[currentChar + 1] == '*')) {
            return;
        }

        currentChar += 2

        while (currentChar < code.length - 1 /* Sufficient space for munching the cositos */ && !(code[currentChar + 1] == '/' && code[currentChar] == '*')) {
            munchedText += code[currentChar];

            if (code[currentChar] == '\n') {
                currentColumn = 0;
                currentLine++;
            }
            else {
                currentColumn++;
            }
            currentChar++;
        }

        if (!(currentChar < code.length - 1)) {
            // unclosed trash
            syntaxError(`Unclosed comment at end of source`, currentChar, currentLine, currentColumn, code ?? "", sourceFile);
        }

        currentChar += 2

        return munchedText;
    }

    let munchNumber = (): string | undefined => {
        let munchedText = "";

        let counter = 0;

        while (currentChar < code.length) {
            if ((code.charCodeAt(currentChar) >= 48 && code.charCodeAt(currentChar) <= 57) || (counter && ['.', 'x', 'o', 'b'].includes(code[currentChar]))) {
                munchedText += code[currentChar];
                currentChar++;
                counter++;
                currentColumn++;
            }
            else {
                break
            }
        }

        return munchedText;
    }

    let munchString = (): string | undefined => {
        let munchedText = "";

        if (!(currentChar < code.length && code[currentChar] == '"')) {
            return;
        }

        currentChar++;

        while (currentChar < code.length && !(code[currentChar] == '"')) {
            munchedText += code[currentChar];

            if (code[currentChar] == '\n') {
                currentColumn = 0;
                currentLine++;
            }
            else {
                currentColumn++;
            }
            currentChar++;
        }

        if (!(currentChar < code.length)) {
            // unclosed trash
            syntaxError(`Unclosed string at end of source`, currentChar, currentLine, currentColumn, code ?? "", sourceFile);
        }

        currentChar++

        return munchedText;
    }

    let munchWord = (): string | undefined => {
        let munchedText = "";

        let counter = 0;

        while (currentChar < code.length) {
            if ((code.charCodeAt(currentChar) >= 65 && code.charCodeAt(currentChar) <= 90) || (code.charCodeAt(currentChar) >= 97 && code.charCodeAt(currentChar) <= 122) || (counter && (code.charCodeAt(currentChar) >= 48 && code.charCodeAt(currentChar) <= 57)) || code.charAt(currentChar) == '_') {
                munchedText += code[currentChar];
                currentChar++;
                counter++;
                currentColumn++;
            }
            else {
                break
            }
        }

        return munchedText;
    }

    while (currentChar < code.length) {
        let lastPosition = currentChar;

        if (munchTrailing()) {
            // Do Nothing Xd
        }
        else if (text = munchComment()) {
            docstring = text;
            // Useful for docs or something of that wtfness
            // console.log(docstring);
        }
        else if (text = munchNumber()) {
            tokens.push({
                kind: 'number',
                text: text + "",
                line: currentLine,
                column: currentColumn,
                index: currentChar
            })
        }
        else if (text = munchString()) {
            tokens.push({
                kind: 'string',
                text: text + "",
                line: currentLine,
                column: currentColumn,
                index: currentChar
            })
        }
        else if (text = munchWord()) {
            let info: any = TOKEN_MAP[text];

            tokens.push({
                kind: info?.kind || 'identifier',
                text: text + "",
                code: info?.code ?? -1,
                line: currentLine,
                column: currentColumn,
                index: currentChar
            })
        }
        else {
            text = "";
            let info = undefined;
            let tempinfo = undefined;

            /**
             * The green pipe black chained muncher (GPBCM)
             */

            while (currentChar < code.length && LEGAL_EXTRA_CHARS.has(code[currentChar])) {
                if ( (tempinfo = OPERATOR_MAP[text + code[currentChar]]) )
                {
                    info = tempinfo;
                    text += code[currentChar]
                }
                else
                {
                    break;
                }
                currentChar++;
                currentColumn++;
            }
            
            if (text.length <= 0 || !info)
            {
                syntaxError(`Illegal character sequence`, currentChar, currentLine, currentColumn, code ?? "", sourceFile);
                return [];
            }

            if (info.paren)
            {
                if (info.opening)
                {
                    bracketStack.push({ type: info.code ?? -1, index: tokens.length })
                }
                else
                {
                    let lastBracket = bracketStack[ bracketStack.length - 1 ];
                    if (info.alternate != lastBracket?.type)
                    {
                        syntaxError(`Bracket closing mismatch`, currentChar, currentLine, currentColumn, code ?? "", sourceFile);
                    }
                    tokens[ lastBracket.index ].closingPair = tokens.length;
                    bracketStack.pop()
                }
            }

            tokens.push({
                kind: info.kind ?? 'none',
                text: text + "",
                code: info.code,
                paren: !!info.paren,
                line: currentLine,
                column: currentColumn,
                index: currentChar
            })
        }
    }

    // Implementación
    return tokens;
}