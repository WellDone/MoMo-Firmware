module library libgpsim_modules
module load i2cmaster u1
node scl
node sda

stimulus asynchronous_stimulus
initial_state 1
start_cycle 1000
{ 1040, 0,
1055, 1,
1060, 0,
1075, 1,
1080, 0,
1095, 1,
1100, 0,
1115, 1,
1120, 0,
1135, 1,
1140, 0,
1155, 1,
1160, 0,
1175, 1,
1180, 0,
1195, 1,
1200, 0,
1215, 1,
1220, 0,
2235, 1,
2240, 0,
2255, 1,
2260, 0,
2275, 1,
2280, 0,
2295, 1,
2300, 0,
2315, 1,
2320, 0,
2335, 1,
2340, 0,
2355, 1,
2360, 0,
2375, 1,
2380, 0,
2395, 1,
2400, 0,
3415, 1,
3420, 0,
3435, 1,
3440, 0,
3455, 1,
3460, 0,
3475, 1,
3480, 0,
3495, 1,
3500, 0,
3515, 1,
3520, 0,
3535, 1,
3540, 0,
3555, 1,
3560, 0,
3575, 1,
3580, 0,
4595, 1,
4600, 0,
4615, 1,
4620, 0,
4635, 1,
4640, 0,
4655, 1,
4660, 0,
4675, 1,
4680, 0,
4695, 1,
4700, 0,
4715, 1,
4720, 0,
4735, 1,
4740, 0,
4755, 1,
4760, 0,
5775, 1,
5780, 0,
5795, 1,
5800, 0,
5815, 1,
5820, 0,
5835, 1,
5840, 0,
5855, 1,
5860, 0,
5875, 1,
5880, 0,
5895, 1,
5900, 0,
5915, 1,
5920, 0,
5935, 1,
5940, 0,
6950, 1,
6990, 0,
7005, 1,
7010, 0,
7025, 1,
7030, 0,
7045, 1,
7050, 0,
7065, 1,
7070, 0,
7085, 1,
7090, 0,
7105, 1,
7110, 0,
7125, 1,
7130, 0,
7145, 1,
7150, 0,
7165, 1,
7170, 0 }
name scl_seq
end

stimulus asynchronous_stimulus
initial_state 1
start_cycle 1000
{ 1020, 0,
1045, 0,
1065, 0,
1085, 0,
1105, 1,
1125, 0,
1145, 1,
1165, 0,
1185, 0,
1205, 0,
2225, 0,
2245, 0,
2265, 0,
2285, 0,
2305, 1,
2325, 0,
2345, 1,
2365, 0,
2385, 0,
3405, 0,
3425, 0,
3445, 0,
3465, 0,
3485, 0,
3505, 0,
3525, 0,
3545, 0,
3565, 0,
4585, 0,
4605, 0,
4625, 0,
4645, 0,
4665, 0,
4685, 0,
4705, 0,
4725, 0,
4745, 0,
5765, 1,
5785, 1,
5805, 1,
5825, 1,
5845, 0,
5865, 1,
5885, 1,
5905, 0,
5925, 0,
6945, 1,
6970, 0,
6995, 0,
7015, 0,
7035, 0,
7055, 1,
7075, 0,
7095, 1,
7115, 0,
7135, 1,
7155, 0 }
name sda_seq
end

attach scl porta1 scl_seq
attach sda porta2 sda_seq
run
