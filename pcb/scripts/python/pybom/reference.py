#reference.py
#A manager for reference information used in creating circuit board BOMs
#and production files

import json
import os.path

data_filename = 'library.json'
data_file = os.path.join(os.path.dirname(__file__), 'resources', data_filename)

class PCBReferenceLibrary:
	def __init__(self):
		with open(data_file, "r") as f:
			self.lib = json.load(f)

		self._import_packages()

	def _import_packages(self):
		pkgs = self.lib['footprint_names']

		self.packages = {}

		for name,val in pkgs.iteritems():
			self.packages[name] = set(val)

	def find_package(self, pkg):
		"""
		Look for the footprint in our list of known packages.  Return a tuple
		of (package name, bool found).  
		"""
		for name, val in self.packages.iteritems():
			if pkg in val:
				return (name, True)

		return (pkg, False)