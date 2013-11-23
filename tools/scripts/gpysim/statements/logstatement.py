
class LogStatement:
	def pull_info(self, log):
		self.address = log.current_address

	def keep(self):
		"""
		Override in subclasses if this logging statement doesn't stand on its own but 
		just sets state for future logging statements.  It will be processed and then
		deleted from the Log object list of logging statements
		"""
		return True

	def error(self):
		"""
		Override for logging statements that indicate errors, rather than ones that are just informational
		"""
		return False

	def format_line(self, symtab):
		func = symtab.map_address(self.address)

		header = 'Address 0x%X: ' % self.address
		if func is not None:
			header = '%s + %d: ' % (func[0], func[1])

		return header + self.format()