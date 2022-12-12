import argparse
import csv
import hashlib
import re
from pathlib import Path

TRACING_SOURCE_FILES = (".cpp", ".h")
TRACING_PATTERN = r'HashTrace::(.*)[ ]*?\([ ]*?"(.*)"'


def get_level_str(level: str) -> str:
    if level == "info":
        return "I:"
    elif level == "warning":
        return "W:"
    elif level == "error":
        return "E:"
    else:
        return ""


def get_hash(text: str) -> str:
    return hashlib.md5(text.encode("ascii")).hexdigest()


def get_hash_map_from_trace_list(trace_list: list[tuple[str, str]]) -> list[tuple[str, str]]:
    output = []
    for trace in trace_list:
        level, text = trace
        text = get_level_str(level) + text
        hash = (get_hash(text), text)
        output.append(hash)
    return output


def get_hash_map(directory: Path, source_files: str) -> list[tuple[str, str]]:
    trace_list = []
    for source in directory.glob("**/*" + source_files):
        with open(source, "r") as file:
            trace = re.findall(TRACING_PATTERN, file.read(), re.MULTILINE)
            if trace:
                trace_list += trace

    return get_hash_map_from_trace_list(trace_list)


def main():
    parser = argparse.ArgumentParser(description="Generate Tracing Hash Map")
    parser.add_argument("root", type=Path, help="Root directory")
    parser.add_argument("output", type=Path, help="Output directory")

    args = parser.parse_args()

    root = args.root.expanduser().resolve()
    output = args.output.expanduser().resolve()

    hash_map = []
    for source_files in TRACING_SOURCE_FILES:
        hash_map += get_hash_map(root, source_files)

    # TODO detect collisions
    with open(output / "trace.csv", "w", newline="") as csvfile:
        spamwriter = csv.writer(csvfile, delimiter=";")
        for hash in hash_map:
            spamwriter.writerow(hash)


if __name__ == "__main__":
    main()
