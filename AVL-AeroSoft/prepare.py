import csv
from pathlib import Path

directory = Path(__file__).resolve().parent

with (directory / "airport-codes.csv").open(encoding="utf-8") as fin, \
     (directory / "airports.txt").open("w", encoding="utf-8") as fout:
    reader = csv.DictReader(fin)
    for row in reader:
        code = row["iata_code"].strip()
        name = row["name"].strip()
        if code:
            fout.write(f"{code}:{name}\n")
