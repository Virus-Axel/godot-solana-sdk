import re

CLASS_TYPE = "HoneyComb"
RETURN_TYPE = "Variant"

SIGNER_TYPE = "const Variant&"
SERVER_SIGNER = 'Pubkey::new_from_string("11111111111111111111111111111111")'

QL_TO_VARIANT = {
   "String": "String",
   "[String!]": "PackedStringArray",
   "BigInt": "int64_t",
   "Int": "int32_t",
}

QL_SPECIAL_TYPE_PARAM = {
   "InitResourceInput": ([
      ("name", "String"),
      ("symbol", "String"),
      ("uri", "String"),
      ("decimals", "int32_t"),
      ("storage", "ResourceStorage"),
   ], []),
}

bind_methods = f"void {CLASS_TYPE}::_bind_methods()" + "{\n"
class GQLParse:
  def __init__(self):
    self.required_args = []
    self.optional_args = []
    self.original_required_args = []
    self.original_optional_args = []
    self.str = ""

  def is_special_type(self, type_name):
    return type_name in QL_SPECIAL_TYPE_PARAM.keys()

  def ql_type_to_godot(self, ql_type):
    return QL_TO_VARIANT[ql_type]

  def add_function_start(self, method_name):
    global bind_methods
    bind_methods += f'ClassDB::bind_method(D_METHOD("{method_name}", '

  def add_function_stop(self, method_name):
    global bind_methods
    bind_methods += f'), {CLASS_TYPE}::{method_name});\n'

  def add_required_arg_to_bind(self, arg):
    global bind_methods
    bind_methods += ""

  def process_arg(self, arg):
    (name, data_type) = arg.split(":")
    name = name.lstrip()
    data_type = data_type.lstrip().rstrip()
    assert(name[0] == '$')
    if(data_type[-1] == "!"):
      self.add_required_arg_to_bind(arg)
      data_type = data_type[0:-1]
      self.original_required_args.append((name[1:], data_type))
      if self.is_special_type(data_type):
        (additional_required_args, additional_optional_args) = QL_SPECIAL_TYPE_PARAM[data_type]
        self.required_args += additional_required_args
        self.optional_args += additional_optional_args
      else:
        data_type = self.ql_type_to_godot(data_type)
        self.required_args.append((name[1:], data_type))
    else:
      self.original_optional_args.append((name[1:], data_type))
      if self.is_special_type(data_type):
        (additional_required_args, additional_optional_args) = QL_SPECIAL_TYPE_PARAM[data_type]
        self.optional_args += additional_required_args
        self.optional_args += additional_optional_args
      else:
        data_type = self.ql_type_to_godot(data_type)
        self.optional_args.append((name[1:], data_type))

  def process_function_header(self, str):
      (function_name, str) = str.split("(", maxsplit=1)

      assert(str[-1] == ")")
      str = str[0:-1]

      args = str.split(",")
      for arg in args:
          self.process_arg(arg)

      self.function_name = function_name


  def print_args_section(self, args, original_args, optional=False):
    ADD_ARG_FUNCTION_NAME = "add_arg"
    if optional:
      ADD_ARG_FUNCTION_NAME = "add_optional_arg"

    result_string = ""
    original_counter = 0

    i = 0
    while i < len(args):

        (arg_name, arg_type) = args[i]
        if args[i] in original_args:
            original_counter += 1
        else:
            (original_name, original_type) = original_args[original_counter]
            result_string += f"\n\tDictionary {original_name};\n"
            while args[i] not in original_args:
              (arg_name, arg_type) = args[i]
              result_string += f'\t{original_name}["{arg_name}"] = {arg_name}\n'
              i += 1

              if i >= len(args):
                  break
          
            result_string += f'\n\t{ADD_ARG_FUNCTION_NAME}("{original_name}", "{original_type}", {arg_name});\n'
            original_counter += 1

        i += 1

        if(arg_type == "String"):
            result_string += f'\t{ADD_ARG_FUNCTION_NAME}("{arg_name}", "{arg_type}", Pubkey::string_from_variant({arg_name}));\n'

    return result_string



  def graphql_to_function(self, str, signers, non_signers):
      self.str = str
      (firstWord, self.str) = self.str.split(maxsplit=1)
      #print(firstWord)
      assert(firstWord == "query")

      func_header = re.match(r"(.*)\(([\s\S]*?)\)", self.str)
      #print(func_header.group())

      self.process_function_header(func_header.group())

      result_string = ""
      result_string += f"{RETURN_TYPE} {CLASS_TYPE}::{self.function_name}("

      for required_arg in self.required_args:
        (arg_name, arg_type) = required_arg

        if arg_name in signers or arg_name in non_signers:
            arg_type = SIGNER_TYPE
            
        result_string += f"{arg_type} {arg_name}, "

      if self.required_args or self.optional_args:
        result_string = result_string[0:-2]

      result_string += "){\n"
      for signer in signers:
        if signer == ".":
            result_string += f"\tsigners.append({SERVER_SIGNER});\n"
        else:
            result_string += f"\tsigners.append({signer});\n"

      result_string += f'\n\tmethod_name = "{self.function_name}";\n'

      result_string += self.print_args_section(self.required_args, self.original_required_args)
      result_string += self.print_args_section(self.optional_args, self.original_optional_args, True)

      self.str = self.str[func_header.span()[1]:]
      #print(str)
      chunk_string = re.search(r"\{\s*[\w\s:,$]*?\([\s\S]*?\)", self.str)
      #print(chunk_string.group())
      self.str = self.str[chunk_string.span()[1]:]
      #print(str)

      chunk_string = re.search(r"\{\s*tx\s*\{\s*[\w\s]*\}\s*[\w\s]*\}", self.str)
      self.str = chunk_string.group()
      #print(chunk_string.group())

      self.str = self.str.lstrip()[1:]
      self.str = self.str.rstrip()[0:-1]

      query_fields_re = re.sub(r"[\r\n]+", "", self.str)
      #query_fields = str.replace("\n", "").replace("\r", "").replace("\t", "")
      query_fields = re.sub(r'\s+', ' ', query_fields_re)

      result_string += f'\n\tquery_fields = "{query_fields}";\n'
      result_string += "\tsend_query();\n"
      result_string += "}"

      self.str = self.str[chunk_string.span()[1]:]
      print(result_string)
      return result_string


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

print('#include "honeycomb.hpp"\n\n')
print("namespace godot{\n")
parser = GQLParse()
parser.graphql_to_function(CREATE_NEW_RESOURCE, ["authority", "."], ["project"])
#parser.graphql_to_function(CREATE_NEW_RESOURCE_TREE, ["authority", "delegateAuthority", "payer"], ["project", "resource"])
#parser.graphql_to_function(MINT_RESOURCE_TRANSACTION, ["owner", "authority", "delegateAuthority", "payer"], ["resource"])
print("} // godot")