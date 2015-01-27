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

attach scl portc0 pscl.pin
attach sda portc1 psda.pin
attach alarm porta2 palarm.pin
run
