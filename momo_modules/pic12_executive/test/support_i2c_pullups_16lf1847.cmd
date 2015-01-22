module library libgpsim_modules
node scl
node sda
node alarm

module load pullup pscl
module load pullup psda
module load pullup palarm

pscl.voltage = 5
pscl.resistance = 10000
psda.voltage = 5
psda.resistance = 10000
palarm.voltage = 5
palarm.resistance = 10000

attach scl portb4 pscl.pin
attach sda portb1 psda.pin
attach alarm portb0 palarm.pin
run
