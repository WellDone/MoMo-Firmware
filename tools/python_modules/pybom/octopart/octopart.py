#octopart.py
#Wrapper arround Octopart REST API

import os
import json
import urllib
import pprint
from physicalpart import PhysicalPart

class Octopart:
	URL = 'http://octopart.com/api/v3/'
	def __init__(self, key=None):
		if key is not None:
			self.api_key = key
		elif 'OCTOPART_KEY' in os.environ:
			self.api_key = os.environ['OCTOPART_KEY']
		else:
			raise ValueError('You need to either specify an Octopart API Key or have OCTOPART_KEY set in the shell')

	def _build_call(self, method, args):
		"""
		Build a URL call with arg=val pairs including the apikey to the url specifying the method
		given.
		"""

		url = Octopart.URL + method + '?'

		out_args = []
		out_args.extend(args)
		out_args.append(('apikey', self.api_key))
		out_strings = map(lambda x: urllib.quote(x[0])+'='+urllib.quote(x[1]), out_args)

		query_string = "&".join(out_strings)

		return url + query_string

	def _call(self, url):
		data = urllib.urlopen(url).read()
		response = json.loads(data)

		return response

	def _call_method(self, method, args):
		url = self._build_call(method, args)
		return self._call(url)

	def match_identifiers(self, skus):
		matched = {}
		unmatched = []
		for i in xrange(0, len(skus), 20):
			req = skus[i:i+20]
			queries = map(lambda x: x.build_query(), req)

			query = json.dumps(queries)

			resp = self._call_method('parts/match',[('queries',query)])

			for result in resp['results']:
				ref = result['reference']

				if len(result['items']) == 0:
					unmatched.append(ref)
					print "Did not match %s" % ref
					continue

				if len(result['items']) > 1:
					print "Found %d parts for %s" % (len(result['items']), ref)
				
				part = PhysicalPart(result['items'][0])

				matched[ref] = part 

		return matched