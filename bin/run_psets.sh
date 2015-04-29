#./TAppEncoderStatic  -c ../cfg/encoder_lowdelay_P_main.cfg -c ~/hm-cfgs/cropped/BQSquare.cfg --MaxPartitionDepth=4 --QuadtreeTUMaxDepthInter=3 --SearchRange=64 --AMP=1 --HadamardME=1 -f $1 --QP=$2 > pset100.txt

#./TAppEncoderStatic  -c ../cfg/encoder_lowdelay_P_main.cfg -c ~/hm-cfgs/cropped/BQSquare.cfg --MaxPartitionDepth=4 --QuadtreeTUMaxDepthInter=3 --SearchRange=32 --AMP=1 --HadamardME=1 -f $1 --QP=$2 > pset80.txt

./TAppEncoderStatic  -c ../cfg/encoder_lowdelay_P_main.cfg -c ~/hm-cfgs/cropped/BQSquare.cfg --MaxPartitionDepth=4 --QuadtreeTUMaxDepthInter=1 --SearchRange=32 --AMP=0 --HadamardME=1 -f $1 --QP=$2 > pset60.txt

#./TAppEncoderStatic  -c ../cfg/encoder_lowdelay_P_main.cfg -c ~/hm-cfgs/cropped/BQSquare.cfg --MaxPartitionDepth=3 --QuadtreeTUMaxDepthInter=1 --SearchRange=16 --AMP=0 --HadamardME=0 -f $1 --QP=$2 > pset40.txt

#./TAppEncoderStatic  -c ../cfg/encoder_lowdelay_P_main_1ref.cfg -c ~/hm-cfgs/cropped/BQSquare.cfg --MaxPartitionDepth=3 --QuadtreeTUMaxDepthInter=1 --SearchRange=8 --AMP=0 --HadamardME=0 -f $1 --QP=$2 > pset20.txt
