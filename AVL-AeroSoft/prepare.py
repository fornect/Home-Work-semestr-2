import csv

with open("/home/fornect/Home-Work-semestr-2/AVL-AeroSoft/airport-codes.csv", encoding="utf-8") as fin, \
     open("/home/fornect/Home-Work-semestr-2/AVL-AeroSoft/airports.txt", "w", encoding="utf-8") as fout:
    reader = csv.DictReader(fin)
    for row in reader:
        code = row["iata_code"].strip()
        name = row["name"].strip()
        if code:
            fout.write(f"{code}:{name}\n")
