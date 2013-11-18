#create_bom.py

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'python_modules'))

import momo_utilities.config
from pybom.board import Board
import collections
from math import ceil
import csv

cfile = '/home/timburke/src/welldone/MoMo-Firmware/pcb/momo_modular/combined_controller/battery_controller.brd'
gfile = '/home/timburke/src/welldone/MoMo-Firmware/pcb/momo_modular/gsm_communication/gsm_module.brd'
#sfile = '/Users/timburke/Projects/MoMo-Firmware(personal)/pcb/momo_modular/sensor_dev_board/sensor dev board.brd'

cboard = Board.FromEagle(cfile)
#cboard.lookup_parts()
gboard = Board.FromEagle(gfile)
#sboard  = Board.FromEagle(sfile)

#cbom_ext = cboard.export_bom('EXTPOWER','/home/timburke/test_external_power.csv', include_costs=True, cost_quantity=100)
#cbom_bat = cboard.export_bom('BATT','/Users/timburke/Dropbox/Shared - Other People/MoMo Core Hardware/combined_controller/battery_power.csv')
#cbom_sol = cboard.export_bom('SOLAR','/Users/timburke/Dropbox/Shared - Other People/MoMo Core Hardware/combined_controller/solar.csv')
gbom = gboard.export_bom('MAIN', '/home/timburke/gsm_module.csv', include_costs=True, cost_quantity=100)
#sbom = sboard.export_bom('MAIN', '/Users/timburke/Dropbox/Shared - Other People/MoMo Core Hardware/sensor_dev_board/sensor_dev_board.csv')