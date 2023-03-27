#!/usr/bin/env python

import os
import sys
from pathlib import Path

home_directory = Path.home()
cargo_build_command = str(home_directory) + '/.cargo/bin/cargo build --release'

env = SConscript("godot-cpp/SConstruct")

platform_arg = ARGUMENTS.get("platform", ARGUMENTS.get("p", False))

# Link rust solana sdk library
target_arg = ""
linker_settings = ""
library_path = "solana/target/release/"

if platform_arg == "android":
    linker_settings = 'AR=llvm-ar RUSTFLAGS="-C linker=aarch64-linux-android30-clang"'
    target_arg = "--target aarch64-linux-android"
    library_path = "solana/target/aarch64-linux-android/release/"

elif platform_arg == "macos":
    target_arg = "--target x86_64-apple-darwin"
    library_path = "solana/target/x86_64-apple-darwin/release/"

elif platform_arg == "windows":
    target_arg = "--target x86_64-pc-windows-gnu"
    library_path = "solana/target/x86_64-pc-windows-gnu/release/"

elif platform_arg == "linux":
    target_arg = "--target x86_64-unknown-linux-gnu"
    library_path = "solana/target/x86_64-unknown-linux-gnu/release/"

elif platform_arg == "javascript":
    target_arg = "--target wasm-unknown-unknown"
    library_path = "solana/target/wasm-unknown-unknown/release/"


env.Command (library_path + 'libsolana_sdk.so', '', 'cd solana/sdk && ' + linker_settings + ' ' + cargo_build_command + ' ' + target_arg)
env.Append(LIBPATH = [library_path])
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