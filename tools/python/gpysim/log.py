#log.py
#Parse the log file from a gpsim test run and convert it to a series of events

import sys
import re
import statements
from statements.unknown import UnknownStatement

class LogFile:	

	def __init__(self, filename, symtab=None):
		with open(filename, "r") as f:
			lines = f.readlines()

		self.entries = []

		#Only keep entries where we write to the logging registers
		lines = filter(lambda x: not x.startswith('  Read:'), lines)
		lines = filter(lambda x: not x.startswith('  BREAK:'), lines)

		if len(lines) % 2 != 0:
			raise ValueError("File length is invalid, filtered entries should be a multiple of 2, len=%d." % len(lines))

		entries = zip(lines[0::2], lines[1::2])
		entries = map(lambda x: (x[0]+x[1]).rstrip(), entries)
		entries = map(lambda x: re.split('\W+', x), entries)
		info = map(extract_info, entries)
		info = info[:-1] #chomp last entry which is always a duplicate of test ended

		#find control entries
		controls = filter(lambda x: x[1]['dest'] == 'ccpr1l', enumerate(info))
		controls = map(lambda x: x[0], controls)
		lengths = [x-controls[i] for i,x in enumerate(controls[1:])]

		lengths.append(1) #final control statement has no data entries

		if len(lengths) != len(controls):
			raise ValueError("Logic error in log file handling, control statements were not matched with data statements")

		#at this point we have the indices corresponding to all control statements
		#and their lengths build the statements

		#initialize program counter
		self.current_address = 0

		statements = [{'control': info[c], 'data':info[c+1:c+lengths[i]]} for i,c in enumerate(controls)]
		entries = map(lambda x:self._process_statement(x), statements)
		
		self.entries = filter(lambda x:x.keep() == True, entries)

	def _process_statement(self, statement):
		if statement['control']['value'] in statements.statements:
			return statements.statements[statement['control']['value']](statement, self)

		return UnknownStatement(statement, self)

	def pretty_print(self):
	    HEADER = '\033[95m'
	    OKBLUE = '\033[94m'
	    OKGREEN = '\033[92m'
	    WARNING = '\033[93m'
	    FAIL = '\033[91m'
	    ENDC = '\033[0m'

	    for entry in self.entries:
	    	if entry.error():
	    		print FAIL + entry.format() + ENDC
	    	else:
	    		print OKBLUE + entry.format() + ENDC
def extract_info(entry):
	info = {}

	if len(entry) != 13:
		raise ValueError("Invalid entry had illegal length %d" % len(entry))

	info['cycle'] = int(entry[0],0)
	info['proc'] = entry[1]
	info['value'] = int(entry[7],0)
	info['dest'] = entry[9]

	return info
	

log = LogFile(sys.argv[1], symtab=sys.argv[2])
log.pretty_print()