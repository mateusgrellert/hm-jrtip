import os
from os import system
from sys import argv

def buildSequenceList(path):
	fin = open(path,"r")
	seq_frames = []
	for l in fin.readlines():
		if len(l) > 2:
			seq_frames.append(l.split())
	return seq_frames

def buildTimeTable():
	fin = open("rdo_times.inp","r")
	time_table = {}
	for l in fin.readlines():
		if len(l) > 2:
			time_table[l.split()[0]] = [float(x) for x in l.split()[1:]]
	return time_table


path_to_yuv = '/Users/grellert/origCfP/cropped/'
path_to_app = argv[1]
path_to_input = argv[2]

sequence_list = buildSequenceList(path_to_input)
time_table = buildTimeTable()

QP_list = ['22','27','32','37']

KP = '0.84'
KI = '0.42'
KD = '0.42'

if 'rdo' in path_to_app:
	RDO = True
else:
	RDO = False

budget_list = ['3'] # 0 - Uniform Estimation 1 - Uniform Incremental 2 - BottomUp 3 - ICIP 4 - Set All to PS60
configs = ['encoder_lowdelay_P_main']
time_factors = [0.4]

for config in configs:
	i = 0
	for time_factor in time_factors:
		for [sequence, nFrames] in sequence_list:
			for budget in budget_list:
				if ('Nebuta' in sequence) or ('SteamLocomotive' in sequence):
					strSeq = './' + path_to_app + ' -c ../cfg/'+config+'.cfg -c  /Users/grellert/hm-cfgs/cropped/'+sequence + '_10bit.cfg'
				else:
					strSeq = './' + path_to_app + ' -c ../cfg/'+config+'.cfg -c  /Users/grellert/hm-cfgs/cropped/'+sequence + '.cfg'

				if sequence == 'BasketballDrill':
					seq_path = os.popen('ls ' + path_to_yuv +sequence+'_*').readlines()
				elif sequence == 'RaceHorsesC':
					seq_path = os.popen('ls ' + path_to_yuv +sequence +'*8*').readlines()
				else:
					seq_path = os.popen('ls ' + path_to_yuv +sequence + '*').readlines()
				
				seq_path = seq_path[0].strip('\n')

				strSeq += ' --InputFile=\"'+seq_path+'\"'
				strSeq += ' --FramesToBeEncoded=' + nFrames
				i += 1
				j = 0
				for QP in QP_list:


					rdo_time = time_table[sequence][QP_list.index(QP)]
					SP = str(rdo_time*time_factor)

					strQP = ' --QP='+QP

					strQP += ' --KP=' + KP
					strQP += ' --KI=' + KI
					strQP += ' --KD=' + KD
					strQP += ' --SP=' + SP
					strQP += ' --BudgetAlgorithm=' + budget
					strQP += ' > HM_out.txt 2> HM_warn.txt'
					execLine = strSeq + strQP
					print execLine
					system(execLine)

					system("mkdir -p QP_"+QP)
					system("cp -r *csv ./QP_"+QP)
					system("cp -r *txt ./QP_"+QP)
					system("rm *csv *txt")

					j += 1

				if RDO:
					system("mkdir -p "+ sequence + '_' + config + '_RDO')
					system("cp -r "+ 'QP_* ./' + sequence + '_' + config + '_RDO')
				else:
					new_config = config + 'TimeFactor=' + str(time_factor) + '_Budget=' + budget + '_KPKIKD=' + ('_').join([KP,KI,KD])
					system("mkdir -p "+ sequence + '_' + new_config)
					system("cp -r "+ 'QP_* ./' + sequence + '_' + new_config)
				system("rm -rf QP_*")
		if RDO:
			break
