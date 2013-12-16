#!/usr/bin/python

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'python_modules'))

import argparse
import pybom.board
import itertools
import math

parser = argparse.ArgumentParser(description='Process EAGLE board files to produce production files for fabrication, costing and assembly.')

#Configure Command Line Arguments
parser.add_argument('-b', '--bom', action='store', help='Create BOM', metavar='<csv file>')
parser.add_argument('-d', '--digikey', action='store', help='Create Digikey Order File', metavar='<csv file>')
parser.add_argument('-e', '--excess', action='store', help='Excess parts to purchase (in %%)', metavar='PERCENTAGE')
parser.add_argument('-v', '--variant', action='append', help='Assembly variant to use in each board file', metavar='VARIANT')
parser.add_argument('-q', '--quantity', action='store', type=int, default=1, help='Number of units to plan for', metavar='N')
parser.add_argument('boards', nargs='+', help='EAGLE brd files to process', metavar='BOARD')


args = parser.parse_args()

def create_order(board, output, excess):
	pass


###Main Logic###
boards = map(lambda x: pybom.board.Board.FromEagle(x), args.boards)
order_rat = 1.0

print "Done loading boards."

if hasattr(args, 'excess'):
	order_rat = 1.0 + float(args.excess.strip('%'))/100.0
	print "Ordering %.0f%% of everything!" % (order_rat*100)

if args.variant is None:
	variants = ['MAIN'] * len(args.boards)
else:
	variants = args.variant

if len(variants) != len(args.boards):
	print "You must specify a variant for all boards or none to prevent ambiguity"
	sys.exit(1)

#Create Digikey order if we're instructed to
if hasattr(args, 'digikey'):
	parts = []

	for i in xrange(0,len(boards)):
		brdparts = boards[i].export_list(variants[i])
		parts += brdparts

	parts.sort()

	order = [(key, len(list(group))) for key, group in itertools.groupby(parts)]

	final_order = map(lambda x: (x[0], math.ceil(args.quantity*x[1]*order_rat)), order)

	with open(args.digikey, "w") as f:
		lines = map(lambda x: str(int(x[1])) + ',' + x[0] + '\n', final_order)
		f.writelines(lines)

