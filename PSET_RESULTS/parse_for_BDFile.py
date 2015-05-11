import os

def parseOutput(dir,qp):
	f = open(dir+'/QP_'+qp+'/HM_out.txt','r')
	count = 0
	psnr_count = False
	for l in (f.readlines()):
		if 'Total Time' in l:
			time = float(l.split()[2])
		elif 'SUMMARY' in l:
			psnr_count = True
		elif count == 2:
			bitrate = float(l.split()[2])
			y_psnr = float(l.split()[3])
			u_psnr = float(l.split()[4])
			v_psnr = float(l.split()[5])

		if psnr_count:
			count += 1

	return [str(x) for x in [bitrate,y_psnr,u_psnr,v_psnr,time]]







sequences = ["Traffic","PeopleOnStreet","Nebuta","SteamLocomotive","Kimono","ParkScene","Cactus","BasketballDrive","BQTerrace"]
sequences = sequences + ["BasketballDrill","BQMall","PartyScene","RaceHorsesC","BasketballPass","BQSquare","BlowingBubbles","RaceHorses"]
sequences = sequences + ["FourPeople","Johnny","KristenAndSara","BasketballDrillText","ChinaSpeed","SlideEditing","SlideShow"]

psets = ['PSET0','PSET1','PSET2','PSET3']

QPs = ['22','27','32','37']
for pset in psets:
	fout = open('parse_BD_results_'+ pset+ '.csv','w')
	for seq in sequences:
		found = False
		for dir in os.listdir('./'):
			if pset not in dir:
				continue
			if seq in dir:
				if seq == 'BasketballDrill' and 'BasketballDrillText' in dir:
					continue
				if seq == 'RaceHorses' and 'RaceHorsesC' in dir:
					continue
				if seq == 'BasketballDrillText' and 'BasketballDrill_' in dir:
					continue
				if seq == 'RaceHorsesC' and 'RaceHorses_' in dir:
					continue
				found = True
				for qp in QPs:
					output_vet = []
					if QPs.index(qp) == 0:
						output_vet.append(seq)
					else:
						output_vet.append(' ')

					output_vet.append(qp)
					output_vet += parseOutput(dir, qp)
					print >> fout, '\t'.join(output_vet)

		if not found:
			for qp in QPs:
				output_vet = []
				if QPs.index(qp) == 0:
					output_vet.append(seq)
				else:
					output_vet.append(' ')
				output_vet.append(qp)
				print >> fout, '\t'.join(output_vet)

