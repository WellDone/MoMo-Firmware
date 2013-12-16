
import os.path

class MomoPaths:
	def __init__(self):
		self.base = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..' ))

		self.config = os.path.join(self.base, 'config')
		self.modules = os.path.join(self.base, 'momo_modules')
		self.templates = os.path.join(self.config, 'templates')