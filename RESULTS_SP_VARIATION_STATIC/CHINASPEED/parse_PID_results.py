import os
from sys import argv

def parsePVGOP(dir,qp):
	f = open(dir+'/QP_'+qp+'/controlOut.csv','r')
	count = 0.0
	GOPSize = 4.0
	acc_gop = 0.0
	inc_avg = 0.0
	row = []
	lines = (f.readlines()[1:])
	for l in lines:
		if len(l) < 10: continue
		count += 1
		sp = float(lines[9].split(';')[2])
		pv = float(l.split(';')[3])
		acc_gop += pv
		inc_avg = (inc_avg*(count-1) + pv)/count
		if count % GOPSize == 0:
			pv_gop = acc_gop/GOPSize
			row.append('\t'.join([str(x) for x in [sp, pv_gop]]))
			acc_gop = 0
	return row



def parseAVGGOP(dir,qp):
	f = open(dir+'/QP_'+qp+'/controlOut.csv','r')
	count = 0.0
	GOPSize = 4.0
	acc_gop = 0.0
	inc_avg = 0.0
	row = []
	first_count = True
	lines = (f.readlines()[1:])
	for l in lines:
		if len(l) < 10: continue
		if count == 7 and first_count:
			inc_avg = 0
			count = 0
			first_count = False
		count += 1

		sp = float(lines[9].split(';')[2])
		pv = float(l.split(';')[3])
		acc_gop += pv
		inc_avg = (inc_avg*(count-1) + pv)/count
		if count % GOPSize == 0:
			pv_gop = acc_gop/GOPSize
			row.append('\t'.join([str(x) for x in [sp, inc_avg]]))
			acc_gop = 0
	return row



sequences = ["Traffic","PeopleOnStreet","Nebuta","SteamLocomotive","Kimono","ParkScene","Cactus","BasketballDrive","BQTerrace"]
sequences = sequences + ["BasketballDrill","BQMall","PartyScene","RaceHorsesC","BasketballPass","BQSquare","BlowingBubbles","RaceHorses"]
sequences = sequences + ["FourPeople","Johnny","KristenAndSara","BasketballDrillText","ChinaSpeed","SlideEditing","SlideShow"]

qp = argv[1]
fout_pv = open('parsed_PID_results_PVGOP.csv','w')
fout_avg = open('parsed_PID_results_AVGGOP.csv','w')
pvgop_vet = []
avggop_vet = []
first_row = []

for seq in sequences:
	for dir in os.listdir('./'):
		if seq in dir:
			if seq == 'BasketballDrill' and 'BasketballDrillText' in dir:
				continue
			if seq == 'RaceHorses' and 'RaceHorsesC' in dir:
				continue
			target = dir.split('mainTimeFactor=')[1].split('_')[0] 
			first_row.append('SP;Target='+target)
			pvgop_vet.append(parsePVGOP(dir, qp))
			avggop_vet.append(parseAVGGOP(dir, qp))
	
print >> fout_pv, '\t'.join(first_row)
print >> fout_avg, '\t'.join(first_row)
result = [[pvgop_vet[j][i] for j in range(len(pvgop_vet))] for i in range(len(pvgop_vet[0]))]
for r in result:
	print >> fout_pv, '\t'.join(r)

result = [[avggop_vet[j][i] for j in range(len(avggop_vet))] for i in range(len(avggop_vet[0]))]
for r in result:
	print >> fout_avg, '\t'.join(r)

