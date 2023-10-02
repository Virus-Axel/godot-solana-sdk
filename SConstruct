#!/usr/bin/env python

import os

CONTAINER_BUILD_PATH = "build-containers-with-rust"
CONTAINER_NAME = "godot-solana-sdk-container"
LIBRARY_NAME = "godot-solana-sdk"

def image_id_from_repo_name(repository_name):
    return os.popen('podman images --format {{.ID}} --filter=reference=' + repository_name).read()

def get_build_command(platform, architecture):
    arguments = ""
    env_options = ""
    if platform == 'ios':
        arguments = 'IOS_SDK_PATH="/root/ioscross/arm64/SDK/iPhoneOS16.1.sdk" IOS_TOOLCHAIN_PATH="/root/ioscross/arm64" ios_triple="arm-apple-darwin11-"'
    elif platform == 'macos':
        arguments = 'macos_sdk_path="/root/osxcross/target/SDK/MacOSX13.0.sdk/" osxcross_sdk="darwin22"'    
    elif platform == 'android':
        arguments = ""
    elif platform == 'javascript':
        env_options = 'bash -c "source /root/emsdk/emsdk_env.sh && '
        arguments = '"'

    return '{} scons -j 4 platform={} arch={} target=template_release {}'.format(env_options, platform, architecture, arguments)


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
    env.Execute('cd {} && {}'.format(container_path, CONTAINER_BUILD_COMMAND))

    image_id = image_id_from_repo_name(REPOSITORY_NAME[platform])

    env.Execute('podman run --mount type=bind,source=.,target=/root/godot-solana-sdk -d -it --name {} {}'.format(CONTAINER_NAME, image_id))
    #env.Execute('podman cp . test_container:/root/')
    
    build_command = get_build_command(platform, architecture)
    
    env.Execute('podman exec -w /root/godot-solana-sdk/ {} {}'.format(CONTAINER_NAME ,build_command))

    #env.Execute('podman cp test_container:/root/godot-solana-sdk/example/bin/ .')
    
    if not keep_container:
        env.Execute('podman rm -f {}'.format(CONTAINER_NAME))

    if not keep_images:
        env.Execute('podman rmi -f {}'.format(image_id))


def build_all(env, container_path, keep_images):
    # Remove existing container
    env.Execute('podman rm -fi {}'.format(CONTAINER_NAME))

    #build_in_container('linux', container_path, 'x86_64', keep_images=keep_images)
    #build_in_container('windows', container_path, 'x86_64', keep_images=keep_images)
    build_in_container('javascript', container_path, 'wasm32', keep_images=keep_images, keep_container=True)
    #build_in_container('android', container_path, 'aarch64', keep_images=keep_images)
    #build_in_container('ios', container_path, 'arm64', keep_images=keep_images)
    #build_in_container('macos', container_path, 'aarch64', keep_images=keep_images)

AddOption('--keep_images', dest='keep_images', default=False, action='store_true', help='Keeps the podman images for future builds.')
AddOption('--container_build', dest='container_build', default=False, action='store_true', help='Build in containers for all platforms (specify one to override)')

env = SConscript("godot-cpp/SConstruct")

# Link rust solana sdk library

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
env.Append(CPPPATH=["sha256/"])
env.Append(CPPPATH=["cryptopp/"])
env.Append(CPPPATH=["BLAKE3/c/"])
env.Append(CCFLAGS=[
    "-DBLAKE3_NO_SSE41",
    "-DBLAKE3_NO_SSE2",
    "-DBLAKE3_NO_AVX512",
    "-DBLAKE3_NO_AVX2",
    "-DCRYPTOPP_DISABLE_SSE3",
    "-DCRYPTOPP_DISABLE_SSSE3",
    "-DCRYPTOPP_DISABLE_SSE4",
    "-DCRYPTOPP_DISABLE_CLMUL",
    "-DCRYPTOPP_DISABLE_AESNI",
    "-DCRYPTOPP_DISABLE_AVX",
    "-DCRYPTOPP_DISABLE_AVX2",
    "-DCRYPTOPP_DISABLE_SHANI",])
sources = Glob("src/*.cpp")
blak3_sources = Glob("BLAKE3/c/blake3.c")
blak3_sources.append(Glob("BLAKE3/c/blake3_dispatch.c")[0])
blak3_sources.append(Glob("BLAKE3/c/blake3_portable.c")[0])

sha256_sources = Glob("sha256/sha256.cpp")

#cryptopp_sources = ['cryptopp/randpool.cpp',
#                    'cryptopp/xed25519.cpp',
#                    'cryptopp/osrng.cpp',
#                    'cryptopp/cryptlib.cpp',
#                    'cryptopp/rng.cpp',
#                    'cryptopp/fips140.cpp',
#                    'cryptopp/sha.cpp',
#                    'cryptopp/rijndael.cpp',
#                    'cryptopp/modes.cpp',]
cryptopp_sources = Glob('cryptopp/*.cpp')

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
            source=sources + blak3_sources + sha256_sources + cryptopp_sources,
        )
    else:
        library = env.SharedLibrary(
            "bin/lib" + LIBRARY_NAME + "{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
            source=sources + blak3_sources + sha256_sources + cryptopp_sources,
        )

    #wrapper = SConscript("wrapper/SConstruct", exports={'env': env})

    #env.Depends(library, wrapper)
    Default(library)
