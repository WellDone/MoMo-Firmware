#!/usr/bin/env python
#pcbtool
#A program for creating BOMs, gerbers and assembly drawings from annotated EAGLE circuit board diagrams.

import sys
import os.path

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.sim import Simulator
from pymomo.exceptions import *

def main():
	if len(sys.argv) != 4:
		print "Usage: picunit (pic24super|pic24fpsuper) ELF_FILE OUTPUT_LOG"
		return 1

	model = sys.argv[1]
	elf = sys.argv[2]
	log = sys.argv[3]

	try:
		sim = Simulator('pic24')
		sim.set_param('model', model)
		sim.attach_log()
		sim.load(elf)
		sim.execute()
		sim.wait(2)
		logdata = sim.get_log()
	except MoMoException as e:
		print e.format()
		return 1

	with open(log, "w") as f:
		f.write(logdata)

	return 0

if __name__ == "__main__":
	sys.exit(main())