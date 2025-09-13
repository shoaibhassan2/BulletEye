import os

# Files to skip content for
SKIP_CONTENT = {'cJSON.c', 'cJSON.h',"DarkMode.h","darkmode_dynamic.c"}

# File extensions to exclude completely
EXCLUDE_EXTENSIONS = {'.py', '.pyc', '.pyo', '.pyw', '.bat','.bmp','.wav', '.png', '.ico', '.dll'}

def print_tree_and_files(root_path, prefix=''):
    entries = sorted(os.listdir(root_path))
    for index, entry in enumerate(entries):
        path = os.path.join(root_path, entry)
        is_last = index == len(entries) - 1
        branch = '└── ' if is_last else '├── '
        print(prefix + branch + entry)

        new_prefix = prefix + ('    ' if is_last else '│   ')

        if os.path.isdir(path):
            print_tree_and_files(path, new_prefix)
        else:
            _, ext = os.path.splitext(entry)
            if ext in EXCLUDE_EXTENSIONS:
                continue
            if entry in SKIP_CONTENT:
                continue  # Skip content, already printed name above
            try:
                print_file_content(path)
            except Exception as e:
                print(f"{new_prefix}Error reading {path}: {e}")

def print_file_content(file_path):
    rel_path = os.path.relpath(file_path)
    print(f"\n--- File: {rel_path} ---")
    with open(file_path, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()
        print(content)
    print(f"--- End of {rel_path} ---\n")

if __name__ == '__main__':
    print("Project Structure and File Contents:\n")
    print_tree_and_files(os.getcwd())
