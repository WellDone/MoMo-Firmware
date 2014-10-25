module library libgpsim_modules
node scl
node sda

module load FileRecorder record_clock
module load FileRecorder record_data
module load pullup pscl
module load pullup psda

record_clock.digital = true
record_data.digital = true
record_clock.file = clock.txt
record_data.file = data.txt

pscl.voltage = 5
pscl.resistance = 10000
psda.voltage = 5
psda.resistance = 10000

attach scl porta1 pscl.pin record_clock.pin
attach sda porta2 psda.pin record_data.pin
run

