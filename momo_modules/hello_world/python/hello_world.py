from pymomo.commander.proxy.proxy12 import MIB12ProxyObject
from pymomo.utilities.typedargs import param, context, return_type

@context("TutorialModule")
class TutorialModule (MIB12ProxyObject):
	"""
	TutorialModule

	A demonstration module showing how to create python interfaces around
	MoMo module boards and interact with the boards using either the momo 
	tool or a python script.
	"""

	def __init__(self, stream, addr):
		super(TutorialModule, self).__init__(stream, addr)
		self.name = 'GSM Module'

	@return_type("float")
	def sample_v1(self):
		"""
		Return the analog voltage present on the V1 pin

		Voltage is returned as a floating point number expressed in volts.
		"""

		res = self.rpc(20, 0, result_type=(1, False))

		sample_reading = res['ints'][0]
		
		#ADC reading is 10 bits (so 1024 codes) and the reference full-scale voltage is
		#nominally 2.8 volts but typically around 2.78
		sample_voltage = sample_reading/1024. * 2.8
		
		#Pin is connected to a divide by 15 resistor network
		actual_voltage = sample_voltage*15.0
		return actual_voltage

	@return_type("float")
	def sample_v2(self):
		"""
		Return the analog voltage present on the V2 pin

		Voltage is returned as a floating point number expressed in volts.
		"""

		res = self.rpc(20, 1, result_type=(1, False))

		sample_reading = res['ints'][0]
		
		#ADC reading is 10 bits (so 1024 codes) and the reference full-scale voltage is
		#nominally 2.8 volts but typically around 2.78
		sample_voltage = sample_reading/1024. * 2.8
		
		#Pin is connected to a divide by 15 resistor network
		actual_voltage = sample_voltage*15.0
		return actual_voltage

	@param("enable", "bool", desc="Enable power to voltage output pin")
	def set_power(self, enable):
		"""
		Enable or disable power to voltage output pin

		passing true enables power, passing false disables power
		"""

		self.rpc(20, 4, int(enable))