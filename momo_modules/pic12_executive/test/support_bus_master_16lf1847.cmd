module library libgpsim_modules
node scl
node sda

module load MoMoSlave slave
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

slave.address = 8
slave.logfile = mibcalls.txt

attach scl portb4 pscl.pin slave.scl record_clock.pin
attach sda portb1 psda.pin slave.sda record_data.pin
run
