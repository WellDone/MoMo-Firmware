#board.py

import csv
import itertools
from part import Part

class BOMEntry:
	def __init__(digikey_pn, refs):
		self.digikey_pn = digikey_pn
		self.refs = list(refs)
		self.count = len(refs)
		2
class Board:
	def __init__(self, file):
		parts = []
		with open(file, "r") as f:
			reader = csv.reader(f, delimiter=',',)
			next(reader, None) #skip header

			parts = [reader]

		#process parts
		self.parts = map(lambda x: Part(x, type='eagle_export'), parts)
		sorted_parts = sorted(self.parts, key=lambda x:x.digikey_pn)

		self._create_bom(sorted_parts)

	def _create_bom(self, sorted_parts):
		"""
		Create a BOMEntry item for each unique part in sorted_parts.
		Unique parts are determined by having unique digikey part numbers
		"""

		self.bom = []

		for k, g in itertools.groupby(sorted_parts, lambda x: x.digikey_pn)
			self.bom.append(BOMEntry(k, g))
