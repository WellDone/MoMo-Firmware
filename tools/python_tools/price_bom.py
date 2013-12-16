import csv
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'python_modules'))
import pybom.octopart.octopart
from pybom.octopart.identifier import PartIdentifier

if len(sys.argv) < 4:
	print "Usage: price_bom.py <csv file> <out file> <units>"
	print "Given a list of quantity, digikey part numbers lines,"
	print "return the BOM price when purchasing in a quantity of units"
	sys.exit(0)

parts = []
mult = float(sys.argv[3])

with open(sys.argv[1], "r") as f:
	reader = csv.reader(f)

	next(reader, None)

	for row in reader:
		if row[1] == '':
			continue

		parts.append((PartIdentifier(digipn=row[1]), int(row[0])))

op = pybom.octopart.octopart.Octopart()

matched = op.match_identifiers(map(lambda x: x[0], parts))

priced = []

for part,quant in parts:
	if part.build_reference() in matched:
		q = int(quant*mult)
		best = matched[part.build_reference()].best_price(q)

		price = best[0]
		seller = best[1]
		pkg = best[2]

		priced.append((part.sku, str(q), str(price), str(q*price), seller, pkg))
		print part.sku, q, price, str(q*price), seller, pkg 

with open(sys.argv[2],"w") as f:
	writer = csv.writer(f)

	for price in priced:
		writer.writerow(price)
