import os
from os import system

sequence_list = ['BQMall', 'BQSquare', 'BQTerrace', 'BasketballDrillText', 'BasketballDrill', 'BasketballDrive', 
				'BasketballPass', 'BlowingBubbles', 'Cactus', 'Kimono', 'ChinaSpeed', 
				'NebutaFestival', 'ParkScene', 'PartyScene', 'PeopleOnStreet', 'RaceHorsesC', 'RaceHorses', 
				'SlideEditing', 'SlideShow', 'SteamLocomotiveTrain', 'Traffic']

sequence_list = ['PeopleOnStreet','Kimono']
nFrames = ['64','64','64','64','64']
nFrames = '64'
QP_list = ['22','27','32','37']

KP = '0.4'
KI = '0.84'
KD = '0.4'

RDO = False

budget_list = ['3'] # 0 - Uniform Estimation 1 - Uniform Incremental 2 - BottomUp 3 - ICIP 4 - Set All to PS60
configs = ['encoder_lowdelay_P_main']

for config in configs:
	i = 0
	for sequence in sequence_list:
		for budget in budget_list:

			strSeq = './TAppEncoderStatic -c ../cfg/'+config+'.cfg -c ~/hm-cfgs/cropped/'+sequence

			if sequence == 'BasketballDrill':
				seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'_*').readlines()

			if sequence == 'RaceHorsesC':
				seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'*8*').readlines()
			else:
				seq_path = os.popen('ls /Volumes/Time\\ Capsule/origCfP/'+sequence+'*').readlines()
			
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
				strQP += ' --BudgetAlgorithm=' + budget
				strQP += ' > HM_out.txt 2> HM_warn.txt'
				execLine = strSeq + strQP
				print execLine
				system(execLine)

				system("mkdir QP_"+QP)
				system("mv *csv ./QP_"+QP)
				system("mv *txt ./QP_"+QP)

				j += 1

			if RDO:
				system("mkdir "+ sequence + '_' + config + '_RDO')
				system("mv "+ 'QP_* ./' + sequence + '_' + config + '_RDO')
			else:
				system("mkdir "+ sequence + '_' + config)
				system("mv "+ 'QP_* ./' + sequence + '_' + config)