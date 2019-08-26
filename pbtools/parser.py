import textparser
from textparser import Sequence
from textparser import ZeroOrMore
from textparser import OneOrMore
from textparser import ZeroOrMoreDict
from textparser import choice
from textparser import Optional
from textparser import NoMatch
from textparser import Forward
from textparser import Tag


class Parser(textparser.Parser):

    def token_specs(self):
        return [
            ('SKIP',          r'[ \r\n\t]+|//.*?\n'),
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
            ('LPAREN',   '(', r'\('),
            ('RPAREN',   ')', r'\)'),
            ('LBRACK',   '[', r'\['),
            ('RBRACK',   ']', r'\]'),
            ('MISMATCH',      r'.')
        ]

    def keywords(self):
        return set([
            'syntax',
            'package',
            'message',
            'repeated',
            'enum',
            'service',
            'rpc',
            'returns',
            'stream',
            'import',
            'oneof'
        ])

    def grammar(self):
        ident = choice('IDENT',
                       'syntax',
                       'package',
                       'message',
                       'repeated',
                       'enum',
                       'service',
                       'rpc',
                       'returns',
                       'stream',
                       'import',
                       'oneof')
        full_ident = choice(ident, 'LIDENT')
        empty_statement = ';'
        message_type = Sequence(Optional('.'), full_ident)
        constant = full_ident

        import_ = Sequence('import',
                           Optional(choice('weak', 'public')),
                           'STRING')

        package = Sequence('package', full_ident, ';')

        option = NoMatch()

        enum_field = Sequence(ident, '=', 'INT', ';')
        enum = Sequence('enum',
                        ident,
                        '{',
                        ZeroOrMore(choice(enum_field, empty_statement)),
                        '}')


        oneof_field = Sequence(message_type, ident, '=', 'INT', ';')
        oneof = Sequence('oneof',
                         ident,
                         '{',
                         ZeroOrMore(choice(oneof_field, empty_statement)),
                         '}')

        field = Sequence(Optional('repeated'),
                         message_type, ident, '=', 'INT',
                         Optional(Sequence('[',
                                           OneOrMore(
                                               Sequence(ident, '=', constant)),
                                           ']')),
                         ';')
        message = Forward()
        message <<= Sequence('message',
                             ident,
                             '{',
                             ZeroOrMore(choice(Tag('field', field),
                                               enum,
                                               message,
                                               oneof,
                                               empty_statement)),
                             '}')

        rpc = Sequence('rpc',
                       ident,
                       '(', Optional('stream'), message_type, ')',
                       'returns',
                       '(', Optional('stream'), message_type, ')',
                       ';')

        service = Sequence('service',
                           ident,
                           '{',
                           ZeroOrMore(choice(option, rpc, empty_statement)),
                           '}')

        top_level_def = choice(message, enum, service)

        syntax = Sequence('syntax', '=', 'PROTO3', ';')

        proto = Sequence(syntax,
                         ZeroOrMoreDict(choice(import_,
                                               package,
                                               option,
                                               top_level_def,
                                               empty_statement)))

        return proto


def load_message_type(tokens):
    return tokens[1]


class EnumField:

    def __init__(self, tokens):
        self.name = tokens[0]
        self.field_number = int(tokens[2])


class Enum:

    def __init__(self, tokens):
        self.name = tokens[1]
        self.fields = []

        for item in tokens[3]:
            self.fields.append(EnumField(item))


class OneofField:

    def __init__(self, tokens):
        print(tokens)
        self.type = load_message_type(tokens[0])
        self.name = tokens[1]
        self.field_number = int(tokens[3])


class Oneof:

    def __init__(self, tokens):
        self.name = tokens[1]
        self.fields = []

        for item in tokens[3]:
            self.fields.append(OneofField(item))


class MessageField:

    def __init__(self, tokens):
        self.type = load_message_type(tokens[1])
        self.name = tokens[2]
        self.field_number = int(tokens[4])
        self.repeated = bool(tokens[0])


class Message:

    def __init__(self, tokens):
        self.name = tokens[1]
        self.fields = []
        self.enums = []
        self.messages = []
        self.oneofs = []

        for item in tokens[3]:
            kind = item[0]

            if kind == 'field':
                self.fields.append(MessageField(item[1]))
            elif kind == 'enum':
                self.enums.append(Enum(item))
            elif kind == 'message':
                self.messages.append(Message(item))
            elif kind == 'oneof':
                self.oneofs.append(Oneof(item))
            elif kind == ';':
                pass
            else:
                raise RuntimeError(kind)


class Rpc:

    def __init__(self, tokens):
        self.name = tokens[1]
        self.request_type = tokens[4][1]
        self.request_stream = False
        self.response_type = tokens[9][1]
        self.response_stream = False


class Service:

    def __init__(self, tokens):
        self.name = tokens[1]
        self.rpcs = []

        for item in tokens[3]:
            kind = item[0]

            if kind == 'rpc':
                self.rpcs.append(Rpc(item))
            else:
                raise RuntimeError(kind)


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


def load_services(tokens):
    services = []

    for service in tokens[1].get('service', []):
        services.append(Service(service))

    return services


def load_enums(tokens):
    enums = []

    for enum in tokens[1].get('enum', []):
        enums.append(Enum(enum))

    return enums


class Proto:

    def __init__(self, tree):
        self.package = load_package(tree)
        self.messages = load_messages(tree)
        self.services = load_services(tree)
        self.enums = load_enums(tree)


def parse_string(text):
    return Proto(Parser().parse(text))


def parse_file(filename):
    with open(filename, 'r') as fin:
        return parse_string(fin.read())
