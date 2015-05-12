#include "TComComplexityManagement.h"
#include "TComCABACTables.h"
#include <cmath>

using namespace std;

Int64 TComComplexityBudgeter::countBudget[NUM_PSETS];
UInt new_hist[50][50];
vector<vector <cuStats> > TComComplexityBudgeter::history;
vector<vector <config> > TComComplexityBudgeter::configMap;
vector<Double> TComComplexityBudgeter::weightsVector;
UInt TComComplexityBudgeter::origGop;
Int TComComplexityBudgeter::currPSet;
UInt TComComplexityBudgeter::picWidth;
UInt TComComplexityBudgeter::picHeight;
UInt TComComplexityBudgeter::intraPeriod;
UInt TComComplexityBudgeter::maxCUDepth;
UInt TComComplexityBudgeter::maxTUDepth;
UInt TComComplexityBudgeter::maxNumRefPics;
UInt TComComplexityBudgeter::nEncoded;
UInt TComComplexityBudgeter::currPoc;
Int TComComplexityBudgeter::searchRange;
Bool TComComplexityBudgeter::hadME;
Bool TComComplexityBudgeter::en_FME;
Bool TComComplexityBudgeter::testAMP;
Bool TComComplexityBudgeter::activateControl;
Bool TComComplexityBudgeter::restore_AMVPInfo;
Double TComComplexityBudgeter::frameBudget;
Double TComComplexityBudgeter::estimatedComp;
std::ofstream TComComplexityBudgeter::budgetFile;
AMVPInfo* TComComplexityBudgeter::keep_AMVP;

Void TComComplexityBudgeter::init(UInt w, UInt h, UInt gopSize, UInt intraP){
   // vector<Double> directionVector;
    vector<cuStats> tempHistRow;
    vector<config> tempConfigRow;
    config conf;
    estimatedComp = 0.0;
    weightsVector.push_back(0); // first frame = I_Frame
    origGop = gopSize;
    intraPeriod = intraP;
    maxCUDepth = 4;
    maxTUDepth = 3;
    maxNumRefPics = 4;
    picWidth = w;
    picHeight = h;
    hadME = 1;
    en_FME = 1;
    testAMP = 1;
    activateControl = false;
    searchRange = 64;
        currPSet = 0;

    for(int i = 0; i < 50; i++)
        for(int j = 0; j < 50; j++)
                new_hist [i][j] = -1;
    
    for(int i = 0; i < (w >> 6) + 1; i++){
        tempHistRow.clear();
        tempConfigRow.clear();
        for(int j = 0; j < (h >> 6) + 1; j++){
              conf.clear();
   //       directionVector.clear();
            // for history table 7 directions plus no motion position = 8
    //        for(int k = 0; k < 9; k++){
     //           directionVector.push_back(0.0);
     //       }
            //tempHistRow.push_back(make_pair(-1, directionVector));
              
            tempHistRow.push_back(make_pair(-1, -1));

            // for config map
            for(int k = 0; k < 8; k++){
                conf.push_back(-1);
            }
            tempConfigRow.push_back(conf);
        }
        history.push_back(tempHistRow);
        configMap.push_back(tempConfigRow);
    }
}



void TComComplexityBudgeter::setDepthHistory(TComDataCU *&pcCU, UInt pu_idx){
      
    //TODO: implement CU extrapolation
    UInt x = pcCU->getCUPelX();
    UInt y = pcCU->getCUPelY();
    UInt d = pcCU->getDepth(pu_idx);
    Int xP, yP, nPSW, nPSH;
    
//    Double theta = -1;
//    Int idx;
//    Int vectorX = pcCU->getCUMvField(REF_PIC_LIST_0)->getMv(pu_idx).getHor() >> 2;
//    Int vectorY = pcCU->getCUMvField(REF_PIC_LIST_0)->getMv(pu_idx).getVer() >> 2;
    
    pcCU->getPartPosition(pu_idx, xP, yP, nPSW, nPSH);

   //history[x >> 6][y >> 6].first = d;
    new_hist[x >> 6][y >> 6] = d;
//    if((pcCU->isIntra(pu_idx)) or (pcCU->isSkipped(pu_idx))){
//        history[x >> 6][y >> 6].second[8] += ((nPSW*nPSH)/(64.0*64)); // no movement idx
//    }
//    else{
//        // test if PU is inside LCU no motion
//        if(vectorIsInCU(x,y,xP,yP,vectorX,vectorY))
//            history[x >> 6][y >> 6].second[8] += ((nPSW*nPSH)/(64.0*64)); // no movement idx
//        else{
//            theta = calcMotionAngle(xP, yP, vectorX + xP, vectorY + yP);
//            if(theta == 360) theta = 0;
//            idx = (Int) (floor(theta/45.0));
//            history[x >> 6][y >> 6].second[idx] += ((nPSW*nPSH)/(64.0*64));
//        }
//    }
    
}

void TComComplexityBudgeter::updateConfig(TComDataCU*& cu){
    Int x = cu->getCUPelX() >> 6;
    Int y = cu->getCUPelY() >> 6;

    maxCUDepth    = configMap[x][y][0];
    maxTUDepth    = configMap[x][y][1];
    testAMP       = configMap[x][y][2];
    searchRange   = configMap[x][y][3];
    hadME         = configMap[x][y][4];
    maxNumRefPics = configMap[x][y][5];
    en_FME        = configMap[x][y][6];
    // en_FME = configMap[x][y][6];
}

void TComComplexityBudgeter::resetConfig(TComDataCU*& cu){
    maxCUDepth = 4;
    maxTUDepth = 3;
    searchRange = 64;
    maxNumRefPics = 4;
    testAMP = 1;
    hadME = 1;
}

Bool TComComplexityBudgeter::isConstrained(){

    if(nEncoded <= NUM_RD_FRAMES)
      return false;
    //if(nEncoded > 40 and nEncoded < 80)
   //     return false;
    else if (not(nEncoded % MANAGE_GOP))
        return false;
    return true;
}

Bool TComComplexityBudgeter::isTraining(){
    return (nEncoded >= NUM_RD_FRAMES and nEncoded < NUM_RD_FRAMES + NUM_TRAINING_FRAMES);
}

UInt TComComplexityBudgeter::promote(UInt ctux, UInt ctuy){
    UInt new_pset = configMap[ctux][ctuy][7]-1; // upgrading pset
    setConfigMap(ctux,ctuy,new_pset);
    return new_pset;
}

UInt TComComplexityBudgeter::demote(UInt ctux, UInt ctuy){
    UInt new_pset = configMap[ctux][ctuy][7]+1; // downgrading pset
    setConfigMap(ctux,ctuy,new_pset);
    return new_pset;
}

Void TComComplexityBudgeter::setMaxPS(){
        Double estCycleCount = 0.0;

 for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1)
                continue;
            setConfigMap(i,j,PS100);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS100);
        }
    }
}

Void TComComplexityBudgeter::uniformEstimationBudget(){ 
    Double estCycleCount = 0.0;
    currPSet = 0;
    while( currPSet < NUM_PSETS -1 ){
        if (estimateCycleCount(3,currPSet) <= frameBudget){
            break;
        }
        currPSet ++;
    }
   estimatedComp = estimateCycleCount(4,currPSet);



    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1)
                continue;
            setConfigMap(i,j,currPSet);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,currPSet);
        }
    }
}

Void TComComplexityBudgeter::uniformIncrementalBudget(){ 
   

                Double estCycleCount = 0.0;

    if(TComComplexityController::PV_avg > 1.05*frameBudget)
        currPSet++;
    else if(TComComplexityController::PV_avg < 0.90*frameBudget)
        currPSet--;
    
    currPSet = (currPSet > NUM_PSETS-1 ) ? NUM_PSETS-1 : currPSet;
    currPSet = (currPSet < 0) ? 0 : currPSet ;
   estimatedComp = estimateCycleCount(4,currPSet);



    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1)
                continue;
            setConfigMap(i,j,currPSet);
                       
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,currPSet);

        }
    }
}

Void TComComplexityBudgeter::topDownBudget(){
    Double estCycleCount = 0.0;
    
  //  Int curr_depth = 3;
    UInt new_pset,old_pset, n_demotions = 0;
    
    
    // start by assigning PS100 to all
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                continue;

            setConfigMap(i,j,PS100);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS100);
        }
    }
    
    n_demotions = 0;
    while(n_demotions < 4){ // maximum #iterations -- all PSETs are already set to PS20
        for (Int demote_depth = 0; demote_depth < 4 and estCycleCount > frameBudget; demote_depth++){
            for(int i = 0; i < history.size() and estCycleCount > frameBudget; i++){
                for(int j = 0; j < history[0].size() and estCycleCount > frameBudget; j++){
                    if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                        continue;

                    if (new_hist[i][j] == demote_depth){
                        old_pset = configMap[i][j][7];
                        new_pset = demote(i,j);
                        estCycleCount = updateEstimationAndStats(estCycleCount,old_pset,new_pset);
                    }
                }
            }          
        }
        n_demotions++;
    }
    
        estimatedComp = estCycleCount;
       

}

Void TComComplexityBudgeter::bottomUpBudget(){
    Double estCycleCount = 0.0;
    
  //  Int curr_depth = 3;
    UInt new_pset;
    
    
    // start by assigning PS20 to all
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                continue;

            setConfigMap(i,j,PS20);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS20);
        }
    }
    
    UInt promote_pset = PS20;
    
    while(promote_pset > PS100){ // maximum #iterations -- all PSETs are already set to PS100
        for(int i = 0; i < history.size() and estCycleCount < frameBudget; i++){
            for(int j = 0; j < history[0].size() and estCycleCount < frameBudget; j++){
                if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                    continue;

                    if(configMap[i][j][7] == promote_pset){
                        new_pset = promote(i,j);
                        estCycleCount = updateEstimationAndStats(estCycleCount,promote_pset,new_pset);
                    }
            }
        }
        promote_pset--;
    }
        estimatedComp = estCycleCount;
}

void TComComplexityBudgeter::printEstimatedComp(){
   TComComplexityController::pidFile << ";" << estimatedComp << ";;;" << TComComplexityController::kp*TComComplexityController::error << ";" << TComComplexityController::ki*TComComplexityController::accumError << ";" <<  TComComplexityController::kd*(TComComplexityController::error - TComComplexityController::prevError) << endl;
   TComComplexityController::pidFile << TComComplexityBudgeter::currPoc;
    
}
        
Void TComComplexityBudgeter::knapSackBudget(){

    return;
}


Void TComComplexityBudgeter::setPSetToAllCTUs(UInt pset) {
    Double estCycleCount = 0.0;
     for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1)
                        continue;
            setConfigMap(i,j,pset);
                       
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,pset);

        }
     }
    estimatedComp = estCycleCount;
}


Void TComComplexityBudgeter::ICIPBudget(){
    Double estCycleCount = 0.0;
    UInt new_pset;

    // first step - set HIGH and LOW configs and estimate cycle count
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (new_hist[i][j] == -1)
                        continue;
                   
            if(new_hist[i][j] == 0){ //set LOW budget!
                setConfigMap(i,j,PS40);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS40);
            }
            
            else if(new_hist[i][j] == 3){ //set HIGH budget!
                setConfigMap(i,j,PS100);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS100);
            }
            else{
                setConfigMap(i,j,PS60);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,PS60);
            }
        }
    }
    
           // second step - start demoting until available computation is reached
    
    UInt promote_pset = PS20;
    
    while(promote_pset > PS100){ // maximum #iterations -- all PSETs are already set to PS100
        for(int i = 0; i < history.size() and estCycleCount < frameBudget; i++){
            for(int j = 0; j < history[0].size() and estCycleCount < frameBudget; j++){
                if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                    continue;

                    if(configMap[i][j][7] == promote_pset){
                        new_pset = promote(i,j);
                        estCycleCount = updateEstimationAndStats(estCycleCount,promote_pset,new_pset);
                    }
            }
        }
        promote_pset--;
    }
    
    UInt demote_pset = PS100;

    // second step - start demoting until available computation is reached
    while(demote_pset < PS20){ // maximum #iterations -- all PSETs are already set to PS20
        for(int i = 0; i < history.size() and estCycleCount > frameBudget; i++){
            for(int j = 0; j < history[0].size() and estCycleCount > frameBudget; j++){
                if (new_hist[i][j] == -1) // sometimes the history table has more nodes than CTUs
                    continue;

                    if(configMap[i][j][7] == demote_pset){
                        new_pset = demote(i,j);
                        estCycleCount = updateEstimationAndStats(estCycleCount,demote_pset,new_pset);
                    }
            }
        }          
        demote_pset++;
    }
    estimatedComp = estCycleCount;

}




// conf 0 = low, 1 = medL, 2 = medH, 3 = high

Double TComComplexityBudgeter::updateEstimationAndStats(Double est, Int old_pset, UInt new_pset){
    Double nCU = (picHeight*picWidth)/(64.0*64.0);
   

    if (old_pset != -1)
        countBudget[old_pset]--;
    countBudget[new_pset]++;
       
    if (old_pset != -1)
        est -= estimateCycleCount(4,old_pset)/nCU;
    est += estimateCycleCount(4,new_pset)/nCU;
    
    return est;
}

Void TComComplexityBudgeter::setConfigMap(UInt i, UInt j, UInt pset){
               
            configMap[i][j][0] = PSET_TABLE[pset][0]; // Max CU Depth
            configMap[i][j][1] = PSET_TABLE[pset][1]; // Max TU Depth
            configMap[i][j][2] = PSET_TABLE[pset][2]; // AMP
            configMap[i][j][3] = PSET_TABLE[pset][3]; // SR
            configMap[i][j][4] = PSET_TABLE[pset][4]; // HAD ME
            configMap[i][j][5] = PSET_TABLE[pset][5]; // Max Num Ref Pics
            configMap[i][j][6] = PSET_TABLE[pset][6]; // FME
            configMap[i][j][7] = pset;

}

Double TComComplexityBudgeter::estimateCycleCount(UInt d, UInt conf){
    Double count = 0;
    Double factor = 0.0;
    switch (conf){
        case PS100: factor = 1.0; break; //PS100
        case PS80: factor = 0.8; break; //PS80
        case PS60: factor = 0.6; break; //PS60
        case PS40: factor = 0.4; break; //PS40
        case PS20: factor = 0.2; break; //PS20
        default: factor = 1.0; break;
    }
    
    // for(int i = 0; i <= d; i++){
     for(int i = 0; i < 4; i++){
        count += (TComAnalytics::sadCount[i][0]) * CYCLES_SAD;
        count += (TComAnalytics::sseCount[i][0]) * CYCLES_SSE;
        count += (TComAnalytics::satdCount[i][0]) * CYCLES_SATD;
        count += (TComAnalytics::transfCount[i][0]) * CYCLES_TRANSF;
    }
    
    //return (count*factor);
    return (TComComplexityController::totalAchievedComp/TComComplexityController::trainingCount)*factor;
            
}

Void TComComplexityBudgeter::distributeBudget(){
    UInt alg = TComComplexityController::budgetAlg;
    resetBudgetStats();

    if (1)
        switch(alg){
            case 0: uniformEstimationBudget(); break;
            case 1: uniformIncrementalBudget(); break;
            case 2: bottomUpBudget(); break;
            case 3: ICIPBudget(); break;
            case 4: setPSetToAllCTUs(PS60); break;
            default: uniformEstimationBudget(); break;
        }
    else
        setMaxPS();
    
    printBudgetStats();

}

Void TComComplexityBudgeter::resetBudgetStats(){

    for (int i = 0; i < NUM_PSETS; i++)
         countBudget[i] = 0;
}

Void TComComplexityBudgeter::printBudgetStats(){
    
    if(!budgetFile.is_open()){
        budgetFile.open("budgetDist.csv",ofstream::out);
        budgetFile << "PS100;PS80;PS60;PS40;PS20" << endl;
    }

    Double total = 0.0;
    for (int i = 0; i < NUM_PSETS; i++)
        total += countBudget[i];
    
    for (int i = 0; i < NUM_PSETS; i++)
        budgetFile << (Double) countBudget[i]/total << ";";

    budgetFile << endl;
}

Void TComComplexityBudgeter::setFrameBudget(Double budget){
    frameBudget = budget;
    //cout << budget << "\t" << calcWeight(t_layer) << endl;
}

Void TComComplexityBudgeter::setEncodedCount(UInt n){
    nEncoded = n;
}

Double TComComplexityBudgeter::calcMotionAngle(Int x1, Int y1, Int x2, Int y2){
    Double theta;
    
    // calculate theta = arctan(co/ca)
    theta = atan(abs(y2-y1)/abs(x2-x1+0.01))*180/M_PI;

    if((x2 - x1) >= 0 and (y2 - y1) < 0){ // second quadrat
        theta = 180 - theta;
    }
    else if((x2 - x1) < 0 and (y2 - y1) <= 0){ // third quadrat
        theta = 270 - (theta - 90);
    }
    else if((x2 - x1) >= 0 and (y2 - y1) > 0){
        theta = 270 + (theta - 90);
    }
    
    return theta;
}

//Void TComComplexityBudgeter::calcDominantDirection(){
//    Double temp_max;
//    Int domDir = 8;
//    for(int i = 0; i < history.size() -1; i++){
//        for(int j = 0; j < history[0].size() -1; j++){
//            temp_max = 0;
//            for(int k = 0; k < 9; k++){
//                // 1 - calculate dominant direction
//                if(temp_max < history[i][j].second[k]){
//                    temp_max = history[i][j].second[k];
//                    domDir = k;
//                }
//                // 2 - switch depths
//                switch(domDir){
//                    case 0:
//                        if(i+1 < picWidth)
//                            history[i+1][j].first = max(new_hist[i][j], history[i+1][j].first);
//                        break;
//                    case 1:
//                        if(i+1 < picWidth)
//                            history[i+1][j].first = max(new_hist[i][j], history[i+1][j].first);
//                        if((i+1 < picWidth) and (j-1  >= 0))
//                            history[i+1][j-1].first = max(new_hist[i][j], history[i+1][j-1].first);
//                        if(j-1 >= 0)
//                            history[i][j-1].first = max(new_hist[i][j], history[i][j-1].first);
//                        break;
//                    case 2:
//                        if(j-1 >= 0)
//                            history[i][j-1].first = max(new_hist[i][j], history[i][j-1].first);
//                        break;
//                    case 3:
//                        if(j-1 >= 0)
//                            history[i][j-1].first = max(new_hist[i][j], history[i][j-1].first);
//                        if((i-1  >= 0) and (j-1  >= 0))
//                            history[i-1][j-1].first = max(new_hist[i][j], history[i-1][j-1].first);
//                        if(i-1  >= 0)
//                            history[i-1][j].first = max(new_hist[i][j], history[i-1][j].first);
//                        break;
//                    case 4:
//                        if(i-1 >= 0)
//                            history[i-1][j].first = max(new_hist[i][j], history[i-1][j].first);
//                        break;
//                    case 5:
//                        if(i-1 >= 0)
//                            history[i-1][j].first = max(new_hist[i][j], history[i-1][j].first);
//                        if((i-1  >= 0) and (j+1 < picHeight))
//                            history[i-1][j+1].first = max(new_hist[i][j], history[i-1][j+1].first);
//                        if(j+1 < picHeight)
//                            history[i][j+1].first = max(new_hist[i][j], history[i][j+1].first);
//                        break;
//                    case 6:
//                        if(j+1 < picHeight)
//                            history[i][j+1].first = max(new_hist[i][j], history[i][j+1].first);
//                        break;
//                    case 7:
//                        if(j+1 < picHeight)
//                            history[i][j+1].first = max(new_hist[i][j], history[i][j+1].first);
//                        if((i+1 < picWidth) and (j+1 < picHeight))
//                            history[i+1][j+1].first = max(new_hist[i][j], history[i+1][j+1].first);
//                        if(i+1 < picWidth)
//                            history[i+1][j].first = max(new_hist[i][j], history[i+1][j].first);
//                        break;
//                    default:
//                        break;
//                }
//            }
//        }
//    }
//}

Bool TComComplexityBudgeter::vectorIsInCU(Int cuX, Int cuY, Int puX, Int puY, Int vecX, Int vecY){
    
    if(vecX == 0 and vecY == 0)
        return true;
    
    if(vecX <= 0){ // left
        if (vecY <= 0) // above left
            return (( vecX <= (puX - cuX)) and ( vecY <= (puY - cuY)));
        else // below left
            return (( vecX <= (puX - cuX)) and ( vecY <= (64 - puY + cuY)));
    }
    else{ // right
        if (vecY <= 0) // above right
            return (( vecX <= (64 - puX + cuX)) and ( vecY <= (puY - cuY)));
        else // below right
            return (( vecX <= (64 - puX + cuX)) and ( vecY <= (64 - puY + cuY)));
    }
}
