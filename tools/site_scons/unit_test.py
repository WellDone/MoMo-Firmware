#unit_test.py

import os.path
import os

class UnitTest:
	def __init__(self, files):
		self.files = files
		self.desc = ''
		self.targets = None

		self.basedir = os.path.dirname(files[0])

		self._check_files()
		self._extract_header(files[0])
		self._check_files()

		self.status = "Unknown"
	
	def show(self):
		print "#Unit Test#"
		print self.name

		if self.targets is None:
			print "Targets: All"
		else:
			print "Targets:", ", ".join(self.targets)
		print "Status:", self.status
		print self.desc

	def build_target(self, target):
		raise ValueError('The build_target method must be overriden by a UnitTest subclass')

	def build(self, module_targets):
		"""
		Build this unit test for the intersection of the targets that it is designed for
		and the targets that the module it is targeted at is designed for.
		"""

		mod_targets = set(module_targets)
		targets = set()

		if self.targets is None:
			targets = mod_targets
		else:
			for target in self.targets:
				if target in mod_targets:
					targets.add(target)

		for target in targets:
			self.build_target(target)

	def _check_files(self):
		"""
		All files should be c or assembly files (.c or .as) and should
		exist.
		"""

		for file in self.files:
			valid = set (['.c', '.as', '.asm'])
			ext = os.path.splitext(file)[1]

			if not os.path.exists(file):
				raise ValueError('Test source file does not exist: %s' % file)
			if ext not in valid:
				raise ValueError('Test source file does not has .c,.as or .asm extension: %s' % os.path.basename(file))

		return True

	def _parse_targets(self, targets):
		"""
		Parse the targets specified for this unit test into canonical format
		allowing subclasses to handle targets specified for them by overriding
		the _parse_target(self, target) method to return the canonical name
		for the target
		"""

		if targets.lower() == 'all':
			self.targets = None
			return

		targets = targets.split(',')

		canonical_targets = []

		for target in targets:
			t = target.strip()
			canonical_targets.append(self._parse_target(t))

		self.targets = canonical_targets

	def _extract_header(self, file):
		"""
		Process the first commented block in the file and extract the assignments
		from it, parsing out the name, targets, and description
		"""

		header = []
		with open(file, "r") as f:
			for line in f:
				line = line.strip()

				#ignore blank lines
				if line == '':
					continue

				if line[0] == ';':
					header.append(line[1:])
				elif line[0:2] == '//':
					header.append(line[2:])
				else:
					break

		for line in header:
			v = line.split(':')

			#Undelimited lines after the description are assumed
			#to be continuations of the description
			if len(v) < 2:
				if hasattr(self, 'desc'):
					self.desc += v[0]
			elif len(v) == 2:
				name = v[0]
				val = v[1]
				name = name.strip()
				val = val.strip()

				name = name.lower()
				if name == 'name':
					self.name = val
				elif name == 'targets':
					self._parse_targets(val)
				elif name == 'description':
					self.desc = val
				elif name == 'additional':
					self.files += (os.path.join(self.basedir, val), )
				elif name == 'type':
					self.type = val

def find_units(parent, subclass):
	files = [f for f in os.listdir(parent) if os.path.isfile(os.path.join(parent, f))]

	#ignore hidden files
	files = filter(lambda x: x[0] != '.', files)

	#ignore files that start with support_
	files = filter(lambda x: not x.startswith("support_"), files)

	files = [os.path.join(parent, f) for f in files]


	tests = []

	for f in files:
		tests.append(subclass([f]))

	return tests

def build_units(parent, targets, subclass):
	tests = find_units(parent, subclass)

	for test in tests:
		test.build(targets)