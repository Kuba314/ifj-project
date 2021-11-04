from grammar_definition import table, terminals as ts, non_terminals as nts
from typing import Dict, List
import json
import random

def save_config(ts, nts, mod, shift, nt_main):
    with open('parser-config.json', 'w') as f:
        f.write(json.dumps({'ts': ts, 'nts': nts, 'mod': mod, 'shift': shift, 'nt-main': nt_main}))

shift = 6

def hash(a, b):
    return (a << shift) + b

rule_count = len([rule for nt, expd in table.items() for t, rule in expd.items()])

indexed_nonterminals: Dict[int, List[str]]
indexed_terminals: Dict[int, List[str]]

min_mod = 100000
while True:
    indexed_nonterminals = {hash(nts.index(nt), ts.index(t)): rule for nt, expd in table.items() for t, rule in expd.items()}
    indexed_terminals = {hash(ts.index(t), nts.index(nt)): rule for nt, expd in table.items() for t, rule in expd.items()}

    if len(indexed_terminals) != rule_count or len(indexed_nonterminals) != rule_count:
        print('error: hash function collision')
        exit(1)

    for name, indexed_tokens in (('nt', indexed_nonterminals), ('t', indexed_terminals)):
        max_index = max(indexed_tokens.keys())
        for mod in range(1, min(min_mod, max_index)):
            index_set = set()
            for i in indexed_tokens.keys():
                calc_ix = i % mod
                if calc_ix in index_set:
                    # found duplicate
                    break
                index_set.add(calc_ix)
            else:
                if mod < min_mod:
                    print(f'[{name}] {mod} (of {max_index})')
                    min_mod = mod
                    save_config(ts, nts, mod, shift, name == 'nt')
                    print(ts, nts)
                break

    # shuffle tokens and attempt a better fit
    random.shuffle(ts)
    random.shuffle(nts)
