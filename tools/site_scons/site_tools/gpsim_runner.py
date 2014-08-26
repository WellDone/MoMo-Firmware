#gpsim_runner.py

import SCons.Builder
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))
from pymomo.utilities import config

settings = config.ConfigFile('settings')
gpsim_path = settings['external_tools/gpsim']

run_gpsim = SCons.Builder.Builder(
	action = '%s -c $SOURCE -i > /dev/null' % gpsim_path,
	)

def generate(env):
	env['BUILDERS']['gpsim_run'] = run_gpsim

def exists(env):
	return 1