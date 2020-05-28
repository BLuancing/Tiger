#!/usr/bin/env python3
import os
import subprocess
import re
from pprint import pprint

if __name__ == '__main__':

    # run all the tests
    results = {}
    for f in os.listdir('test-cases'):
        c = subprocess.run(['./tigerc', 'test-cases/' + f], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        d = {}
        test = c.args[1].split('/')[1]
        category = None
        if 'lexer' in test:
            category = 'lexer'
        elif 'parser' in test:
            category = 'parser'
        elif 'semantic-basic' in test:
            category = 'basic'
        elif 'semantic-complex' in test:
            category = 'complex'
        elif 'semantic-recursive' in test:
            category = 'recursive'
        elif 'valid' in test:
            category = 'valid'
        d['category'] = category
        d['exit'] = c.returncode
        d['output'] = ''.join( chr(x) for x in c.stdout )
        d['errors'] = []
        results[test] = d

    # collect errors
    lexer_error = re.compile('ERROR: \d+: Lexer:')
    parser_error = re.compile('ERROR: \d+: Parser:')
    semantic_error = re.compile('ERROR: \d+: Semantic:')
    for r in results.values():
        # output should be a single line
        if r['output'].count('\n') > 1:
            r['errors'].append('Too much output:\n' + r['output'])

        # lexer errors
        if r['category'] == 'lexer':
            if r['exit'] != 1:
                r['errors'].append('Incorrect exit code: got ' + str(r['exit']) + ', should be 1')
            if not lexer_error.match(r['output']):
                r['errors'].append('Incorrect error message: should be of the form "ERROR: <line>: Lexer: <message>"')

        # parser errors
        if r['category'] == 'parser':
            if r['exit'] != 2:
                r['errors'].append('Incorrect exit code: got ' + str(r['exit']) + ', should be 2')
            if not parser_error.match(r['output']):
                r['errors'].append('Incorrect error message: should be of the form "ERROR: <line>: Parser: <message>"')

        # semantic errors
        if r['category'] == 'basic' or r['category'] == 'complex' or r['category'] == 'recursive':
            if r['exit'] != 3:
                r['errors'].append('Incorrect exit code: got ' + str(r['exit']) + ', should be 3')
            if not semantic_error.match(r['output']):
                r['errors'].append('Incorrect error message: should be of the form "ERROR: <line>: Semantic: <message>"')

        # valid errors
        if r['category'] == 'valid':
            if r['output']:
                r['errors'].append('Too much output:\n' + r['output'])


    print('Summary')
    print('-------')
    print('')

    # Lexer results
    lexer_results = []
    for r in results.values():
        if r['category'] == 'lexer':
            if len(r['errors']) == 0:
                lexer_results.append(1)
            else:
                lexer_results.append(0)
        else:
            if r['exit'] in [2,3,0]:
                lexer_results.append(1)
            else:
                lexer_results.append(0)
    print('Lexer: {:d}/{:d}: {:.2%} Correct'.format(sum(lexer_results), len(lexer_results), sum(lexer_results)/len(lexer_results)))

    # Parser
    parser_results = []
    for r in results.values():
        if r['category'] == 'lexer':
            continue
        if r['category'] == 'parser':
            if len(r['errors']) == 0:
                parser_results.append(1)
            else:
                parser_results.append(0)
        else:
            if r['exit'] in [3,0]:
                parser_results.append(1)
            else:
                parser_results.append(0)
    print('Parser: {:d}/{:d}: {:.2%} Correct'.format(sum(parser_results), len(parser_results), sum(parser_results)/len(parser_results)))

    # Basic
    basic_results = []
    for t, r in results.items():
        if r['category'] == 'basic':
            if len(r['errors']) == 0:
                basic_results.append(1)
            else:
                basic_results.append(0)
        if 'valid-basic' in t:
            if len(r['errors']) != 0:
                basic_results.append(0)
            else:
                basic_results.append(1)
    print('Basic: {:d}/{:d}: {:.2%} Correct'.format(sum(basic_results), len(basic_results), sum(basic_results)/len(basic_results)))

    # Complex
    complex_results = []
    for t, r in results.items():
        if r['category'] == 'complex':
            if len(r['errors']) == 0:
                complex_results.append(1)
            else:
                complex_results.append(0)
        if 'valid-complex' in t:
            if len(r['errors']) != 0:
                complex_results.append(0)
            else:
                complex_results.append(1)
    print('Complex: {:d}/{:d}: {:.2%} Correct'.format(sum(complex_results), len(complex_results), sum(complex_results)/len(complex_results)))

    # Recursive
    recursive_results = []
    for t, r in results.items():
        if r['category'] == 'recursive':
            if len(r['errors']) == 0:
                recursive_results.append(1)
            else:
                recursive_results.append(0)
        if 'valid-complex' in t:
            if len(r['errors']) != 0:
                recursive_results.append(0)
            else:
                recursive_results.append(1)
    print('Recursive: {:d}/{:d}: {:.2%} Correct'.format(sum(recursive_results), len(recursive_results), sum(recursive_results)/len(recursive_results)))

    print('')
    print('')
    print('Log')
    print('---')
    for t, r in sorted(results.items()):
        if len(r['errors']) != 0:
            print()
            print(t)
            for e in r['errors']:
                print('  ' + e)
