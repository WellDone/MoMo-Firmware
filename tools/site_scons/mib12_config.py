#mib12_config.py

class MIB12Processor:
	"""
	Collection of settings pertaining to the location of various important
	ROM and RAM regions in the MIB12 framework.
	"""


	def __init__(self, name, settings):
		self.settings = settings
		self.name = name

		self._calculate()

	def _calculate(self):
		self.exec_rom = self.settings['executive_rom']
		self.app_rom = [self.exec_rom[1] +1, self.settings['total_rom']-1]

		self.api_range = [self.app_rom[0] - 16, self.app_rom[0] - 1]
		self.mib_range = [self.app_rom[1] - 15, self.app_rom[1]]

		self.first_app_page = self.app_rom[0] / 16