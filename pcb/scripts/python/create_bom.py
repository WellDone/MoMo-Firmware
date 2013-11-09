#create_bom.py

from pybom.board import Board
import collections
from math import ceil
import sys
import csv

cfile = '/home/timburke/src/welldone/MoMo-Firmware/pcb/momo_modular/combined_controller/battery_controller.brd'
#gfile = '/Users/timburke/Projects/MoMo-Firmware(personal)/pcb/momo_modular/gsm_communication/gsm_module.brd'
#sfile = '/Users/timburke/Projects/MoMo-Firmware(personal)/pcb/momo_modular/sensor_dev_board/sensor dev board.brd'

cboard = Board.FromEagle(cfile)
#cboard.lookup_parts()
#gboard = Board.FromEagle(gfile)
#sboard  = Board.FromEagle(sfile)

cbom_ext = cboard.export_bom('EXTPOWER','/home/timburke/test_external_power.csv', include_costs=False, cost_quantity=5)
#cbom_bat = cboard.export_bom('BATT','/Users/timburke/Dropbox/Shared - Other People/MoMo Core Hardware/combined_controller/battery_power.csv')
#cbom_sol = cboard.export_bom('SOLAR','/Users/timburke/Dropbox/Shared - Other People/MoMo Core Hardware/combined_controller/solar.csv')
#gbom = gboard.export_bom('MAIN', '/Users/timburke/Dropbox/Shared - Other People/MoMo Core Hardware/gsm_module/gsm_module.csv')
#sbom = sboard.export_bom('MAIN', '/Users/timburke/Dropbox/Shared - Other People/MoMo Core Hardware/sensor_dev_board/sensor_dev_board.csv')