import os
import re
import textparser
from textparser import Sequence
from textparser import ZeroOrMore
from textparser import ZeroOrMoreDict
from textparser import choice
from textparser import Optional
from textparser import Forward
from textparser import Tag
from textparser import DelimitedList
from textparser import ParseError
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


def format_parse_error(string, offset):
    return str(ParseError(string, offset))


def ignore_comments(string):
    """Ignore comments in given string by replacing them with spaces. This
    reduces the parsing time by roughly a factor of two.

    """

    comments = [
        (mo.start(), mo.group(0))
        for mo in re.finditer(r'(/\*|\*/|//|\n)', string)
    ]

    comments.sort()

    in_single_line_comment = False
    in_multi_line_comment = False
    start_offset = 0
    non_comment_offset = 0
    chunks = []

    for offset, kind in comments:
        if in_single_line_comment:
            if kind == '\n':
                in_single_line_comment = False
                chunks.append(' ' * (offset - start_offset))
                non_comment_offset = offset
        elif in_multi_line_comment:
            if kind == '/*':
                raise Error(format_parse_error(string, start_offset))
            elif kind == '*/':
                in_multi_line_comment = False
                offset += 2
                chunks.append(' ' * (offset - start_offset))
                non_comment_offset = offset
        elif kind == '//':
            in_single_line_comment = True
            start_offset = offset
            chunks.append(string[non_comment_offset:start_offset])
        elif kind == '/*':
            in_multi_line_comment = True
            start_offset = offset
            chunks.append(string[non_comment_offset:start_offset])

    if in_single_line_comment:
        offset = len(string)
        chunks.append(' ' * (offset - start_offset))
        non_comment_offset = offset

    if in_multi_line_comment:
        raise Error(format_parse_error(string, start_offset))

    chunks.append(string[non_comment_offset:])

    return ''.join(chunks)


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
        'map',
        'reserved',
        'max',
        'to',
        'weak',
        'public'
    ]

    def token_specs(self):
        return [
            ('SKIP',          r'[ \r\n\t]+|//.*?\n'),
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
        full_ident = DelimitedList(ident, '.')
        empty_statement = ';'
        message_type = Sequence(Optional('.'), full_ident)
        constant = choice(Tag('bool', choice('true', 'false')),
                          Tag('ident', full_ident),
                          Tag('string', 'STRING'),
                          Tag('integer', 'INT'))
        option_name = Sequence(choice(ident,
                                      Sequence('(', full_ident, ')')),
                               ZeroOrMore(Sequence('.', ident)))
        options = Optional(Sequence('[',
                                    DelimitedList(
                                        Sequence(option_name, '=', constant)),
                                    ']'))

        # Import.
        import_ = Sequence('import',
                           Optional(choice('weak', 'public')),
                           'STRING')

        # Package.
        package = Sequence('package', full_ident, ';')

        # Option.
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

        # Reserved.
        field_number_range = Sequence('INT',
                                      Optional(Sequence('to',
                                                        choice('INT', 'max'))))
        reserved = Sequence('reserved', choice(DelimitedList(field_number_range),
                                               DelimitedList('STRING')))

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
                                               reserved,
                                               empty_statement)),
                             '}')

        # Service.
        rpc_name = ident
        rpc = Sequence('rpc',
                       rpc_name,
                       '(', Optional('stream'), message_type, ')',
                       'returns',
                       '(', Optional('stream'), message_type, ')',
                       choice(Sequence('{',
                                       ZeroOrMore(option),
                                       '}'),
                              ';'))
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
        if tokens[1][0][0] == '(':
            full_ident = '.'.join(tokens[1][0][1])
            self.name = f'({full_ident})'
        else:
            self.name = tokens[1][0]

        self.name += ''.join([f'.{ident}' for _, ident in tokens[1][1]])

        kind, value = tokens[3]

        if kind == 'string':
            value = value[1:-1]
        elif kind == 'bool':
            value = (value == 'true')
        elif kind == 'ident':
            value = value[0]
        elif kind == 'integer':
            value = int(value[0])

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

    @property
    def name_snake_case(self):
        return camel_to_snake_case(self.name)


class MessageField(Field):

    def __init__(self, type, name, field_number, repeated):
        super().__init__(type, name, field_number)
        self.repeated = repeated

    @classmethod
    def from_field_tokens(cls, tokens):
        return cls(load_message_type(tokens[1]),
                   tokens[2],
                   int(tokens[4]),
                   bool(tokens[0]))


class Message:
    """A message.

    """

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
                self.fields.append(MessageField.from_field_tokens(item[1]))
            elif kind == 'enum':
                self.enums.append(Enum(item, sub_namespace))
            elif kind == 'message':
                self.messages.append(Message(item, sub_namespace))
            elif kind == 'oneof':
                self.oneofs.append(Oneof(item, sub_namespace))
            elif kind == 'map':
                self._load_map(item, sub_namespace)
            elif kind in ['reserved', ';']:
                pass
            else:
                raise InternalError(kind)

    def _load_map(self, tokens, sub_namespace):
        key_type = tokens[2]
        value_type = load_message_type(tokens[4])[-1]
        name = tokens[6]
        field_number = int(tokens[8])
        map_type = f'PbtoolsMap{name.title()}'
        map_message_tokens = [
            'message',
            map_type, '{', [
                ('field', [[], [[], [key_type]], 'key', '=', '1', [], ';']),
                ('field', [[], [[], [value_type]], 'value', '=', '2', [], ';'])
            ], '}'
        ]
        self.messages.append(Message(map_message_tokens, sub_namespace))
        self.fields.append(MessageField([map_type], name, field_number, True))

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
        self.request_type = tokens[4][1][0]
        self.request_stream = bool(tokens[3])
        self.response_type = tokens[9][1][0]
        self.response_stream = bool(tokens[8])


class Service:

    def __init__(self, tokens):
        self.name = tokens[1]
        self.rpcs = []

        for item in tokens[3]:
            kind = item[0]

            if kind == 'rpc':
                self.rpcs.append(Rpc(item))
            elif kind == ';':
                pass
            else:
                raise InternalError(kind)


class ImportedProto:

    def __init__(self, tokens, import_paths):
        self.path = tokens[2].strip('"')
        self.abspath = find_file(self.path, import_paths)

        with open(self.abspath) as fin:
            tree = Parser().parse(ignore_comments(fin.read()))

        # ToDo: Public imports should be found as well.
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
    """A proto3-file. :func:`~pbtools.parse_file()` returns an instance of
    this class.

    """

    def __init__(self, tree, abspath, import_paths):
        self.abspath = abspath
        self._package = load_package(tree)
        self._imports = load_imports(tree, import_paths)
        namespace = self.namespace_base()
        self._options = load_options(tree)
        self._messages = load_messages(tree, namespace)
        self._services = load_services(tree)
        self._enums = load_enums(tree, namespace)
        self.messages_stack = []
        self.resolve_messages_fields_types()
        self.resolve_messages_fields_type_kinds()
        self._messages = self.sort_messages_by_usage(self._messages)

    @property
    def package(self):
        """Package name, or ``None`` if missing.

        """

        return self._package

    @property
    def imports(self):
        """A list of all imports.

        """

        return self._imports

    @property
    def options(self):
        """A list of all options.

        """

        return self._options

    @property
    def services(self):
        """A list of all services.

        """

        return self._services

    @property
    def messages(self):
        """A list of all messages.

        """

        return self._messages

    @property
    def enums(self):
        """A list of all enums.

        """

        return self._enums

    @property
    def type_names(self):
        type_names = [enum.name for enum in self._enums]
        type_names += [message.name for message in self._messages]

        return type_names

    def namespace_base(self):
        if self._package is None:
            return []
        else:
            return [self._package]

    def resolve_messages_fields_types(self):
        for message in self._messages:
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
                package = self._package
                break
        else:
            if field.type in self.type_names:
                namespace = self.namespace_base()
                package = self._package
            else:
                for imported in self._imports:
                    if imported.package == self._package:
                        if field.type in imported.type_names:
                            namespace = self.namespace_base()
                            package = self._package
                            break
                else:
                    namespace = []
                    package = None

        field.namespace = namespace
        field.package = package

    def resolve_messages_fields_type_kinds(self):
        for message in self._messages:
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
        for imported in self._imports:
            if field.package == imported.package:
                if field.type in imported.enums:
                    return True
                elif field.type in imported.messages:
                    return False

        if field.package == self._package:
            offset = len(self.namespace_base())
            type = self.lookup_type(field.namespace[offset:] + [field.type],
                                    self._enums,
                                    self._messages)

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
    return tokens[1]


def load_package(tokens):
    try:
        return '.'.join(tokens[1]['package'][0][1])
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
        raise Error(f"'{filename}' not found in import path.")

    return filepath


def parse_file(filename, import_paths=None):
    """Parse given proto3-file `filename` and its imports. Returns a
    :class:`~pbtools.parser.Proto` object.

    `import_paths` is a list of paths where to search for imported
    files.

    """

    if import_paths is None:
        import_paths = []

    with open(filename, 'r') as fin:
        return Proto(Parser().parse(ignore_comments(fin.read())),
                     find_file(filename, import_paths),
                     import_paths)
