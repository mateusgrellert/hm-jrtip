import os

vet = []
i = 0
for path in os.listdir('./'):
	row = []
	if 'SP' in path:
		[kp,ki,kd] = path.split('_')[2:5]
		f = open(path,'r')
		row.append("/".join([kp,ki,kd]))
		diff_sp_pv = 0
		mse = 0
		count = 0
		for l in f.readlines()[1:]:
			if len(l.split(';')) < 3: continue
			count+=1
			row.append(l.split(';')[3])
			error = float(l.split(';')[2]) - float(l.split(';')[3])
			sq_error = error*error
			mse += sq_error
			diff_sp_pv += error
		row.append(str(diff_sp_pv/count))
		row.append(str(mse/count))

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
