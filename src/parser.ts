import { AstNode, NodeType } from "./ast";
import { parserError, syntaxError } from "./errors";
import { Token, TokenKind, TokenName } from "./token";
import * as util from 'util';

const UNNAMED_MEMBER_NAME = '<unnamed>'

export function parse(source: any) {
    let currentToken: Token | undefined;
    let cursor = 0;

    let scopeStack: (AstNode | any)[] = []

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

    let parseType = (leaf: AstNode) => {
        return consume({ kind: 'identifier' })?.text ?? 'void';
    }

    let parseFunction = (leaf: AstNode) => {
        let functionNode = new AstNode();

        expect({ code: TokenName.kw_function });
        /* Try to consume a name if it is not an anonymous function */
        let name = consume({ kind: 'identifier' });
        expect({ code: TokenName.paren_open });
        /* for now, don't parse args */
        expect({ code: TokenName.paren_close });

        functionNode.type = NodeType.function;
        functionNode.name = name?.text ?? UNNAMED_MEMBER_NAME;

        let colon = consume({ code: TokenName.colon });
        if (colon)
        {
            functionNode.returnType = parseType(functionNode);
        }
        else
        {
            functionNode.returnType = 'void';
        }

        scopeStack.push(functionNode);

        let beginning = consume({ code: TokenName.brace_open });

        if (beginning) {
            functionNode.completeMember = true;
            cursor = (beginning?.closingPair ?? cursor) + 1;
        }
        else {
            functionNode.completeMember = false;
            expect({ code: TokenName.eol });
        }

        scopeStack.pop()

        return functionNode;
    }

    let parseNamespace = (leaf: AstNode) => {
        let namespaceNode = new AstNode();

        expect({ code: TokenName.kw_namespace });
        let name = expect({ kind: 'identifier' });
        let beginning = expect({ code: TokenName.brace_open });

        namespaceNode.type = NodeType.namespace;
        namespaceNode.name = name?.text;

        scopeStack.push(namespaceNode);

        while (cursor < source.tokens.length && cursor < (beginning?.closingPair ?? 0)) {
            currentToken = source.tokens[cursor];

            switch (currentToken?.code) {
                case TokenName.kw_namespace: {
                    namespaceNode.addChild( parseNamespace(namespaceNode) );
                } break;
                case TokenName.kw_function: {
                    namespaceNode.addChild( parseFunction(namespaceNode) );
                } break;
                default:
                    parserError(`Only declarations are permitted inside a namespace, found "${currentToken?.text}"`, currentToken?.index ?? 0, currentToken?.line ?? 0, currentToken?.column ?? 0, source.text, source.path);
                    break;
            }
        }

        cursor = (beginning?.closingPair ?? cursor) + 1;
        scopeStack.pop()

        return namespaceNode;
    }

    let parseTopLevel = (leaf?: AstNode) => {
        let root = new AstNode();
        root.type = NodeType.root;
        root.name = '<root>'

        while (cursor < source.tokens.length) {
            currentToken = source.tokens[cursor];

            switch (currentToken?.code) {
                case TokenName.kw_namespace: {
                    root.addChild( parseNamespace(root) );
                } break;
                default:
                    console.log(currentToken);
                    cursor++;
                    break;
            }
        }

        return root;
    }

    let topNode: AstNode = parseTopLevel();

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