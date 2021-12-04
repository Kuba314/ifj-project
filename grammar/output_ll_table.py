from grammar_definition import is_term, rules, terminals, EPS

def format_nut(nut: str) -> str:
    if is_term(nut):
        if nut[0] == '<' and nut[-1] == '>':
            return nut[1:-1]
        return f'"{nut}"'
    else:
        if nut == EPS:
            return '/* eps */'
        return nut.replace('-', '_')[1:-1]

print('%token ' + ' '.join(format_nut(s) for s in filter(lambda x: x.startswith('<') and x.endswith('>'), terminals)))
print('%%')
for nt, exps in rules.items():
    for i, exp in enumerate(exps):
        if i == 0:
            print(f'{format_nut(nt)} : ', end='')
        else:
            print(f'{" " * len(format_nut(nt))} | ', end='')

        for nut in exp:
            print(format_nut(nut), end=' ')

        if i == len(exps) - 1:
            print(';')
        else:
            print()
