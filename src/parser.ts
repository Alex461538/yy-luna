import { Symbol, SymbolType } from "./symbol";
import { parserError, syntaxError } from "./errors";
import { Token, TokenKind, TokenName } from "./token";
import * as util from 'util';

const UNNAMED_MEMBER_NAME = '<unnamed>'

export function parse(source: any) {
    let currentToken: Token | undefined;
    let cursor = 0;

    let scopeStack: (Symbol | any)[] = []

    let consume = (query: { kind?: TokenKind, code?: number }) => {
        let k = !!query.kind;
        let c = !!query.code;
        let K = currentToken?.kind == query.kind;
        let C = currentToken?.code == query.code;
        /*
            Para los jovenes de sistemas digitales... Pélelo, porque aquí usé un mapa carnoso.
        */
        if (
            (K && C) || (!k && C) || (!c && K)
        ) {
            cursor++;
            let old = currentToken;
            currentToken = source.tokens[cursor];
            return old;
        } else {
            return undefined;
        }
    }

    let expect = (query: { kind?: TokenKind, code?: number }) => {
        let token = consume({ kind: query.kind, code: query.code })
        if (token) {
            return token;
        }
        token = source.tokens[cursor];
        // @ts-ignore
        syntaxError("I am waiting for other thing to come here bruh", token.index ?? 0, token.line ?? 0, token.column ?? 0, source.text, source.path);
    }

    let parseType = () => {
        return consume({ kind: 'identifier' })?.text ?? 'void';
    }

    let parseFunction = () => {
        let current = new Symbol();

        expect({ code: TokenName.kw_function });
        /* Try to consume a name if it is not an anonymous function */
        let name = consume({ kind: 'identifier' });
        expect({ code: TokenName.paren_open });
        /* for now, don't parse args */
        expect({ code: TokenName.paren_close });

        current.type = SymbolType.function;
        current.name = name?.text ?? UNNAMED_MEMBER_NAME;

        let colon = consume({ code: TokenName.colon });
        if (colon)
        {
            current.returnType = parseType();
        }
        else
        {
            current.returnType = 'void';
        }

        scopeStack.push(current);

        let beginning = consume({ code: TokenName.brace_open });

        if (beginning) {
            current.completeMember = true;
            cursor = (beginning?.closingPair ?? cursor) + 1;
        }
        else {
            current.completeMember = false;
            expect({ code: TokenName.eol });
        }

        scopeStack.pop()

        return current;
    }

    let parseNamespace = (is_root: boolean = false) => {
        let current = new Symbol();

        expect({ code: TokenName.kw_namespace });
        let name = expect({ kind: 'identifier' });
        let beginning = expect({ code: TokenName.brace_open });

        current.type = SymbolType.namespace;
        current.name = name?.text;

        scopeStack.push(current);

        while (cursor < source.tokens.length && cursor < (beginning?.closingPair ?? 0)) {
            currentToken = source.tokens[cursor];

            switch (currentToken?.code) {
                case TokenName.kw_namespace: {
                    let newSymbol = parseNamespace();
                    current.define( newSymbol.name!, newSymbol );
                    newSymbol.parent = newSymbol;
                } break;
                case TokenName.kw_function: {
                    let newSymbol = parseFunction();
                    current.define( newSymbol.name!, newSymbol );
                    newSymbol.parent = newSymbol;
                } break;
                default:
                    parserError(`Only declarations are permitted inside a namespace, found "${currentToken?.text}"`, currentToken?.index ?? 0, currentToken?.line ?? 0, currentToken?.column ?? 0, source.text, source.path);
                    break;
            }
        }

        cursor = (beginning?.closingPair ?? cursor) + 1;

        scopeStack.pop()

        return current;
    }

    let parseTopLevel = () => {
        while (cursor < source.tokens.length) {
            currentToken = source.tokens[cursor];
            switch (currentToken?.code) {
                case TokenName.kw_namespace: {
                    return parseNamespace(true)
                } break;
                default:
                    parserError("Only a namespace should be present at the root", currentToken?.index!, currentToken?.line!, currentToken?.column!, source.text, source.path);
                    break;
            }
        }
    }

    let topNode: Symbol = parseTopLevel()!;

    console.log(
        util.inspect(topNode, { compact: false, colors: true, depth: 5, showHidden: false })
    );
}


/**

[]

namespace Test {
++++++++++++++
    function main(): i32
    {
    --------------
        return 0;
    --------------
    }
++++++++++++++
}

*/