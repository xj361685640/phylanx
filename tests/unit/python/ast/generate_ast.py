#  Copyright (c) 2017 Hartmut Kaiser
#
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

import phylanx

###############################################################################
class traverse_ast:
    def __init__(self):
        self.result = ''

    def __call__(self, ast, delimiter = ''):
        self.result += str(ast) + delimiter
        return True

class traverse_ast_enter_exit:
    def __init__(self):
        self.result = ''

    def on_enter(self, ast, delimiter = ''):
        self.result += str(ast) + delimiter
        return True

    # optional, can be ommitted
    def on_exit(self, ast, *args):
        return True

def test_expression(expr, expected, delimiter):
    ast = phylanx.ast.generate_ast(expr)

    visitor = traverse_ast()
    phylanx.ast.traverse(ast, visitor, delimiter)
    assert(visitor.result == expected)

    visitor = traverse_ast_enter_exit()
    phylanx.ast.traverse(ast, visitor, delimiter)
    assert(visitor.result == expected)


###############################################################################
test_expression(
    'A + B',
    '(A + B)\n' +
        'A\n' +
        'B\n' +
        '+\n',
    '\n')

test_expression(
    'A + B + -C',
    '(A + B + -C)\n' +
        'A\n' +
        'B\n' +
        '+\n' +
        'C\n' +
        '-\n' +
        '+\n',
    '\n')

test_expression(
    'A + B * C',
    '(A + B * C)\n' +
        'A\n' +
        'B\n' +
        'C\n' +
        '*\n' +
        '+\n',
    '\n')

test_expression(
    'A * B + C',
    '(A * B + C)\n' +
        'A\n' +
        'B\n' +
        '*\n' +
        'C\n' +
        '+\n',
    '\n')

test_expression(
    'func(A, B)',
    'func(A, B)\n' +
        'func(A, B)\n' +
            'func\n' +
            'A\n' +
                'A\n' +
            'B\n' +
                'B\n',
    '\n')
