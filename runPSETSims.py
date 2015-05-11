import os
from os import system
from sys import argv

def buildSequenceList():
	fin = open("sequences.inp","r")
	seq_frames = []
	for l in fin.readlines():
		if len(l) > 2:
			seq_frames.append(l.split())
	return seq_frames


def makePSets():
	fin = open("psets.inp","r")
	pvet = []
	for l in fin.readlines():
		pset = []
		for pval in l.split():
			p = pval.split('=')[0]
			val = pval.split('=')[1]
			pset.append([p, val])
		pvet.append(pset)
	return pvet

sequence_list = buildSequenceList()
psets = makePSets()
QP_list = ['22','27','32','37']

path_to_yuv = '/Users/grellert/origCfP/cropped/'
path_to_app = argv[1]

configs = ['encoder_lowdelay_P_main']

for config in configs:
	i = 0
	for [sequence, nFrames] in sequence_list:
		for pset in psets:
			new_config = config
			for p in pset:
				if p[0] == 'NumRefs':
					new_config = config+ '_'+p[1]+ 'ref'


			strSeq = './' + path_to_app + ' -c ../cfg/'+new_config+'.cfg -c  /Users/grellert/hm-cfgs/cropped/'+sequence + '.cfg'

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
			for QP in QP_list:

				strQP = ' --QP='+QP

				for p in pset:
					strQP += ' --'+p[0]+'='+p[1]

				strQP += ' > HM_out.txt 2> HM_warn.txt'
				execLine = strSeq + strQP
				print execLine
				system(execLine)

				system("mkdir -p QP_"+QP)
				system("cp -r *csv ./QP_"+QP)
				system("cp -r *txt ./QP_"+QP)
				system("rm *csv *txt")


			system("mkdir -p "+ sequence + '_' + new_config + '_PSET' + str(psets.index(pset)))
			system("cp -r "+ 'QP_* ./' + sequence + '_' + new_config + '_PSET' + str(psets.index(pset)))
			
			system("rm -rf QP_*")
