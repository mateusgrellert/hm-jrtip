import os

vet = []
i = 0
for path in os.listdir('./'):
	row = []
	if 'Target' in path:
		[kp,ki,kd] = path.split('_')[2:5]
		f = open(path,'r')
		row.append("/".join([kp,ki,kd]))
		diff_sp_pv = 0

		for l in f.readlines()[1:]:
			if len(l.split(';')) < 3: continue

			row.append(l.split(';')[3])
			diff_sp_pv += float(l.split(';')[2]) - float(l.split(';')[3])
		row.append(str(diff_sp_pv))
		if vet == []:
			vet = ['']*len(row)
			for  r in range(len(vet)):
				vet[r] = []

		for r in range(len(row)):
			vet[r].append(row[r])
		i += 1

fout = open('parsed_pid_constants.csv','w')
for row in vet:
	print >> fout, ";".join(row)
