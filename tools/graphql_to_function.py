import re
import argparse


CLASS_TYPE = "HoneyComb"
RETURN_TYPE = "Variant"

SIGNER_TYPE = "const Variant&"
SERVER_SIGNER = 'Pubkey::new_from_string("11111111111111111111111111111111")'

header_includes = [
  '"pubkey.hpp"',
]

QL_TO_VARIANT = {
   "String": "String",
   "[String!]": "PackedStringArray",
   "BigInt": "int64_t",
   "Int": "int32_t",
   "InitResourceInput": "Variant",
   "TreeSetupConfig": "Variant",
   "UserInfoInput": "Variant",
}

GODOT_TYPE_DEFVAL = {
  "String": '""',
  "int64_t": '-1',
  "int32_t": "-1",
  "PackedStringArray": "PackedStringArray()",
  "Variant": "Variant(nullptr)",
}


class GQLParse:
  def __init__(self):
    self.required_args = []
    self.optional_args = []
    self.original_required_args = []
    self.original_optional_args = []
    self.str = ""
    self.method_definitions = ""
    self.bound_methods = ""
    self.method_implementations = ""

  def function_name_to_alias(self, function_name):
    alias = function_name[6:-11]
    return ''.join(['_'+c.lower() if c.isupper() else c for c in alias]).lstrip('_')


  def ql_type_to_godot(self, ql_type):
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


  def print_arg(self, arg, optional=False):
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

    result_string += "\tadd_arg"

    if godot_type == "String":
      result_string += f'("{arg_name}", "{arg_type}", Pubkey::string_from_variant({arg_name}), {is_optional});\n'
    elif godot_type == "Variant":
       result_string += f'("{arg_name}", "{arg_type}", Object::cast_to<{arg_type}>({arg_name})->to_dict(), {is_optional});\n'
    else:
      result_string += f'("{arg_name}", "{arg_type}", {arg_name}, {is_optional});\n'

    if optional:
      result_string += '\t}\n'

    return result_string

  def print_args_section(self):

    result_string = ""

    for required_arg in self.required_args:
       result_string += self.print_arg(required_arg)
    for optional_arg in self.optional_args:
       result_string += self.print_arg(optional_arg, True)

    return result_string


  def graphql_to_function(self, str, signers, non_signers):
      self.str = str
      self.signers = signers
      self.non_signers = non_signers
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

      chunk_string = re.search(r"\{\s*tx\s*\{\s*[\w\s]*\}\s*[\w\s]*\}", self.str)
      if not chunk_string:
         chunk_string = re.search(r"\{\s*[\w\s]*\}", self.str)

      self.str = chunk_string.group()
      self.str = self.str.lstrip()[1:]
      self.str = self.str.rstrip()[0:-1]

      # Make a line of query
      query_fields_re = re.sub(r"[\r\n]+", "", self.str)
      self.query_fields = re.sub(r'\s+', ' ', query_fields_re)

      self.method_definitions += f'\t{self.print_function_name_header()}\n'

      self.method_implementations += self.print_function_name()
      self.method_implementations += "{\n"
      self.method_implementations += "\tif(pending){\n"
      self.method_implementations += "\t\treturn ERR_BUSY;\n"
      self.method_implementations += "\t}\n"
      self.method_implementations += self.print_signer_section() + '\n'
      self.method_implementations += self.print_args_section() + '\n'
      self.method_implementations += self.print_method_name() + '\n'
      self.method_implementations += self.print_last_section() + '\n'

      self.append_method_bind()



      self.str = self.str[chunk_string.span()[1]:]
      return ""
  

  def append_method_bind(self):
    result_string = f'ClassDB::bind_method(D_METHOD("{self.function_name_to_alias(self.function_name)}", '

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
    #temp_optional_args.reverse()

    for optional_arg in temp_optional_args:
      (_arg_name, arg_type) = optional_arg
      godot_type = QL_TO_VARIANT[arg_type]
      result_string += f'DEFVAL({GODOT_TYPE_DEFVAL[godot_type]}), '

    if self.optional_args:
      result_string = result_string[0:-2]

    result_string += ');\n'

    self.bound_methods += "\t" + result_string

    return result_string


  def print_function_name(self):
    result_string = ""
    result_string += f"{RETURN_TYPE} {CLASS_TYPE}::{self.function_name}("

    for required_arg in self.required_args:
      result_string += f'{self.print_function_param(required_arg)}, '
    for optional_arg in self.optional_args:
      result_string += f'{self.print_function_param(optional_arg)}, '

    if self.required_args or self.optional_args:
      result_string = result_string[0:-2]

    result_string += ")"
    return result_string


  def print_function_name_header(self):
    result_string = ""
    result_string += f"{RETURN_TYPE} {self.function_name}("

    for required_arg in self.required_args:
      result_string += f'{self.print_function_param(required_arg)}, '
    for optional_arg in self.optional_args:
      (_arg_name, arg_type) = optional_arg
      godot_type = QL_TO_VARIANT[arg_type]
      result_string += f'{self.print_function_param(optional_arg)} = {GODOT_TYPE_DEFVAL[godot_type]}, '

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
          result_string += f"\tsigners.append({signer});\n"
    return result_string


  def print_method_name(self):
     result_string = f'\n\tmethod_name = "{self.function_name}";\n'
     return result_string


  def print_last_section(self):
    result_string = f'\n\tquery_fields = "{self.query_fields}";\n'
    result_string += "\tsend_query();\n"
    result_string += "\treturn OK;\n"
    result_string += "}\n"

    return result_string

  def print_function_param(self, arg):
    (arg_name, arg_type) = arg

    if arg_name in self.signers or arg_name in self.non_signers:
        godot_type = SIGNER_TYPE
    else:
      godot_type = self.ql_type_to_godot(arg_type)

    result_string = f"{godot_type} {arg_name}"
    #print(result_string)

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
    result = self.print_header_includes()
    result += "\nnamespace godot{\n"
    result += f"\nClass {CLASS_TYPE} : public Node" + "{\n"
    result += f"GDCLASS({CLASS_TYPE}, Node)\n"
    result += "private:\n"
    result += "protected:\n"
    result += "static void _bind_methods();\n"
    result += "public:\n"
    result += self.method_definitions
    result += "};\n"
    result += "} // godot"

    return result


  def print_cpp_file(self):
    result = '#include "honeycomb.hpp"\n\n'
    result += "namespace godot{\n\n"
    result += self.method_implementations
    result += parser.print_bind_methods() + '\n'
    result += "} // godot"

    return result


CREATE_NEW_RESOURCE = """
query CreateCreateNewResourceTransaction(
  $project: String!,
  $authority: String!,
  $params: InitResourceInput!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateNewResourceTransaction(
    project: $project,
    authority: $authority,
    params: $params,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    resource
  }
}
"""

CREATE_NEW_RESOURCE_TREE = """
query CreateCreateNewResourceTreeTransaction(
  $project: String!,
  $resource: String!,
  $authority: String!,
  $treeConfig: TreeSetupConfig!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateNewResourceTreeTransaction(
    project: $project,
    resource: $resource,
    authority: $authority,
    treeConfig: $treeConfig,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    treeAddress
    proofBytes
    space
    cost
    maxTreeCapacity
  }
}
"""

MINT_RESOURCE_TRANSACTION = """
query CreateMintResourceTransaction(
  $resource: String!,
  $owner: String!,
  $authority: String!,
  $amount: BigInt!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createMintResourceTransaction(
    resource: $resource,
    owner: $owner,
    authority: $authority,
    amount: $amount,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CREATE_BURN_RESOURCE_TRANSACTION = """
query CreateBurnResourceTransaction(
  $resource: String!,
  $amount: BigInt!,
  $authority: String!,
  $owner: String,
  $payer: String,
  $delegateAuthority: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createBurnResourceTransaction(
    resource: $resource,
    amount: $amount,
    authority: $authority,
    owner: $owner,
    payer: $payer,
    delegateAuthority: $delegateAuthority,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CREATE_NEW_USER_TRANSACTION = """
query CreateNewUserTransaction(
  $info: UserInfoInput,
  $wallet: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createNewUserTransaction(
    info: $info,
    wallet: $wallet,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""


parser = GQLParse()
parser.graphql_to_function(CREATE_NEW_RESOURCE, ["authority", "."], ["project"])
parser.graphql_to_function(CREATE_NEW_RESOURCE_TREE, ["authority", "delegateAuthority", "payer"], ["project", "resource"])
parser.graphql_to_function(MINT_RESOURCE_TRANSACTION, ["owner", "authority", "delegateAuthority", "payer"], ["resource"])
parser.graphql_to_function(CREATE_BURN_RESOURCE_TRANSACTION, ["authority", "owner", "payer", "delegateAuthority"], ["resource"])
parser.graphql_to_function(CREATE_NEW_USER_TRANSACTION, ["wallet", "payer"], [])

#print(parser.print_header_file())
#print(parser.print_cpp_file())


def main():
    parser = argparse.ArgumentParser(description='Generates HoneyComb interface source code.')
    parser.add_argument('-o', '--out_directory', metavar="PATH", type=str, default="src/HoneyComb/generated",
                        help='Output directory of generated source files.')
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
    print(args.out_directory)

if __name__ == "__main__":
    main()

