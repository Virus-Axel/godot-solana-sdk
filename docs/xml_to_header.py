import xml.etree.ElementTree as ET
import re
import os


# Godot does not handle \t in docs
TAB = "        "

DOC_HEADER_NAME = "include/doc_data_godot-solana-sdk.gen.h"


def add_refs_to_description(text):
    TYPES_TO_REF = [
        "Pubkey",
        "Variant",
        "Resource"
    ]
    for ref in TYPES_TO_REF:
        text = text.replace(f' {ref}', f' [{ref}]')

    return text


def build_xml_func(method):
    root = ET.Element("method", name = method["name"])
    if "deprecated" in method:
        root.set("deprecated", method["deprecated"])
    ET.SubElement(root, "return", type = method["return"])
    for idx, param in enumerate(method["params"]):
        ET.SubElement(root, "param", index=str(idx), name=param["name"], type=param["type"])

    description_element = ET.SubElement(root, "description")

    splits = method["detailed"].split("\n", 1)
    args = re.sub(r'(\n)+', r'\n', splits[1]).lstrip().rstrip().split("\n")
    arg_description = ""
    for i in range(0, len(args)-1, 2):
        arg_description += f'{TAB}[i]{args[i]}[/i]: {args[i+1]}\n'
    
    if len(args) % 2:
        if args[-1] == '':
            arg_description += f'{TAB}[i]return[/i]: {splits[0]}'
        else:
            arg_description += f'{TAB}[i]return[/i]: {args[-1]}'

    brief = method["brief"].rstrip()
    desctiption_text = re.sub(r'(\n)+', r'\n', f'{brief}\n{splits[0]}\n{arg_description}')
    
    description_element.text = add_refs_to_description(desctiption_text)

    return root


def build_xml(data):
    root = ET.Element("class", name = data["class_name"])
    brief = ET.SubElement(root, "brief_description")
    brief.text = data["brief"]
    detailed = ET.SubElement(root, "description")
    detailed.text = data["detailed"]

    method_element = ET.SubElement(root, "methods")

    for method in data["methods"]:
        method_child = build_xml_func(method)
        method_element.append(method_child)

    return ET.tostring(root, encoding='unicode', method='xml')


def tree_to_text(tree):
    result = ""
    for child in tree:
        if child.tag == "para":
            result += ET.tostring(child, encoding='unicode', method='text')

    return result


def get_param_type(tree):
    if tree.find("ref"):
        return "Variant"
    return tree.text.replace("const", "").replace("&", "").lstrip().rstrip()


def tree_to_func(tree):
    GODOT_ACCESSIBLE = "Godot Accessible!\n \n"

    result = {}

    return_type = tree.find("type")
    result["return"] = return_type.text
    function_name = tree.find("name")
    result["name"] = function_name.text

    # params
    param_list = []
    params = tree.findall(".//param")
    for param in params:
        param_info = {}
        param_type = param.find("type")
        param_info["type"] = get_param_type(param_type)
        param_name = param.find("declname")
        param_info["name"] = param_name.text

        param_list.append(param_info)

    result["params"] = param_list

    brief = tree.find("briefdescription")
    if brief:
        result["brief"] = tree_to_text(brief)

    detailed = tree.find("detaileddescription")


    for child in detailed:
        simplesect = child.find('simplesect')
        if simplesect is not None:
            if simplesect.get("kind") == "note":
                result["note"] = simplesect.find("para").text
                child.remove(simplesect)

        xreftitles = child.findall('.//xreftitle')
        for xreftitle in xreftitles:
            if xreftitle.text == "Deprecated":
                descriptions = child.findall('.//xrefdescription')
                if not descriptions:
                    continue

                result["deprecated"] = descriptions[0].find("para").text
                child.remove(child.find("xrefsect"))


    if detailed:
        result["detailed"] = tree_to_text(detailed)
        if result["detailed"].find(GODOT_ACCESSIBLE) > -1:
            result["detailed"] = result["detailed"].replace(GODOT_ACCESSIBLE, '')
            result["godot"] = True
        else:
            result["godot"] = False
    return result


def parse_doxy_class(source):
    data = ET.parse(source)
    root = data.getroot()
    result = {}

    class_name = root.findall(".//compoundname")
    result["class_name"] = class_name[0].text.split("::")[-1]

    doc_class = root.findall('.//compounddef')
    class_description = doc_class[0].find('briefdescription')
    result["brief"] = tree_to_text(class_description)
    class_detailed = doc_class[0].find('detaileddescription')

    result["detailed"] = tree_to_text(class_detailed)
    
    memberdefs = doc_class[0].findall('.//memberdef')

    godot_funcs = []

    for memberdef in memberdefs:
        member_type = memberdef.get("kind")

        if(member_type == "function"):
            parsed_func = tree_to_func(memberdef)

            if not "godot" in parsed_func:
                continue
            if parsed_func["godot"]:
                godot_funcs.append(parsed_func)

    result["methods"] = godot_funcs

    if len(godot_funcs) == 0:
        return ""
    else:
        return build_xml(result)


def doxy_to_header(sources, target):
    xml_string = ""
    for source in sources:
        xml_string += parse_doxy_class(source)

    make_doc_header(target, xml_string)


def make_doc_header(target, raw_data):
    dst = str(target)
    g = open(dst, "w", encoding="utf-8")

    g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
    g.write("#ifndef _DOC_DATA_RAW_H\n")
    g.write("#define _DOC_DATA_RAW_H\n")

    g.write("static const int _doc_data_size = " + str(len(raw_data)) + ";\n")
    g.write("static const char _doc_data[] = {\n")
    for i in range(len(raw_data)):
        g.write("\t" + str(ord(raw_data[i])) + ",\n")
    g.write("};\n")

    g.write("#endif")

    g.close()

def get_classes_list(folder_path):
    PREFIX = "classgodot_1_1_"
    class_list = []

    try:
        with os.scandir(folder_path) as entries:
            for entry in entries:
                # Check if the entry is a file and if its name starts with the prefix
                if entry.is_file() and entry.name.startswith(PREFIX):
                    class_list.append(os.path.join(folder_path, entry.name))

    except FileNotFoundError:
        print("The specified folder path does not exist.")
    except Exception as e:
        print(f"An error occurred: {e}")
    
    return class_list


class_list = get_classes_list("docs/xml")
doxy_to_header(class_list, DOC_HEADER_NAME)
#doxy_to_header(["docs/xml/classgodot_1_1_pubkey.xml"], DOC_HEADER_NAME)