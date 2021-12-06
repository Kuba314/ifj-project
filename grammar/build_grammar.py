from typing import Dict, List, Set, Callable, Any, Tuple
from grammar_definition import (
    terminals,
    non_terminals,
    rules,
    table,
    get_token_name,
    EPS,
)
import subprocess
import json
import sys


def load_config() -> Dict[str, Any]:
    with open('parser-config.json') as f:
        return json.loads(f.read())

def output_table_c(table: Dict[str, Dict[str, List[str]]], source_fname: str, header_fname: str):

    # hash function and mod found with `bruteforce_hash.py`
    def hash_nt(nti, ti):
        return ((nti << shift) + ti) % mod

    def hash_t(nti, ti):
        return ((ti << shift) + nti) % mod

    config = load_config()
    mod = config['mod']
    nts = config['nts']
    ts = config['ts']
    shift = config['shift']
    nt_main = config['nt-main']
    hashf = hash_nt if nt_main else hash_t

    total_exp_len = sum(sum(len(exp) for exp in expd.values() if exp != [EPS]) for nterm, expd in table.items())

    # generate source file
    with open(source_fname, 'w') as f:
        f.write(f'/*\n * This file was generated by {sys.argv[0]}, DO NOT MODIFY!\n */\n')
        f.write( '#include <stdio.h>\n')
        f.write( '#include <stdlib.h>\n')
        f.write( '#include <stdbool.h>\n\n')
        f.write( '#include "parser-generated.h"\n')
        f.write( '#include "error.h"\n\n')
        f.write( 'parser_table_t *table;\n\n')
        f.write( 'static struct {\n')
        f.write( '    void *data;\n')
        f.write( '    size_t size;\n')
        f.write( '    size_t offset;\n')
        f.write( '} mempool;\n\n')
        f.write( 'static void *alloc_tokens(size_t n_tokens)\n')
        f.write( '{\n')
        f.write( '    void *ret = (uint8_t *) mempool.data + mempool.offset;\n')
        f.write( '    mempool.offset += n_tokens * sizeof(nut_type_t);\n')
        f.write( '    return ret;\n')
        f.write( '}\n')
        f.write( 'size_t parser_get_table_index(nterm_type_t nterm, term_type_t term)\n{\n')
        if nt_main:
            f.write(f'    return ((nterm << {shift}) + term) % {mod};\n')
        else:
            f.write(f'    return ((term << {shift}) + nterm) % {mod};\n')
        f.write( '}\n')

        # generate pretty-print functions for tokens
        f.write('const char *nterm_to_readable(nterm_type_t nterm)\n{\n')
        f.write('    switch(nterm) {\n')
        for i, nterm in enumerate(nts):
            token_name = get_token_name(nterm)
            f.write(f'    case NT_{token_name}:\n')
            f.write(f'        return "{nterm}";\n')
        f.write('    }\n')
        f.write('    return "<unknown-nterm>";\n')
        f.write('}\n')

        f.write('const char *term_to_readable(term_type_t term)\n{\n')
        f.write('    switch(term) {\n')
        for i, term in enumerate(ts):
            token_name = get_token_name(term)
            f.write(f'    case T_{token_name}:\n')
            f.write(f'        return "{term}";\n')
        f.write('    }\n')
        f.write('    return "<unknown-term>";\n')
        f.write('}\n')

        f.write( 'int parser_init()\n{\n')
        f.write( '    mempool.offset = 0;\n')
        f.write(f'    mempool.data = calloc({total_exp_len}, sizeof(nut_type_t));\n')
        f.write( '    if(mempool.data == NULL) {\n')
        f.write( '        return E_INT;\n')
        f.write( '    }\n')
        f.write(f'    mempool.size = {total_exp_len} * sizeof(nut_type_t);\n\n')
        f.write(f'    size_t rule_count = {mod};\n')
        f.write( '    table = calloc(1, sizeof(parser_table_t) + rule_count * sizeof(exp_list_t));\n')
        f.write( '    if(table == NULL) {\n')
        f.write( '        return E_INT;\n')
        f.write( '    }\n\n')
        f.write( '    table->bucket_count = rule_count;\n')

        index_set = set()
        for nterm, expd in table.items():
            for term, exp in expd.items():

                nti = nts.index(nterm)
                ti = ts.index(term)
                table_ix = hashf(nti, ti)
                if table_ix in index_set:
                    print(f'error: index {table_ix} occurs multiple times')
                index_set.add(table_ix)

                # epsilon expansion
                if exp == [EPS]:
                    f.write(f'    table->data[{table_ix}] = (exp_list_t){{ .valid = true, .size = 0 }};\n')
                    continue

                f.write(f'    table->data[{table_ix}] = (exp_list_t){{ .valid = true, .size = {len(exp)}, .data = alloc_tokens({len(exp)}) }};\n')

                for i, token in enumerate(exp):
                    token_name = get_token_name(token)
                    if token in nts:
                        f.write(f'    table->data[{table_ix}].data[{i}].is_nterm = true;\n')
                        f.write(f'    table->data[{table_ix}].data[{i}].nterm = NT_{token_name};\n')
                    else:
                        f.write(f'    table->data[{table_ix}].data[{i}].term = T_{token_name};\n')
        f.write('    return E_OK;\n')
        f.write('}\n')

        f.write('void parser_free()\n{\n')
        f.write('    free(table);\n')
        f.write('    free(mempool.data);\n')
        f.write('}\n')

    with open(header_fname, 'w') as f:
        f.write(f'/*\n * This file was generated by {sys.argv[0]}, DO NOT MODIFY!\n */\n')
        f.write('#pragma once\n\n')
        f.write('#include <stddef.h>\n')
        f.write('#include <stdint.h>\n')
        f.write('#include <stdbool.h>\n\n')
        f.write('#include "type.h"\n')
        f.write('#include "dynstring.h"\n\n')
        f.write('// order of these two enums is crucial\n')
        f.write('typedef enum\n{\n')
        for nterm in nts:
            token_name = get_token_name(nterm)
            f.write(f'    NT_{token_name},\n')
        f.write('} nterm_type_t;\n\n')

        f.write('typedef enum\n{\n')
        for term in ts:
            token_name = get_token_name(term)
            f.write(f'    T_{token_name},\n')
        f.write('} term_type_t;\n\n')

        f.write('// nterm U term => nut\n')
        f.write('typedef struct {\n')
        f.write('    bool is_nterm;\n')
        f.write('    union {\n')
        f.write('        nterm_type_t nterm;\n')
        f.write('        term_type_t term;\n')
        f.write('    };\n')
        f.write('} nut_type_t;\n')
        f.write('\n')
        f.write('typedef struct {\n')
        f.write('    size_t size;\n')
        f.write('    nut_type_t *data;\n')
        f.write('    bool valid;\n')
        f.write('} exp_list_t;\n')
        f.write('\n')
        f.write('// hashmap without collisions\n')
        f.write('typedef struct {\n')
        f.write('    size_t bucket_count;\n')
        f.write('    exp_list_t data[];\n')
        f.write('} parser_table_t;\n\n')

        f.write('size_t parser_get_table_index(nterm_type_t nterm, term_type_t term);\n')
        f.write('const char *nterm_to_readable(nterm_type_t nterm);\n')
        f.write('const char *term_to_readable(term_type_t term);\n')
        f.write('int parser_init();\n')
        f.write('void parser_free();\n\n')
        f.write('extern parser_table_t *table;\n')



if __name__ == '__main__':

    # calculate total amount of expansion rules and all the unique ones
    all_exp_rules: List[Tuple[str, ...]] = sum(([tuple(exp) for exp in exps] for exps in rules.values()), [])
    print(f'{len(all_exp_rules)} expansion rules ({len(set(all_exp_rules))} unique)', file=sys.stderr)
    print(f'terms: {len(terminals)}', file=sys.stderr)
    print(f'nonterms: {len(non_terminals)}', file=sys.stderr)

    if len(sys.argv) != 3:
        print(f'usage: {sys.argv[0]} src_file header_file', file=sys.stderr)
        exit(1)

    src_file, header_file = sys.argv[1:3]

    print('Generating files...')
    output_table_c(table, src_file, header_file)

    # try:
    #     print('Formatting files...')
    #     subprocess.run(['clang-format', '--style=file', '-i', src_file, header_file], check=True)
    # except subprocess.CalledProcessError:
    #     print('failed to format files with `clang-format`', file=sys.stderr)

