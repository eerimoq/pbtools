import textparser
from textparser import Sequence
from textparser import ZeroOrMore
from textparser import ZeroOrMoreDict
from textparser import choice
from textparser import Optional
from textparser import NoMatch
from textparser import Forward
from textparser import Tag


class Parser(textparser.Parser):

    def token_specs(self):
        return [
            ('SKIP',          r'[ \r\n\t]+'),
            ('SYNTAX',        r'syntax'),
            ('PACKAGE',       r'package'),
            ('MESSAGE',       r'message'),
            ('REPEATED',      r'repeated'),
            ('ENUM',          r'enum'),
            ('LIDENT',        r'[a-zA-Z]\w*(\.[a-zA-Z]\w*)+'),
            ('IDENT',         r'[a-zA-Z]\w*'),
            ('INT',           r'0[xX][a-fA-F0-9]+|[0-9]+'),
            ('PROTO3',        r'"proto3"'),
            ('STRING',        r'"(\\"|[^"])*?"'),
            ('DOT',      '.', r'\.'),
            ('COMMA',    ',', r','),
            ('EQ',       '=', r'='),
            ('SCOLON',   ';', r';'),
            ('LBRACE',   '{', r'{'),
            ('RBRACE',   '}', r'}'),
            ('MISMATCH',      r'.')
        ]

    def grammar(self):
        ident = choice('IDENT',
                       'SYNTAX',
                       'PACKAGE',
                       'MESSAGE',
                       'REPEATED',
                       'ENUM')
        full_ident = choice(ident, 'LIDENT')
        empty_statement = ';'

        import_ = NoMatch()

        package = Sequence('PACKAGE', full_ident, ';')

        option = NoMatch()

        enum_field = Sequence(ident, '=', 'INT', ';')
        enum = Sequence('ENUM',
                        ident,
                        '{',
                        ZeroOrMore(choice(enum_field, empty_statement)),
                        '}')

        field = Sequence(Optional('REPEATED'), ident, ident, '=', 'INT', ';')
        message = Forward()
        message <<= Sequence('MESSAGE',
                             ident,
                             '{',
                             ZeroOrMore(choice(Tag('field', field),
                                               enum,
                                               message)),
                             '}')

        top_level_def = choice(message, enum)

        syntax = Sequence('SYNTAX', '=', 'PROTO3', ';')

        proto = Sequence(syntax,
                         ZeroOrMoreDict(choice(import_,
                                               package,
                                               option,
                                               top_level_def,
                                               empty_statement)))

        return proto


class EnumField:

    def __init__(self, tokens):
        self.name = tokens[0]
        self.tag = int(tokens[2])


class Enum:

    def __init__(self, tokens):
        self.name = tokens[2]
        self.fields = []

        for item in tokens[3]:
            self.fields.append(EnumField(item))
            

class MessageField:

    def __init__(self, tokens):
        self.type = tokens[1]
        self.name = tokens[2]
        self.tag = int(tokens[4])
        self.repeated = bool(tokens[0])
                        

class Message:

    def __init__(self, tokens):
        self.name = tokens[1]
        self.fields = []
        self.enums = []
        self.messages = []

        for item in tokens[3]:
            kind = item[0]

            if kind == 'field':
                self.fields.append(MessageField(item[1]))
            elif kind == 'enum':
                self.enums.append(Enum(item))
            elif kind == 'message':
                self.messages.append(Message(item))
            else:
                raise RuntimeError()


def load_package(tokens):
    try:
        return tokens[1]['package'][0][1]
    except KeyError:
        raise RuntimeError()


def load_messages(tokens):
    messages = []

    for message in tokens[1].get('message', []):
        messages.append(Message(message))

    return messages


class Proto:

    def __init__(self, tree):
        self.package = load_package(tree)
        self.messages = load_messages(tree)


def parse_string(text):
    return Proto(Parser().parse(text))


def parse_file(filename):
    with open(filename, 'r') as fin:
        return parse_string(fin.read())
