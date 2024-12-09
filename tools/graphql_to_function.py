import re
import argparse
import os

from honeycomb_api.transaction_creation import *
from honeycomb_api.types import *
from honeycomb_api.fetchers import *


CLASS_TYPE = "HoneyComb"
RETURN_TYPE = "Variant"
TYPE_DIR = "honeycomb/types/"
TYPE_INCLUDE_GUARD_PREFIX = "GODOT_SOLANA_SDK_HONEYCOMB_TYPE_"

SIGNER_TYPE = "const Variant&"
SERVER_SIGNER = 'Pubkey::new_from_string("11111111111111111111111111111111")'

resource_includes = [
    'godot_cpp/variant/variant.hpp',
    'godot_cpp/core/class_db.hpp',
    'godot_cpp/classes/resource.hpp',
]

header_includes = [
    '"pubkey.hpp"',
]

QL_TO_VARIANT = {
   "String": "String",
   "String!": "String",
   "[String!]": "PackedStringArray",
   "[String]": "PackedStringArray",
   "[Int!]": "PackedInt32Array",
   "BigInt": "int64_t",
   "Int": "int32_t",
   "InitResourceInput": "Variant",
   "TreeSetupConfig": "Variant",
   "UserInfoInput": "Variant",
   "[IngredientsInput!]": "Array",
   "MealInput": "Variant",
   "ModifyDelegationInput": "Variant",
   "CreateBadgeCriteriaInput": "Variant",
   "[AssociatedProgramInput!]": "Array",
   "ProfileDataConfigInput": "Variant",
   "Boolean": "bool",
   "ServiceDelegationInput": "Variant",
   "ProfileInfoInput": "Variant",
   "CustomDataInput": "Variant",
   "ClaimBadgeCriteriaInput": "Variant",
   "UpdateBadgeCriteriaInput": "Variant",
   "PartialUserInfoInput": "Variant",
   "UpdateWalletInput": "Variant",
   "CreateStakingPoolMetadataInput": "Variant",
   "InitStakingMultiplierMetadataInput": "Variant",
   "UpdateStakingPoolMetadataInput": "Variant",
   "[AddMultiplierMetadataInput]": "Array",
   "AddMultiplierMetadataInput": "Variant",
   "[CharacterTraitInput!]": "Array",
   "CharacterConfigInput": "Variant",
   "VecMapGeneric": "Array",
   "RecallFromMissionData": "Variant",
   "[Bytes!]": "Array",
   "Bytes": "PackedByteArray",
   "TransactionBundlesOptions": "Variant",
   "SendTransactionBundlesOptions": "Variant",
   "[UserInfoInput!]": "Array",
   "NewMissionPoolData": "Variant",
   "UpdateMissionPoolData": "Variant",
   "NewMissionData": "Variant",
   "UpdateMissionInput": "Variant",
   "ParticipateOnMissionData": "Variant",
   "Pubkey": "Variant",
   '[Pubkey!]': "Array",
   "ModifyServiceDelegationInput": "Variant",
   "[ServiceDelegationHiveControl!]": "Array",
   "[ServiceDelegationAssetAssembler!]": "Array",
   "[ServiceDelegationAssetManager!]": "Array",
   "[ServiceDelegationCurrencyManager!]": "Array",
   "[ServiceDelegationNectarStaking!]": "Array",
   "[ServiceDelegationNectarMissions!]": "Array",
   "[ServiceDelegationBuzzGuild!]": "Array",
   "[NftWrapCriteriaInput!]": "Array",
   "JSON": "Dictionary",
   "BasicTreeConfig": "Variant",
   "AdvancedTreeConfig": "Variant",
   "AssemblerConfigInput": "Variant",
   "MultiplierTypeInput": "Variant",
   "LockTypeEnum": "int32_t",
   "ResourceStorageEnum": "int32_t",
   "BadgesCondition": "int32_t",
   "AssetAssemblerPermissionInput": "int32_t",
   "AssetManagerPermissionInput": "int32_t",
   "BuzzGuildPermissionInput": "int32_t",
   "HiveControlPermissionInput": "int32_t",
   "CurrencyManagerPermissionInput": "int32_t",
   "NectarMissionsPermissionInput": "int32_t",
   "NectarStakingPermissionInput": "int32_t",
   "NewMissionCost": "Variant",
   "[MissionReward!]": "Array",
   "[MissionReward]": "Array",
   "[NftCreatorInput!]": "Array",
   "ServiceDelegationHiveControl": "Variant",
   "ServiceDelegationAssetAssembler": "Variant",
   "ServiceDelegationAssetManager": "Variant",
   "ServiceDelegationCurrencyManager": "Variant",
   "ServiceDelegationNectarStaking": "Variant",
   "ServiceDelegationNectarMissions": "Variant",
   "ServiceDelegationBuzzGuild": "Variant",
   "[SerializableActionsInput!]": "Array",
   "[ServiceDelegation!]": "Array",
   "InitResourceInput!": "Variant",
   "TreeSetupConfig!": "Variant",
   "BigInt!": "Variant",
   '[IngredientsInput!]!': "Array",
   'MealInput!': "Variant",
   '[String!]!': "PackedStringArray",
   'NewMissionPoolData!': "Variant",
   'UpdateMissionPoolData!': "Variant",
   'NewMissionData!': "Variant",
}

GODOT_TYPE_DEFVAL = {
  "String": '""',
  "int64_t": '-1',
  "int32_t": "-1",
  "PackedStringArray": "PackedStringArray()",
  "Variant": "Variant(nullptr)",
  "Array": "Array()",
  "bool": "false",
}

GODOT_VARIANT_ENUM_TYPE = {
  "bool": "BOOL",
  "int32_t": "INT",
  "int64_t": "INT",
  "String": "STRING",
  "PackedByteArray": "PACKED_BYTE_ARRAY",
  "PackedInt32Array": "PACKED_INT32_ARRAY",
  "PackedStringArray": "PACKED_STRING_ARRAY",
  "Variant": "OBJECT",
  "Array": "ARRAY",
  "Dictionary": "DICTIONARY",
}

def strip(input_string: str):
    """Strips whitespaces in beginning and end of input string.

    Args:
        input (str): Input string to strip whitespaces from.

    Returns:
        _type_: String without whitespaces trailing and leading whitespaces.
    """
    return input_string.lstrip().rstrip()


class CppBuilder:
    """Simple CPP builder that keeps states of brackets.
    """
    def __init__(self):
        self.namespace_stack = []
        self.include_guard_stack = []
        self.indent_level = 0

    def indent(self) -> str:
        """Returns the appropriate level of indentation.

        Returns:
            str: Indentation string.
        """
        return '\t' * self.indent_level


    def print_include_statement(self, filename: str, use_angle_brackets: bool = False) -> str:
        """Prints an include statement, including the provided file.

        Args:
            filename (str): File to include.
            use_angle_brackets (bool, optional): Specifies if angle brackets should be used.
            Defaults to False.

        Returns:
            str: Include statement.
        """
        if use_angle_brackets:
            return f'#include <{filename}>\n'

        return f'#include "{filename}"\n'

    def start_namespace(self, namespace: str) -> str:
        """Prints a namespace start.

        Args:
            namespace (str): name of namespace.

        Returns:
            str: Cpp code for namespace start.
        """
        self.namespace_stack.append(namespace)
        namespace_string = f'{self.indent()}namespace {namespace}'
        self.indent_level += 1

        return namespace_string + '{\n'

    def start_include_guard(self, guard_name: str) -> str:
        """Starts an include guard.

        Args:
            guard_name (str): Include guard name.

        Returns:
            str: Cpp code for include guard start.
        """
        return f'#ifndef {guard_name}\n#define {guard_name}\n'

    def assign_query_fields(self, fields: str) -> str:
        """Flattens given fields and assigns it to CPP property.

        Args:
            fields (str): Input query fields.

        Returns:
            str: Assigning query fields code.
        """
        return f'{self.indent()}query_fields = "{re.sub(r'\s+', ' ', fields.replace('\n', ''))}";\n'


class QLArgumentList:
    """Argument list for a graphQL query.
    """
    def __init__(self):
        self.optional_names = []
        self.optional_types = []
        self.required_names = []
        self.required_types = []

    def has_required_and_optional(self) -> bool:
        """Checks if there are both required and optional arguments.

        Returns:
            bool: Both required and optional arguments exist.
        """
        return self.required_names and self.optional_names

    def is_valid(self) -> bool:
        """Checks if argument list names has a corresponding types.

        Returns:
            bool: Argument names has corresponding type.
        """
        if len(self.optional_names) != len(self.optional_types):
            return False

        if len(self.required_names) != len(self.required_types):
            return False

        return True


class SimpleQLRequestParser:
    """Simple query parser for the honeycomb transaction creating queries.
    """
    def __init__(self):
        self.query_name = ""
        self.function_name = ""
        self.query_fields = ""
        self.names = []
        self.types = []

    def parse(self, request_query: str):
        """Parses a string of a request query.

        Args:
            request_query (str): The request query to parse.
        """
        # Parse Query name.
        query_keyword = "query"
        start_pos = request_query.find(query_keyword) + len(query_keyword)
        end_pos = request_query.find('(')
        self.query_name = strip(request_query[start_pos:end_pos])

        # Parse argument names and types.
        start_pos = request_query.find('(') + 1
        end_pos = request_query.find(')')

        argument_section = request_query[start_pos:end_pos]
        argument_section = strip(argument_section)

        arguments = argument_section.split(',')
        for argument in arguments:
            (name, type_name) = argument.split(':')

            # Remove $ from name and type.
            name = strip(name)[1:]
            type_name = strip(type_name)

            self.names.append(name)
            self.types.append(type_name)

        # Fetch function part of input.
        start_pos = request_query.find('{')
        function_section = request_query[start_pos:]

        # Strip one layer of {}'s.
        function_section = strip(function_section)[1:-1]

        # Parse function name.
        end_pos = function_section.find('(')
        self.function_name = strip(function_section[:end_pos])

        # Parse query fields.
        start_pos = function_section.find('{')
        self.query_fields = strip(function_section[start_pos:])
        
        # Strip one layer of {}'s.
        self.query_fields = self.query_fields[1:-1]


    def get_optional_argument_names(self) -> list[str]:
        """Gets all optional argument names.

        Returns:
            list[str]: Optional arguments names.
        """
        return [n for n, t in zip(self.names, self.types) if not t.endswith('!')]

    def get_optional_argument_types(self) -> list[str]:
        """Gets all optional argument types

        Returns:
            list[str]: Optional argument types.
        """
        return [t for t in self.types if not t.endswith('!')]

    def get_required_argument_names(self) -> list[str]:
        """Gets all required arguments.

        Returns:
            list[str]: Required argument names.
        """
        return [n for n, t in zip(self.names, self.types) if t.endswith('!')]

    def get_required_argument_types(self) -> list[str]:
        """Gets all required argument types.

        Returns:
            list[str]: Required argument types.
        """
        return [t[:-1] for t in self.types if t.endswith('!')]


    def get_argument_list(self) -> QLArgumentList:
        """Returns information about optional and required arguments.

        Returns:
            QLArgumentList: Information about required and optional arguments.
        """
        argument_list = QLArgumentList()
        argument_list.required_names = self.get_required_argument_names()
        argument_list.required_types = self.get_required_argument_types()
        argument_list.optional_names = self.get_optional_argument_names()
        argument_list.optional_types = self.get_optional_argument_types()

        return argument_list


class GQLParse:
    def __init__(self):
        self.signers = []
        self.non_signers = []
        self.required_args = []
        self.optional_args = []
        self.original_required_args = []
        self.original_optional_args = []
        self.str = ""
        self.method_definitions = ""
        self.bound_methods = ""
        self.method_implementations = ""
        self.types = []
        self.builder = CppBuilder()

    def save_cpp_file(self, filepath, outdir_hpp):
        with open(filepath, 'w', encoding='utf-8') as file:
            file.write(self.print_cpp_file(outdir_hpp))

    def save_hpp_file(self, filepath):
        file = open(filepath, "w")
        file.write(self.print_header_file())
        file.close()

    def save_resource_files(self, outdir_cpp, outdir_hpp):
        for i in range(len(self.types)):
            type = self.types[i]
            class_name = type["classname"]
            hpp_file = open(os.path.join(outdir_hpp, f"{class_name}.hpp"), "w")
            hpp_file.write(self.print_resource_hpp_file(i))
            hpp_file.close()

            cpp_file = open(os.path.join(outdir_cpp, f"{class_name}.cpp"), "w")
            cpp_file.write(self.print_resource_cpp_file(i, outdir_hpp.split("/", maxsplit=1)[-1]))
            cpp_file.close()


    def function_name_to_alias(self, function_name):
        alias = function_name[6:-11]
        return ''.join(['_'+c.lower() if c.isupper() else c for c in alias]).lstrip('_')


    def ql_type_to_godot(self, ql_type):
        if ql_type[-1] == '!':
            return QL_TO_VARIANT[ql_type[:-1]]
        else:
            return QL_TO_VARIANT[ql_type]


    def add_required_arg_to_bind(self, arg):
        self.bind_methods = ""


    def add_required_arg(self, name, data_type):
        self.required_args.append((name, data_type))


    def add_optional_arg(self, name, data_type):
        self.optional_args.append((name, data_type))


    def process_arg(self, arg):
        (name, data_type) = arg.split(":")
        name = name.lstrip()
        data_type = data_type.lstrip().rstrip()
        assert(name[0] == '$')
        name = name[1:]
        if(data_type[-1] == "!"):
            data_type = data_type[0:-1]

            self.add_required_arg_to_bind(arg)
            self.add_required_arg(name, data_type)

        else:
            self.add_optional_arg(name, data_type)


    def read_args(self, str):
        (function_name, str) = str.split("(", maxsplit=1)
        function_name = function_name[0].lower() + function_name[1:]

        assert(str[-1] == ")")
        str = str[0:-1]

        args = str.split(",")
        for arg in args:
            self.process_arg(arg)

        self.function_name = function_name


    def print_arg(self, arg_name, arg_type, optional=False, special=False):
        result_string = ""
        godot_type = self.ql_type_to_godot(arg_type)

        is_optional = "false"
        if optional and not special:
            is_optional = "true"
            if arg_name in self.signers or arg_name in self.non_signers:
                result_string += f'\tif({arg_name} != Variant(nullptr))' + "{\n\t"
            else:
                result_string += f'\tif({arg_name} != {GODOT_TYPE_DEFVAL[godot_type]})' + "{\n\t"

        result_string += "\tadd_arg"

        if godot_type == "String":
            result_string += f'("{arg_name}", "{arg_type}", Pubkey::string_from_variant({arg_name}), {is_optional});\n'
        elif godot_type == "Variant" and arg_type == "Pubkey":
            result_string += f'("{arg_name}", "{arg_type}", Object::cast_to<{arg_type}>({arg_name})->to_string(), {is_optional});\n'
        elif godot_type == "Variant":
            result_string += f'("{arg_name}", "{arg_type}", Object::cast_to<godot::honeycomb_resource::{arg_type}>({arg_name})->to_dict(), {is_optional});\n'
        else:
            result_string += f'("{arg_name}", "{arg_type}", {arg_name}, {is_optional});\n'

        if optional and not special:
            result_string += '\t}\n'

        return result_string


    def print_arg_array(self, arg, optional=False):
        result_string = ""
        (arg_name, arg_type) = arg
        godot_type = self.ql_type_to_godot(arg_type)

        is_optional = "false"
        if optional:
            is_optional = "true"
            if arg_name in self.signers or arg_name in self.non_signers:
                    result_string += f'\tif({arg_name} != Variant(nullptr))' + "{\n\t"
            else:
                    result_string += f'\tif({arg_name} != {GODOT_TYPE_DEFVAL[godot_type]})' + "{\n\t"

        result_string += "\targs.append"

        if godot_type == "String":
            result_string += f'("{arg_name}", "{arg_type}", Pubkey::string_from_variant({arg_name}), {is_optional});\n'
        elif godot_type == "Variant" and arg_type == "Pubkey":
            result_string += f'("{arg_name}", "{arg_type}", Object::cast_to<{arg_type}>({arg_name})->to_string(), {is_optional});\n'
        elif godot_type == "Variant":
            result_string += f'("{arg_name}", "{arg_type}", Object::cast_to<godot::honeycomb_resource::{arg_type}>({arg_name})->to_dict(), {is_optional});\n'
        else:
            result_string += f'("{arg_name}", "{arg_type}", {arg_name}, {is_optional});\n'

        if optional:
            result_string += '\t}\n'

        return result_string


    def print_args_section(self, required_arg_names: list[str], required_arg_types: list[str], optional_arg_names: list[str], optional_arg_types: list[str], signers: list[str] = None, non_signers: list[str] = None):
        """Returns the section of code that adds arguments to QL client.

        Args:
            required_arg_names (list[str]): List of required argument names.
            required_arg_types (list[str]): List of required argument types.
            optional_arg_names (list[str]): List of optional argument names.
            optional_arg_types (list[str]): List of optional argument types.
            signers (list[str], optional): List of argument names that are signers. Defaults to None.
            non_signers (list[str], optional): List of argument names that are non signers. Defaults to None.
        """

        result_string = ""

        result_string += "".join(self.print_arg(n, t, False, True) for n, t in zip(required_arg_names, required_arg_types))
        result_string += "".join(self.print_arg(n, t, True, True) for n, t in zip(optional_arg_names, optional_arg_types))

        return result_string


    def print_args_section_special(self):

        result_string = ""

        for required_arg in self.required_args:
            result_string += self.print_arg_array(required_arg)
        for optional_arg in self.optional_args:
            result_string += self.print_arg_array(optional_arg, True)

        return result_string


    def graphql_to_type(self, type_str):
        new_type = {}
        (firstWord, type_str) = type_str.split(maxsplit=1)

        new_type['classname'] = firstWord
        new_type['properties'] = []

        lines = type_str.splitlines()

        for line in lines:
            (name, type) = line.split('-', maxsplit=1)
            new_type['properties'].append((name.rstrip(), type.lstrip()))

        self.types.append(new_type)


    def graphql_to_fetcher(self, str):
        request_parser = SimpleQLRequestParser()
        request_parser.parse(str)
        self.str = str
        self.required_args = []
        self.optional_args = []
        self.original_required_args = []
        self.original_optional_args = []

        (firstWord, self.str) = self.str.split(maxsplit=1)
        assert(firstWord == "query")

        func_header = re.match(r"(.*)\(([\s\S]*?)\)", self.str)

        self.read_args(func_header.group())

        self.str = self.str[func_header.span()[1]:]
        chunk_string = re.search(r"\{\s*[\w\s:,$]*?\([\s\S]*?\)", self.str)
        self.str = self.str[chunk_string.span()[1]:]

        self.query_fields = re.sub(r"[\r\n]+", "", self.str.lstrip().rstrip()[1:-1].rstrip()[:-1])

        chunk_string = re.search(r"\{\s*tx\s*\{\s*[\w\s]*\}\s*[\w\s]*\}", self.str)
        if not chunk_string:
            chunk_string = re.search(r"\{\s*[\w\s]*\}", self.str)

        self.str = chunk_string.group()
        self.str = self.str.lstrip()[1:]
        self.str = self.str.rstrip()[0:-1]

        # Make a line of query
        query_fields_re = re.sub(r"[\r\n]+", "", self.str)
        #print(query_fields_re)
        #exit(1)
        #self.query_fields = re.sub(r'\s+', ' ', query_fields_re)

        self.method_definitions += f'\t{self.print_function_name_header_special()}\n'
        self.method_implementations += self.print_function_name_special()

        self.method_implementations += "{\n"
        self.method_implementations += "\tif(pending){\n"
        self.method_implementations += "\t\treturn ERR_BUSY;\n"
        self.method_implementations += "\t}\n"
        self.method_implementations += self.print_args_section(request_parser.get_required_argument_names(), request_parser.get_required_argument_types(), request_parser.get_optional_argument_names(), request_parser.get_optional_argument_types(), self.signers, self.non_signers) + '\n'
        self.method_implementations += self.print_method_name() + '\n'
        self.method_implementations += self.print_last_section_special() + '\n'

        self.append_method_bind_special()

        self.str = self.str[chunk_string.span()[1]:]

        # Find type that this function returns and add special attributes to it.
        for type in self.types:
            if type['classname'][1:] == self.function_name[1:]:
                self.function_name = "fetch"
                type['fetch_declaration'] = f'\t{self.print_function_name_header(request_parser.function_name, request_parser.get_required_argument_names(), request_parser.get_required_argument_types(), request_parser.get_optional_argument_names(), request_parser.get_optional_argument_types())}\n'
                type['fetch_definition'] = self.print_function_name(request_parser.function_name, request_parser.get_argument_list()).replace(CLASS_TYPE, type['classname'])
                type['fetch_definition'] += "{\n"
                type['fetch_definition'] += self.print_fetch_implementation()
                type['fetch_definition'] += "}\n"

        return ""


    def print_fetch_implementation(self):
        result = "\tif(honey_comb == nullptr){\n\t\thoney_comb = memnew(HoneyComb);\n\t}\n"

        return result

    def graphql_to_function(self, str, signers, non_signers):
        self.str = str
        self.signers = signers
        self.non_signers = non_signers
        self.required_args = []
        self.optional_args = []
        self.original_required_args = []
        self.original_optional_args = []

        request_parser = SimpleQLRequestParser()
        request_parser.parse(str)

        self.function_name = request_parser.function_name
        self.query_fields = request_parser.query_fields

        method_definition = self.print_function_name_header(request_parser.function_name, request_parser.get_required_argument_names(), request_parser.get_required_argument_types(), request_parser.get_optional_argument_names(), request_parser.get_optional_argument_types(), signers, non_signers)
        self.method_definitions += f'\t{method_definition}\n'

        self.method_implementations += self.print_function_name(request_parser.function_name, request_parser.get_argument_list())
        self.method_implementations += "{\n"
        self.method_implementations += "\tif(pending){\n"
        self.method_implementations += "\t\treturn ERR_BUSY;\n"
        self.method_implementations += "\t}\n"
        self.method_implementations += self.print_signer_section() + '\n'
        self.method_implementations += self.print_args_section(request_parser.get_required_argument_names(), request_parser.get_required_argument_types(), request_parser.get_optional_argument_names(), request_parser.get_optional_argument_types(), self.signers, self.non_signers) + '\n'
        self.method_implementations += self.print_method_name() + '\n'
        self.method_implementations += self.print_last_section() + '\n'

        self.append_method_bind(request_parser.function_name, request_parser.get_argument_list(), signers, non_signers)

        return ""
  

    def append_method_bind(self, function_name : str, argument_list: QLArgumentList, signers: list[str] = None, non_signers: list[str] = None):
        result_string = f'ClassDB::bind_method(D_METHOD("{self.function_name_to_alias(function_name)}", '

        result_string += ", ".join([f'"{n}"' for n in argument_list.required_names])

        if argument_list.required_names and argument_list.optional_names:
            result_string += ', '

        result_string += ", ".join([f'"{n}"' for n in argument_list.optional_names])
        result_string += f'), &{CLASS_TYPE}::{function_name}'

        if argument_list.optional_names:
            result_string += ', '

        def get_defval(name, ql_type):
            if name in (signers or []) or name in (non_signers or []):
                return f'DEFVAL({GODOT_TYPE_DEFVAL["Variant"]})'
            
            return f'DEFVAL({GODOT_TYPE_DEFVAL[QL_TO_VARIANT[ql_type]]})'

        result_string += ', '.join([f'{get_defval(n, t)}' for n, t in zip(argument_list.optional_names, argument_list.optional_types)])
        result_string += ');\n'

        self.bound_methods += "\t" + result_string

        return result_string


    def append_method_bind_special(self):
        result_string = f'ClassDB::bind_method(D_METHOD("{self.function_name}", '

        for required_arg in self.required_args:
            (arg_name, _arg_type) = required_arg
            result_string += f'"{arg_name}", '

        for optional_arg in self.optional_args:
            (arg_name, _arg_type) = optional_arg
            result_string += f'"{arg_name}", '
        
        if self.required_args or self.optional_args:
            result_string = result_string[0:-2]

        result_string += f'), &{CLASS_TYPE}::{self.function_name}'

        if self.optional_args:
            result_string += ', '

        temp_optional_args = self.optional_args

        for optional_arg in temp_optional_args:
            (arg_name, arg_type) = optional_arg
            godot_type = QL_TO_VARIANT[arg_type]
            if arg_name in self.signers or arg_name in self.non_signers:
                result_string += f'DEFVAL({GODOT_TYPE_DEFVAL["Variant"]}), '
            else:
                result_string += f'DEFVAL({GODOT_TYPE_DEFVAL[godot_type]}), '

        if self.optional_args:
            result_string = result_string[0:-2]

        result_string += ');\n'

        self.bound_methods += "\t" + result_string

        return result_string


    def print_function_name(self, function_name: str, argument_list: QLArgumentList):
        result_string = ""
        result_string += f"{RETURN_TYPE} {CLASS_TYPE}::{function_name}("

        result_string += ", ".join([f'{self.print_function_param(n, t)}' for n, t in zip(argument_list.required_names, argument_list.required_types)])

        # Add comma if needed.
        if argument_list.required_names and argument_list.optional_names:
            result_string += ', '
        
        result_string += ", ".join([f'{self.print_function_param(n, t)}' for n, t in zip(argument_list.optional_names, argument_list.optional_types)])

        result_string += ")"
        return result_string
  

    def print_function_name_special(self):
        result_string = ""
        result_string += f"{RETURN_TYPE} {CLASS_TYPE}::{self.function_name}("

        for required_arg in self.required_args:
            result_string += f'{self.print_function_param(required_arg[0], required_arg[1])}, '
        for optional_arg in self.optional_args:
            result_string += f'{self.print_function_param(optional_arg[0], optional_arg[1])}, '

        if self.required_args or self.optional_args:
            result_string = result_string[0:-2]

        result_string += ")"
        return result_string


    def print_function_name_header(self, function_name: str, required_arg_names: list[str], required_arg_types: list[str], optional_arg_names: list[str], optional_arg_types: list[str], signers: list[str] = None, non_signers: list[str] = None):
        result_string = ""
        result_string += f"{RETURN_TYPE} {function_name}("

        def get_godot_type(name, ql_type):
            if name in (signers or []) or name in (non_signers or []):
                return SIGNER_TYPE
            return self.ql_type_to_godot(ql_type)
        
        def get_defval(name, ql_type):
            if name in (signers or []) or name in (non_signers or []):
                return GODOT_TYPE_DEFVAL["Variant"]
            return GODOT_TYPE_DEFVAL[QL_TO_VARIANT[ql_type]]

        result_string += ", ".join(f'{get_godot_type(n, t)} {n}' for t, n in zip(required_arg_types, required_arg_names))
        
        # Add comma if needed.
        if optional_arg_names and required_arg_names:
            result_string += ', '
        
        result_string += ", ".join(f'{get_godot_type(n, t)} {n} = {get_defval(n, t)}' for t, n in zip(optional_arg_types, optional_arg_names))

        result_string += ");"
        return result_string
  

    def print_function_name_header_special(self):
        result_string = ""
        result_string += f"{RETURN_TYPE} {self.function_name}("

        for required_arg in self.required_args:
            result_string += f'{self.print_function_param(required_arg[0], required_arg[1])}, '
        for optional_arg in self.optional_args:
            (arg_name, arg_type) = optional_arg
            godot_type = QL_TO_VARIANT[arg_type]
            if arg_name in self.signers or arg_name in self.non_signers:
                result_string += f'{self.print_function_param(optional_arg[0], optional_arg[1])} = {GODOT_TYPE_DEFVAL["Variant"]}, '
            else:
                result_string += f'{self.print_function_param(optional_arg[0], optional_arg[1])} = {GODOT_TYPE_DEFVAL[godot_type]}, '

        if self.required_args or self.optional_args:
            result_string = result_string[0:-2]

        result_string += ");"
        return result_string
  

    def print_signer_section(self):
        result_string = ""
        for signer in self.signers:
            if signer == ".":
                result_string += f"\tsigners.append({SERVER_SIGNER});\n"
            else:
                result_string += f'\tif({signer}.get_type() != Variant::NIL)'
                result_string += "{\n"
                result_string += f"\t\tsigners.append({signer});\n"
                result_string += "\t}\n"
                
        return result_string


    def print_method_name(self):
        result_string = f'\n\tmethod_name = "{self.function_name}";\n'
        return result_string


    def print_last_section(self):
        result_string = self.builder.assign_query_fields(self.query_fields)
        #result_string = f'\n\tquery_fields = "{self.query_fields}";\n'
        result_string += "\tsend_query();\n"
        result_string += "\treturn OK;\n"
        result_string += "}\n"

        return result_string
  
    def print_last_section_special(self):
        class_name = self.function_name
        class_name = class_name[0].upper() + class_name[1:]
        result_string = f'\n\tquery_fields = "{self.query_fields}";\n'
        result_string += f'\tfetch_type<honeycomb_resource::{class_name}>();\n'
        result_string += "\treturn OK;\n"
        result_string += "}\n"

        return result_string

    def ql_types_to_godot_types(self, ql_types: list[str]):
        return [self.ql_type_to_godot(ql_type) for ql_type in ql_types]


    def print_function_param(self, argument_name: str, argument_type: str):
        if argument_name in self.signers or argument_name in self.non_signers:
            godot_type = SIGNER_TYPE
        else:
            godot_type = self.ql_type_to_godot(argument_type)

        result_string = f"{godot_type} {argument_name}"

        return result_string
     

    def print_bind_methods(self):
        result_string = f"void {CLASS_TYPE}::_bind_methods()" + "{\n"
        result_string += f'\tbind_non_changing_methods();\n'
        result_string += self.bound_methods
        result_string += "}"
        return result_string


    def print_header_includes(self):
        result = ""
        for header_include in header_includes:
            result += f'#include {header_include}\n'
        
        return result

    def print_header_file(self):
        result = "#define HONEYCOMB_METHOD_DEFS "
        result += self.method_definitions.replace("\n", "\\\n")
        result = result[:-2] + "\n\n"

        result += "#define REGISTER_HONEYCOMB_TYPES "
        includes = ""
        for type in self.types:
            include_path = os.path.join(TYPE_DIR, type["classname"] + ".hpp")
            includes += self.builder.print_include_statement(include_path)

            result += f'ClassDB::register_class<honeycomb_resource::{type["classname"]}>(); \\\n'
        result = result[:-2] + "\n\n"
        print(includes)
        return result

    def print_resource_hpp_file(self, index):
        type = self.types[index]
        class_name = type["classname"]
        properties = type["properties"]

        result = self.builder.start_include_guard(f'{TYPE_INCLUDE_GUARD_PREFIX}{class_name.upper()}')

        for include in resource_includes:
            result += self.builder.print_include_statement(include)
        result += "\nnamespace godot{\nnamespace honeycomb_resource{\n\n"

        result += f'class {class_name} : public Resource'
        result += "{\n"
        result += f"GDCLASS({class_name}, Resource)\n"

        result += "private:\n"
        for prop in properties:
            (prop_name, prop_type) = prop
            prop_type = self.ql_type_to_godot(prop_type)
            result += f'{prop_type} {prop_name};\n'

        result += "protected:\n"
        result += f'static void _bind_methods();\n'
        result += "public:\n"
        result += f"Dictionary to_dict();\n"
        result += f"void from_dict(const Dictionary& dict);\n"

        for prop in properties:
            (prop_name, prop_type) = prop
            prop_type = self.ql_type_to_godot(prop_type)
            result += f"void set_{prop_name}(const {prop_type}& val);\n"
            result += f"{prop_type} get_{prop_name}();\n"

        result += "};\n"
        result += "} // honeycomb_resource\n} // godot\n"
        result += "#endif"

        return result

    def print_resource_cpp_file(self, index, outdir_hpp):
        type = self.types[index]
        class_name = type["classname"]
        properties = type["properties"]

        extra_includes = []

        for prop in properties:
            (prop_name, og_prop_type) = prop
            prop_type = self.ql_type_to_godot(og_prop_type)
            if prop_type == "Variant":
                extra_includes.append(og_prop_type + ".hpp")

        result = f'#include "{os.path.join(outdir_hpp, class_name)}.hpp"\n\n'

        for include in extra_includes:
            if include.replace("!", "") == "Pubkey.hpp":
                result += self.builder.print_include_statement('pubkey.hpp')
            else:
                result += f'#include "{os.path.join(outdir_hpp, include)}"\n'.replace("!", "")

        result += "namespace godot{\nnamespace honeycomb_resource{\n\n"

        bind_methods = f'void {class_name}::_bind_methods()'
        bind_methods += '{\n'

        to_dict = f"Dictionary {class_name}::to_dict()"
        to_dict += "{\nDictionary res;\n"

        from_dict = f"void {class_name}::from_dict(const Dictionary& dict)"
        from_dict += "{\n"

        for prop in properties:
            (prop_name, og_prop_type) = prop
            prop_type = self.ql_type_to_godot(og_prop_type)
            if prop_type == "Variant" and og_prop_type.replace("!", "") == "Pubkey":
                to_dict += f'res["{prop_name}"] = Object::cast_to<{og_prop_type.replace("!", "")}>({prop_name})->to_string();\n'
                from_dict += f'{prop_name} = Pubkey::new_from_string(dict["{prop_name}"]);\n'
            elif prop_type == "Variant":
                to_dict += f'res["{prop_name}"] = Object::cast_to<godot::honeycomb_resource::{og_prop_type.replace("!", "")}>({prop_name})->to_dict();\n'
                from_dict += f'Object::cast_to<godot::honeycomb_resource::{og_prop_type.replace("!", "")}>({prop_name})->from_dict(dict["{prop_name}"]);\n'
            else:
                to_dict += f'res["{prop_name}"] = {prop_name};\n'
                from_dict += f'{prop_name} = dict["{prop_name}"];\n'
            result += f"void {class_name}::set_{prop_name}(const {prop_type}& val)"
            result += "{\n"
            result += f"this->{prop_name} = val;\n"
            result += "}\n\n"

            result += f"{prop_type} {class_name}::get_{prop_name}()"
            result += "{\n"
            result += f"return this->{prop_name};\n"
            result += "}\n\n"

            bind_methods += f'ClassDB::bind_method(D_METHOD("get_{prop_name}"), &{class_name}::get_{prop_name});\n'
            bind_methods += f'ClassDB::bind_method(D_METHOD("set_{prop_name}", "value"), &{class_name}::set_{prop_name});\n'
            bind_methods += f'ClassDB::add_property("{class_name}", PropertyInfo(Variant::Type::{GODOT_VARIANT_ENUM_TYPE[prop_type]}, "{prop_name}"), "set_{prop_name}", "get_{prop_name}");\n'

        bind_methods += f'ClassDB::bind_method(D_METHOD("to_dict"), &{class_name}::to_dict);\n'
        bind_methods += f'ClassDB::bind_method(D_METHOD("from_dict", "dict"), &{class_name}::from_dict);\n'

        result += to_dict + "return res;\n}\n\n"
        result += from_dict + "}\n\n"

        result += bind_methods
        result += "}\n} // honeycomb_resource\n} // godot"

        return result

    def print_cpp_file(self, outdir_hpp):

        include_file = os.path.join(outdir_hpp, "honeycomb.hpp")
        result = self.builder.print_include_statement(include_file)
        for type in self.types:
            result += f'#include "{os.path.join(outdir_hpp, "types/" + type["classname"])}.hpp"\n'.replace("!", "")
        result += "namespace godot{\n\n"
        result += self.method_implementations
        result += self.print_bind_methods() + '\n'
        result += "} // godot"

        return result


def main():
    parser = argparse.ArgumentParser(description='Generates HoneyComb interface source code.')
    parser.add_argument('-o', '--out_cpp_directory', metavar="PATH", type=str, default="src/honeycomb/generated",
                        help='Output directory of generated source files.')
    parser.add_argument('-d', '--out_hpp_directory', metavar="PATH", type=str, default="include/honeycomb/generated",
                        help='Output directory of generated header files.')
    parser.add_argument('-c', '--cpp_filename', type=str, default="honeycomb_generated.cpp",
                        help='File name of generated cpp file.')
    parser.add_argument('-p', '--hpp_filename', type=str, default="honeycomb_generated.hpp",
                        help='File name of generated hpp file.')
    parser.add_argument('--skip_hpp_file', action="store_true", default=False,
                        help='Tells generator to skip creating a hpp file.')
    parser.add_argument('--skip_cpp_file', action="store_true", default=False,
                        help='Tells generator to skip creating a cpp file.')
    parser.add_argument('--skip_resources', action="store_true", default=False,
                        help='Tells generator to skip creating resource classes.')

    args = parser.parse_args()

    qlparser = GQLParse()
    
    request_parser = SimpleQLRequestParser()
    request_parser.parse(CREATE_NEW_RESOURCE)
    
    qlparser.graphql_to_type(HoneycombTransaction)
    qlparser.graphql_to_type(SendTransactionBundlesOptions)
    qlparser.graphql_to_type(CreateBadgeCriteriaInput)
    qlparser.graphql_to_type(ModifyDelegationInput)
    qlparser.graphql_to_type(ServiceDelegationInput)
    qlparser.graphql_to_type(ProfileDataConfigInput)
    qlparser.graphql_to_type(UpdateBadgeCriteriaInput)
    qlparser.graphql_to_type(ClaimBadgeCriteriaInput)
    qlparser.graphql_to_type(ProfileInfoInput)
    qlparser.graphql_to_type(CustomDataInput)
    qlparser.graphql_to_type(UpdateWalletInput)
    qlparser.graphql_to_type(PartialUserInfoInput)
    qlparser.graphql_to_type(TreeSetupConfig)
    qlparser.graphql_to_type(UserInfoInput)
    qlparser.graphql_to_type(RecallFromMissionData)
    qlparser.graphql_to_type(CharacterConfigInput)
    qlparser.graphql_to_type(AddMultiplierMetadataInput)
    qlparser.graphql_to_type(UpdateStakingPoolMetadataInput)
    qlparser.graphql_to_type(InitStakingMultiplierMetadataInput)
    qlparser.graphql_to_type(CreateStakingPoolMetadataInput)
    qlparser.graphql_to_type(ParticipateOnMissionData)
    qlparser.graphql_to_type(UpdateMissionInput)
    qlparser.graphql_to_type(NewMissionData)
    qlparser.graphql_to_type(UpdateMissionPoolData)
    qlparser.graphql_to_type(NewMissionPoolData)
    qlparser.graphql_to_type(MealInput)
    qlparser.graphql_to_type(InitResourceInput)
    qlparser.graphql_to_type(MultiplierTypeInput)
    qlparser.graphql_to_type(AssemblerConfigInput)
    qlparser.graphql_to_type(ModifyServiceDelegationInput)
    qlparser.graphql_to_type(ServiceDelegationHiveControl)
    qlparser.graphql_to_type(TransactionBundlesOptions)
    qlparser.graphql_to_type(ServiceDelegationAssetAssembler)
    qlparser.graphql_to_type(NewMissionCost)
    qlparser.graphql_to_type(ServiceDelegationAssetManager)
    qlparser.graphql_to_type(ServiceDelegationCurrencyManager)
    qlparser.graphql_to_type(ServiceDelegationNectarStaking)
    qlparser.graphql_to_type(ServiceDelegationNectarMissions)
    qlparser.graphql_to_type(ServiceDelegationBuzzGuild)
    qlparser.graphql_to_type(BasicTreeConfig)
    qlparser.graphql_to_type(AdvancedTreeConfig)
    qlparser.graphql_to_type(AssociatedProgramInput)
    qlparser.graphql_to_type(DelegateAuthority)

  
    qlparser.graphql_to_function(CREATE_NEW_RESOURCE, ["authority", "."], ["project"])
    qlparser.graphql_to_function(CREATE_NEW_RESOURCE_TREE, ["authority", "delegateAuthority", "payer"], ["project", "resource"])
    qlparser.graphql_to_function(MINT_RESOURCE_TRANSACTION, ["owner", "authority", "delegateAuthority", "payer"], ["resource"])
    qlparser.graphql_to_function(CREATE_BURN_RESOURCE_TRANSACTION, ["authority", "owner", "payer", "delegateAuthority"], ["resource"])
    qlparser.graphql_to_function(CREATE_UNWRAP_HOLDING_TRANSACTION, ["authority", "payer"], [])
    qlparser.graphql_to_function(CreateCreateWrapHoldingTransaction, ["authority", "payer"], [])
    qlparser.graphql_to_function(CreateTransferResourceTransaction, ["owner", "payer"], [])
#    qlparser.graphql_to_function(CreateInitializeRecipeTransaction, ["authority", "payer"], [])


    qlparser.graphql_to_function(CreateInitializeRecipeTransaction, ["authority", "delegateAuthority", "payer"], ["project"])
    qlparser.graphql_to_function(CreateAddIngredientsTransaction, ["authority", "delegateAuthority", "payer"], ["recipe"])
    qlparser.graphql_to_function(CreateRemoveIngredientsTransaction, ["authority", "delegateAuthority", "payer"], ["recipe"])
    qlparser.graphql_to_function(CreateInitCookingProcessTransactions, ["authority", "payer"], ["recipe"])

    qlparser.graphql_to_function(CreateCreateMissionPoolTransaction, [], [])
    qlparser.graphql_to_function(CreateUpdateMissionPoolTransaction, [], [])
    qlparser.graphql_to_function(CreateCreateMissionTransaction, [], [])
    qlparser.graphql_to_function(CreateUpdateMissionTransaction, ["authority", "payer", "delegateAuthority"], ["missionAddress"])
    qlparser.graphql_to_function(CreateSendCharactersOnMissionTransaction, [], [])

    qlparser.graphql_to_function(CreateCreateStakingPoolTransaction, ["authority", "delegateAuthority", "payer"], ["project", "resource"])
    qlparser.graphql_to_function(CreateUpdateStakingPoolTransaction, ["authority", "delegateAuthority", "payer"], ["project", "stakingPool", "characterModel", "resource"])
    qlparser.graphql_to_function(CreateInitMultipliersTransaction, ["authority", "delegateAuthority", "payer"], ["project", "stakingPool"])
    qlparser.graphql_to_function(CreateAddMultiplierTransaction, ["authority", "delegateAuthority", "payer"], ["project", "multiplier"])
    qlparser.graphql_to_function(CreateStakeCharactersTransactions, ["feePayer"], ["project", "characterModel", "stakingPool"])
    qlparser.graphql_to_function(CreateClaimStakingRewardsTransactions, ["feePayer"], ["characterModel"])
    qlparser.graphql_to_function(CreateUnstakeCharactersTransactions, ["feePayer"], ["characterModel"])

    qlparser.graphql_to_function(CreateAddCharacterTraitsTransactions, ["authority", "payer"], ["assemblerConfig"])
    qlparser.graphql_to_function(CreateRemoveCharacterTraitsTransactions, ["authority", "payer"], ["assemblerConfig"])
    qlparser.graphql_to_function(CreateCreateCharacterModelTransaction, ["authority", "payer"], ["project"])
    qlparser.graphql_to_function(CreateCreateCharactersTreeTransaction, ["authority", "payer"], ["project", "characterModel"])
    qlparser.graphql_to_function(CreateAssembleCharacterTransaction, ["wallet"], ["project", "assemblerConfig", "characterModel", "charactersTree"])
    qlparser.graphql_to_function(CreateUpdateCharacterTraitsTransaction, ["wallet"], ["characterAddress", "project", "assemblerConfig", "characterModel", "charactersTree"])
    qlparser.graphql_to_function(CreatePopulateAssembleablCharacterTransaction, ["owner", "updateAuthority", "payer"], ["project", "characterModel", "charactersTree", "mint"])
    qlparser.graphql_to_function(CreateWrapAssetsToCharacterTransactions, ["wallet"], ["project", "characterModel"])
    qlparser.graphql_to_function(CreateUnwrapAssetsFromCharacterTransactions, ["wallet"], ["project"])
    qlparser.graphql_to_function(CreateRecallCharactersTransaction, [], [])
    qlparser.graphql_to_function(SignWithShadowSignerAndSendTransactionBundles, [], [])
    qlparser.graphql_to_function(CreateInitializeFaucetTransaction, ["authority", "delegateAuthority", "payer"], ["resource"])
    qlparser.graphql_to_function(CreateClaimFaucetTransaction, ["owner", "payer"], ["faucet"])
    qlparser.graphql_to_function(CreateNewUserTransaction, ["wallet", "payer"], [])
    qlparser.graphql_to_function(CreateNewUserBulkTransaction, ["payer"], [])
    qlparser.graphql_to_function(CreateUpdateUserTransaction, ["payer"], [])
    qlparser.graphql_to_function(CreateCreateProfilesTreeTransaction, ["payer"], ["project"])
    qlparser.graphql_to_function(CreateNewProfileTransaction, ["payer"], ["project"])
    qlparser.graphql_to_function(CreateUpdateProfileTransaction, ["profile", "payer"], [])
    qlparser.graphql_to_function(CreateNewUserWithProfileTransaction, ["wallet", "payer"], ["project"])
    qlparser.graphql_to_function(CreateClaimBadgeCriteriaTransaction, [], [])
    qlparser.graphql_to_function(CreateUpdateBadgeCriteriaTransaction, [], [])
    qlparser.graphql_to_function(CreateCreateProjectTransaction, ["authority", "payer"], ["driver"])
    qlparser.graphql_to_function(CreateChangeProjectDriverTransaction, ["authority", "payer"], ["project", "driver"])
    qlparser.graphql_to_function(CreateCreateDelegateAuthorityTransaction, ["authority", "payer"], ["project", "delegate"])
    qlparser.graphql_to_function(CreateModifyDelegationTransaction, ["authority", "payer"], ["project", "delegate"])
    qlparser.graphql_to_function(CreateCreateAssemblerConfigTransaction, ["authority", "payer"], ["project"])
    qlparser.graphql_to_function(CreateInitializeBadgeCriteriaTransaction, [], [])

    qlparser.graphql_to_fetcher(findDelegateAuthority)

    print(qlparser.print_header_file())

    qlparser.save_cpp_file(os.path.join(args.out_cpp_directory, args.cpp_filename), args.out_hpp_directory.split("/", maxsplit=1)[-1])
    qlparser.save_hpp_file(os.path.join(args.out_hpp_directory, args.hpp_filename))
    qlparser.save_resource_files(args.out_cpp_directory + "/types", args.out_hpp_directory + "/types")

if __name__ == "__main__":
    main()

