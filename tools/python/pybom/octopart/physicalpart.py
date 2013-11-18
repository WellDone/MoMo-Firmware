#physicalpart.py
#A class representing a Part in the Octopart API

from partoffer import PartOffer
import utilities

class PhysicalPart:
	def __init__(self, response):
		utilities.assert_class(response, 'Part')

		self.offers = map(lambda x: PartOffer(x), response['offers'])

	def best_price(self, quantity, **kw):
		prices = map(lambda x: (x.best_price(quantity, **kw), x.seller_name, x.packaging), self.offers)		
		valid_prices = filter(lambda x: x[0] is not None, prices)

		sorted_prices = sorted(valid_prices, key=lambda x:x[0])

		if len(sorted_prices) > 0:
			return sorted_prices[0]

		return None