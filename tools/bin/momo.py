import sys
import os
import readline
import shlex

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.utilities.typedargs import shell
from pymomo.utilities.typedargs.exceptions import *
from pymomo.utilities.typedargs import annotate
from pymomo.commander.meta import initialization
from pymomo.hex.hexfile import HexFile
from pymomo.sim.simulator import Simulator
from pymomo.utilities import build
builtins = ['help', 'back', 'quit']

@annotate.context("root")
class InitialContext(dict):
	pass

root = InitialContext()
root.update(annotate.find_all(initialization))
root.update(annotate.find_all(build))
root['HexFile'] = HexFile
root['Simulator'] = Simulator

line = sys.argv[1:]
finished = False
contexts = [root]

try:
	while len(line) > 0:
		line, finished = shell.invoke(contexts, line)
except NotFoundError as e:
	print "NotFoundError: %s" % str(e)
except ValidationError as e:
	print "ValidationError: %s" % str(e)
except ArgumentError as e:
	print "ArgumentError: %s" % str(e)
except InternalError as e:
	print "InternalError: %s" % str(e)

#Setup file path and function name completion
import readline, glob
def complete(text, state):
	buf = readline.get_line_buffer()
	if buf.startswith('help ') or " " not in buf:
		funcs = annotate.find_all(contexts[-1]).keys() + builtins
		return filter(lambda x: x.startswith(text), funcs)[state]

	return (glob.glob(os.path.expanduser(text)+'*')+[None])[state]

readline.set_completer_delims(' \t\n;')
readline.parse_and_bind("tab: complete")
readline.set_completer(complete)

#If we ended the initial command with a context, start a shell
if not finished:
	try:
		while True:
			linebuf = raw_input("(%s) " % annotate.context_name(contexts[-1]))
			line = shlex.split(linebuf)

			try:
				while len(line) > 0:
						line, finished = shell.invoke(contexts, line)
			except NotFoundError as e:
				print "NotFoundError: %s" % str(e)
			except ValidationError as e:
				print "ValidationError: %s" % str(e)
			except ArgumentError as e:
				print "ArgumentError: %s" % str(e)
			except InternalError as e:
				print "InternalError: %s" % str(e)

			if len(contexts) == 0:
				sys.exit(0)

	except EOFError:
		print ""
