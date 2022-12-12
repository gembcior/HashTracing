import argparse
import csv
import re
import subprocess
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description="Hashing app runner")
    parser.add_argument("app", type=Path, help="Path to app")
    parser.add_argument("csv", type=Path, help="Path to csv file")

    args = parser.parse_args()

    app = args.app.expanduser().resolve()
    tracecsv = args.csv.expanduser().resolve()

    trace_hash_map = {}
    with open(tracecsv, mode="r") as file:
        csvfile = csv.reader(file, delimiter=";")
        for line in csvfile:
            trace_hash_map.update({line[0]: line[1]})

    process = subprocess.run(str(app), shell=True, capture_output=True, encoding="UTF-8")
    for line in process.stdout.splitlines():
        if len(line) >= 32:
            if line[:32] in trace_hash_map:
                splited = line.split(" ")
                line = trace_hash_map[line[:32]]
                if len(splited) > 1:
                    line = line.format(*[int(x, 16) for x in splited[1:]])
        print(line)


if __name__ == "__main__":
    main()
