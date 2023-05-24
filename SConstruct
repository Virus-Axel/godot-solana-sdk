#!/usr/bin/env python

import os
import sys
from pathlib import Path

CONTAINER_BUILD_PATH = "build-containers-with-rust"
LIBRARY_NAME = "godot-solana-sdk"

target_arg = "";
home_directory = Path.home()
cargo_build_command = str(home_directory) + '/.cargo/bin/cargo build'
linker_settings = ""

def build_rustlib(target, source, env):
    env.Execute('cd wrapper && ' + linker_settings + ' ' + cargo_build_command + ' ' + target_arg + ' --release')
    if platform_arg == 'javascript':
        env.Execute('emcc -s MODULARIZE=1 wrapper/target/wasm32-unknown-unknown/release/libwrapper.a -o wrapper/target/wasm32-unknown-unknown/release/module.js')


def image_id_from_repo_name(repository_name):
    return os.popen('podman images --format {{.ID}} --filter=reference=' + repository_name).read()

def get_build_command(platform, architecture):
    arguments = ""
    env_options = ""
    if platform == 'ios':
        arguments = ' IOS_SDK_PATH="/root/ioscross/arm64/SDK/iPhoneOS16.1.sdk" IOS_TOOLCHAIN_PATH="/root/ioscross/arm64" ios_triple="arm-apple-darwin11-"'
    elif platform == 'macos':
        arguments = ' macos_sdk_path="/root/osxcross/target/SDK/MacOSX13.0.sdk/" osxcross_sdk="darwin22"'    
    elif platform == 'android':
        arguments = ""
    elif platform == 'javascript':
        env_options = 'bash -c "source /root/emsdk/emsdk_env.sh && '
        arguments = '"'

    return env_options + 'scons -j 4 platform=' + platform + ' arch=' + architecture + ' target=template_release' + arguments


def build_in_container(platform, container_path, architecture, keep_container=False, keep_images=False):
    CONTAINER_BUILD_COMMAND = 'echo y | ./build.sh 4.x f36'
    REPOSITORY_NAME = {
        'ios': 'localhost/godot-ios',
        'macos': 'localhost/godot-osx',
        'linux': 'localhost/godot-linux',
        'windows': 'localhost/godot-windows',
        'android': 'localhost/godot-android',
        'javascript': 'localhost/godot-web'
    }

    # Build missing containers
    env.Execute('cd ' + container_path + ' && ' + CONTAINER_BUILD_COMMAND)

    image_id = image_id_from_repo_name(REPOSITORY_NAME[platform])

    env.Execute('podman run --mount type=bind,source=.,target=/root/godot-solana-sdk -d -it --name test_container ' + image_id)
    #env.Execute('podman cp . test_container:/root/')
    
    build_command = get_build_command(platform, architecture)
    
    env.Execute('podman exec -w /root/godot-solana-sdk/ test_container ' + build_command)

    #env.Execute('podman cp test_container:/root/godot-solana-sdk/example/bin/ .')
    
    if not keep_container:
        env.Execute('podman rm -f test_container')

    if not keep_images:
        env.Execute('podman rmi -f ' + image_id)


def build_all(env, container_path, keep_images):
    pass
    build_in_container('linux', container_path, 'x86_64', keep_images=keep_images)
    build_in_container('windows', container_path, 'x86_64', keep_images=keep_images)
    build_in_container('javascript', container_path, 'wasm32', keep_images=keep_images)
    build_in_container('android', container_path, 'aarch64', keep_images=keep_images)
    build_in_container('ios', container_path, 'arm64', keep_images=keep_images)
    build_in_container('macos', container_path, 'aarch64', keep_images=keep_images)

AddOption('--keep_images', dest='keep_images', default=False, action='store_true', help='Keeps the podman images for future builds.')
AddOption('--container_build', dest='container_build', default=False, action='store_true', help='Build in containers for all platforms (specify one to override)')

env = SConscript("godot-cpp/SConstruct")

platform_arg = ARGUMENTS.get("platform", ARGUMENTS.get("p", False))

# Link rust solana sdk library
library_path = "wrapper/target/release/"

if platform_arg == "android":
    linker_settings = 'AR=llvm-ar RUSTFLAGS="-C linker=aarch64-linux-android31-clang"'
    target_arg = "--target aarch64-linux-android"
    library_path = "wrapper/target/aarch64-linux-android/release/"

elif platform_arg == "macos":
    target_arg = "--target aarch64-apple-darwin"
    library_path = "wrapper/target/aarch64-apple-darwin/release/"

elif platform_arg == "windows":
    target_arg = "--target x86_64-pc-windows-gnu"
    library_path = "wrapper/target/x86_64-pc-windows-gnu/release/"

elif platform_arg == "linux":
    target_arg = "--target x86_64-unknown-linux-gnu"
    library_path = "wrapper/target/x86_64-unknown-linux-gnu/release/"

elif platform_arg == "javascript":
    target_arg = "--target wasm32-unknown-unknown"
    library_path = "wrapper/target/wasm32-unknown-unknown/release/"
    env.Append(LINKFLAGS=['--js-library', 'wrapper/target/wasm32-unknown-unknown/release/module.js'])

elif platform_arg == "ios":
    linker_settings = 'LD_LIBRARY_PATH=/root/ioscross/arm64/lib/ SDKROOT=/root/ioscross/arm64/SDK/iPhoneOS16.1.sdk/'
    target_arg = "--target aarch64-apple-ios"
    library_path = "wrapper/target/aarch64-apple-ios/release/"
    # Adjust environment in build containers to work with rust wrapper

    env.Append(LIBS = ['objc'])
    env.Append(LIBS = ['c'])
    env.Append(LIBS = ['c++'])
    env.Append(LINKFLAGS=['-framework', 'Security', '-L', '/root/ioscross/arm64/lib/'])

    env['LINK'] = "/root/osxcross/target/bin/aarch64-apple-darwin22-ld"
    env.Append(LD_LIBRARY_PATH=['/root/ioscross/arm64/lib/'])

    #env['LINKFLAGS'].remove(["-isysroot", env["IOS_SDK_PATH"], "-F" + env["IOS_SDK_PATH"]])
    #env.Subst('LINKFLAGS', "-isysroot", "-syslibroot")
    #env['LINKFLAGS'] = [s.subst('-isysroot', '-syslibroot') for s in env['LINKFLAGS']]
    for index in range(len(env['LINKFLAGS'])):
        if env['LINKFLAGS'][index] == "-isysroot":
            env['LINKFLAGS'][index] = "-syslibroot"

    env['SHLINKFLAGS'].remove("-shared")
    env.Append(LINKFLAGS=['-dylib'])

rust_env = Environment(ENV=os.environ)

env.Append(LIBPATH = [library_path])

if platform_arg != 'javascript':
    env.Append(LIBS = ['wrapper'])

if platform_arg == 'windows':
    env.Append(LIBS = ['ws2_32'])
    env.Append(LIBS = ['bcrypt'])
    env.Append(LIBS = ['userenv'])

# Build wrapper library


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
sources = Glob("src/*.cpp")


# Handle the container build
if env.GetOption('container_build'):
    keep_images = False

    if env.GetOption('keep_images'):
        keep_images = True

    if 'platform' in ARGUMENTS:
        architecture = 'x86_64'
        if 'arch' in ARGUMENTS:
            architecture = ARGUMENTS['arch']

            build_in_container(ARGUMENTS['platform'], CONTAINER_BUILD_PATH, architecture, keep_images=keep_images, keep_container=True)

    else:
        build_all(env, CONTAINER_BUILD_PATH, keep_images)

else:
    if env["platform"] == "macos":
        library = env.SharedLibrary(
            "bin/lib" + LIBRARY_NAME + ".{}.{}.framework/lib".format(
                env["platform"], env["target"],
            ) + LIBRARY_NAME + ".{}.{}".format(
                env["platform"], env["target"]
            ),
            source=sources,
        )
    else:
        library = env.SharedLibrary(
            "bin/lib" + LIBRARY_NAME + "{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
            source=sources,
        )

    rust_sources = Glob("wrapper/src/*.rs")
    rust_sources += Glob("wrapper/src/*/*.rs")
    wrapper = rust_env.AlwaysBuild(rust_env.Alias("phony", rust_sources, build_rustlib))

    env.Depends(library, wrapper)

    Default(library)