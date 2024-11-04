import re
import argparse
import os


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
   "VecMapGeneric": "Variant",
   "RecallFromMissionData": "Variant",
   "[Bytes!]": "PackedByteArray",
   "SendTransactionBundlesOptions": "Variant",
   "[UserInfoInput!]": "Array",
   "NewMissionPoolData": "Variant",
   "UpdateMissionPoolData": "Variant",
   "NewMissionData": "Variant",
   "UpdateMissionInput": "Variant",
   "ParticipateOnMissionData": "Variant",
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

  def save_cpp_file(self, filepath):
    file = open(filepath, "w")
    file.write(self.print_cpp_file())
    file.close()

  def save_hpp_file(self, filepath):
    file = open(filepath, "w")
    file.write(self.print_header_file())
    file.close()

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
    #result = self.print_header_includes()
    #result += "\nnamespace godot{\n"
    #result += f"\nClass {CLASS_TYPE} : public Node" + "{\n"
    #result += f"GDCLASS({CLASS_TYPE}, Node)\n"
    #result += "private:\n"
    #result += "protected:\n"
    #result += "static void _bind_methods();\n"
    #result += "public:\n"
    result = "#define HONEYCOMB_METHOD_DEFS "
    result += self.method_definitions.replace("\n", "\\\n")
    #result += "};\n"
    #result += "} // godot"

    return result


  def print_cpp_file(self):
    result = '#include "honeycomb.hpp"\n\n'
    result += "namespace godot{\n\n"
    result += self.method_implementations
    result += self.print_bind_methods() + '\n'
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

CREATE_UNWRAP_HOLDING_TRANSACTION = """
query CreateCreateUnwrapHoldingTransaction(
  $resource: String!,
  $amount: BigInt!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateUnwrapHoldingTransaction(
    resource: $resource,
    amount: $amount,
    authority: $authority,
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

CreateCreateWrapHoldingTransaction = """
query CreateCreateWrapHoldingTransaction(
  $resource: String!,
  $amount: BigInt!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateWrapHoldingTransaction(
    resource: $resource,
    amount: $amount,
    authority: $authority,
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

CreateTransferResourceTransaction = """
query CreateTransferResourceTransaction(
  $resource: String!,
  $owner: String!,
  $recipient: String!,
  $amount: BigInt!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createTransferResourceTransaction(
    resource: $resource,
    owner: $owner,
    recipient: $recipient,
    amount: $amount,
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

CreateInitializeRecipeTransaction = """
query CreateInitializeRecipeTransaction(
  $project: String!,
  $xp: BigInt!,
  $ingredients: [IngredientsInput!]!,
  $meal: MealInput!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitializeRecipeTransaction(
    project: $project,
    xp: $xp,
    ingredients: $ingredients,
    meal: $meal,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions {
      transactions
      blockhash
      lastValidBlockHeight
    }
    recipe
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

CreateInitializeRecipeTransaction = """
query CreateInitializeRecipeTransaction(
  $project: String!,
  $xp: BigInt!,
  $ingredients: [IngredientsInput!]!,
  $meal: MealInput!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitializeRecipeTransaction(
    project: $project,
    xp: $xp,
    ingredients: $ingredients,
    meal: $meal,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions {
      transactions
      blockhash
      lastValidBlockHeight
    }
    recipe
  }
}"""

CreateAddIngredientsTransaction = """
query CreateAddIngredientsTransaction(
  $recipe: String!,
  $ingredients: [IngredientsInput!]!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createAddIngredientsTransaction(
    recipe: $recipe,
    ingredients: $ingredients,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateRemoveIngredientsTransaction = """
query CreateRemoveIngredientsTransaction(
  $recipe: String!,
  $ingredients: [String!]!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createRemoveIngredientsTransaction(
    recipe: $recipe,
    ingredients: $ingredients,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateInitCookingProcessTransactions = """
query CreateInitCookingProcessTransactions(
  $recipe: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitCookingProcessTransactions(
    recipe: $recipe,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateCreateMissionPoolTransaction = """
query CreateCreateMissionPoolTransaction(
  $data: NewMissionPoolData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateMissionPoolTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    missionPoolAddress
  }
}"""

CreateUpdateMissionPoolTransaction = """
query CreateUpdateMissionPoolTransaction(
  $data: UpdateMissionPoolData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateMissionPoolTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
  }
}"""

CreateCreateMissionTransaction = """
query CreateCreateMissionTransaction(
  $data: NewMissionData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateMissionTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    missionAddress
  }
}
"""

CreateUpdateMissionTransaction = """
query CreateUpdateMissionTransaction(
  $missionAddress: String!,
  $authority: String!,
  $params: UpdateMissionInput!,
  $payer: String,
  $delegateAuthority: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateMissionTransaction(
    missionAddress: $missionAddress,
    authority: $authority,
    params: $params,
    payer: $payer,
    delegateAuthority: $delegateAuthority,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateSendCharactersOnMissionTransaction = """
query CreateSendCharactersOnMissionTransaction(
  $data: ParticipateOnMissionData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createSendCharactersOnMissionTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateStakingPoolTransaction = """
query CreateCreateStakingPoolTransaction(
  $project: String!,
  $resource: String!,
  $metadata: CreateStakingPoolMetadataInput!,
  $authority: String!,
  $multiplier: InitStakingMultiplierMetadataInput,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateStakingPoolTransaction(
    project: $project,
    resource: $resource,
    metadata: $metadata,
    authority: $authority,
    multiplier: $multiplier,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions {
      transactions
      blockhash
      lastValidBlockHeight
    }
    stakingPoolAddress
    multipliersAddress
  }
}"""

CreateUpdateStakingPoolTransaction = """
query CreateUpdateStakingPoolTransaction(
  $project: String!,
  $stakingPool: String!,
  $metadata: UpdateStakingPoolMetadataInput,
  $authority: String!,
  $characterModel: String,
  $resource: String,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateStakingPoolTransaction(
    project: $project,
    stakingPool: $stakingPool,
    metadata: $metadata,
    authority: $authority,
    characterModel: $characterModel,
    resource: $resource,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateInitMultipliersTransaction = """
query CreateInitMultipliersTransaction(
  $project: String!,
  $stakingPool: String!,
  $decimals: Int!,
  $multipliers: [AddMultiplierMetadataInput]!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitMultipliersTransaction(
    project: $project,
    stakingPool: $stakingPool,
    decimals: $decimals,
    multipliers: $multipliers,
    authority: $authority,
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
    multipliersAddress
  }
}"""

CreateAddMultiplierTransaction = """
query CreateAddMultiplierTransaction(
  $project: String!,
  $multiplier: String!,
  $metadata: AddMultiplierMetadataInput!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createAddMultiplierTransaction(
    project: $project,
    multiplier: $multiplier,
    metadata: $metadata,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateStakeCharactersTransactions = """
query CreateStakeCharactersTransactions(
  $characterAddresses: [String!]!,
  $project: String!,
  $characterModel: String!,
  $stakingPool: String!,
  $feePayer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createStakeCharactersTransactions(
    characterAddresses: $characterAddresses,
    project: $project,
    characterModel: $characterModel,
    stakingPool: $stakingPool,
    feePayer: $feePayer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateClaimStakingRewardsTransactions = """
query CreateClaimStakingRewardsTransactions(
  $characterAddresses: [String!]!,
  $characterModel: String!,
  $feePayer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createClaimStakingRewardsTransactions(
    characterAddresses: $characterAddresses,
    characterModel: $characterModel,
    feePayer: $feePayer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateUnstakeCharactersTransactions = """
query CreateUnstakeCharactersTransactions(
  $characterAddresses: [String!]!,
  $characterModel: String!,
  $feePayer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUnstakeCharactersTransactions(
    characterAddresses: $characterAddresses,
    characterModel: $characterModel,
    feePayer: $feePayer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateAddCharacterTraitsTransactions = """
query CreateAddCharacterTraitsTransactions(
  $assemblerConfig: String!,
  $traits: [CharacterTraitInput!]!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createAddCharacterTraitsTransactions(
    assemblerConfig: $assemblerConfig,
    traits: $traits,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateRemoveCharacterTraitsTransactions = """
query CreateRemoveCharacterTraitsTransactions(
  $assemblerConfig: String!,
  $traitsAddresses: [String!]!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createRemoveCharacterTraitsTransactions(
    assemblerConfig: $assemblerConfig,
    traitsAddresses: $traitsAddresses,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateCharacterModelTransaction = """
query CreateCreateCharacterModelTransaction(
  $config: CharacterConfigInput!,
  $attributes: VecMapGeneric,
  $project: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateCharacterModelTransaction(
    config: $config,
    attributes: $attributes,
    project: $project,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    characterModel
  }
}"""

CreateCreateCharactersTreeTransaction = """
query CreateCreateCharactersTreeTransaction(
  $treeConfig: TreeSetupConfig!,
  $project: String!,
  $characterModel: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateCharactersTreeTransaction(
    treeConfig: $treeConfig,
    project: $project,
    characterModel: $characterModel,
    authority: $authority,
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
}"""

CreateAssembleCharacterTransaction = """
query CreateAssembleCharacterTransaction(
  $attributes: VecMapGeneric!,
  $project: String!,
  $assemblerConfig: String!,
  $characterModel: String!,
  $charactersTree: String!,
  $wallet: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createAssembleCharacterTransaction(
    attributes: $attributes,
    project: $project,
    assemblerConfig: $assemblerConfig,
    characterModel: $characterModel,
    charactersTree: $charactersTree,
    wallet: $wallet,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateUpdateCharacterTraitsTransaction = """
query CreateUpdateCharacterTraitsTransaction(
  $characterAddress: String!,
  $attributes: VecMapGeneric!,
  $project: String!,
  $assemblerConfig: String!,
  $characterModel: String!,
  $charactersTree: String!,
  $wallet: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateCharacterTraitsTransaction(
    characterAddress: $characterAddress,
    attributes: $attributes,
    project: $project,
    assemblerConfig: $assemblerConfig,
    characterModel: $characterModel,
    charactersTree: $charactersTree,
    wallet: $wallet,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreatePopulateAssembleablCharacterTransaction = """
query CreatePopulateAssembleablCharacterTransaction(
  $attributes: VecMapGeneric!,
  $project: String!,
  $characterModel: String!,
  $charactersTree: String!,
  $mint: String!,
  $owner: String!,
  $updateAuthority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createPopulateAssembleablCharacterTransaction(
    attributes: $attributes,
    project: $project,
    characterModel: $characterModel,
    charactersTree: $charactersTree,
    mint: $mint,
    owner: $owner,
    updateAuthority: $updateAuthority,
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

CreateWrapAssetsToCharacterTransactions = """
query CreateWrapAssetsToCharacterTransactions(
  $mintList: [String!]!,
  $project: String!,
  $characterModel: String!,
  $wallet: String!,
  $libreplexDeployment: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createWrapAssetsToCharacterTransactions(
    mintList: $mintList,
    project: $project,
    characterModel: $characterModel,
    wallet: $wallet,
    libreplexDeployment: $libreplexDeployment,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateUnwrapAssetsFromCharacterTransactions = """
query CreateUnwrapAssetsFromCharacterTransactions(
  $characterAddresses: [String!]!,
  $project: String!,
  $characterModel: String!,
  $wallet: String!,
  $libreplexDeployment: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUnwrapAssetsFromCharacterTransactions(
    characterAddresses: $characterAddresses,
    project: $project,
    characterModel: $characterModel,
    wallet: $wallet,
    libreplexDeployment: $libreplexDeployment,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateRecallCharactersTransaction = """
query CreateRecallCharactersTransaction(
  $data: RecallFromMissionData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createRecallCharactersTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

SignWithShadowSignerAndSendTransactionBundles = """
query SignWithShadowSignerAndSendTransactionBundles(
  $txs: [Bytes!]!,
  $blockhash: String!,
  $lastValidBlockHeight: Int!,
  $authToken: String!,
  $options: SendTransactionBundlesOptions
) {
  signWithShadowSignerAndSendTransactionBundles(
    txs: $txs,
    blockhash: $blockhash,
    lastValidBlockHeight: $lastValidBlockHeight,
    authToken: $authToken,
    options: $options
  ) {
    bundleId
    responses {
      signature
      error
      status
    }
  }
}"""

CreateInitializeFaucetTransaction = """
query CreateInitializeFaucetTransaction(
  $resource: String!,
  $amount: Int!,
  $repeatInterval: Int!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitializeFaucetTransaction(
    resource: $resource,
    amount: $amount,
    repeatInterval: $repeatInterval,
    authority: $authority,
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
    faucet
  }
}"""

CreateClaimFaucetTransaction = """
query CreateClaimFaucetTransaction(
  $faucet: String!,
  $owner: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createClaimFaucetTransaction(
    faucet: $faucet,
    owner: $owner,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateNewUserBulkTransaction = """
query CreateNewUserBulkTransaction(
  $info: [UserInfoInput!]!,
  $wallet: [String!]!,
  $payer: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createNewUserBulkTransaction(
    info: $info,
    wallet: $wallet,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateNewUserTransaction = """
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

CreateUpdateUserTransaction = """
query CreateUpdateUserTransaction(
  $info: PartialUserInfoInput,
  $wallets: UpdateWalletInput,
  $populateCivic: Boolean,
  $payer: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateUserTransaction(
    info: $info,
    wallets: $wallets,
    populateCivic: $populateCivic,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateProfilesTreeTransaction = """
query CreateCreateProfilesTreeTransaction(
  $treeConfig: TreeSetupConfig!,
  $project: String!,
  $payer: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateProfilesTreeTransaction(
    treeConfig: $treeConfig,
    project: $project,
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

CreateNewProfileTransaction = """
query CreateNewProfileTransaction(
  $project: String!,
  $payer: String!,
  $identity: String,
  $info: ProfileInfoInput,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createNewProfileTransaction(
    project: $project,
    payer: $payer,
    identity: $identity,
    info: $info,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateUpdateProfileTransaction = """
query CreateUpdateProfileTransaction(
  $profile: String!,
  $payer: String!,
  $info: ProfileInfoInput,
  $customData: CustomDataInput,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateProfileTransaction(
    profile: $profile,
    payer: $payer,
    info: $info,
    customData: $customData,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateNewUserWithProfileTransaction = """
query CreateNewUserWithProfileTransaction(
  $userInfo: UserInfoInput,
  $project: String!,
  $profileIdentity: String,
  $profileInfo: ProfileInfoInput,
  $wallet: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createNewUserWithProfileTransaction(
    userInfo: $userInfo,
    project: $project,
    profileIdentity: $profileIdentity,
    profileInfo: $profileInfo,
    wallet: $wallet,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateClaimBadgeCriteriaTransaction = """
query CreateClaimBadgeCriteriaTransaction(
  $args: ClaimBadgeCriteriaInput!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createClaimBadgeCriteriaTransaction(
    args: $args,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateUpdateBadgeCriteriaTransaction = """
query CreateUpdateBadgeCriteriaTransaction(
  $args: UpdateBadgeCriteriaInput!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateBadgeCriteriaTransaction(
    args: $args,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""


CreateCreateProjectTransaction = """
query CreateCreateProjectTransaction(
  $authority: String!,
  $name: String!,
  $driver: String,
  $associatedPrograms: [AssociatedProgramInput!],
  $profileDataConfig: ProfileDataConfigInput,
  $subsidizeFees: Boolean,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateProjectTransaction(
    authority: $authority,
    name: $name,
    driver: $driver,
    associatedPrograms: $associatedPrograms,
    profileDataConfig: $profileDataConfig,
    subsidizeFees: $subsidizeFees,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    project
  }
}"""

CreateChangeProjectDriverTransaction = """
query CreateChangeProjectDriverTransaction(
  $project: String!,
  $driver: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createChangeProjectDriverTransaction(
    project: $project,
    driver: $driver,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateDelegateAuthorityTransaction = """
query CreateCreateDelegateAuthorityTransaction(
  $project: String!,
  $delegate: String!,
  $serviceDelegations: ServiceDelegationInput!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateDelegateAuthorityTransaction(
    project: $project,
    delegate: $delegate,
    serviceDelegations: $serviceDelegations,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateModifyDelegationTransaction = """
query CreateModifyDelegationTransaction(
  $project: String!,
  $delegate: String!,
  $modifyDelegation: ModifyDelegationInput!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createModifyDelegationTransaction(
    project: $project,
    delegate: $delegate,
    modifyDelegation: $modifyDelegation,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateAssemblerConfigTransaction = """
query CreateCreateAssemblerConfigTransaction(
  $treeConfig: TreeSetupConfig!,
  $ticker: String!,
  $order: [String!]!,
  $project: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateAssemblerConfigTransaction(
    treeConfig: $treeConfig,
    ticker: $ticker,
    order: $order,
    project: $project,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    assemblerConfig
    treeAddress
    proofBytes
    space
    cost
    maxTreeCapacity
  }
}"""

CreateInitializeBadgeCriteriaTransaction = """
query CreateInitializeBadgeCriteriaTransaction(
  $args: CreateBadgeCriteriaInput!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitializeBadgeCriteriaTransaction(
    args: $args,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""



def main():
    parser = argparse.ArgumentParser(description='Generates HoneyComb interface source code.')
    parser.add_argument('-o', '--out_directory', metavar="PATH", type=str, default="src/honeycomb/generated",
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

    qlparser = GQLParse()
    qlparser.graphql_to_function(CREATE_NEW_RESOURCE, ["authority", "."], ["project"])
    qlparser.graphql_to_function(CREATE_NEW_RESOURCE_TREE, ["authority", "delegateAuthority", "payer"], ["project", "resource"])
    qlparser.graphql_to_function(MINT_RESOURCE_TRANSACTION, ["owner", "authority", "delegateAuthority", "payer"], ["resource"])
    qlparser.graphql_to_function(CREATE_BURN_RESOURCE_TRANSACTION, ["authority", "owner", "payer", "delegateAuthority"], ["resource"])
    qlparser.graphql_to_function(CREATE_NEW_USER_TRANSACTION, ["wallet", "payer"], [])
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

    print(qlparser.print_header_file())
    qlparser.print_cpp_file()

    qlparser.save_cpp_file(os.path.join(args.out_directory, args.cpp_filename))
    qlparser.save_hpp_file(os.path.join(args.out_directory, args.hpp_filename))

if __name__ == "__main__":
    main()

