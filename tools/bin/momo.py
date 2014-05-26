import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.utilities.typedargs import shell
from pymomo.utilities.typedargs.exceptions import *
from pymomo.utilities.typedargs import annotate
from pymomo.commander.meta import initialization

context = annotate.find_all(initialization)
line = sys.argv[1:]

try:
	while len(line) > 0:
		context, line = shell.invoke(context, line)
except NotFoundError as e:
	print "NotFoundError: %s" % str(e)
except ValidationError as e:
	print "ValidationError: %s" % str(e)