import intelhex
import sys

ih = intelhex.IntelHex16bit(sys.argv[1])

numfeataddr = 0x7FA
getfeataddr = 0x7FB
getcmdaddr = 0x7FC
gethanaddr = 0x7FE
magicaddr = 0x7FF

print "MIB Block at 0x7F0"
print "Magic Number: 0x%X" % ih[magicaddr]

def decode(ih, addr):
	instr = ih[addr]

	prefix = instr >> 11
	retpref = instr >> 8
	lowbyte = instr & 0xFF

	if prefix == 0b101:
		return ('goto', instr & ((1 << 11)-1))
	elif prefix == 0b110:
		if retpref == 0b110100:
			return ('retlw', instr & 0xFF)
		elif retpref == 0b110001:
			return ('movlp', instr & 0xFE)
	elif prefix == 0:
		if lowbyte == 0b1011:
			return ('brw', 1)
		

	print "Unknown Instruction at 0x%X: 0x%X" % (addr, instr)
	print "Prefix was: %s" % bin(prefix)
	print "High byte was: %s" % bin(retpref)
def process_instr(ih, page, addr):
	instr = decode(ih, addr)

	print "0x%X: %s 0x%X" % (addr, instr[0], instr[1])

	mno = instr[0]
	newaddr = addr + 1

	if mno == 'goto':
		newaddr = instr[1] | ((page & 0b01111000) << 8)
		return (page, newaddr)
	elif mno == 'movlp':
		return (instr[1], newaddr)
	elif mno == 'brw':
		return(page, newaddr + instr[1])
	elif mno == 'retlw':
		return None

def find_return(ih, addr):
	page = 0
	while True:
		x = process_instr(ih, page, addr)
		if x is None:
			break
		
		page = x[0]
		addr = x[1]

print "Get Feature"
#find_return(ih, getfeataddr)

print "Number of Features"
#find_return(ih, numfeataddr)

print "Get Command"
#find_return(ih, getcmdaddr)

print "Get Handler"
find_return(ih, gethanaddr)