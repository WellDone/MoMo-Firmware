#partoffer.py
#Class encapsulating an Octopart PartOffer response, exposing the 
#price breaks 

from prices import PriceList
import utilities

class PartOffer:
	def __init__(self, response):
		utilities.assert_class(response, 'PartOffer')

		self.seller_name = response['seller']['name']
		self.seller_uid = response['seller']['uid']
		self.packaging = response['packaging']
		self.invalid = False

		try:
			self.breaks = PriceList(response['prices'])
		except ValueError, e:
			self.breaks = None
			self.invalid = True

		self.in_stock_quant = response['in_stock_quantity']


	def best_price(self, quantity, in_stock=False, seller=None, seller_uid=None, exclude_pkg=None):
		if self.invalid:
			return None

		if exclude_pkg is not None and self.packaging in exclude_pkg:
			return None

		if in_stock and quantity > self.in_stock_quant:
			return None

		if seller is not None and self.seller_name != seller:
			return None

		if seller_uid is not None and self.seller_uid != seller_uid:
			return None

		return self.breaks.unit_price(quantity)