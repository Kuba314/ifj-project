from typing import Set, Dict, List

def is_term(t):
    return t != '<expression>' and t in terminals

# https://github.com/PranayT17/Finding-FIRST-and-FOLLOW-of-given-grammar/blob/master/first_follow.py
def get_first(seq: List[str]) -> Set[str]:

    if len(seq) == 1:
        token = seq[0]

        if is_term(token):
            return {token}
        elif token in non_terminals:
            first = set()
            for exp in rules[token]:
                first |= get_first(exp)
            return first
        else:
            print(f'error: token isn\'t term and neither is an nterm: "{token}"')
            return set()

    # we now know that seq is longer than 1 token
    tmp_first = get_first([seq[0]])
    first = tmp_first - {EPS}

    while EPS in tmp_first:

        seq = seq[1:]
        if len(seq) == 1 and is_term(seq[0]):
            first |= {seq[0]}
            break
        if not seq:
            first |= {EPS}
            break

        tmp_first = get_first(seq)
        first = first | tmp_first - {EPS}

    return first | tmp_first - {EPS}


def get_follow(nT: str, parents: List[str]) -> Set[str]:

    parents = parents[:] + [nT]
    follow = set()
    if nT == starting_symbol:
        follow |= {"$"}

    # loop over every token in rules and execute inner block only if nT is found
    for nt, exps in rules.items():
        for exp in exps:
            for i, c in enumerate(exp, 1):
                if c != nT:
                    continue

                next_tokens = exp[i:]
                if next_tokens:
                    follow_tmp = get_first(next_tokens)

                    # following string has epsilon (can be ignored), have to recursively check parent
                    if EPS in follow_tmp:
                        follow |= get_follow(nt, parents)

                    follow |= follow_tmp - {EPS}

                # nT is at the end of string, have to recursively check parent if not checked already
                elif nt not in parents:
                    follow |= get_follow(nt, parents)
    return follow

def get_token_name(token):
    if token == '$':
        return 'EOF'
    elif token[0] == '<' and token[-1] == '>':
        return token[1:-1].replace('-', '_').upper()
    else:
        d = {
            '(':    'LPAREN',
            ')':    'RPAREN',
            '+':    'PLUS',
            '-':    'MINUS',
            '*':    'ASTERISK',
            '/':    'SLASH',
            '%':    'PERCENT',
            '^':    'CARET',
            '//':   'DOUBLE_SLASH',
            '=':    'EQUALS',
            '~=':   'TILDE_EQUALS',
            '..':   'DOUBLE_DOT',
            '#':    'HASH',
            ':':    'COLON',
            '<':    'LT',
            '>':    'GT',
            '<=':   'LTE',
            '>=':   'GTE',
            '==':   'DOUBLE_EQUALS',
            ',':    'COMMA',
        }
        return d.get(token, token.upper())


EPS = 'ε'
starting_symbol = '<program>'

# rules is a dict connecting non-terminals to possible expansions to grammar strings
# list of python strings is considered a grammar string
rules: Dict[str, List[List[str]]] = {
    '<program>':                [['require', '<string>', '<global-statement-list>']],
    '<global-statement-list>':  [['<global-statement>', '<global-statement-list>'], [EPS]],
    '<global-statement>':       [['<func-decl>'], ['<func-def>'], ['<func-call>']],

    '<func-decl>':                      [['global', '<identifier>', ':', 'function', '(', '<type-list>', ')', '<func-type-list>']],
    '<type-list>':                      [['<type>', '<type-list2>'], [EPS]],
    '<type-list2>':                     [[',', '<type>', '<type-list2>'], [EPS]],
    '<func-def>':                       [['function', '<identifier>', '(', '<identifier-list-with-types>', ')', '<func-type-list>', '<statement-list>', 'end']],
    '<identifier-list-with-types>':     [['<identifier-with-type>', '<identifier-list-with-types2>'], [EPS]],
    '<identifier-list-with-types2>':    [[',', '<identifier-with-type>', '<identifier-list-with-types2>'], [EPS]],
    '<func-type-list>':                 [[':', '<type>', '<func-type-list2>'], [EPS]],
    '<func-type-list2>':                [[',', '<type>', '<func-type-list2>'], [EPS]],

    # had to add a continuation even if it's not needed here. parser had to know when to allocate body ast node
    '<statement-list>':             [['<statement>', '<statement-list2>'], [EPS]],
    '<statement-list2>':            [['<statement>', '<statement-list2>'], [EPS]],
    '<statement>':                  [['<cond-statement>'], ['<while-loop>'], ['<for-loop>'], ['<repeat-until>'], ['<declaration>'], ['<identifier>', '<paren-exp-list-or-id-list2>'], ['<return-statement>'], ['break']],
    '<paren-exp-list-or-id-list2>': [['(', '<optional-fun-expression-list>', ')'], ['<identifier-list2>', '=', '<expression-list>'], ['=', '<expression-list>']],

    '<cond-statement>':     [['if', '<expression>', 'then', '<statement-list>', '<cond-opt-elseif>']],
    '<cond-opt-elseif>':    [['elseif', '<expression>', 'then', '<statement-list>', '<cond-opt-elseif>'], ['else', '<statement-list>', 'end'], ['end']],
    '<while-loop>':         [['while', '<expression>', 'do', '<statement-list>', 'end']],
    '<for-loop>':           [['for', '<identifier>', '=', '<expression>', ',', '<expression>', '<optional-for-step>', 'do', '<statement-list>', 'end']],
    '<optional-for-step>':  [[',', '<expression>'], [EPS]],
    '<repeat-until>':       [['repeat', '<statement-list>', 'until', '<expression>']],

    '<declaration>':                [['local', '<identifier-with-type>', '<decl-optional-assignment>']],
    '<decl-optional-assignment>':   [['=', '<expression>'], [EPS]],
    '<identifier-with-type>':       [['<identifier>', ':', '<type>']],

    '<assignment>':             [['<identifier-list>', '=', '<expression-list>']],
    '<identifier-list>':        [['<identifier>', '<identifier-list2>']],
    '<identifier-list2>':       [[',', '<identifier>', '<identifier-list2>'], [EPS]],
    '<expression-list>':        [['<expression>', '<expression-list2>']],
    '<expression-list2>':       [[',', '<expression>', '<expression-list2>'], [EPS]],

    '<return-statement>':       [['return', '<ret-expression-list>']],
    '<ret-expression-list>':    [['<expression>', '<ret-expression-list2>']],
    '<ret-expression-list2>':   [[',', '<expression>', '<ret-expression-list2>'], [EPS]],

    '<func-call>':                      [['<identifier>', '(', '<optional-fun-expression-list>', ')']],
    '<optional-fun-expression-list>':   [['<expression>', '<fun-expression-list2>'], [EPS]],
    '<fun-expression-list2>':           [[',', '<expression>', '<fun-expression-list2>'], [EPS]],

    '<expression>': [['<term>', '<opt-binop>'], ['<unop>', '<term>', '<opt-binop>']],
    '<opt-binop>':  [['<binop>', '<expression>'], [EPS]],
    '<binop>':      [['+'], ['-'], ['*'], ['/'], ['%'], ['^'], ['//'], ['..'], [':'], ['<'], ['>'], ['<='], ['>='], ['=='], ['~='], ['and'], ['or']],
    '<unop>':       [['-'], ['#'], ['not']],
    '<term>':       [['(', '<expression>', ')'], ['<identifier>'], ['<number>'], ['<integer>'], ['<string>'], ['<bool>'], ['nil']],

}

# construct terminals and non_terminals lists from rules
terminals: List[str] = list({
    token
    for exps in rules.values()
    for exp in exps
    for token in exp
    if token not in rules
})
non_terminals: List[str] = list(rules.keys())


# construct derivation table
table: Dict[str, Dict[str, List[str]]] = {}

for nt, exps in rules.items():
    table[nt] = {}
    got_eps = False
    for exp in exps:
        for first_token in get_first(exp):
            if first_token == EPS:
                got_eps = True
            # elif first_token == '<expression>':
            #     continue
            table[nt][first_token] = exp

    if got_eps:
        for follow_token in get_follow(nt, []):
            table[nt][follow_token] = [EPS]

# change eps terminal to '$' in header signalling end of program
for expd in table.values():
    if EPS in expd:
        expd['$'] = expd[EPS]
        del expd[EPS]
terminals.remove(EPS)
terminals.append('$')
