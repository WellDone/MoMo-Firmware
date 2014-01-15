module library libgpsim_modules
node scl
node sda

module load pullup pscl
module load pullup psda

pscl.voltage = 5
pscl.resistance = 10000
psda.voltage = 5
psda.resistance = 10000

attach scl porta1 pscl.pin
attach sda porta2 psda.pin
run

