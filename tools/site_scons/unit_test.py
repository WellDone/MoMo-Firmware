#unit_test.py

import os.path
import os
from SCons.Environment import Environment
import test_summary
import fnmatch
from pymomo.utilities.typedargs.exceptions import *

known_types = {}

class UnitTest:
	def __init__(self, files):
		self.files = files
		self.additional_sources = []
		self.extra_modules = []
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
			print "Targets: All module targets"
		else:
			print "Targets:", ", ".join(self.targets)
		print "Status:", self.status
		print self.desc

	def build_dirs(self, chip):
		"""
		Return the appropriate build directories for this unit test.  The build hierarchy is:
		- test: build/test/<CHIP>/<TEST>
		- objects: build/test/<CHIP>/<TEST>/objects
		"""

		basedirs = chip.build_dirs()

		testdir = os.path.join(basedirs['test'], self.name, 'objects')
		outdir = os.path.join(basedirs['test'], self.name)
		finaldir = os.path.join('build', 'test', 'output')
		logdir = os.path.join(finaldir, 'logs')

		return {'test':outdir, 'objects': testdir, 'logs': logdir}

	def get_path(self, obj, chip):
		"""
		Get the canonical path for various build products of the unit test.  Currently understands:
		- rawlog: the raw output from the simulator run
		- log: the processed version of rawlog into a standard format
		"""

		dirs = self.build_dirs(chip)

		if obj == 'rawlog':
			name = self.name + '@' + chip.arch_name() + '.raw'
			return os.path.join(dirs['objects'], name)
		elif obj == 'log':
			name = self.name + '@' + chip.arch_name() + '.log'
			return os.path.join(dirs['logs'], name)
		elif obj == 'status':
			name = self.name + '@' + chip.arch_name() + '.status'
			return os.path.join(dirs['logs'], name)
		elif obj == 'elf':
			name = self.name + '.elf'
			return os.path.join(dirs['objects'], name)
		else:
			raise BuildError("Path to unknown build product asked for", unit_test=self.name, object=obj, arch=chip.name)


	def build_target(self, target, summary_env):
		raise ValueError('The build_target method must be overriden by a UnitTest subclass')

	def build(self, module_targets, summary_env):
		"""
		Build this unit test for the intersection of the targets that it is designed for
		and the targets that the module it is targeted at is designed for.
		"""

		for mod_target in module_targets:
			found = False
			if self.targets is not None:
				for target in self.targets:
					target_archs = set(target.split('/'))
					if target_archs <= mod_target.archs():
						found = True
						break

				if found is False:
					continue

			self.build_target(mod_target, summary_env)

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
					self._parse_additional(val)
				elif name == 'sources':
					self._parse_sources(val)
				elif name == 'type':
					self.type = val
				elif name == 'modules':
					self._parse_modules(val)

		#Make sure that all of the right information has been found
		required_attributes = ['name', 'type']
		for attr in required_attributes:
			if not hasattr(self, attr):
				raise BuildError("test does not have a complete header", file=file, missing_attribute=attr)

	def _parse_target(self, value):
		return value

	def _parse_sources(self, value):
		"""
		Parse an additional source directory other than simply src
		"""

		basedir = os.path.dirname(self.files[0])
		srcpath = os.path.normpath(os.path.join(basedir, value))
		self.additional_sources.append(srcpath)

	def _parse_modules(self, value):
		"""
		For module unit tests where the test just compiles one or several modules, allow
		the user to specify extra modules that need to be compiled in.
		"""

		parsed = value.split(',')
		files = map(lambda x: x.rstrip().lstrip(), parsed)
		self.extra_modules = files

	def _parse_additional(self, value):
		"""
		Parse an Additional:<file1>,<fileN>
		header line, taking the appropriate action
		"""

		parsed = value.split(',')
		files = map(lambda x: x.rstrip().lstrip(), parsed)

		for f in files:
			base, ext = os.path.splitext(f)

			if ext == '.as':
				self.files += (os.path.join(self.basedir, f), )
			elif ext == '.cmd':
				self.cmdfile = os.path.join(self.basedir, f)
			else:
				raise ValueError("Unknown file extension in Additional header for test %s: %s" % (self.name, f))

def find_units(parent):
	files = [f for f in os.listdir(parent) if os.path.isfile(os.path.join(parent, f))]

	#ignore hidden files
	files = filter(lambda x: x[0] != '.', files)

	#ignore files that start with support_
	files = filter(lambda x: not x.startswith("support_"), files)

	files = [os.path.join(parent, f) for f in files]

	tests = []

	for f in files:
		unit = UnitTest([f])
		
		if unit.type not in known_types:
			raise InternalError("Unknown test type (%s) in unit test: %s" % (unit.type, f))

		tests.append(known_types[unit.type]([f]))

	return tests

def find_sources(src_dir, patterns=('*.c', '*.as', '*.asm')):
	"""
	Given a source directory, recursively find all source and header files under that directory
	"""

	include_dirs = set()
	src = {}
	headers = {}

	for root, dirnames, filenames in os.walk(src_dir):
		dir_headers = fnmatch.filter(filenames, '*.h')
		if len(dir_headers) > 0:
			include_dirs.add(root)

		headers.update({os.path.splitext(x)[0]: os.path.join(root, x) for x in dir_headers})

		for s in patterns:
			dir_srcs = fnmatch.filter(filenames, s)
			src.update({os.path.splitext(x)[0]: os.path.join(root, x) for x in dir_srcs})

	return include_dirs, src, headers


def build_units(parent, targets):
	tests = find_units(parent)

	summary_env = Environment()
	summary_env['TESTS'] = []

	for test in tests:
		test.build(targets, summary_env)

	build_summary(summary_env)

def build_summary_name():
	return os.path.join('build', 'test', 'output', 'results.txt')

def build_summary(env):
	env.Command([build_summary_name()], env['TESTS'], action=env.Action(test_summary.build_summary_cmd, "Creating test summary"))
