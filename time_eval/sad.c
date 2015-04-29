#include "hm_functions.h"


int sad(int *vetA, int *vetB, int rows, int cols){
	unsigned int uiSum = 0;
	iRows = rows;
	iCols = cols;
	for( ; iRows != 0; iRows-- )
  	{
   		for (int n = 0; n < iCols; n++ )
    	{
   			uiSum += abs( vetA[n] - vetB[n] );
    	}
    	vetA += iCols;
    	vetB += iCols;
  	}
  return ( uiSum );
}

int sse(int *vetA, int *vetB, int rows, int cols){
	unsigned int uiSum = 0;
	iRows = rows;
	iCols = cols;
	int iTemp;

	for( ; iRows != 0; iRows-- )
  	{
		for (int n = 0; n < iCols; n++ )
		{
		  iTemp = vetA[n  ] - vetB[n  ];
		  uiSum += Distortion(( iTemp * iTemp ));
		}
		vetA += iCols;
		vetB += iCols;
	}
  return ( uiSum );
}

int satd(int *piOrg, int *piCur, int rows, int cols){

	unsigned int uiSum = 0;
	iRows = rows;
	iCols = cols;
	int iTemp;
	int iStep = 1;
	int iStrideOrg = cols;
	int iStrideCur = cols;
	int x,y;

  if( ( iRows % 8 == 0) && (iCols % 8 == 0) )
  {
    int  iOffsetOrg = iCols<<3;
    int  iOffsetCur = iCols<<3;
    for ( y=0; y<iRows; y+= 8 )
    {
      for ( x=0; x<iCols; x+= 8 )
      {
        uiSum += xCalcHADs8x8( &piOrg[x], &piCur[x*iStep], iStrideOrg, iStrideCur, iStep );
      }
      piOrg += iOffsetOrg;
      piCur += iOffsetCur;
    }
  }
  else if( ( iRows % 4 == 0) && (iCols % 4 == 0) )
  {
    int  iOffsetOrg = iStrideOrg<<2;
    int  iOffsetCur = iStrideCur<<2;

    for ( y=0; y<iRows; y+= 4 )
    {
      for ( x=0; x<iCols; x+= 4 )
      {
        uiSum += xCalcHADs4x4( &piOrg[x], &piCur[x*iStep], iStrideOrg, iStrideCur, iStep );
      }
      piOrg += iOffsetOrg;
      piCur += iOffsetCur;
    }
  }
  else if( ( iRows % 2 == 0) && (iCols % 2 == 0) )
  {
    int  iOffsetOrg = iStrideOrg<<1;
    int  iOffsetCur = iStrideCur<<1;
    for ( y=0; y<iRows; y+=2 )
    {
      for ( x=0; x<iCols; x+=2 )
      {
        uiSum += xCalcHADs2x2( &piOrg[x], &piCur[x*iStep], iStrideOrg, iStrideCur, iStep );
      }
      piOrg += iOffsetOrg;
      piCur += iOffsetCur;
    }
  }
  else
  {
    assert(false);
  }

  return ( uiSum );
}
