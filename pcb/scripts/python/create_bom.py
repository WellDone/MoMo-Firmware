#create_bom.py

from pybom.board import Board
import collections
from math import ceil
import sys
import csv

cfile = '/Users/timburke/Projects/MoMo-Firmware/pcb/momo_modular/combined_controller/battery_controller.brd'
gfile = '/Users/timburke/Projects/MoMo-Firmware/pcb/momo_modular/gsm_communication/gsm_module.brd'
sfile = '/Users/timburke/Projects/MoMo-Firmware/pcb/momo_modular/sensor_dev_board/sensor dev board.brd'

cboard = Board.FromEagle(cfile)
gboard = Board.FromEagle(gfile)
sboard  = Board.FromEagle(sfile)

cbom_ext = cboard.export_list('EXTPOWER')
cbom_bat = cboard.export_list('BATT')
cbom_sol = cboard.export_list('SOLAR')
gbom = gboard.export_list('MAIN')
sbom = sboard.export_list('MAIN')

gbom *= 5
sbom *= 5
cbom_ext *= 2
cbom_bat *= 2
cbom_sol *= 2

total = gbom + sbom + cbom_ext + cbom_sol + cbom_bat

order = collections.Counter(total)

order = [(k,v) for (k,v) in order.iteritems()]

order = map(lambda x: (x[0], int(ceil(x[1]*1.1))), order)

for line in order:
	print "%d, %s" % (line[1],line[0])