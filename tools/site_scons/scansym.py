#symscan.py
#Create a list of the symbols in an xc8 *.sym file

import sys

class XC8SymbolTable:
	def __init__(self, path):
		self._parse_symtab(path)
		self._calc_sizes()

	def _parse_symtab(self, path):
		print path
		lines = [line.strip() for line in open(path)]

		self.symtab = {}
		self.sects = {}
		self.known_functions = []

		for line in lines:
			if line == "%segments":	
				break

			fields = line.split(' ')

			if len(fields) != 5:
				raise ValueError("File did not have a valid format, too many fields in line:\n%s\n" % line)

			name = fields[0]
			addr = int(fields[1], 16)
			sect = fields[3]

			if name.startswith('__end_of'):
				self.known_functions.append((name[8:], addr))
				continue

			if name.startswith('__size_of'):
				continue

			self.symtab[name] = (addr, sect)

			#Keep track of which symbols are in which sections
			if sect not in self.sects:
				self.sects[sect] = [name]
			else:
				self.sects[sect].append(name)

	def _calc_sizes(self):
		"""
		Calculate the sizes of all known_functions: functions with a 
		symbol defining the end of the function, i.e. __end_of_foo
		
		Converts the known_functions list in 3-tuples with (name, starting_address, size)
		"""

		self.known_functions = map(lambda x: (x[0], self.symtab[x[0]][0], x[1] - self.symtab[x[0]][0]), self.known_functions)
		self.total_size = reduce(lambda x,y: x+y, map(lambda x: x[2], self.known_functions))


	def _gen_define(self, func):
		line = '#define %s_address 0x%x\n' % (func[0], func[1])

		return line

	def _gen_c_call(self, func):
		line = '#define c_call%s() asm("call 0x%x")\n' % (func[0], func[1])

		return line 

	def _gen_asm_call(self, func):
		line = '#define asm_call%s() call 0x%x\n' % (func[0], func[1])

		return line 

	def generate_h_file(self, path):
		defs = map(lambda x: self._gen_define(x), self.known_functions)
		c_calls = map(lambda x: self._gen_c_call(x), self.known_functions)
		asm_calls = map(lambda x: self._gen_asm_call(x), self.known_functions)

		with open(path, "w") as f:
			f.writelines(defs)
			f.writelines(c_calls)
			f.writelines(asm_calls)