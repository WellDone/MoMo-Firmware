import sys
import os
import readline
import shlex

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.utilities.typedargs import shell
from pymomo.exceptions import *
from pymomo.utilities.typedargs import annotate
from pymomo.commander.meta import initialization
from pymomo.hex import ControllerBlock, HexFile
from pymomo.sim.simulator import Simulator
from pymomo.utilities import build

builtins = ['help', 'back', 'quit']

@annotate.context("root")
class InitialContext(dict):
	pass

def run_momo():
	root = InitialContext()
	root.update(annotate.find_all(initialization))
	root.update(annotate.find_all(build))
	root['ControllerBlock'] = ControllerBlock
	root['HexFile'] = HexFile
	root['Simulator'] = Simulator

	line = sys.argv[1:]
	finished = False
	contexts = [root]

	try:
		while len(line) > 0:
			line, finished = shell.invoke(contexts, line)
			first_cmd = False
	except MoMoException as e:
		print e.format()

		#if the command passed on the command line fails, then we should
		#just exit rather than drop the user into a shell.
		sys.exit(1)

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

				#Catch exception outside the loop so we stop invoking submethods if a parent
				#fails because then the context and results would be unpredictable
				try:
					while len(line) > 0:
						line, finished = shell.invoke(contexts, line)
				except MoMoException as e:
					print e.format()

				if len(contexts) == 0:
					sys.exit(0)

		#Make sure to catch ^C and ^D so that we can cleanly dispose of subprocess resources if 
		#there are any.
		except EOFError:
			print ""
		except KeyboardInterrupt:
			print ""

if __name__ == "__main__":
	sys.exit(run_momo())
