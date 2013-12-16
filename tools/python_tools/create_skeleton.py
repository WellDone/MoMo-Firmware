#create_skeleton.py

import argparse
import sys
import os.path

sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'python_modules'))
import momo_utilities.template
from momo_utilities.paths import MomoPaths

typemap = {
	'mib12': 'mib12_module'
}


parser = argparse.ArgumentParser(description='Create a skeleton code for a MoMo module (pic12 or pic24)')

parser.add_argument('type', choices=['mib12'], help='The type of module to generate')
parser.add_argument('name', help='The name of module')

args = parser.parse_args()

t = args.type
name = args.name

fillvars = {
	'module' : {
		'name': name
	}
}

if t == 'mib12':
	template = momo_utilities.template.RecursiveTemplate('mib12_module', rename=name)
else:
	print "Invalid type"
	sys.exit(1)

template.add(fillvars)

template.render(output_dir=MomoPaths().modules)