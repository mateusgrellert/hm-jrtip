import os





inc = 2
MIN_KP = 2
MAX_KP = 50

MIN_KI = 0
MAX_KI = 0

MIN_KD = 0
MAX_KD = 0

QPS = [22,27,32,37]
qp = 32

sp = 5.888
NUM_PSETS = 5
NUM_RDFRAMES = 4
NUM_TRAIN_FRAMES = 0

error = 1.05

seq = "RaceHorsesC.cfg"
best_bdrate = 9999.00

for kp in range(MIN_KP, MAX_KP+1, inc):
	for ki in range(MIN_KI, MAX_KI+1, inc):
		for kd in range(MIN_KD, MAX_KD+1, inc):
			numFrames = NUM_TRAIN_FRAMES + NUM_RDFRAMES + 128
			line = ("./TAppEncoderStatic  -c ../cfg/encoder_lowdelay_P_main.cfg -c ~/hm-cfgs/cropped/%s -f %d -balg 2 --KP=%.2f --KI=%.2f --KD=%.2f --SP=%.2f --QP=%d > dummy.txt 2> warn.txt") % (seq, numFrames, kp/10.0, ki/10.0, kd/10.0, sp, qp)
			print line					
			os.system(line)
			os.system("mv controlOut.csv controlOut_SP="+str(sp)+"QP="+("_".join([str(x) for x in [qp, kp/10.0,ki/10.0,kd/10.0]]))+".csv")

					


