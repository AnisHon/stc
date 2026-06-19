import os


def write_codes(path: str, code: str):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w+", encoding="utf-8") as f:
        f.write(code)
        print(f"已经写入: {path}")
