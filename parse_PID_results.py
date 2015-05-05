import os
from sys import argv

def parseOutput(dir,qp):
	f = open(dir+'/QP_'+qp+'/controlOut.csv','r')
	count = 0.0
	GOPSize = 4.0
	acc_gop = 0.0
	inc_avg = 0.0
	row = []
	for l in (f.readlines()[8:]):
		if len(l) < 10: continue
		count += 1
		sp = float(l.split(';')[2])
		pv = float(l.split(';')[3])
		acc_gop += pv
		inc_avg = (inc_avg*(count-1) + pv)/count
		if count % GOPSize == 0:
			pv_gop = acc_gop/GOPSize
			row.append('\t'.join([str(x) for x in [sp, pv_gop]]))
			acc_gop = 0
	return row







sequences = ["Traffic","PeopleOnStreet","Nebuta","SteamLocomotive","Kimono","ParkScene","Cactus","BasketballDrive","BQTerrace"]
sequences = sequences + ["BasketballDrill","BQMall","PartyScene","RaceHorsesC","BasketballPass","BQSquare","BlowingBubbles","RaceHorses"]
sequences = sequences + ["FourPeople","Johnny","KristenAndSara","BasketballDrillText","ChinaSpeed","SlideEditing","SlideShow"]

qp = argv[1]
fout = open('parsed_PID_results.csv','w')
output_vet = []
first_row = []

for seq in sequences:
	for dir in os.listdir('./'):
		if seq in dir:
			if seq == 'BasketballDrill' and 'BasketballDrillText' in dir:
				continue
			if seq == 'RaceHorses' and 'RaceHorsesC' in dir:
				continue
			target = dir.split('mainTimeFactor=')[1].split('_')[0] 
			first_row.append('Target='+target+';PV'+target)
			output_vet.append(parseOutput(dir, qp))
	
print >> fout, '\t'.join(first_row)
result = [[output_vet[j][i] for j in range(len(output_vet))] for i in range(len(output_vet[0]))]
for r in result:
	print >> fout, '\t'.join(r)

