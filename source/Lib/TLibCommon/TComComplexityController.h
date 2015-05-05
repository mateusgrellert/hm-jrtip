/* 
 * File:   TComComplexityController.h
 * Author: mateusgrellert
 *
 * Created on November 27, 2012, 5:14 PM
 */

#ifndef TCOMCOMPLEXITYCONTROLLER_H
#define	TCOMCOMPLEXITYCONTROLLER_H

class TComComplexityController {

public:
    
    static Double PV, SP, new_SP, PV_avg;
    static Double kp, ki, kd;
    static Double nSteps,targetSavings;
    static Double error, prevError;
    static Double accumError, controlOutput;
    
    static Double procAvail,totalAchievedComp;
    static UInt procFreq, trainingCount;
    static Double fps;
    static UInt countToOFF;
    static UInt budgetAlg;
    
    static UInt nEncoded;
    static std::ofstream pidFile;
    static clock_t lBefore;
    static Double timer;
    
    
    TComComplexityController();
    
    static Void init(UInt);
    static Void setDesiredComplexity (Double);
    static Double getTargetComplexity();
    static Double calcAchievedComp();
    static Double calcTotalComp();
    static Void printAchievedComputation();
    static Double calcPID(UInt,UInt);
    static Double calcWeight(UInt t_layer);
    static void setSP(double);
    static void calcPV();
    static void setPV();
    static void printPIDStats();
    static void initTimer();
    static void endTimer();

    static Double calcSimpleControl(UInt);
    static Double calcZieglerNichols();
    static Void openPidFile();

    static Void setProcFreq(UInt f) {procFreq = f;};
    static Void setProcAvail(Double a) {procAvail = a;};
    static Void setFPS(Double f) {fps = f;};
    static Void setBudgetAlg(UInt b) {budgetAlg = b;};
};

#endif	/* TCOMCOMPLEXITYCONTROLLER_H */

