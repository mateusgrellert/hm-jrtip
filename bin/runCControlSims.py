import os
from os import system

def buildSequenceList():
	fin = open("sequences.inp","r")
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
			time_table[l.split()[0]] = float(l.split()[1])
	return time_table

sequence_list = buildSequenceList()
time_table = buildTimeTable()

QP_list = ['22','27','32','37']

KP = '0.4'
KI = '0.84'
KD = '0.4'
path_to_yuv = '/Users/grellert/origCfP/cropped/'

RDO = True

budget_list = ['3'] # 0 - Uniform Estimation 1 - Uniform Incremental 2 - BottomUp 3 - ICIP 4 - Set All to PS60
configs = ['encoder_lowdelay_P_main']
time_factor = 0.6

for config in configs:
	i = 0
	for [sequence, nFrames] in sequence_list:
		SP = str(time_table[sequence]*time_factor/int(nFrames))
		for budget in budget_list:

			strSeq = './TAppEncoderStatic -c ../cfg/'+config+'.cfg -c  /Users/grellert/hm-cfgs/cropped/'+sequence + '.cfg'

			if sequence == 'BasketballDrill':
				seq_path = os.popen('ls ' + path_to_yuv +sequence+'_*').readlines()

			if sequence == 'RaceHorsesC':
				seq_path = os.popen('ls ' + path_to_yuv +sequence +'*8*').readlines()
			else:
				seq_path = os.popen('ls ' + path_to_yuv +sequence + '*').readlines()
			
			seq_path = seq_path[0].strip('\n')

			strSeq += ' --InputFile=\"'+seq_path+'\"'
			strSeq += ' --FramesToBeEncoded=' + nFrames
			i += 1
			j = 0
			for QP in QP_list:
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
				config = config + 'TimeFactor=' + time_factor + '_Budget=' + budget + '_KPKIKD=' + ('_').join(KP,KI,KD)
				system("mkdir -p "+ sequence + '_' + config)
				system("cp -r "+ 'QP_* ./' + sequence + '_' + config)
			system("rm -rf QP_*")
