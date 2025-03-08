""" Tool to create header files with stringified JSON data """

import os
import json

def json_to_header(filename: str, header_name: str, output_directory: str) -> None:
    """ Writes a header file with a stringified json file.
    
    Args:
        filename (str): Input json file to stringify.
        header_name (str): Name of the header file (without file extension).
        output_directory (str): Directory to place header file in.
    """
    with open(filename, "r", encoding="utf8") as file:
        json_content = json.load(file)

    header_guard_name = f"GODOT_SOLANA_SDK_{header_name.upper()}_HPP"

    modify_json_content = json.dumps(json_content, separators=(",", ":")).replace('\\', '\\\\').replace('\"', '\\"')
    header_content = f"""// This file is generated, do not edit.
#ifndef {header_guard_name}
#define {header_guard_name}

#include <string>

const std::string {header_name + '_' + 'idl'} = "{modify_json_content}";

#endif // {header_guard_name}
"""

    with open(os.path.join(output_directory, f'{header_name}.hpp'), "w", encoding="utf8") as file:
        file.write(header_content)

    print("Header file generated: data.h")
