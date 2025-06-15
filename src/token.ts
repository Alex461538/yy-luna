export type TokenKind = 'none' | 'identifier' | 'keyword' | 'number' | 'string' | 'boolean' | 'operator' | 'other'

export const TokenName = {
    co_false: 0,
    co_true: 1,
    co_infinity: 2,

    eol: 8,
    colon: 9,

    kw_import: 10,
    kw_namespace: 20,
    kw_function: 30,
    kw_class: 40,
    kw_var: 50,
    kw_as: 60,

    brace_open: 500,
    brace_close: 501,
    paren_open: 502,
    paren_close: 503,
    square_open: 504,
    square_close: 505,

    op_plus: 1000,
    op_plusEqual: 1001,

    op_less: 1010,
    op_lshift: 1011,
    op_lshiftEqual: 1012,
}

export const TOKEN_MAP = {
    true: {code: TokenName.co_true, kind: 'boolean'},
    false: {code: TokenName.co_false, kind: 'boolean'},
    
    import: {code: TokenName.kw_import, kind: 'keyword'},
    as: {code: TokenName.kw_as, kind: 'keyword'},
    namespace: {code: TokenName.kw_namespace, kind: 'keyword'},
    function: {code: TokenName.kw_function, kind: 'keyword'},
    class: {code: TokenName.kw_class, kind: 'keyword'},
    var: {code: TokenName.kw_var, kind: 'keyword'},
}

export const OPERATOR_MAP = {
    "{": {code: TokenName.brace_open, kind: 'other', paren: true, opening: true},
    "}": {code: TokenName.brace_close, kind: 'other', paren: true, opening: false, alternate: TokenName.brace_open},
    "(": {code: TokenName.paren_open, kind: 'other', paren: true, opening: true},
    ")": {code: TokenName.paren_close, kind: 'other', paren: true, opening: false, alternate: TokenName.paren_open},
    "[": {code: TokenName.square_open, kind: 'other', paren: true, opening: true},
    "]": {code: TokenName.square_close, kind: 'other', paren: true, opening: false, alternate: TokenName.square_open},

    ";": {code: TokenName.eol, kind: 'other'},
    ":": {code: TokenName.colon, kind: 'other'},

    "+": {code: TokenName.op_plus, kind: 'operator'},
    "+=": {code: TokenName.op_plusEqual, kind: 'operator'},

    "<": {code: TokenName.op_less, kind: 'operator'},
    "<<": {code: TokenName.op_lshift, kind: 'operator'},
    "<<=": {code: TokenName.op_lshiftEqual, kind: 'operator'},
}

export const LEGAL_EXTRA_CHARS = new Set([ '+', '<', '=', ';', ':', '[', ']', '{', '}', '(', ')' ])

export class Token
{
    kind: TokenKind = 'none';
    code?: number = TokenName.kw_namespace;
    text: string = "";
    paren?: boolean = false;
    closingPair?: number = 0;
    canHaveDocs?: boolean = false;

    line?: number = 0;
    column?: number = 0;
    index?: number = 0;
}