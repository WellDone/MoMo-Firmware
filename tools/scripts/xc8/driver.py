#Scripts to control xc8 executable

import os.path
import subprocess

#XC8 include directories (relative to momo_modules)
incdirs = [
	os.path.join('..', 'shared', 'pic12', 'src'),
	os.path.join('..', 'shared', 'portable', 'core'),
	os.path.join('..', 'shared', 'portable', 'core'),
	os.path.join('..', 'shared', 'portable', 'mib'),
	os.path.join('..', 'shared', 'portable', 'mib', 'master'),
	os.path.join('..', 'shared', 'portable', 'mib', 'slave'),
	os.path.join('..', 'shared', 'portable', 'util'),
	os.path.join('src'),
]

class XC8:
	pic12lf1822 = ('12F1822', '__PIC12LF1822__')
	pic16lf1823 = ('16F1823', '__PIC16LF1823__')

	def __init__(self):
		self.asm_files = []
		self.c_files = []

		self.options = []
		self.output = None
		self.proc = None
		self.builddir = None

	def add_option(self,opt):
		self.options.append(opt)

	def set_builddir(self, build):
		self.builddir = build

	def set_output(self, outfile):
		self.output = outfile

	def set_proc(self, proc):
		self.proc = proc

	def call(self):
		args = self._gen_cmd()

		try:
			output = subprocess.check_output(args, stderr=subprocess.STDOUT)
		except subprocess.CalledProcessError, e:
			print("Compilation Error")
			print(e.output)
			return

		#Everything was fine
		print(output)

	def _gen_define(self, define):
		return ['-D%s' % define]

	def _gen_proc(self):
		if self.proc is None:
			raise ValueError('No processor specified')

		return ['--chip=%s' % self.proc[0]] + self._gen_define(self.proc[1])

	def _gen_cfiles(self):
		return self.c_files

	def _gen_asfiles(self):
		return self.asm_files

	def _gen_outfile(self):
		if self.output is None or self.builddir is None:
			raise ValueError('No output file or directory specified')

		return ['-o%s' % os.path.join(self.builddir, self.output)]

	def _gen_includes(self):
		return map(lambda x: '-I{0}'.format(x), incdirs)

	def _gen_options(self):
		return self.options

	def _gen_cmd(self):
		args = ['xc8']
		args += self._gen_proc() 
		args += self._gen_cfiles()
		args += self._gen_asfiles() 
		args += self._gen_outfile()
		args += self._gen_includes()
		args += self._gen_options()

		print("Generated Command Line")
		print(args)

		return args

xc8 = XC8()
xc8.c_files.append('src/i2c.c')
xc8.c_files.append('src/mib_state.c')
xc8.c_files.append('../support/test_8bit/harness/test_main.c')
xc8.c_files.append('../support/test_8bit/harness/test_log.as')
xc8.set_proc(XC8.pic12lf1822)
xc8.set_output('out.hex')
xc8.set_builddir('test')
xc8.add_option('-P')
xc8.add_option('-V')

xc8.call()