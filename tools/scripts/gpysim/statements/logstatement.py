
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