#descriptor.py
#Define a Domain Specific Language for specifying MIB endpoints

from pyparsing import Word, Regex, nums, Literal, Optional, Group
from handler import MIBHandler

#DSL for mib definitions
#Format:
#feature <i>
#[j:] _symbol(n [ints], yes|no)
#[j+1:] _symbol2(n [ints], yes|no)
#The file should contain a list of statements beginning with a feature definition
#and followed by 1 or more command definitions.  There may be up to 128 feature definitions
#each or which may have up to 128 command definitions.  Whitespace is ignored.  Lines starting
#with # are considered comments and ignored.
number = Word(nums).setParseAction(lambda s,l,t: [int(t[0])])
symbol = Regex('[_a-zA-Z][_a-zA-Z0-9]*')
ints = number('num_ints') + Optional(Literal('ints') | Literal('int'))
has_buffer = (Literal('yes') | Literal('no')).setParseAction(lambda s,l,t: [t[0] == 'yes'])
comma = Literal(',').suppress()
left = Literal('(').suppress()
right = Literal(')').suppress()
colon = Literal(':').suppress()
comment = Literal('#')

cmd_def = Optional(number("cmd_number") + colon) + symbol("symbol") + left + ints + comma + has_buffer('has_buffer') + right + ";"
feat_def = (Literal("feature") | Literal("Feature")) + number("feature_number")

statement = cmd_def | feat_def | comment



class MIBDescriptor:
	"""
	Class that parses a .mib file which contains a DSL specifying the valid MIB endpoints
	in a MIB12 module and can output an asm file containing the proper MIB command map
	for that architecture.
	"""
	def __init__(self, filename):
		self.curr_feature = -1
		self.curr_cmd = -1
		self.features = {}

		with open(filename, "r") as f:
			for l in f:
				line = l.lstrip().rstrip()

				if line == "":
					continue

				self._parse_line(line)

	def _add_feature(self, feature):
		self.features[feature] = []
		self.curr_feature = feature
		self.curr_cmd = 0

	def _add_cmd(self, symbol, num_ints, has_buffer):
		handler = MIBHandler.Create(symbol=symbol, ints=num_ints, has_buffer=has_buffer)
		self.features[self.curr_feature].append(handler)
		self.curr_cmd += 1

	def _parse_cmd(self, match):
		symbol = match['symbol']

		if self.curr_feature < 0:
			raise ValueError("MIB Command specified without first declaring a feature.")

		if 'cmd_number' in match:
			num = match['cmd_number']

			if num != self.curr_cmd:
				raise ValueError("Nonsequential command number (feature %d, command %d).  Command number should have been %d" % (self.curr_feature, num, self.curr_cmd))

		has_buffer = match['has_buffer']
		num_ints = match['num_ints']

		self._add_cmd(symbol, has_buffer, num_ints)

	def _parse_line(self, line):
		matched = statement.parseString(line)

		if 'feature_number' in matched:
			self._add_feature(matched['feature_number'])
		elif 'symbol' in matched:
			print line
			self._parse_cmd(matched)


import sys

d = MIBDescriptor(sys.argv[1])
print d.features