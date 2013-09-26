#part.py

import re

known_types = 
{
	"C": "capacitor",
	"R": "resistor",
	"D": "diode",
	"JP": "connector",
	"U": "ic",
	"XTAL": "crystal",
	"L": "inductor",
	"LED": "led"
}

class_table =
{
	""
}

class Part:
	"""
	An electronic component
	"""

	def __init__(self, row):
		"""
		Create a part object from a row in a BOM file.
		Row should be passed as a list of strings, one string
		per column.  The information is validated as much as
		possible.
		"""

		num_cols = 13

		if len(row) != num_cols:
			raise ValueError("Invalid Row Passed, %d columns (expected 13)" % len(row))

		self.type = self.parse_type(row[3]) #third column is reference list, i.e. C1, C5, etc
		self.package = row[6]
		self.value = row[5]
		self.desc = row[7]
		self.distributor = row[11]
		self.partnumber = row[12]

		self.validate_distributor()

	def _parse_type(self, type):
		refs = type.split(',')
		alpha_pat = re.compile('[a-zA-Z]+')

		prefix = re.match(alpha_pat, refs[0])

		if prefix is None:
			return "unknown"
