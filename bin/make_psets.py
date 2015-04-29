import os

def buildParamLine(params):
	line = ''
	diff_cfg = False
	for param in params:
		if param == 'RefFrames':
			diff_cfg = True
			line = '-c ../cfg/encoder_lowdelay_P_main_1ref.cfg ' + line
		else:
			line = '%s --%s=%d ' % (line, param, params[param])
	if not diff_cfg:
		line = '-c ../cfg/encoder_lowdelay_P_main.cfg ' + line
	return line

def parseHM():
	f = open('HM_out.txt','r')

	for l in (f.readlines()):
		if 'Total Time' in l:
			time = float(l.split()[2])
		elif 'Bytes written to file' in l:
			bitrate = float(l.split('(')[1].split()[0])
	
	f.close()

	return [time,bitrate]

def runHM(params, seq, nfr, qp):
	print 'HM cfg: ', params
	line = './TAppEncoderStatic '
	line += ' -c ~/hm-cfgs/cropped/'+seq + ' '
	line += params
	line = '%s --QP=%d -f %d' % (line, qp, nfr)
	line += ' > HM_out.txt 2> HM_warn.txt'
	os.system(line)
	return parseHM()

cfgs = [''] * 13

cfgs[0] = ['']
cfgs[1] = ['SearchRange', 32]
cfgs[2] = ['SearchRange', 16]
cfgs[3] = ['RefFrames', 1]
cfgs[4] = ['SearchRange', 8]
cfgs[5] = ['AMP', 0]
cfgs[6] = ['HadamardME', 0]
cfgs[7] = ['QuadtreeTUMaxDepthInter',1]
cfgs[8] = ['FME', 0]
cfgs[9] = ['MaxPartitionDepth', 3]
cfgs[10] = ['MaxPartitionDepth', 2]
cfgs[11] = ['QuadtreeTUMaxDepthInter', 2]
cfgs[12] = ['MaxPartitionDepth', 1]

SPs = [0.8,0.6,0.4,0.2]


sequence = 'BQSquare.cfg'
n_frames = 16
qp = 32
[ref_time, ref_bitrate] = runHM(buildParamLine(''), sequence, n_frames, qp)
best_params = {}
params = {}
idx = 1
for sp in SPs:
	best_bitrate_ratio = 999999
	best_found = False
	skip_idx = []
	params = {}
	idx = 1
	pivot = 0
	print 'Running SP: ', sp

	while pivot < len(cfgs):
		if idx == pivot:
			idx = (idx + 1) % len(cfgs)
			continue

		cfg = cfgs[idx]
		idx = (idx + 1) % len(cfgs)
		currParam = cfg[0]
		currVal = cfg[1]
		params[currParam] = currVal
		params_line = buildParamLine(params)
		
		[test_time, test_bitrate] = runHM(params_line, sequence, n_frames, qp)
		

		time_ratio = test_time/ref_time
		bitrate_ratio = test_bitrate/ref_bitrate
		print '\tTime Ratio: %.2f\n' % (time_ratio)
		
		if time_ratio <= 1.01*sp and time_ratio >= (sp - 0.1):
			if bitrate_ratio <= best_bitrate_ratio:
				best_bitrate_ratio = bitrate_ratio
				best_params[sp] = [buildParamLine(params), best_bitrate_ratio]
				best_found = True

		elif time_ratio < (sp - 0.1):
			pivot += 1
			cfg = cfgs[pivot]
			idx = 1
			del params[cfg[0]]

for bp in best_params:
	print bp, ": ", best_params[bp]
