#!/usr/bin/env python3
import argparse
import subprocess
import glob
import os
import re

script_dir = os.path.dirname(os.path.realpath(__file__))
fishes_dir = os.path.join(script_dir, "fishes")
patches_dir = os.path.join(script_dir, "patches")

targets = {
    "sf-nnue-40": {"url": "https://github.com/official-stockfish/Stockfish", "commit": "68e1e9b"},
    "fsf": {"url": "https://github.com/fairy-stockfish/Fairy-Stockfish", "commit": "a621470"},
}

ignore_sources = ["syzygy/tbprobe.cpp", "pyffish.cpp", "ffishjs.cpp"]


def makefile(target, sources, flags, link_flags):
    # DO NOT replace tabs with spaces
    # fmt: off
    return f"""

CXX = em++
EXE = {target}

CXX_FLAGS = {flags} -Isrc -pthread -msse -msse2 -mssse3 -msse4.1 -msimd128 -flto -fno-exceptions \\
	-DUSE_SSE2 -DUSE_SSSE3 -DUSE_SSE41 -DUSE_POPCNT -DNNUE_EMBEDDING_OFF -DNO_PREFETCH

LD_FLAGS = {link_flags} \\
	--pre-js=../../src/initModule.js -sEXPORT_ES6 -sEXPORT_NAME={mod_name(target)} -sFILESYSTEM=0 \\
	-sEXPORTED_FUNCTIONS='[_malloc,_main]' -sEXPORTED_RUNTIME_METHODS='[stringToUTF8,UTF8ToString,HEAPU8]' \\
	-sINCOMING_MODULE_JS_API='[locateFile,print,printErr,wasmMemory,buffer,instantiateWasm]' \\
	-sINITIAL_MEMORY=64MB -sALLOW_MEMORY_GROWTH -sSTACK_SIZE=2MB -sSTRICT -sPROXY_TO_PTHREAD \\
	-sALLOW_BLOCKING_ON_MAIN_THREAD=0 -sEXIT_RUNTIME -Wno-pthreads-mem-growth

SRCS = {sources}
OBJS = $(addprefix src/, $(SRCS:.cpp=.o)) src/glue.o

$(EXE).js: $(OBJS)
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(OBJS) -o $(EXE).js

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

src/glue.o: ../../src/glue.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

"""


# fmt: on

def mod_name(target):
    return f"{''.join(seg.capitalize() for seg in target.split('-'))}Web"

def main():
    parser = argparse.ArgumentParser(description="build stockfish wasms")
    parser.add_argument("--flags", help="em++ cxxflags", default="-O3 -DNDEBUG --closure=1")
    parser.add_argument("--node", action="store_true", help="target node.js")
    parser.add_argument(
        "target",
        nargs="*",
        help=f"{', '.join(list(targets.keys()))}, clean, or all (default: 'sf-nnue-60')",
    )

    args = parser.parse_args()
    arg_targets = list(args.target)
    if len(arg_targets) == 0:
        arg_targets = ["sf-nnue-60"]

    if "clean" in arg_targets:
        clean()
        arg_targets.remove("clean")

    if "all" in arg_targets:
        arg_targets = list(targets.keys())

    if len(arg_targets) > 0:
        print(f"building: {', '.join(arg_targets)}{' for node.js' if args.node else ''}")
        print(f"flags: {args.flags}")
        print("")
    
    for target in arg_targets:
        build_target(target, args.flags, args.node)


def build_target(target, flags, node):  # changes cwd
    target_dir = os.path.join(fishes_dir, target)
    fetch_sources(target)

    os.chdir(os.path.join(target_dir, "src"))

    sources = [f for f in glob.glob("**/*.cpp", recursive=True) if f not in ignore_sources]

    os.chdir(target_dir)

    if not node:
        more_flags = "-sENVIRONMENT=web,worker -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency"
    else:
        more_flags = "-sENVIRONMENT=node"

    with open("Makefile.tmp", "w") as f:
        f.write(makefile(target, " ".join(sources), flags, more_flags))

    subprocess.run(["make", "-f", "Makefile.tmp", "-j"], check=True)

    for f in [f"{target}.js", f"{target}.wasm"]:
        os.replace(os.path.join(target_dir, f), os.path.join(script_dir, f))


def fetch_sources(target):
    if target not in targets:
        raise Exception(f"unknown target {target}")
    target_dir = os.path.join(fishes_dir, target)
    if not os.path.exists(target_dir):
        os.makedirs(target_dir)
        os.chdir(fishes_dir)
        subprocess.run(["git", "clone", targets[target]["url"], target], check=True)
        subprocess.run(["git", "-C", target_dir, "checkout", targets[target]["commit"]], check=True)
        subprocess.run(
            ["git", "-C", target_dir, "apply", os.path.join(patches_dir, f"{target}.patch")],
            check=True,
        )


def clean():
    clean_list = [
        os.path.join(script_dir, x) for x in ["stockfishWeb.js*", "stockfishWeb.worker.js*"]
    ]
    clean_list.extend(glob.glob(f"{fishes_dir}/**/*.o", recursive=True))
    for target in targets.keys():
        clean_list.append(os.path.join(fishes_dir, target, "Makefile.tmp"))
        clean_list.extend(
            os.path.join(script_dir, f"{target}.{ext}")
            for ext in ["js", "worker.js", "wasm", "js.map", "worker.js.map"]
        )

    subprocess.run(["rm", "-rf"] + clean_list)
    return


def assert_emsdk():
    try:
        result = subprocess.run(['emcc', '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        if result.stderr:
            print("Error:", result.stderr)
            exit(1)
        
        version_match = re.search(f"(\d+)\.(\d+)\.(\d)", result)
        if version_match:
            major, minor, patch = version_match.groups()
            if int(major) < 3 or (int(major) == 3 and int(minor) < 1) or (int(major) == 3 and int(minor) == 1 and int(patch) < 59):
                print("emsdk 3.1.59 or later is required.")
                exit(1)
            else:
                return
    except FileNotFoundError:
        print("emcc not installed or not found in the system path.")
        exit(1)


if __name__ == "__main__":
    main()
