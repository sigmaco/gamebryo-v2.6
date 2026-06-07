// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef MDTANIM_H
#define MDTANIM_H

void gAnimDataGetCSlopeIn( int iShapeID, int iGroupID, int iFrame,
    float *pIn);

void gAnimDataGetCSlopeOut( int iShapeID, int iGroupID, int iFrame, 
    float *pOut);

int  gAnimDataGetCKeyFrames(int iShapeID, int iGroupID);
float gAnimDataGetCKeyFrameTime(int iShapeID,int iGroupID, int iFrame);
void gAnimDataGetCKeyFrameValue( int iShapeID, int iGroupID, int iFrame, 
    float *fvals);

void gAnimDataReset(void);
int gAnimDataAnimNew(void);

void gAnimDataGetSlopeInOut( int iShapeID, int iCurveType, int iFrame, 
    float *pIn, float *pOut);

int    gAnimDataGetNumKeyFrames(int iShapeID, int iCurveType );
float  gAnimDataGetKeyFrameTime(int iShapeID, int iCurveType, int iFrame);
double gAnimDataGetKeyFrameValue(int iShapeID, int iCurveType, int iFrame);

bool   gAnimDataIsShapeAnimated(int iShapeID, bool *bPos, bool *bRot);

double gAnimControlGetPlayBackSpeed(void);
double gAnimControlGetFramesPerSecond(void);
double gAnimControlGetMinTime(void);
double gAnimControlGetMaxTime(void);
int    gAnimControlGetPlayBackMode(void);

#endif