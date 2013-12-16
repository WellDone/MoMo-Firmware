#utilities.py

from SCons.Script import *
from SCons.Environment import Environment
import os
import fnmatch
import json as json
import mib12_config

def find_files(dirname, pattern):
	"""
	Recursively find all files matching pattern under path dirname
	"""

	matches = []
	for root, dirnames, filenames in os.walk(dirname, followlinks=True):
		print dirnames, filenames
		for filename in fnmatch.filter(filenames, pattern):
			matches.append(os.path.join(root,filename))

	return matches

def load_settings():
	directory = os.path.dirname(__file__)
	filename = os.path.join(directory,'..','..','config','build_settings.json')

	with open(filename,'r') as f:
		return json.load(f)

	ValueError('Could not load global build settings file (config/build_settings.json)')

def build_app_for_chip(name, chip):
	"""
	Configure Scons to build an application module for the 8bit pic microchip indicated in the argument chip.
	"""

	mib12conf = MIB12Config()
	dirs = mib12conf.build_dirs(chip)

	builddir = dirs['build']
	VariantDir(builddir, 'src', duplicate=0)

	env = Environment(tools=['xc8_compiler', 'patch_mib12', 'xc8_symbols'], ENV = os.environ)
	env.AppendENVPath('PATH','../../tools/scripts')
	
	#Load in all of the xc8 configuration from build_settings
	mib12conf.config_env_for_app(env, chip)
	Export('env')
	SConscript(os.path.join(builddir, 'SConscript'))

	prods = [os.path.join(dirs['build'], 'mib12_app_module.hex'), os.path.join(dirs['build'], 'mib12_app_module_symbols.h'), os.path.join(dirs['build'], 'mib12_app_module_symbols.stb')]

	hexfile = env.InstallAs(os.path.join(dirs['output'], '%s_%s.hex' % (name, chip)), prods[0])
	symheader = env.InstallAs(os.path.join(dirs['output'], '%s_symbols_%s.h' % (name, chip)), prods[1])
	symtable = env.InstallAs(os.path.join(dirs['output'], '%s_symbols_%s.stb' % (name, chip)), prods[2])


def for_all_targets(module, func):
	targets = get_module_targets(module)

	for target in targets:
		func(target)

def get_module_targets(module):
	conf = MIB12Config()
	targets = conf.get_targets(module)

	return targets

class MIB12Config:
	def __init__(self):
		conf = load_settings()

		self.common_flags = conf['mib12']['common_xc8_flags']
		self.exec_flags = conf['mib12']['exec_xc8_flags']
		self.app_flags = conf['mib12']['app_xc8_flags']
		self.common_incs = conf['mib12']['includes']
		self.targets = conf['mib12']['known_targets']
		self.mod_targets = conf['mib12']['module_targets']

		self.common_incs = map(lambda x: os.path.normpath(x), self.common_incs); #Change path separator on Windows if necessary

		self.conf = conf

		#Load chip information
		self.chip_names = {}
		self.chip_info = {}
		
		for key in self.targets:
			self.chip_names[key] = set([key])
			self._load_chip_info(key)

	def _load_chip_info(self, chip):
		"""
		Load chip info from chip_settings dictionary in build_settings.json
		"""

		settings = self.conf['mib12']['chip_settings'][chip]

		if 'aliases' in settings:
			self.chip_names[chip].update(settings['aliases'])

		default = self.conf['mib12']['default_settings'].copy()

		self.chip_info[chip] = merge_dicts(default, settings)

	def chip_def(self, chip, val, default=None):
		"""
		Get the value of the given property for the specified chip
		"""

		if val in self.chip_info[chip]:
			return self.chip_info[chip][val]	
		
		if default is not None:
			return default

		raise ValueError("property %s not found for chip %s" % (val, chip))

	def add_common_incs(self, env):
		"""
		Add common include directories for 8 bit pics from build_settings.json
		"""
		if 'INCLUDE' not in env:
			env['INCLUDE'] = []

		env['INCLUDE'] += self.common_incs

	def config_env_for_exec(self, env, name):
		"""
		Configure the SCons build environment for building the MIB12 Executive
		"""

		chip = self.get_chip_name(name)

		self._ensure_flags(env)

		exec_range = self.chip_def(chip, 'executive_rom')

		env['ROMSTART'] = exec_range[0]
		env['ROMEND'] = exec_range[1]

		env['XC8FLAGS'] += self.common_flags
		env['XC8FLAGS'] += self.exec_flags

		self.config_env_for_chip(chip, env)
		self.add_common_incs(env)

		env['RAMEXCLUDE'] = [self.chip_def(chip, 'application_ram')]

	def config_env_for_app(self, env, name):
		"""
		Configure the SCons build environment for building a MIB12 application module
		"""

		chip = self.get_chip_name(name)

		self._ensure_flags(env)
		self.config_env_for_chip(chip, env)

		exec_range = self.chip_def(chip, 'executive_rom')

		env['ROMSTART'] = exec_range[1]+1
		env['ROMEND'] = self.chip_def(chip, 'total_rom') - 16 - 1 #Make sure we don't let the code overlap with the MIB map in high memory

		env['XC8FLAGS'] += self.common_flags
		env['XC8FLAGS'] += self.app_flags
		env['XC8FLAGS'] += ['-L-preset_vec=%xh' % (exec_range[1]+1)]

		self.add_common_incs(env)

		#MIB12 Executive takes all ram in first 
		env['RAMEXCLUDE'] = self.chip_def(chip, 'executive_ram')
		env['NO_STARTUP'] = True

	def config_env_for_chip(self, chip, env):
		"""
		Configure SCons environment to build for a specific 8-bit chip
		"""

		info = self.get_chip(chip)

		try:
			env['CHIPINFO'] = info
			env['CHIPNAME'] = info.name
			env['CHIP'] = self.chip_def(chip, 'xc8_target')
			env['CHIPDEFINE'] = self.chip_def(chip, 'xc8_define')
			env['XC8FLAGS'] += ['-DkFirstApplicationRow=%d' % info.first_app_page]
			env['MIB_API_BASE'] = str(info.api_range[0])
		except KeyError:
			raise ValueError("Chip %s not found in build_settings.json, cannot target that chip." % chip)

	def get_chip_name(self, chip):
		found = None
		for key,vals in self.chip_names.iteritems():
			if chip in vals:
				return key

		raise ValueError('Chip %s not found in build_settings, do not know about this chip target' % chip)

	def get_chip(self, chip):
		"""
		Get a MIB12Processor instance containing all of the information about this
		chip.
		"""

		name = self.get_chip_name(chip)
		return mib12_config.MIB12Processor(name, self.chip_info[name])

	def get_targets(self, module):
		"""
		Return a list of the targest that this module should be built for
		"""
		if module in self.mod_targets:
			return self.mod_targets[module]

		return self.targets

	def build_dirs(self, chip):
		"""
		Return a dictionary with the cannonical build directory hierarchy for a given chip target.
		Defines:
		- build: build/chip
		- output: build/output
		- test: build/test/chip
		where chip is the cannonical name for the chip passed in
		"""

		name = self.get_chip_name(chip)

		build = os.path.join('build', name)
		output = os.path.join('build', 'output')
		test = os.path.join('build', 'test', name)

		return {'build': build, 'output': output, 'test': test}

	def output_dir(self):
		return os.path.join('build', 'output')

	def _ensure_flags(self, env):
		if "XC8FLAGS" not in env:
			env['XC8FLAGS'] = []


def merge_dicts(a, b):
    "merges b into a"

    for key in b:
        if key in a:
            if isinstance(a[key], dict) and isinstance(b[key], dict):
                merge_dicts(a[key], b[key])
            else:
            	a[key] = b[key]
        else:
            a[key] = b[key]
    
    return a