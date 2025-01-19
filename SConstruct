#!/usr/bin/env python

import os
import platform
from docs.xml_to_header import doxy_to_header, get_classes_list
from tools.js_to_header import js_to_cpp_header

CONTAINER_BUILD_PATH = "build-containers"
CONTAINER_NAME = "godot-solana-sdk-container"
LIBRARY_NAME = "godot-solana-sdk"
IPHONE_SDK_VERSION = 17.0
IOS_OSXCROSS_TRIPPLE = 23

DOC_HEADER_NAME = "include/doc_data_godot-solana-sdk.gen.h"


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

sources = Glob("src/*.cpp")

sha256_sources = Glob("sha256/sha256.cpp")
ed25519_sources = Glob("ed25519/src/*.c")

wallet_sources = Glob("wallet_adapter/*.cpp")
instruction_sources = Glob("instructions/src/*.cpp")
other_sources = Glob("src/*/*.cpp")

# Handle the container build
if env.GetOption("container_build"):
    build_all(env, CONTAINER_BUILD_PATH, env.GetOption("remove_images"))

    # Stop default targets from dependency SCons files.
    exit(0)

else:

    build_docs(env)

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
            f"npm install --prefix {WALLET_ADAPTER_BRIDGE_DIR} --no-audit --no-fund"
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
            + other_sources,
        )
    else:
        library = env.SharedLibrary(
            "bin/lib" + LIBRARY_NAME + "{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
            source=sources
            + sha256_sources
            + ed25519_sources
            + wallet_sources
            + instruction_sources
            + other_sources,
        )

    Default(library)

    if env["platform"] == "web":
        Depends(library, wallet_adapter_header_target)

    # Define targets related to Clang tidy.

    lint_sources = sources + wallet_sources + instruction_sources + other_sources

    lint_env = Environment()  # SConscript("godot-cpp/SConstruct")
    lint_env["CLANG_TIDY"] = os.environ.get("CLANG_TIDY", "clang-tidy")
    lint_env["CLANG_FORMAT"] = os.environ.get("CLANG_FORMAT", "clang-tidy")
    lint_env.Tool("compilation_db")

    lint_filenames = [str(f) for f in lint_sources]
    build_defines = ["-DWEB_ENABLED"]
    extra_arg = f'--extra-arg {" ".join(build_defines)}' if build_defines else ""
    tidy_command = f'{lint_env['CLANG_TIDY']} -p compile_commands.json {extra_arg} {" ".join(lint_filenames)}'
    clang_tidy_action = lint_env.Action([tidy_command])
    clang_tidy_command = lint_env.Command(
        "lint", "compile_commands.json", clang_tidy_action
    )
