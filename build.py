import hashlib
import json
import os
import subprocess
import sys
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading
import shutil

# ================================================================
# ⚙️ CONFIGURATION
# ================================================================
PROJECT_NAME = "BulletEyeV1.0.exe"
CC = "tcc"   # ⚡ Tiny C Compiler
CFLAGS = ["-std=c99", "-DUNICODE", "-D_UNICODE", "-municode", "-s", "-O2"]

INCLUDE_DIRS = [
    "include",
    "darkmodelib/include"
]
LIB_DIRS = ["darkmodelib"]
LIBS = [
    "ws2_32", "shlwapi", "winmm", "gdi32", "user32", "shell32", "kernel32",
    "comctl32", "comdlg32", "advapi32", "ole32", "oleaut32", "dwmapi", 
    "uxtheme", "cabinet", "cJSON", "Dbghelp"
]

SRC_FILES = [

    # 🏗️ Core
    "src/main.c",

    # 💥 Crash
    "src/crash_handler/crash_handler.c",
    "src/crash_handler/crash_ui.c",

    # 🖼️ GUI
    "src/gui/main_proc.c",
    "src/gui/command_handler.c",
    "src/gui/contextmenu_handler.c",
    "src/gui/main_controls.c",
    "src/gui/server_page.c",
    "src/gui/client_page.c",
    "src/gui/log_page.c",
    "src/gui/log_handler.c",
    "src/gui/settings.c",
    "src/gui/about_window.c",
    "src/gui/tools_window.c",
    "src/gui/tooltip.c",
    "src/gui/monitor.c",
    "src/gui/network_settings_window.c",
    "darkmodelib/darkmode_dynamic.c",

    # 📂 GUI: File Explorer
    "src/gui/file_explorer/fe_command_router.c",
    "src/gui/file_explorer/fe_data_caching.c",
    "src/gui/file_explorer/fe_list_view_display.c",
    "src/gui/file_explorer/fe_list_view_events.c",
    "src/gui/file_explorer/fe_main_window.c",
    "src/gui/file_explorer/fe_path_navigator.c",
    "src/gui/file_explorer/fe_preferences_dialog.c",
    "src/gui/file_explorer/fe_remote_operations.c",
    "src/gui/file_explorer/fe_toolbar_view.c",
    "src/gui/file_explorer/fe_tree_view_display.c",
    "src/gui/file_explorer/fe_tree_view_events.c",
    "src/gui/file_explorer/fe_ui_manager.c",

    # 🛠️ GUI: Tools
    "src/gui/tools/icon_changer_page.c",
    "src/gui/tools/builder_page.c",
    "src/gui/tools/binder_page.c",

    # 🗂️ GUI: Registry Settings
    "src/gui/registery_settings.c",

    # 🌐 Server
    "src/server/server_errors.c",
    "src/server/server_logic.c",
    "src/server/server_listview.c",
    "src/server/server_manager.c",

    # 🔧 Helpers
    "src/helpers/icon_changer.c",
    "src/helpers/icon_extractor.c",
    "src/helpers/utils.c",

    # 🎶 Service
    "src/service/soundplayer.c",
    "src/service/notification.c",
    "src/service/encryption_service.c",
    "src/service/compression_service.c",
    "src/service/network_service.c",

    # 📦 Resources
    "res/resource.rc"
]

BUILD_DIR = Path("build")
OBJ_DIR = BUILD_DIR / "obj"
HASH_FILE = BUILD_DIR / "file_hashes.json"

# Paths for DLL copy
DLL_SRC = Path(r"darkmodelib\darkmode.dll")
DLL_DST = Path(r"build\darkmode.dll")
CJSON_SRC = Path(r"darkmodelib\cjson.dll")
CJSON_DST = Path(r"build\cjson.dll")

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
    cmd = [CC, "-municode", "-mwindows", "-s", "-O2", "-o", str(exe_path)]
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

    # 📦 Copy DLLs
    try:
        shutil.copyfile(DLL_SRC, DLL_DST)
        shutil.copyfile(CJSON_SRC, CJSON_DST)
        log(f"📥 Copied {DLL_SRC} → {DLL_DST}")
        log(f"📥 Copied {CJSON_SRC} → {CJSON_DST}")
    except Exception as e:
        log(f"⚠️ Failed to copy DLL: {e}")

    # 💾 Save hashes
    with open(HASH_FILE, "w") as f:
        json.dump(new_hashes, f, indent=2)

    log(f"✅ Build finished: {exe_path}")

if __name__ == "__main__":
    main()
