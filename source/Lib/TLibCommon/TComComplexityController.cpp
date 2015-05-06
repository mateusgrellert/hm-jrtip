#include "TComComplexityManagement.h"

Double TComComplexityController::PV;
Double TComComplexityController::PV_avg;
Double TComComplexityController::SP;
Double TComComplexityController::new_SP;
Double TComComplexityController::error;
Double TComComplexityController::prevError;
Double TComComplexityController::accumError;
Double TComComplexityController::controlOutput;
Double TComComplexityController::kd;
Double TComComplexityController::ki;
Double TComComplexityController::kp;
Double TComComplexityController::targetSavings;
Double TComComplexityController::nSteps;
Double TComComplexityController::procAvail;
Double TComComplexityController::totalAchievedComp;
UInt TComComplexityController::trainingCount;        
UInt TComComplexityController::procFreq;
UInt TComComplexityController::countToOFF;
Double TComComplexityController::fps;
UInt TComComplexityController::budgetAlg;
UInt TComComplexityController::nEncoded;
std::ofstream TComComplexityController::pidFile;
clock_t TComComplexityController::lBefore;
Double TComComplexityController::timer;

Void TComComplexityController::init(UInt n_encFrames){
   // SP = 0.0; // gives number of cycles available per frame per frame
   // kp = KP;
   // ki = KI;
  //  kd = KD;
    nEncoded = n_encFrames;
    nSteps = NUM_STEPS_ZIEGLER;
    error = 0.0;
    countToOFF = 0;
    prevError = 0.0;
    accumError = 0.0;
    totalAchievedComp = 0.0;
    trainingCount = 0;
    controlOutput = 0.0;
    PV_avg = 0.0;
}

void TComComplexityController::setPV(){
    PV = calcAchievedComp();
}

Double TComComplexityController::calcPID(UInt n, UInt t_layer){

    Double PIDOutput;
    PV = calcAchievedComp();
    nEncoded = (n-4-NUM_RD_FRAMES+1);
    prevError = error;
    //int nmod64 = nEncoded % 32;
    TComComplexityBudgeter::activateControl = true;
    
//        nEncoded = (n-NUM_RD_FRAMES) % 60;

////       
//    if(SP*(0.4-0.1*(int)(nEncoded/32)) != new_SP){
//        accumError = 0;
//        prevError = 0;
//        PV_avg = PV;
//    }
//    else
//        PV_avg = (PV_avg*(nmod64-1)+PV)/(nmod64);
//
//        new_SP = SP*(0.4-0.1*(int)(nEncoded/32));


    
    PV_avg = (PV_avg*(nEncoded-1)+PV)/(nEncoded);

        error = SP - PV;
    
  //  if(nEncoded % 4 == 0)
    //    accumError = 0.0;
    accumError += error;           

    PIDOutput = kp*error + ki*accumError+ kd*(error - prevError);
    controlOutput = (PV + PIDOutput);

    //if (finalOutput < 0) finalOutput = 0.0;


    return controlOutput;
 
    //return (desiredComp + PIDOutput);
}

void TComComplexityController::printPIDStats(){
    openPidFile();

    pidFile << ";" << TComComplexityBudgeter::activateControl << ";" <<  new_SP << ";" << PV << ";" << PV_avg << ";" << controlOutput;

}

void TComComplexityController::calcPV( ){
    totalAchievedComp += calcAchievedComp();
    trainingCount++;
}

void TComComplexityController::setSP(double sp){
    //SP = totalAchievedComp/trainingCount*targetSavings;
    SP = sp;
}

Double TComComplexityController::calcWeight(UInt t_layer){


    Double weights8[] = {2.6,1.3,0.86,0.65,0.65,0.86,0.65,0.65};
    Double weights4[] = {0.6,0.9,0.6,1.8};

    //if (t_layer == 0) return (1+origGop*0.2);
   // return (1+origGop*0.2)/(t_layer)*1.0;

    if(TComComplexityBudgeter::origGop == 8)
        return weights8[(TComComplexityBudgeter::currPoc) % (TComComplexityBudgeter::origGop)];
    else
        return weights4[(TComComplexityBudgeter::currPoc) % (TComComplexityBudgeter::origGop)];
    //return (a1 + d*(5-t_layer));
}

Void TComComplexityController::printAchievedComputation(){
    openPidFile();

    PV = calcAchievedComp();
    pidFile << "POC;" << SP << ";" << PV << ";" << 0 << endl;

}

Double TComComplexityController::calcSimpleControl(UInt n){
        Double avgComp = calcTotalComp()/((n-1)*1.0);
        PV = calcAchievedComp();
        openPidFile();

        pidFile <<SP << ";" << PV << ";" << (2*SP - avgComp)  << endl;

        return (2*SP - avgComp);
}

Double TComComplexityController::calcAchievedComp(){
    Double n_cycles = 0.0; // in 10^6 cycles
        // REMINDER - opCount is in 10^6
    for(int i = 0; i < g_uiMaxCUDepth; i++){
        n_cycles += TComAnalytics::sadCount[i][0]*CYCLES_SAD;
        n_cycles += TComAnalytics::sseCount[i][0]*CYCLES_SSE;
        n_cycles += TComAnalytics::satdCount[i][0]*CYCLES_SATD;
        n_cycles += TComAnalytics::transfCount[i][0]*CYCLES_TRANSF;
    }
    
    //return n_cycles;
    return timer;
}


Void TComComplexityController::initTimer(){
  lBefore = clock();
}

Void TComComplexityController::endTimer(){
  timer = (Double)(clock()-lBefore) / CLOCKS_PER_SEC;
}

Double TComComplexityController::calcTotalComp(){
    Double n_cycles = 0.0;
        // REMINDER - opCount is in 10^6
    for(int i = 0; i < g_uiMaxCUDepth; i++){
        n_cycles += TComAnalytics::sadCount[i][1]*CYCLES_SAD;
        n_cycles += TComAnalytics::sseCount[i][1]*CYCLES_SSE;
        n_cycles += TComAnalytics::satdCount[i][1]*CYCLES_SATD;
        n_cycles += TComAnalytics::transfCount[i][1]*CYCLES_TRANSF;
    }
    
    return n_cycles;
}

Void TComComplexityController::openPidFile(){ 
    if(!pidFile.is_open()){
        pidFile.open("controlOut.csv",ofstream::out);
        pidFile << "POC;Control ON/OFF;SP;PV;PVavg;PID;EST;;;e;Se;de\n";
    }
}