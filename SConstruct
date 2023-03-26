#!/usr/bin/env python

import os
import sys
from pathlib import Path

home_directory = Path.home()
cargo_build_command = str(home_directory) + '/.cargo/bin/cargo build --release'

env = SConscript("godot-cpp/SConstruct")

platform_arg = ARGUMENTS.get("platform", ARGUMENTS.get("p", False))

# Link rust solana sdk library

if platform_arg == "android":
    linker_settings = 'AR=llvm-ar RUSTFLAGS="-C linker=aarch64-linux-android30-clang"'
    env.Command ('solana/target/aarch64-linux-android/release/libsolana_sdk.so', '', 'cd solana/sdk && ' + linker_settings + ' ' + cargo_build_command + " --target aarch64-linux-android")
    env.Append(LIBPATH = ['solana/target/aarch64-linux-android/release'])
    env.Append(LIBS = ['libsolana_sdk'])
else:
    env.Command ('solana/target/release/libsolana_sdk.so', '', 'cd solana/sdk && ' + cargo_build_command)
    env.Append(LIBPATH = ['solana/target/release'])
    env.Append(LIBS = ['libsolana_sdk'])


# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")


if env["platform"] == "macos":
    library = env.SharedLibrary(
        "example/bin/libgdexample.{}.{}.framework/libgdexample.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "example/bin/libgdexample{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)