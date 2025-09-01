"""Tool to create header file from bundled javascript. """


def js_to_cpp_header(
    js_file: str, header_file: str, variable_name: str = "js_code"
) -> None:
    """Reads bundled js code and places it in a new header file as a constant string.

    Args:
        js_file (str): File path of bundled javascript.
        header_file (str): Output file.
        variable_name (str, optional): Name of code constant. Defaults to "js_code".
    """
    # Define special characters and their replacements
    special_characters = {
        "\\": "\\\\",  # Escape backslash
        '"': '\\"',  # Escape double quotes
        "'": "\\'",  # Escape single quotes
        "\n": "\\n",  # Escape newlines
        "\t": "\\t",  # Escape tabs
        "\r": "\\r",  # Escape carriage returns
        "\b": "\\b",  # Escape backspace
        "\f": "\\f",  # Escape form feed
        "\v": "\\v",  # Escape vertical tab
        "??": "\\?\\?",  # Escape trigraph
    }

    # Read the JavaScript file
    with open(js_file, "r", encoding="utf8") as js_f:
        js_code = js_f.read()

    # Split the code into manageable chunks
    chunk_size = 100
    chunks = [js_code[i : i + chunk_size] for i in range(0, len(js_code), chunk_size)]

    # Generate the C++ header content
    header_content = (
        f"#ifndef {variable_name.upper()}_H\n#define {variable_name.upper()}_H\n\n"
    )
    header_content += f"//NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)\nconst char {variable_name}[] =\n"
    for chunk in chunks:
        # Replace special characters
        for char, replacement in special_characters.items():
            chunk = chunk.replace(char, replacement)
        header_content += f'    "{chunk}"\n'
    header_content += "    ;\n//NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays) \n\n#endif"

    # Write to the header file
    with open(header_file, "w", encoding="utf8") as header_f:
        header_f.write(header_content)


# Example usage
# js_to_cpp_header("wallet_adapter_bridge/dist/index-x36gJ3Fl.js", "js_bundle.h")
