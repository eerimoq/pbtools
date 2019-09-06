import re
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


SCALAR_VALUE_TYPES = [
    'int32',
    'int64',
    'sint32',
    'sint64',
    'uint32',
    'uint64',
    'fixed32',
    'fixed64',
    'sfixed32',
    'sfixed64',
    'float',
    'double',
    'bool',
    'string',
    'bytes'
]


class InternalError(Exception):
    pass


def canonical(value):
    """Replace anything but 'a-z', 'A-Z' and '0-9' with '_'.

    """

    return re.sub(r'[^a-zA-Z0-9]', '_', value)


def camel_to_snake_case(value):
    value = canonical(value)
    value = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', value)
    value = re.sub(r'(_+)', '_', value)
    value = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', value).lower()

    return value


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

    @property
    def name_snake_case(self):
        return camel_to_snake_case(self.name)


class Enum:

    def __init__(self, tokens, namespace):
        self.name = tokens[1]
        self.fields = []
        self.namespace = namespace

        for item in tokens[3]:
            self.fields.append(EnumField(item))

    @property
    def full_name(self):
        return '.'.join(self.namespace + [self.name])

    @property
    def full_name_snake_case(self):
        return camel_to_snake_case(self.full_name)


class OneofField:

    def __init__(self, tokens):
        self.type = load_message_type(tokens[0])
        self.name = tokens[1]
        self.field_number = int(tokens[3])


class Oneof:

    def __init__(self, tokens):
        self.name = tokens[1]
        self.fields = []
        self.namespace = []

        for item in tokens[3]:
            self.fields.append(OneofField(item))


class MessageField:

    def __init__(self, tokens):
        self.type = load_message_type(tokens[1])
        self.name = tokens[2]
        self.field_number = int(tokens[4])
        self.repeated = bool(tokens[0])
        self.namespace = []

    @property
    def full_type(self):
        return '.'.join(self.namespace + [self.type])

    @property
    def full_type_snake_case(self):
        return camel_to_snake_case(self.full_type)

    @property
    def name_snake_case(self):
        return camel_to_snake_case(self.name)


class Message:

    def __init__(self, tokens, namespace):
        self.name = tokens[1]
        self.fields = []
        self.enums = []
        self.messages = []
        self.oneofs = []
        self.namespace = namespace

        for item in tokens[3]:
            kind = item[0]
            inner_namespace = namespace + [self.name]

            if kind == 'field':
                self.fields.append(MessageField(item[1]))
            elif kind == 'enum':
                self.enums.append(Enum(item, inner_namespace))
            elif kind == 'message':
                self.messages.append(Message(item, inner_namespace))
            elif kind == 'oneof':
                self.oneofs.append(Oneof(item))
            elif kind == ';':
                pass
            else:
                raise InternalError(kind)

    @property
    def repeated_fields(self):
        return [field for field in self.fields if field.repeated]

    @property
    def type_names(self):
        type_names = [enum.name for enum in self.enums]
        type_names += [message.name for message in self.messages]

        return type_names

    @property
    def full_name(self):
        return '.'.join(self.namespace + [self.name])

    @property
    def full_name_snake_case(self):
        return camel_to_snake_case(self.full_name)


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
                raise InternalError(kind)


def load_package(tokens):
    try:
        return tokens[1]['package'][0][1]
    except KeyError:
        raise RuntimeError('Package missing.')


def load_messages(tokens, namespace):
    return [
        Message(message, namespace)
        for message in tokens[1].get('message', [])
    ]


def load_services(tokens):
    return [
        Service(service)
        for service in tokens[1].get('service', [])
    ]


def load_enums(tokens, namespace):
    return [
        Enum(enum, namespace)
        for enum in tokens[1].get('enum', [])
    ]


class Proto:

    def __init__(self, tree):
        self.package = load_package(tree)
        self.messages = load_messages(tree, [self.package])
        self.services = load_services(tree)
        self.enums = load_enums(tree, [self.package])
        self.messages_stack = []
        self.resolve_messages_fields_types()

    def resolve_messages_fields_types(self):
        for message in self.messages:
            self.resolve_message_fields_types(message)

    def resolve_message_fields_types(self, message):
        self.messages_stack.append(message)

        for field in message.fields:
            if field.type in SCALAR_VALUE_TYPES:
                continue

            self.resolve_message_field_type(field)

        for inner_message in message.messages:
            self.resolve_message_fields_types(inner_message)

        self.messages_stack.pop()

    def resolve_message_field_type(self, field):
        for message in reversed(self.messages_stack):
            if field.type in message.type_names:
                namespace = message.namespace + [message.name]
                break
        else:
            namespace = [self.package]

        field.namespace = namespace


def parse_string(text):
    return Proto(Parser().parse(text))


def parse_file(filename):
    with open(filename, 'r') as fin:
        return parse_string(fin.read())
