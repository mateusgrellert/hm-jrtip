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

#define USE_SYSTEM_SPECS 1

#define TIME_ADD 1
#define TIME_SUB 1
#define TIME_MULT 4

#if USE_SYSTEM_SPECS

const double TIME_SAD[4] = {0.9863,0.3315,0.1762,0.1170};  // IN MICROSECONDS!!!!!!
const double TIME_SSE[4] = {8.8339,1.9178,0.4331,0.0991};
const double TIME_SATD[4] = {5.4230,1.4477,0.4282,0.1884};
const double TIME_TRANSF[4] = {9.8506,1.7116,0.4342,0.2539};
const double TIME_HALF_INTER[4] = {40.9460,9.9546,3.2599,1.4155};
const double TIME_QUART_INTER[4] = {91.6610,21.5360,6.3011,2.2778};

#else

#define TIME_SAD 64
#define TIME_SSE 256
#define TIME_SATD 256
#define TIME_TRANSF 136 // 32x32 transform

#endif

#define MANAGE_GOP 500 // same as I-period

#define KU 0.06
#define TU 0.4
#define NUM_STEPS_ZIEGLER 30 // for ziegler-nichols method

//#define KP 0.84  // used in the paper
//#define KI 0.42
//#define KD 0.42

#define KP 1  // for debuging
#define KI 0
#define KD 0


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

