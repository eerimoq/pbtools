import os
import re
import textparser
from textparser import Sequence
from textparser import ZeroOrMore
from textparser import OneOrMore
from textparser import ZeroOrMoreDict
from textparser import choice
from textparser import Optional
from textparser import Forward
from textparser import Tag
from .errors import Error


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

    KEYWORDS = [
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
        'oneof',
        'option',
        'true',
        'false',
        'map'
    ]

    def token_specs(self):
        return [
            ('SKIP',          r'[ \r\n\t]+|//.*?\n'),
            ('LIDENT',        r'[a-zA-Z]\w*(\.[a-zA-Z]\w*)+'),
            ('IDENT',         r'[a-zA-Z]\w*'),
            ('INT',           r'-?(0[xX][a-fA-F0-9]+|[0-9]+)'),
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
            ('LT',       '<', r'<'),
            ('GT',       '>', r'>'),
            ('MISMATCH',      r'.')
        ]

    def keywords(self):
        return set(self.KEYWORDS)

    def grammar(self):
        ident = choice('IDENT', *self.KEYWORDS)
        full_ident = choice(ident, 'LIDENT')
        empty_statement = ';'
        message_type = Sequence(Optional('.'), full_ident)
        constant = choice(Tag('bool', choice('true', 'false')),
                          Tag('full_ident', full_ident),
                          Tag('string', 'STRING'))

        options = Optional(Sequence('[',
                                    OneOrMore(
                                        Sequence(ident, '=', constant)),
                                    ']'))

        # Import.
        import_ = Sequence('import',
                           Optional(choice('weak', 'public')),
                           'STRING')

        # Package.
        package = Sequence('package', full_ident, ';')

        # Option.
        option_name = full_ident
        option = Sequence('option', option_name, '=', constant, ';')

        # Enum.
        enum_field = Sequence(ident, '=', 'INT', options, ';')
        enum = Sequence('enum',
                        ident,
                        '{',
                        ZeroOrMore(choice(enum_field, empty_statement)),
                        '}')

        # Oneof.
        oneof_field = Sequence(message_type, ident, '=', 'INT', ';')
        oneof = Sequence('oneof',
                         ident,
                         '{',
                         ZeroOrMore(choice(oneof_field, empty_statement)),
                         '}')

        # Map.
        map_field = Sequence('map', '<', ident, ',', message_type, '>',
                             ident, '=', 'INT', options, ';')

        # Message.
        field = Sequence(Optional('repeated'),
                         message_type, ident, '=', 'INT', options, ';')
        message = Forward()
        message <<= Sequence('message',
                             ident,
                             '{',
                             ZeroOrMore(choice(Tag('field', field),
                                               enum,
                                               message,
                                               oneof,
                                               map_field,
                                               empty_statement)),
                             '}')

        # Service.
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

        # Proto3-file.
        top_level_def = choice(message, enum, service)
        syntax = Sequence('syntax', '=', 'PROTO3', ';')
        proto = Sequence(syntax,
                         ZeroOrMoreDict(choice(import_,
                                               package,
                                               option,
                                               top_level_def,
                                               empty_statement)))

        return proto


class Field:

    def __init__(self, type, name, field_number):
        self.type = type[-1]
        self.name = name
        self.field_number = field_number
        self.namespace = type[:-1]
        self.type_kind = None
        self.package = '.'.join(self.namespace)

    @property
    def full_type(self):
        return '.'.join(self.namespace + [self.type])

    @property
    def full_type_snake_case(self):
        return camel_to_snake_case(self.full_type)

    @property
    def name_snake_case(self):
        return camel_to_snake_case(self.name)


class Option:

    def __init__(self, tokens):
        self.name = tokens[1]

        kind, value = tokens[3]

        if kind == 'string':
            value = value[1:-1]
        elif kind == 'bool':
            value = (value == 'true')

        self.kind = kind
        self.value = value


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


class OneofField(Field):

    def __init__(self, tokens):
        super().__init__(load_message_type(tokens[0]),
                         tokens[1],
                         int(tokens[3]))


class Oneof:

    def __init__(self, tokens, namespace):
        self.name = tokens[1]
        self.fields = []
        self.namespace = namespace

        for item in tokens[3]:
            self.fields.append(OneofField(item))

    @property
    def full_name(self):
        return '.'.join(self.namespace + [self.name])

    @property
    def full_name_snake_case(self):
        return camel_to_snake_case(self.full_name)


class MessageField(Field):

    def __init__(self, tokens):
        super().__init__(load_message_type(tokens[1]),
                         tokens[2],
                         int(tokens[4]))
        self.repeated = bool(tokens[0])


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
            sub_namespace = namespace + [self.name]

            if kind == 'field':
                self.fields.append(MessageField(item[1]))
            elif kind == 'enum':
                self.enums.append(Enum(item, sub_namespace))
            elif kind == 'message':
                self.messages.append(Message(item, sub_namespace))
            elif kind == 'oneof':
                self.oneofs.append(Oneof(item, sub_namespace))
            elif kind in ['map', ';']:
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

    def used_types(self):
        used_types = []

        for field in self.fields:
            if field.full_type in SCALAR_VALUE_TYPES:
                continue

            used_types.append(field.full_type)

        for oneof in self.oneofs:
            for field in oneof.fields:
                if field.full_type in SCALAR_VALUE_TYPES:
                    continue

                used_types.append(field.full_type)

        for message in self.messages:
            used_types += message.used_types()

        return list(set(used_types))


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


class ImportedProto:

    def __init__(self, tokens, import_paths):
        self.path = tokens[2].strip('"')
        self.abspath = find_file(self.path, import_paths)

        with open(self.abspath) as fin:
            tree = Parser().parse(fin.read())

        self.package = load_package(tree)
        self.enums = [
            tokens[1]
            for tokens in tree[1].get('enum', [])
        ]
        self.messages = [
            tokens[1]
            for tokens in tree[1].get('message', [])
        ]

    @property
    def type_names(self):
        return self.enums + self.messages


class Proto:

    def __init__(self, tree, abspath, import_paths):
        self.abspath = abspath
        self.package = load_package(tree)
        self.imports = load_imports(tree, import_paths)
        namespace = self.namespace_base()
        self.options = load_options(tree)
        self.messages = load_messages(tree, namespace)
        self.services = load_services(tree)
        self.enums = load_enums(tree, namespace)
        self.messages_stack = []
        self.resolve_messages_fields_types()
        self.resolve_messages_fields_type_kinds()
        self.messages = self.sort_messages_by_usage(self.messages)

    @property
    def type_names(self):
        type_names = [enum.name for enum in self.enums]
        type_names += [message.name for message in self.messages]

        return type_names

    def namespace_base(self):
        if self.package is None:
            return []
        else:
            return [self.package]

    def resolve_messages_fields_types(self):
        for message in self.messages:
            self.resolve_message_fields_types(message)

    def resolve_message_fields_types(self, message):
        self.messages_stack.append(message)

        for field in message.fields:
            if field.type in SCALAR_VALUE_TYPES:
                continue

            self.resolve_field_type(field)

        for sub_message in message.messages:
            self.resolve_message_fields_types(sub_message)

        for oneof in message.oneofs:
            for field in oneof.fields:
                if field.type in SCALAR_VALUE_TYPES:
                    continue

                self.resolve_field_type(field)

        self.messages_stack.pop()

    def resolve_field_type(self, field):
        if field.namespace:
            return

        for message in reversed(self.messages_stack):
            if field.type in message.type_names:
                namespace = message.namespace + [message.name]
                package = self.package
                break
        else:
            if field.type in self.type_names:
                namespace = self.namespace_base()
                package = self.package
            else:
                for imported in self.imports:
                    if imported.package == self.package:
                        if field.type in imported.type_names:
                            namespace = self.namespace_base()
                            package = self.package
                            break
                else:
                    namespace = []
                    package = None

        field.namespace = namespace
        field.package = package

    def resolve_messages_fields_type_kinds(self):
        for message in self.messages:
            self.resolve_message_fields_type_kinds(message)

    def resolve_message_fields_type_kinds(self, message):
        self.messages_stack.append(message)

        for field in message.fields:
            self.resolve_field_type_kind(field)

        for sub_message in message.messages:
            self.resolve_message_fields_type_kinds(sub_message)

        for oneof in message.oneofs:
            for field in oneof.fields:
                self.resolve_field_type_kind(field)

        self.messages_stack.pop()

    def resolve_field_type_kind(self, field):
        if field.type in SCALAR_VALUE_TYPES:
            field.type_kind = 'scalar-value-type'
        elif self.is_field_enum(field):
            field.type_kind = 'enum'
        else:
            field.type_kind = 'message'

    def is_field_enum(self, field):
        for imported in self.imports:
            if field.package == imported.package:
                if field.type in imported.enums:
                    return True
                elif field.type in imported.messages:
                    return False

        if field.package == self.package:
            offset = len(self.namespace_base())
            type = self.lookup_type(field.namespace[offset:] + [field.type],
                                    self.enums,
                                    self.messages)

            return isinstance(type, Enum)

        raise Error(f"'{field.type}' is not defined.")

    def lookup_type(self, path, enums, messages):
        name = path[0]
        rest = path[1:]

        if rest:
            for message in messages:
                if message.name == name:
                    return self.lookup_type(rest,
                                            message.enums,
                                            message.messages)
        else:
            for enum in enums:
                if enum.name == name:
                    return enum

            for message in messages:
                if message.name == name:
                    return message

    def sort_messages_by_usage(self, messages):
        reversed_sorted_messages = []

        for message in messages:
            message.messages = self.sort_messages_by_usage(message.messages)

            # Insert first in the reversed list if there are no types
            # using this type.
            insert_index = 0

            for i, sorted_message in enumerate(reversed_sorted_messages, 1):
                if message.full_name in sorted_message.used_types():
                    if i > insert_index:
                        insert_index = i

            reversed_sorted_messages.insert(insert_index, message)

        return list(reversed(reversed_sorted_messages))


def load_message_type(tokens):
    return tokens[1].split('.')


def load_package(tokens):
    try:
        return tokens[1]['package'][0][1]
    except KeyError:
        return None


def load_imports(tokens, import_paths):
    return [
        ImportedProto(imported, import_paths)
        for imported in tokens[1].get('import', [])
    ]


def load_options(tokens):
    return [
        Option(option)
        for option in tokens[1].get('option', [])
    ]


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


def find_file(filename, import_paths):
    for import_path in import_paths:
        filepath = os.path.join(import_path, filename)

        if os.path.exists(filepath):
            break
    else:
        filepath = filename

    filepath = os.path.abspath(filepath)

    if not os.path.exists(filepath):
        raise Exception(f"'{filename}' not found in import path.")

    return filepath


def parse_file(filename, import_paths=None):
    if import_paths is None:
        import_paths = []

    with open(filename, 'r') as fin:
        return Proto(Parser().parse(fin.read()),
                     find_file(filename, import_paths),
                     import_paths)
