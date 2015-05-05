import os

n_cores = 6
for i in range(n_cores):
	os.system('xterm -e \"cd bin_' + str(i) + ';python runCControlSims.py\" &')