#!/usr/bin/env python

import os
import re
import platform
import subprocess
import shutil
from docs.xml_to_header import doxy_to_header, get_classes_list
from tools.js_to_header import js_to_cpp_header
from tools.json_to_header import json_to_header

CONTAINER_BUILD_PATH = "build-containers"
CONTAINER_NAME = "godot-solana-sdk-container"
LIBRARY_NAME = "godot-solana-sdk"
IPHONE_SDK_VERSION = 17.0
IOS_OSXCROSS_TRIPPLE = 23

DOC_HEADER_NAME = "include/doc_data_godot-solana-sdk.gen.h"

TEST_VALIDATOR_PATH = "ledger"

def build_so(target, source, env):
    target_path = str(target[0])
    source_path = str(source[0])
    cmd = f"solana program dump {source_path} > {target_path}"
    return os.system(cmd)

def run_validator(target, source, env):
    """Start solana-test-validator and terminate it on keypress."""
    validator_args = [
        '--ledger', f'{TEST_VALIDATOR_PATH}/test-ledger',
        '-r',
        '--quiet',
        '--bind-address', '0.0.0.0',
        '--rpc-port', '8899',
        '--deactivate-feature', 'EaQpmC6GtRssaZ3PCUM5YksGqUdMLeZ46BQXYtHYakDS',
        '--deactivate-feature', 'zk1snxsc6Fh3wsGNbbHAJNHiJoYgF29mMnTSusGx5EJ',
        '--deactivate-feature', '2KKG3C6RBnxQo9jVVrbzsoSh41TDXLK7gBc9gduyxSzW',
        '--deactivate-feature', 'zkNLP7EQALfC1TYeB3biDU7akDckj8iPkvh9y2Mt2K3',
        '--deactivate-feature', 'DT4n6ABDqs6w4bnfwrXT9rsprcPf6cdDga1egctaPkLC',
        '--deactivate-feature', '8oBxsYqnCvUTGzgEpxPcnVf7MLbWWPYddE33PftFeBBd',
        '--deactivate-feature', 'Gz1aLrbeQ4Q6PTSafCZcGWZXz91yVRi7ASFzFEr1U4sa',
        '--deactivate-feature', '5TuppMutoyzhUSfuYdhgzD47F92GL1g89KpCZQKqedxP',
        '--deactivate-feature', '9onWzzvCzNC2jfhxxeqRgs5q7nFAAKpCUvkj6T6GJK9i',
        '--deactivate-feature', 'EBq48m8irRKuE7ZnMTLvLg2UuGSqhe8s8oMqnmja1fJw',
        '--deactivate-feature', 'capRxUrBjNkkCpjrJxPGfPaWijB7q3JoDfsWXAnt46r',
        '--deactivate-feature', 'G6ANXD6ptCSyNd9znZm7j4dEczAJCfx7Cy43oBx3rKHJ',
        '--deactivate-feature', 'EenyoWx9UMXYKpR8mW5Jmfmy2fRjzUtM7NduYMY8bx33',
        '--deactivate-feature', 'BZn14Liea52wtBwrXUxTv6vojuTTmfc7XGEDTXrvMD7b',
        '--deactivate-feature', '7XRJcS5Ud5vxGB54JbK9N2vBZVwnwdBNeJW1ibRgD9gx',
        '--deactivate-feature', 'zkiTNuzBKxrCLMKehzuQeKZyLtX2yvFcEKMML8nExU8',
        '--deactivate-feature', 'qywiJyZmqTKspFg2LeuUHqcA5nNvBgobqb9UprywS9N',
        '--deactivate-feature', '41tVp5qR1XwWRt5WifvtSQyuxtqQWJgEK8w91AtBqSwP',
        '--deactivate-feature', 'GvDsGDkH5gyzwpDhxNixx8vtx1kwYHH13RiNAPw27zXb',
        '--deactivate-feature', 'chaie9S2zVfuxJKNRGkyTDokLwWxx6kD2ZLsqQHaDD8',
        '--ticks-per-slot', '2',
    ]
    process = subprocess.Popen([env['TEST_VALIDATOR']] + validator_args + env["PROGRAM_ARGS"] + env['ACCOUNT_ARGS'])

    BLUE = "\033[94m"   # ANSI escape code for blue text
    RESET = "\033[0m"   # Reset to default color

    if not env.GetOption('disable_prompts'):
        input(f"\r{BLUE}Press Enter to exit...{RESET}\n")
        process.terminate()  # Gracefully stop the process

    return None  # No file output expected

def clean_filename(filepath):
    # Extract filename without extension
    filename = os.path.splitext(os.path.basename(filepath))[0]

    # Remove all non-alphabetic characters
    cleaned_name = re.sub(r'[^a-zA-Z_]', '', filename)

    return cleaned_name

def build_docs(env):
    class_list = get_classes_list("docs/xml")
    doxy_to_header(class_list, DOC_HEADER_NAME)


def set_tmp_dir(platform, target):
    dir_name = f".godot/build_files/{platform}/{target}/"
    Execute(Mkdir(dir_name))
    CacheDir(dir_name)


def image_id_from_repo_name(repository_name):
    return os.popen(
        "podman images --format {{.ID}} --filter=reference=" + repository_name
    ).read()


def get_build_command(platform, architecture, debug=False, use_threads=True):
    arguments = ""
    env_options = ""
    if platform == "ios":
        arguments = 'IOS_SDK_PATH="/root/ioscross/arm64/SDK/iPhoneOS{}.sdk" IOS_TOOLCHAIN_PATH="/root/ioscross/arm64" ios_triple="arm-apple-darwin11-"'.format(
            IPHONE_SDK_VERSION
        )
    elif platform == "macos":
        arguments = 'macos_sdk_path="/root/osxcross/target/SDK/MacOSX14.0.sdk/" osxcross_sdk="darwin23"'
    elif platform == "android":
        arguments = ""
    elif platform == "web":
        env_options = 'bash -c "source /root/emsdk/emsdk_env.sh && '
        arguments = '"'
    elif platform == "linux":
        if architecture == "arm64" or architecture == "arm32":
            env_options = (
                "alias g++=/root/arm-godot-linux-gnueabihf_sdk-buildroot/bin/g++ && "
            )
        else:
            env_options = "alias g++=/root/{}-godot-linux-gnueabihf_sdk-buildroot/bin/g++ && ".format(
                architecture
            )

    debug_or_release = "release"
    if debug:
        debug_or_release = "debug"

    return "{} scons -j 4 threads={} platform={} arch={} target=template_{} {}".format(
        env_options, use_threads, platform, architecture, debug_or_release, arguments
    )


def build_in_container(
    platform, architectures, keep_container=False, keep_images=False
):
    REPOSITORY_NAME = {
        "ios": "localhost/godot-ios",
        "macos": "localhost/godot-osx",
        "linux": "localhost/godot-linux",
        "windows": "localhost/godot-windows",
        "android": "localhost/godot-android",
        "web": "localhost/godot-web",
    }

    image_id = image_id_from_repo_name(REPOSITORY_NAME[platform])

    env.Execute(
        "podman run --mount type=bind,source=.,target=/root/godot-solana-sdk -d -it --name {} {}".format(
            CONTAINER_NAME, image_id
        )
    )

    for architecture in architectures:
        build_command = get_build_command(platform, architecture, True)
        env.Execute(
            "podman exec -w /root/godot-solana-sdk/ {} {}".format(
                CONTAINER_NAME, build_command
            )
        )

        build_command = get_build_command(platform, architecture)
        env.Execute(
            "podman exec -w /root/godot-solana-sdk/ {} {}".format(
                CONTAINER_NAME, build_command
            )
        )

        if platform == "web":
            build_command = get_build_command(platform, architecture, True, False)
            env.Execute(
                "podman exec -w /root/godot-solana-sdk/ {} {}".format(
                    CONTAINER_NAME, build_command
                )
            )

            build_command = get_build_command(platform, architecture, False, False)
            env.Execute(
                "podman exec -w /root/godot-solana-sdk/ {} {}".format(
                    CONTAINER_NAME, build_command
                )
            )

    if not keep_container:
        env.Execute("podman rm -f {}".format(CONTAINER_NAME))


def remove_all_images(platform):
    id = image_id_from_repo_name("localhost/godot-" + platform)
    while image_id_from_repo_name("localhost/godot-" + platform):
        env.Execute("podman rmi -f {}".format(id))
        id = image_id_from_repo_name("localhost/godot-" + platform)


def build_all(env, container_path, keep_images):
    CONTAINER_BUILD_COMMAND = "echo y | ./build.sh 4.x f36"

    # Remove existing container
    env.Execute("podman rm -fi {}".format(CONTAINER_NAME))

    # Build missing containers
    env.Execute("cd {} && {}".format(container_path, CONTAINER_BUILD_COMMAND))

    build_in_container("linux", ["x86_64"], keep_images=keep_images)
    build_in_container("windows", ["x86_64"], keep_images=keep_images)
    build_in_container("web", ["wasm32"], keep_images=keep_images)
    build_in_container("android", ["aarch64", "x86_64"], keep_images=keep_images)
    build_in_container("ios", ["arm64"], keep_images=keep_images)
    build_in_container("macos", ["aarch64"], keep_images=keep_images)

    if not keep_images:
        # Remove existing images
        remove_all_images("linux")
        remove_all_images("windows")
        remove_all_images("web")
        remove_all_images("android")
        remove_all_images("ios")
        remove_all_images("osx")
        remove_all_images("fedora")
        remove_all_images("xcode")


AddOption(
    "--remove_images",
    dest="remove_images",
    default=True,
    action="store_false",
    help="Remove the podman images after the build.",
)
AddOption(
    "--container_build",
    dest="container_build",
    default=False,
    action="store_true",
    help="Build in containers for all platforms.",
)
AddOption('--disable_prompts',
          dest="disable_prompts",
          default=False,
          action="store_true",
          help='Disable all input prompts')


env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])
env.Append(CPPPATH=["include/"])
env.Append(CPPPATH=["sha256/"])
env.Append(CPPPATH=["ed25519/src/"])
env.Append(CPPPATH=["instructions/include"])
env.Append(CPPPATH=["instructions/src"])
env.Append(CPPPATH=["src/solana_client/"])
env.Append(CPPPATH=["include/solana_client/"])
env.Append(CPPPATH=["src/instructions"])
env.Append(CPPPATH=["include/instructions"])
env.Append(CPPPATH=["include/meta_data"])
env.Append(CPPPATH=["include/transaction"])
env.Append(CPPPATH=["src/transaction"])
env.Append(CPPPATH=["include/wallet_adapter/"])
env.Append(CPPPATH=["src/wallet_adapter/"])
env.Append(CPPPATH=["src/candy_machine/"])
env.Append(CPPPATH=["src/honeycomb"])
env.Append(CPPPATH=["src/honeycomb/types"])
env.Append(CPPPATH=["src/dialogs/"])
env.Append(CPPPATH=["src/anchor/"])

sources = Glob("src/*.cpp")

sha256_sources = Glob("sha256/sha256.cpp")
ed25519_sources = Glob("ed25519/src/*.c")

wallet_sources = Glob("wallet_adapter/*.cpp")
instruction_sources = Glob("instructions/src/*.cpp")
other_sources = Glob("src/*/*.cpp")
honey_sources = Glob("src/honeycomb/types/*.cpp")


# Handle the container build
if env.GetOption("container_build"):
    build_all(env, CONTAINER_BUILD_PATH, env.GetOption("remove_images"))

    # Stop default targets from dependency SCons files.
    exit(0)

else:

    build_docs(env)
    IDL_PATH = "idls"
    IDL_HEADER_PATH = "include/anchor/generated/"
    directory = os.path.dirname(IDL_HEADER_PATH)
    if directory:
        os.makedirs(directory, exist_ok=True)

    for filename in os.listdir(IDL_PATH):
        if filename.endswith(".json"):
            json_to_header(os.path.join(IDL_PATH, filename), clean_filename(filename), IDL_HEADER_PATH)

    set_tmp_dir(env["platform"], env["target"])
    if env["platform"] == "ios" and platform.system() != "Darwin":

        # Add linker settings for ios build.
        env.Append(LIBS=["objc"])
        env.Append(LIBS=["c"])
        env.Append(LIBS=["c++"])
        env.Append(
            LINKFLAGS=["-framework", "Security", "-L", "/root/ioscross/arm64/lib/"]
        )

        # Workaround for broken ios builds.
        env["LINK"] = "/root/osxcross/target/bin/aarch64-apple-darwin{}-ld".format(
            IOS_OSXCROSS_TRIPPLE
        )
        env.Append(LD_LIBRARY_PATH=["/root/ioscross/arm64/lib/"])

        for index in range(len(env["LINKFLAGS"])):
            if env["LINKFLAGS"][index] == "-isysroot":
                env["LINKFLAGS"][index] = "-syslibroot"

        env["SHLINKFLAGS"].remove("-shared")
        env.Append(LINKFLAGS=["-dylib"])

    wallet_adapter_header_target = None

    if env["platform"] == "web":
        env.Append(LINKFLAGS=["-sWASM_BIGINT=1"])

        WALLET_ADAPTER_BRIDGE_DIR = "wallet_adapter_bridge"

        wallet_adapter_header = "include/wallet_adapter/wallet_adapter_generated.hpp"

        js_file = f"{WALLET_ADAPTER_BRIDGE_DIR}/dist/index_minified.js"

        def generate_js_hpp(target=None, source=None, env=None):
            constant_name = "WALLET_ADAPTER_BUNDLED"

            js_to_cpp_header(js_file, wallet_adapter_header, constant_name)

        wallet_adapter_sources = f"{WALLET_ADAPTER_BRIDGE_DIR}/package.json"
        wallet_adapter_lock = f"{WALLET_ADAPTER_BRIDGE_DIR}/package-lock.json"
        vite_config = f"{WALLET_ADAPTER_BRIDGE_DIR}/webpack.config.js"
        js_sources = Glob(f"{WALLET_ADAPTER_BRIDGE_DIR}/src/*.js")

        NPM_INSTALL_COMMAND = (
            f"npm install --prefix {WALLET_ADAPTER_BRIDGE_DIR} --no-audit --no-fund --legacy-peer-deps --force"
        )
        NPM_BUILD_COMMAND = (
            f"npm run build --prefix {WALLET_ADAPTER_BRIDGE_DIR} -- --no-stats-warnings"
        )

        # Define targets for the package-lock.json (wallet adapter package).
        npm_install_target = env.Command(
            wallet_adapter_lock,
            wallet_adapter_sources,
            NPM_INSTALL_COMMAND,
        )
        package_build_target = env.Command(
            js_file,
            [npm_install_target, vite_config, js_sources],
            NPM_BUILD_COMMAND,
        )

        # Define target for the generated .hpp wallet adapter file.
        wallet_adapter_header_target = env.Command(
            wallet_adapter_header,
            package_build_target,
            generate_js_hpp,
        )

    if env["platform"] == "macos":
        library = env.SharedLibrary(
            "bin/lib"
            + LIBRARY_NAME
            + ".{}.{}.framework/lib".format(
                env["platform"],
                env["target"],
            )
            + LIBRARY_NAME
            + ".{}.{}".format(env["platform"], env["target"]),
            source=sources
            + sha256_sources
            + ed25519_sources
            + wallet_sources
            + instruction_sources
            + other_sources
            + honey_sources,
        )
    else:
        library = env.SharedLibrary(
            "bin/lib" + LIBRARY_NAME + "{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
            source=sources
            + sha256_sources
            + ed25519_sources
            + wallet_sources
            + instruction_sources
            + other_sources
            + honey_sources, 
        )

    Default(library)

    if env["platform"] == "web":
        Depends(library, wallet_adapter_header_target)

    # Define targets related to Clang tidy.

    lint_sources = sources + wallet_sources + instruction_sources + other_sources
    hand_written_lint_sources = [s for s in lint_sources if not str(s).endswith("_generated.cpp")]

    lint_env = Environment()  # SConscript("godot-cpp/SConstruct")
    lint_env["CLANG_TIDY"] = os.environ.get("CLANG_TIDY", "clang-tidy")
    lint_env["CLANG_FORMAT"] = os.environ.get("CLANG_FORMAT", "clang-tidy")
    lint_env.Tool("compilation_db")

    lint_filenames = [str(f) for f in hand_written_lint_sources]
    build_defines = ["-DWEB_ENABLED"]
    extra_arg = f'--extra-arg {" ".join(build_defines)}' if build_defines else ""
    tidy_command = f'{lint_env["CLANG_TIDY"]} -p compile_commands.json {extra_arg} {" ".join(lint_filenames)}'
    clang_tidy_action = lint_env.Action([tidy_command])
    clang_tidy_command = lint_env.Command(
        "lint", "compile_commands.json", clang_tidy_action
    )

    # Define test validator target

    VALIDATOR_PROGRAMS = {
        'candy_guard': 'CMAGAKJ67e9hRZgfC5SFTbZH8MgEmtqazKXjmkaJjWTJ',
        'candy_machine_core': 'CMACYFENjoBMHzapRXyo1JZkVS6EtaDDzkjMrmQLvr4J',
        'mpl_core_program': 'CoREENxT6tW1HoK8ypY1SxRMZTcVPm7R94rH4PZNhX7d',
        'MPCG': 'Guard1JwRhJkVH6XZhzoYxeBVQe872VH6QggF4BWmS9g',
        'MPMD': 'metaqbxxUerdq28cj1RbAWkYQm3ybzjb6a8bt518x1s',
        'CMV3': 'CndyV3LdqHUfDLmE5naZjVN8rBZz4tqhdefbAnjHG3JR',
        'shdwdrive': '2e1wdyNhUvE76y6yUCvah2KaviavMJYKoRun8acMRBZZ',
    }

    solana_env = Environment(ENV=os.environ)

    validator_programs = [f'{TEST_VALIDATOR_PATH}/{f}.so' for f in VALIDATOR_PROGRAMS.keys()]
    validator_program_args = [
        arg for f in VALIDATOR_PROGRAMS.keys() 
        for arg in ['--bpf-program', VALIDATOR_PROGRAMS[f], f'{TEST_VALIDATOR_PATH}/{f}.so']
    ]

    for validator_program in VALIDATOR_PROGRAMS.keys():
        program_library_file = f'{TEST_VALIDATOR_PATH}/{validator_program}.so'
        dump_program_command = f'solana program dump --url mainnet-beta {VALIDATOR_PROGRAMS[validator_program]} {program_library_file}'
        dump_program_action = solana_env.Action([dump_program_command])
        dump_program_target = solana_env.Command(program_library_file, None, dump_program_action)

    solana_env['TEST_VALIDATOR'] = os.environ.get("SOLANA_TEST_VALIDATOR", "solana-test-validator")
    account_files = Glob(f'{TEST_VALIDATOR_PATH}/accounts/*.json')
    print(account_files)
    
    
    
    solana_env["ACCOUNT_ARGS"] = [
        item for f in account_files for item in ("--account", os.path.splitext(os.path.basename(str(f)))[0], str(f))
    ]
    solana_env["PROGRAM_ARGS"] = validator_program_args
    test_validator_action = Action(run_validator)
    test_validator_command = solana_env.Command(
        "test-validator", validator_programs, test_validator_action
    )

    gradle_command = env.SConscript('android/SConscript', exports={'env': env})
    
    ANDROID_PLUGIN_DESTINATION = 'addons/SolanaSDK/WalletAdapterAndroid/'
    ANDROID_PLUGIN_SOURCE = 'android/plugin/WalletAdapterAndroid/'
    
    android_plugin_files = [
        "android/plugin/WalletAdapterAndroid/bin/debug/WalletAdapterAndroid-debug.aar",
        "android/plugin/WalletAdapterAndroid/bin/release/WalletAdapterAndroid-release.aar",
    ]
    
    def copy_aar_action(source, target, env):
        shutil.rmtree(ANDROID_PLUGIN_DESTINATION, ignore_errors=True)
        shutil.copytree(ANDROID_PLUGIN_SOURCE, ANDROID_PLUGIN_DESTINATION)
        return None
    
    env.Command(ANDROID_PLUGIN_DESTINATION, android_plugin_files + [gradle_command], copy_aar_action)
    
    BIN_DESTINATION = "addons/SolanaSDK/bin/"
    BIN_SOURCE = "bin/"
    def copy_bin_action(source, target, env):
        shutil.rmtree(BIN_DESTINATION, ignore_errors=True)
        shutil.copytree(BIN_SOURCE, BIN_DESTINATION)
        return None
    
    bin_target = env.Command(BIN_DESTINATION, library, copy_bin_action)
    
    GDEXT_FILE = 'bin/godot-solana-sdk.gdextension'
    gdext_target = env.Command(GDEXT_FILE, env.File("godot-solana-sdk.gdextension"), Copy(GDEXT_FILE, "godot-solana-sdk.gdextension"))

    Depends(gdext_target, bin_target)
    AlwaysBuild(bin_target)
    AlwaysBuild(gdext_target)

    env.Command("assemble", GDEXT_FILE, [copy_bin_action, copy_aar_action])
    env.Alias("addon", [ANDROID_PLUGIN_DESTINATION, gdext_target])
