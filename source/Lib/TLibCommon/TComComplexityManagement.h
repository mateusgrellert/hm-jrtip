/* 
 * File:   TComComplexityManagement.h
 * Author: mateusgrellert
 *
 * Created on December 6, 2012, 6:15 PM
 */

#ifndef TCOMCOMPLEXITYMANAGEMENT_H
#define	TCOMCOMPLEXITYMANAGEMENT_H


#define EN_COMPLEXITY_MANAGING 1
#define NUM_RD_FRAMES 4
#define NUM_TRAINING_FRAMES 0



#define CYCLES_ADD 1
#define CYCLES_SUB 1
#define CYCLES_MULT 4

#define CYCLES_SAD 64
#define CYCLES_SSE 256
#define CYCLES_SATD 256
#define CYCLES_TRANSF 544 // 64x64 transform


#define MANAGE_GOP 500 // same as I-period

#define KU 0.06
#define TU 0.4
#define NUM_STEPS_ZIEGLER 30 // for ziegler-nichols method

#define KP 0.8
#define KI 0.8
//#define KI (2*KP/TU/4)
#define KD 1.0



#define NUM_PARAMS 7 // cu depth, tu depth, AMP, SE, HAD ME, num ref frames, FME
#define NUM_PSETS 5
#define PS100 0
#define PS80 1
#define PS60 2
#define PS40 3
#define PS20 4

const int PSET_TABLE[NUM_PSETS][NUM_PARAMS] = {
    {4,3,1,64,1,4,1}, //100%
    {4,3,0,64,1,3,1}, //80%
    {4,1,0,64,0,4,1}, //60%
    {3,1,0,64,0,4,1}, //40%
    {2,1,0,64,0,2,1}  //20%
};

//    {2,1,0,64,0,1,1}  //20%


#include "MyTypedef.h"
#include "TComAnalytics.h"
#include "TComComplexityController.h"
#include "TComComplexityBudgeter.h"
#endif	/* TCOMCOMPLEXITYMANAGEMENT_H */

