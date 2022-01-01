##
# @file output_ll_table.py
# @author xrozek02 Jakub Rozek
#
# @brief output LL table in a nice format for documentation
#

from grammar_definition import is_term, table, rules, terminals as ts, non_terminals as nts, EPS
from typing import List, Tuple

import re

ts = ['require', 'function', 'global', 'local', 'if', 'then', 'elseif', 'else', 'while', 'do', 'end', 'repeat', 'until', 'for', 'break', 'return', ':', ',', 'and', 'or', 'not', '//', '+', '-', '*', '/', '%', '^', '(', ')', '..', '#', '<', '=', '>', '<=', '==', '>=', '~=', '<identifier>', '<number>', '<string>', '<type>', '<integer>', '<bool>', 'nil', '$']

def print_table(headers, lambdas, data):

    tru_data = [[l(row) for l in lambdas] for row in data]      # pass through the functions
    tru_data = [[str(el) for el in row] for row in tru_data]    # format floats to double precision, other types to string


    lengths = [max(len(x) for x in entries+(h,)) for h, entries in zip(headers, zip(*tru_data))]
    n_dashes = sum(lengths) + len(headers)-1 + len(tru_data[0])*2 + 2

    # header
    s = '-' * n_dashes + '\n| '

    for header, l in zip(headers, lengths):
        s += str(header).ljust(l+1) + '| '

    s += '\n' + '-' * n_dashes + '\n'

    # body
    for i, row in enumerate(tru_data):
        s += '| '
        for j, (el, l) in enumerate(zip(row, lengths)):
            d = el.ljust(l+1)
            s += el.ljust(l+1) + '| '
        s += '\n'
    s += '-' * n_dashes + '\n'
    return s

def find_rule(expansions: List[Tuple[str, List[str]]], expansion: List[str], non_terminal: str):
    for i, (nt, exp) in enumerate(expansions):
        if nt != non_terminal or len(exp) != len(expansion):
            continue

        for a, b in zip(exp, expansion):
            if a != b:
                break
        else:
            return i
    raise Exception('huh')

def put_rule(expansions: List[Tuple[str, List[str]]], expansion: List[str], non_terminal: str):
    for i, (nt, exp) in enumerate(expansions):
        if nt == non_terminal and exp == expansion:
            return
    else:
        expansions += [(non_terminal, expansion)]

# format nut to correct type
def format_nut(nut: str) -> str:
    if nut == '$':
        return '$'
    elif is_term(nut):
        if nut[0] == '<' and nut[-1] == '>':
            return nut[1:-1]
        return f'"{nut}"'
    else:
        if nut == EPS:
            return 'EPS'
        return nut.replace('-', '_')[1:-1]

# generate list of all known expansion rules
expansions: List[Tuple[str, List[str]]] = []
for nt, expd in table.items():
    for t, exp in expd.items():
        if not exp:
            continue
        if exp:
            put_rule(expansions, [format_nut(el) for el in exp], format_nut(nt))

# print expansion rules
for i, (nt, exp) in enumerate(expansions, 1):
    print(f'{i:2}. {nt} -> {" ".join(exp)}')

# print LL table
lambdas = [lambda nt: format_nut(nt)]
for _t in ts:
    lmb = lambda nt, t=_t: 1 + find_rule(expansions, [format_nut(el) for el in table[nt][t]], format_nut(nt)) if t in table[nt] else ''
    lambdas += [lmb]


print(print_table(['LL table'] + [format_nut(t) for t in ts], lambdas, table))
