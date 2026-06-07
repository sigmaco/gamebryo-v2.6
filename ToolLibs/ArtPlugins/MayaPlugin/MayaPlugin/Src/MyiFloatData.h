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

//---------------------------------------------------------------------------

#include <NiAnimation.h>
#include <maya/MStatus.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <maya/MFnAnimCurve.h>

bool PlugIsConnected(const MPlug& kPlug);
bool PlugIsConnected(MFnDependencyNode& kNode, const char* pcAttributeName);
bool PlugIsConnected(MObject& kObject, const char* pcAttributeName);

MStatus ConvertFloatAnim(const MPlug& kPlug, unsigned int& uiNumKeys, 
    NiFloatKey*& pkKeys, NiAnimationKey::KeyType& eType, 
    float fScale = 1.0f);

bool AllKeysAreStepped(const MFnAnimCurve& kAnimCurve);

bool OneKeyIsStepped(const MFnAnimCurve& kAnimCurve);

bool AllKeysAreLinear(const MFnAnimCurve& kAnimCurve);

MStatus CreateNoStepLinFloatAnim(const MFnAnimCurve& kAnimCurve, 
    MFnAnimCurve& kNewAnimCurve);

MStatus ConvertStepFloatAnim(const MFnAnimCurve& kAnimCurve, 
    NiFloatKey*& pkKeys, float fScale = 1.0f);

MStatus ConvertLinFloatAnim(const MFnAnimCurve& kAnimCurve, 
    NiFloatKey*& pkKeys, float fScale = 1.0f);

MStatus ConvertBezFloatAnim(const MFnAnimCurve& kAnimCurve, 
    NiFloatKey*& pkKeys, float fScale = 1.0f);

MStatus ConvertPlugToAnimCurve(const MPlug& kPlug, bool& bAnimated,
        MFnAnimCurve& kAnimCurve);

MStatus ConvertPosAnim(const MPlug& kPlugX, const MPlug& kPlugY, 
    const MPlug& kPlugZ, unsigned int& uiNumKeys, NiPosKey*& pkKeys, 
    NiAnimationKey::KeyType& eType);

unsigned int GetNumUnifiedKeys(const MFnAnimCurve& kAnimCurveX, 
    const MFnAnimCurve& kAnimCurveY, const MFnAnimCurve& kAnimCurveZ,
    bool bAnimX, bool bAnimY, bool bAnimZ);

MStatus ConvertLinPosAnim(const MFnAnimCurve& kAnimCurveX, 
    const MFnAnimCurve& kAnimCurveY, const MFnAnimCurve& kAnimCurveZ,
    bool bAnimX, bool bAnimY, bool bAnimZ, double dDefaultX, double dDefaultY,
    double dDefaultZ, NiPosKey*& pkKeys, unsigned int& uiNumKeys);

MStatus ConvertBezPosAnim(const MFnAnimCurve& kAnimCurveX, 
    const MFnAnimCurve& kAnimCurveY, const MFnAnimCurve& kAnimCurveZ, 
    bool bAnimX, bool bAnimY, bool bAnimZ, double dDefaultX, double dDefaultY,
    double dDefaultZ, NiPosKey*& pkKeys, unsigned int& uiNumKeys);

bool DoubleCloseTo(double dA, double dB, double dEpsilon);

bool DoubleCloseTo(double dA, double dB);

void GetCurveKey(const MFnAnimCurve& kAnimCurve, unsigned int uiIndex,
                 double& dValue, double& dInSlope, double& dOutSlope );

void InterpolateAnimCurve(const MFnAnimCurve& kAnimCurve, 
        MTime kTime, double& dValue, double& dInSlope, double& dOutSlope,
        bool bCreateSteppedKey);

MStatus ConvertColorAlphaAnim(const MPlug& kPlugR, const MPlug& kPlugG, 
     const MPlug& kPlugB, const MPlug& kPlugA, unsigned int& uiNumKeys, 
     NiColorKey*& pkKeys, NiAnimationKey::KeyType& eType);

MStatus ConvertColorAnim(const MPlug& kPlugR, const MPlug& kPlugG, 
     const MPlug& kPlugB, unsigned int& uiNumKeys, 
     NiColorKey*& pkKeys, NiAnimationKey::KeyType& eType);


unsigned int GetNumUnifiedKeys(const MFnAnimCurve& kAnimCurveR, 
    const MFnAnimCurve& kAnimCurveG, const MFnAnimCurve& kAnimCurveB,
    const MFnAnimCurve& kAnimCurveA, bool bAnimR, bool bAnimG, bool bAnimB,
    bool bAnimA);

MStatus ConvertLinColorAAnim(const MFnAnimCurve& kAnimCurveR,
    const MFnAnimCurve& kAnimCurveG, const MFnAnimCurve& kAnimCurveB,
    const MFnAnimCurve& kAnimCurveA, bool bAnimR, bool bAnimG, bool bAnimB,
    bool bAnimA, double dDefaultR, double dDefaultG, double dDefaultB,
    double dDefaultA, NiColorKey*& pkKeys, unsigned int& uiNumKeys);

bool CreateFloatInterpolator(MFnDependencyNode& kObject, 
    const char* pcAttributeName, NiFloatInterpolator*& pkInterp,
    float fScale = 1.0f);

bool CreateFloatInterpolator(MObject& kObject, 
    const char* pcAttributeName, NiFloatInterpolator*& pkInterp,
    float fScale = 1.0f);

NiTimeController::CycleType GetAnimationPostCycleType(MPlug& kPlug,
        MStatus& kStat);