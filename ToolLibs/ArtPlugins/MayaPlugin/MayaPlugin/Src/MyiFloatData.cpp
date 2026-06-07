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

// Precompiled Headers
#include "MayaPluginPCH.h"


//---------------------------------------------------------------------------

bool PlugIsConnected(const MPlug& kPlug)
{
    // Check to see if it is connected
    if (!kPlug.isConnected())
    {
        return false;
    }


    MPlugArray kPlugArr;
    MStatus kStatus;
    kPlug.connectedTo(kPlugArr, true, false, &kStatus);

    if (kStatus != MS::kSuccess)
        return false;

    return (kPlugArr.length() > 0);
}

//---------------------------------------------------------------------------

bool PlugIsConnected(MFnDependencyNode& kNode, const char* pcAttributeName)
{
    MStatus kStatus;

    // Read the Plug
    MPlug kMPlug = kNode.findPlug(MString(pcAttributeName), &kStatus);

    if (kStatus != MS::kSuccess)
        return false;

    return PlugIsConnected(kMPlug);
}

//---------------------------------------------------------------------------

bool PlugIsConnected(MObject& kObject, const char* pcAttributeName)
{
    MStatus kStatus;

    MFnDependencyNode kNode(kObject, &kStatus);
    if (kStatus != MS::kSuccess)
        return false;

    return PlugIsConnected(kNode, pcAttributeName);
}

//---------------------------------------------------------------------------

MStatus ConvertFloatAnim(const MPlug& kPlug, unsigned int& uiNumKeys, 
    NiFloatKey*& pkKeys, NiAnimationKey::KeyType& eType, 
    float fScale/* = 1.0f*/)
{
    uiNumKeys = 0;
    pkKeys = 0;
    eType = NiAnimationKey::NOINTERP;

    // ConvertFloatAnim converts a keyed MFnNumericAttrbute to a set of 
    // NiFloatKeys


    MPlugArray kPlugArr;
    MStatus kStatus;
    kPlug.connectedTo(kPlugArr, true, false, &kStatus);

    if (kStatus != MS::kSuccess)
        return kStatus;

    // I think the apiType of kNumericAttribute guarantees
    // that there will only be one connection, but
    // I'm not sure.  EH.
    if (kPlugArr.length() == 0)
        return MS::kFailure;

    MObject kDestObj = kPlugArr[0].node();

    MFnAnimCurve kAnimCurve(kDestObj, &kStatus);
    if (kStatus != MS::kSuccess)
        return kStatus;

    uiNumKeys = kAnimCurve.numKeyframes();
    NIASSERT(uiNumKeys);
    if (uiNumKeys == 0)
        return MS::kFailure;

    // Caller must check if the plug is connected.
    NIASSERT(kPlug.isConnected());

    bool bStep = AllKeysAreStepped(kAnimCurve);

    if (bStep)
    {
        pkKeys = NiNew NiStepFloatKey[uiNumKeys];

        eType = NiAnimationKey::STEPKEY;
        return ConvertStepFloatAnim(kAnimCurve, pkKeys, fScale);
    }

    MFnAnimCurve kConvertedStepCurve;
    kConvertedStepCurve.create(kAnimCurve.animCurveType());
    
    CreateNoStepLinFloatAnim(kAnimCurve, kConvertedStepCurve);

    // Reset the number of key frames
    uiNumKeys = kConvertedStepCurve.numKeyframes();

    bool bLinear = AllKeysAreLinear(kConvertedStepCurve);
    if (bLinear)
    {
        pkKeys = NiNew NiLinFloatKey[uiNumKeys];

        eType = NiAnimationKey::LINKEY;
        return ConvertLinFloatAnim(kConvertedStepCurve, pkKeys, fScale);
    }
    else
    {
        pkKeys = NiNew NiBezFloatKey[uiNumKeys];

        eType = NiAnimationKey::BEZKEY;
        return ConvertBezFloatAnim(kConvertedStepCurve, pkKeys, fScale);
    }
}

//---------------------------------------------------------------------------

bool AllKeysAreStepped(const MFnAnimCurve& kAnimCurve)
{
    unsigned int uiNumKeys = kAnimCurve.numKeyframes();

    unsigned int i;
    for (i = 0; i < uiNumKeys-1; i++)
    {
        if (kAnimCurve.outTangentType(i) != MFnAnimCurve::kTangentStep)
            return false;
    }
    return true;
}

//---------------------------------------------------------------------------

bool OneKeyIsStepped(const MFnAnimCurve& kAnimCurve)
{
    unsigned int uiNumKeys = kAnimCurve.numKeyframes();

    unsigned int i;
    for (i = 0; i < uiNumKeys-1; i++)
    {
        if (kAnimCurve.outTangentType(i) == MFnAnimCurve::kTangentStep)
            return true;
    }

    return false;
}

//---------------------------------------------------------------------------

bool AllKeysAreLinear(const MFnAnimCurve& kAnimCurve)
{
    unsigned int uiNumKeys = kAnimCurve.numKeyframes();

    unsigned int i;
    for (i = 0; i < uiNumKeys; i++)
    {
        if ((i == 0) && 
            (kAnimCurve.outTangentType(i) != MFnAnimCurve::kTangentLinear))
            return false;

        if ((i == uiNumKeys - 1) && 
            (kAnimCurve.inTangentType(i) != MFnAnimCurve::kTangentLinear))
            return false;

        if ( (i > 0) && (i < uiNumKeys -1))
        {
            if ((kAnimCurve.inTangentType(i) != 
                    MFnAnimCurve::kTangentLinear) ||
                (kAnimCurve.outTangentType(i) != 
                    MFnAnimCurve::kTangentLinear))
                return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------

MStatus CreateNoStepLinFloatAnim(const MFnAnimCurve& kAnimCurve, 
    MFnAnimCurve& kNewAnimCurve)
{
    unsigned int uiNumKeys = kAnimCurve.numKeyframes();

    unsigned int i;
    for (i = 0; i < uiNumKeys; i++)
    {   
        MFnAnimCurve::TangentType kInType = kAnimCurve.inTangentType(i);

        if ((i > 0) &&
            (kAnimCurve.outTangentType(i-1) == MFnAnimCurve::kTangentStep))
        {
            kInType = MFnAnimCurve::kTangentLinear;
        }

            // Create a Step extra key
        if ((i < uiNumKeys - 1) &&
            (kAnimCurve.outTangentType(i) == MFnAnimCurve::kTangentStep))
        {
            // Add a key into the curve
            kNewAnimCurve.addKeyframe(kAnimCurve.time(i),
                kAnimCurve.value(i), kInType, MFnAnimCurve::kTangentLinear);

            // Compute the new time Maya can't represent anything smaller then
            // 1/6000th of a second
            MTime kNewTime = kAnimCurve.time(i+1);
            MTime kDelta(0.2, MTime::kMilliseconds);
            kNewTime -= kDelta.as(kNewTime.unit());

            // Add the ending key
            kNewAnimCurve.addKeyframe( kNewTime,
                kAnimCurve.value(i), MFnAnimCurve::kTangentLinear, 
                MFnAnimCurve::kTangentLinear);
        }
        else 
        {
            // Add a key into the curve

            MTime kKeyFrameTime = kAnimCurve.time(i);
            double fSeconds = kKeyFrameTime.as(MTime::kSeconds);
            NI_UNUSED_ARG(fSeconds);

            double value = kAnimCurve.value(i);

            kNewAnimCurve.addKeyframe(kKeyFrameTime,
                value, kInType, 
                kAnimCurve.outTangentType(i));
        }

    }

    return MS::kSuccess;
}

//---------------------------------------------------------------------------

MStatus ConvertStepFloatAnim(const MFnAnimCurve& kAnimCurve, 
                            NiFloatKey*& pkKeys, float fScale /* = 1.0f*/)
{
    unsigned int uiNumKeys = kAnimCurve.numKeyframes();

    // Create a multiplier to do th inversion of the keys
    float fMultiplier = fScale;

    unsigned int i;
    for (i = 0; i < uiNumKeys; i++)
    {   
        NiStepFloatKey* pkStepFloatKey = (NiStepFloatKey*) pkKeys->GetKeyAt(
            i, sizeof(NiStepFloatKey));
        pkStepFloatKey->SetTime((float) kAnimCurve.time(i).as(
            MTime::kSeconds));
        pkStepFloatKey->SetValue((float) kAnimCurve.value(i) * fMultiplier);
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------

MStatus ConvertLinFloatAnim(const MFnAnimCurve& kAnimCurve, 
                            NiFloatKey*& pkKeys, float fScale/* = 1.0f*/)
{
    unsigned int uiNumKeys = kAnimCurve.numKeyframes();

    // Create a multiplier to do th inversion of the keys
    float fMultiplier = fScale;

    unsigned int i;
    for (i = 0; i < uiNumKeys; i++)
    {   
        NiLinFloatKey* pkLinFloatKey = (NiLinFloatKey*) pkKeys->GetKeyAt(i, 
            sizeof(NiLinFloatKey));

        pkLinFloatKey->SetTime((float) kAnimCurve.time(i).as(MTime::kSeconds));
        pkLinFloatKey->SetValue((float) kAnimCurve.value(i) * fMultiplier);
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
MStatus ConvertBezFloatAnim(const MFnAnimCurve& kAnimCurve, 
                            NiFloatKey*& pkKeys, float fScale/* = 1.0f*/)
{
    unsigned int uiNumKeys = kAnimCurve.numKeyframes();

    bool bWeightedTangents = kAnimCurve.isWeighted();
    NIASSERT(!bWeightedTangents);


    // Create a multiplier to do th inversion of the keys
    float fMultiplier = fScale;

    unsigned int i;
    for (i = 0; i < uiNumKeys; i++)
    {   
        // get key time
        float fTime = (float) kAnimCurve.time(i).as(MTime::kSeconds);

        MFnAnimCurve::TangentType kInType = kAnimCurve.inTangentType(i);
        MFnAnimCurve::TangentType kOutType = kAnimCurve.outTangentType(i);

        // handle tangents
        float fInTan = 0.0f;
        float fOutTan = 0.0f;

        // non-weighted tangents
        // these tangents are not normalized,
        float fInTanX, fInTanY, fOutTanX, fOutTanY;

        // Fixed Tangents can return bogus values
        if (kInType != MFnAnimCurve::kTangentFixed)
        {
            kAnimCurve.getTangent(i, fInTanX, fInTanY, true);
            fInTan = FindSlope(fInTanX, fInTanY);
        }

        // Fixed Tangents can return bogus values
        if (kOutType != MFnAnimCurve::kTangentFixed)
        {
            kAnimCurve.getTangent(i, fOutTanX, fOutTanY, false);
            fOutTan = FindSlope(fOutTanX, fOutTanY);
        }


        // create key
        NiBezFloatKey* pkBezFloatKey = (NiBezFloatKey*) pkKeys->GetKeyAt(i, 
            sizeof(NiBezFloatKey));

        pkBezFloatKey->SetTime(fTime);
        pkBezFloatKey->SetValue((float) kAnimCurve.value(i) * fMultiplier);
        pkBezFloatKey->SetInTan(fInTan * fMultiplier);
        pkBezFloatKey->SetOutTan(fOutTan * fMultiplier);
    }
  
    // normalize tangents
    if (uiNumKeys)
    {
        float fLastTime, fCurTime;

        fLastTime = fCurTime = pkKeys->GetKeyAt(0, 
            sizeof(NiBezFloatKey))->GetTime();

        NiBezFloatKey* pkCurKey = (NiBezFloatKey*) pkKeys->GetKeyAt(0, 
            sizeof(NiBezFloatKey));

        float fTimeDelIn = fCurTime - fLastTime;

        for (int i = 0; i < (int)uiNumKeys; i++)
        {
            NiBezFloatKey* pkNextKey;
            float fNextTime;

            if (i+1 < (int)uiNumKeys)
            {
                pkNextKey = (NiBezFloatKey*) pkKeys->GetKeyAt(i+1, 
                    sizeof(NiBezFloatKey));

                fNextTime = pkNextKey->GetTime();
            }
            else
            {
                pkNextKey = NULL;
                fNextTime = fCurTime;
            }

            float fTimeDelOut = fNextTime - fCurTime;

            float fIn = pkCurKey->GetInTan();
            pkCurKey->SetInTan(fIn*fTimeDelIn);
            float fOut = pkCurKey->GetOutTan();
            pkCurKey->SetOutTan(fOut*fTimeDelOut);

            fTimeDelIn = fTimeDelOut;
            fLastTime = fCurTime;
            fCurTime = fNextTime;
            pkCurKey = pkNextKey;
        }
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
MStatus ConvertPlugToAnimCurve(const MPlug& kPlug, bool& bAnimated,
        MFnAnimCurve& kAnimCurve)
{
    NIASSERT(kPlug.attribute().apiType() == MFn::kNumericAttribute);

    // check to see if any of the plugs are connected
    bAnimated = kPlug.isConnected();

    // Not Animated
    if (!bAnimated)
        return MS::kSuccess;

    // See what we are connected to
    MPlugArray kPlugArr;
    MStatus kStatus;
    kPlug.connectedTo(kPlugArr, true, false, &kStatus);

    if (kStatus != MS::kSuccess)
        return kStatus;

    // SHould only be connected to one thing
    if (kPlugArr.length() == 0)
    {
        bAnimated = false;
        return MS::kSuccess;
    }
    
    MObject kDestObj = kPlugArr[0].node();

    kStatus == kAnimCurve.setObject(kDestObj);
    if (kStatus != MS::kSuccess)
        return kStatus;

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
MStatus ConvertPosAnim(const MPlug& kPlugX, const MPlug& kPlugY, 
    const MPlug& kPlugZ, unsigned int& uiNumKeys, NiPosKey*& pkKeys, 
    NiAnimationKey::KeyType& eType)
{

    uiNumKeys = 0;
    pkKeys = 0;
    eType = NiAnimationKey::NOINTERP;


    double dDefaultX;
    double dDefaultY;
    double dDefaultZ;

    kPlugX.getValue(dDefaultX);
    kPlugY.getValue(dDefaultY);
    kPlugZ.getValue(dDefaultZ);

    // ConvertFloatAnim converts a keyed MFnNumericAttrbute to a set of 
    // NiFloatKeys

    bool bAnimX = false;
    bool bAnimY = false;
    bool bAnimZ = false;

    MFnAnimCurve kAnimCurveX;
    MFnAnimCurve kAnimCurveY;
    MFnAnimCurve kAnimCurveZ;

    if ( ConvertPlugToAnimCurve(kPlugX, bAnimX, kAnimCurveX) == MS::kFailure )
        return MS::kFailure;

    if ( ConvertPlugToAnimCurve(kPlugY, bAnimY, kAnimCurveY) == MS::kFailure )
        return MS::kFailure;

    if ( ConvertPlugToAnimCurve(kPlugZ, bAnimZ, kAnimCurveZ) == MS::kFailure )
        return MS::kFailure;




    bool bLinear = (!bAnimX || AllKeysAreLinear(kAnimCurveX));
    bLinear &= (!bAnimY || AllKeysAreLinear(kAnimCurveY));
    bLinear &= (!bAnimZ || AllKeysAreLinear(kAnimCurveZ));


    if (bLinear)
    {

        eType = NiAnimationKey::LINKEY;
        return ConvertLinPosAnim(kAnimCurveX, kAnimCurveY, kAnimCurveZ, 
                    bAnimX, bAnimY, bAnimZ, dDefaultX, dDefaultY, dDefaultZ,
                    pkKeys, uiNumKeys);
    }
    else
    {
        eType = NiAnimationKey::BEZKEY;
        return ConvertBezPosAnim(kAnimCurveX, kAnimCurveY, kAnimCurveZ, 
                    bAnimX, bAnimY, bAnimZ, dDefaultX, dDefaultY, dDefaultZ,
                    pkKeys, uiNumKeys);
    }

    //return MS::kSuccess;
}
//---------------------------------------------------------------------------
bool CheckForExtraSteppedKey(const MFnAnimCurve& kAnimCurveX, 
                             const MFnAnimCurve& kAnimCurveY,
                             const MFnAnimCurve& kAnimCurveZ,
                             unsigned int uiX,
                             unsigned int uiY,
                             unsigned int uiZ,
                             double dTimeX,
                             double dTimeY,
                             double dTimeZ,
                             double dMinTime)
{
    // Check for Stepped Tangents
    if ( (uiX > 0) && 
        DoubleCloseTo(dTimeX, dMinTime) &&
        (kAnimCurveX.outTangentType(uiX-1) == MFnAnimCurve::kTangentStep))
    {
        return true;
    }

    if ( (uiY > 0) && 
        DoubleCloseTo(dTimeY, dMinTime) &&
        (kAnimCurveY.outTangentType(uiY-1) == MFnAnimCurve::kTangentStep))
    {
        return true;
    }

    if ( (uiZ > 0) && 
        DoubleCloseTo(dTimeZ, dMinTime) &&
        (kAnimCurveZ.outTangentType(uiZ-1) == MFnAnimCurve::kTangentStep))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned int GetNumUnifiedKeys(const MFnAnimCurve& kAnimCurveX, 
                               const MFnAnimCurve& kAnimCurveY,
                               const MFnAnimCurve& kAnimCurveZ,
                               bool bAnimX,
                               bool bAnimY,
                               bool bAnimZ)
{
    // Count the number of Unique keys
    unsigned int uiNumKeysX = 0;
    unsigned int uiNumKeysY = 0;
    unsigned int uiNumKeysZ = 0;

    if (bAnimX)
    {
        uiNumKeysX = kAnimCurveX.numKeyframes();
    }

    if (bAnimY)
    {
        uiNumKeysY = kAnimCurveY.numKeyframes();
    }

    if (bAnimZ)
    {
        uiNumKeysZ = kAnimCurveZ.numKeyframes();
    }

    double dMinTime;
    double dTimeX = NI_INFINITY;
    double dTimeY = NI_INFINITY;
    double dTimeZ = NI_INFINITY;

    MStatus kStatus;
    MTime kTime;
    unsigned int uiNumKeys = 0;

    unsigned int uiX = 0;
    unsigned int uiY = 0;
    unsigned int uiZ = 0;

    while ((uiX < uiNumKeysX) || (uiY < uiNumKeysY) || (uiZ < uiNumKeysZ))
    {
        if ((bAnimX) && (uiX < uiNumKeysX))
        {
            dTimeX = kAnimCurveX.time(uiX, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimY) && (uiY < uiNumKeysY))
        {
            dTimeY = kAnimCurveY.time(uiY, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimZ) && (uiZ < uiNumKeysZ))
        {
            dTimeZ = kAnimCurveZ.time(uiZ, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        dMinTime = min( min(dTimeX, dTimeY), dTimeZ);

        uiNumKeys++;

            // Check for the extra key created by a stepped tangent
        if (CheckForExtraSteppedKey(kAnimCurveX, kAnimCurveY, kAnimCurveZ,
            uiX, uiY, uiZ, dTimeX, dTimeY, dTimeZ, dMinTime))
        {
            uiNumKeys++;
        }

        if (DoubleCloseTo(dTimeX, dMinTime))
        {
            uiX++;

            if (uiX == uiNumKeysX)
            {
                dTimeX = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeY, dMinTime))
        {
            uiY++;

            if (uiY == uiNumKeysY)
            {
                dTimeY = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeZ, dMinTime))
        {
            uiZ++;

            if (uiZ == uiNumKeysZ)
            {
                dTimeZ = NI_INFINITY;
            }
        }
    }

    return uiNumKeys;
}
//---------------------------------------------------------------------------
MStatus ConvertLinPosAnim(const MFnAnimCurve& kAnimCurveX, 
                          const MFnAnimCurve& kAnimCurveY,
                          const MFnAnimCurve& kAnimCurveZ,
                          bool bAnimX,
                          bool bAnimY,
                          bool bAnimZ,
                          double dDefaultX,
                          double dDefaultY,
                          double dDefaultZ,
                          NiPosKey*& pkKeys,
                          unsigned int& uiNumKeys)
{

    // Examine the curves and determine the total number of unique unified keys
    uiNumKeys = GetNumUnifiedKeys(kAnimCurveX, kAnimCurveY, kAnimCurveZ,
                               bAnimX, bAnimY, bAnimZ);

    // Allocate the Keys
    pkKeys = NiNew NiLinPosKey[uiNumKeys];

    // Get the number of keys for each axis
    unsigned int uiNumKeysX = 0;
    unsigned int uiNumKeysY = 0;
    unsigned int uiNumKeysZ = 0;

    if (bAnimX)
    {
        uiNumKeysX = kAnimCurveX.numKeyframes();
    }

    if (bAnimY)
    {
        uiNumKeysY = kAnimCurveY.numKeyframes();
    }

    if (bAnimZ)
    {
        uiNumKeysZ = kAnimCurveZ.numKeyframes();
    }

    // Initialize the default Times and Values
    double dMinTime;
    double dTimeX = NI_INFINITY;
    double dTimeY = NI_INFINITY;
    double dTimeZ = NI_INFINITY;

    float fX = (float)dDefaultX;
    float fY = (float)dDefaultY;
    float fZ = (float)dDefaultZ;

    MStatus kStatus;
    MTime kTime;


    int iCurrentKey = 0;
    unsigned int uiX = 0;
    unsigned int uiY = 0;
    unsigned int uiZ = 0;
    while ((uiX < uiNumKeysX) || (uiY < uiNumKeysY) || (uiZ < uiNumKeysZ))
    {
        if ((bAnimX) && (uiX < uiNumKeysX))
        {
            dTimeX = kAnimCurveX.time(uiX, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimY) && (uiY < uiNumKeysY))
        {
            dTimeY = kAnimCurveY.time(uiY, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimZ) && (uiZ < uiNumKeysZ))
        {
            dTimeZ = kAnimCurveZ.time(uiZ, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        // Determine the minimum time for all Animation Curve
        dMinTime = min( min(dTimeX, dTimeY), dTimeZ);

        MTime kTime(dMinTime, MTime::kSeconds);

        // Find the Value of each Animation curve at the minimum time
        if (bAnimX)
        {
            fX = (float)kAnimCurveX.evaluate(kTime, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);
        }

        if (bAnimY)
        {
            fY = (float)kAnimCurveY.evaluate(kTime, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);
        }

        if (bAnimZ)
        {
            fZ = (float)kAnimCurveZ.evaluate(kTime, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);
        }

        // Create a Keys at the minimum time
        pkKeys[iCurrentKey].SetTime((float)dMinTime);
        pkKeys[iCurrentKey++].SetPos(NiPoint3(fX, fY, fZ));

        // Increment the current key for each animation if it is close to the 
        // minimum time.
        if (DoubleCloseTo(dTimeX, dMinTime))
        {
            uiX++;

            if (uiX == uiNumKeysX)
            {
                dTimeX = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeY, dMinTime))
        {
            uiY++;

            if (uiY == uiNumKeysY)
            {
                dTimeY = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeZ, dMinTime))
        {
            uiZ++;

            if (uiZ == uiNumKeysZ)
            {
                dTimeZ = NI_INFINITY;
            }
        }
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
MStatus ConvertBezPosAnim(const MFnAnimCurve& kAnimCurveX, 
                          const MFnAnimCurve& kAnimCurveY,
                          const MFnAnimCurve& kAnimCurveZ,
                          bool bAnimX,
                          bool bAnimY,
                          bool bAnimZ,
                          double dDefaultX,
                          double dDefaultY,
                          double dDefaultZ,
                          NiPosKey*& pkKeys,
                          unsigned int& uiNumKeys)
{

    // Examine the curves and determine the total number of unique unified 
    // keys
    uiNumKeys = GetNumUnifiedKeys(kAnimCurveX, kAnimCurveY, kAnimCurveZ,
                               bAnimX, bAnimY, bAnimZ);

    // Allocate the Keys
    NiBezPosKey* pkBezKeys = NiNew NiBezPosKey[uiNumKeys];
    pkKeys = (NiPosKey*)pkBezKeys;

    // Get the number of keys for each axis
    unsigned int uiNumKeysX = 0;
    unsigned int uiNumKeysY = 0;
    unsigned int uiNumKeysZ = 0;

    if (bAnimX)
    {
        uiNumKeysX = kAnimCurveX.numKeyframes();
    }

    if (bAnimY)
    {
        uiNumKeysY = kAnimCurveY.numKeyframes();
    }

    if (bAnimZ)
    {
        uiNumKeysZ = kAnimCurveZ.numKeyframes();
    }

    // Initialize the default Times and Values
    double dMinTime;
    double dTimeX = NI_INFINITY;
    double dTimeY = NI_INFINITY;
    double dTimeZ = NI_INFINITY;

    double dX = dDefaultX;
    double dY = dDefaultY;
    double dZ = dDefaultZ;

    double dInX = 0.0f;
    double dInY = 0.0f;
    double dInZ = 0.0f;

    double dOutX = 0.0f;
    double dOutY = 0.0f;
    double dOutZ = 0.0f;

    MStatus kStatus;
    MTime kTime;


    int iCurrentKey = 0;
    unsigned int uiX = 0;
    unsigned int uiY = 0;
    unsigned int uiZ = 0;
    while ((uiX < uiNumKeysX) || (uiY < uiNumKeysY) || (uiZ < uiNumKeysZ))
    {
        if ((bAnimX) && (uiX < uiNumKeysX))
        {
            dTimeX = kAnimCurveX.time(uiX, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimY) && (uiY < uiNumKeysY))
        {
            dTimeY = kAnimCurveY.time(uiY, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimZ) && (uiZ < uiNumKeysZ))
        {
            dTimeZ = kAnimCurveZ.time(uiZ, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        // Determine the minimum time for all Animation Curve
        dMinTime = min( min(dTimeX, dTimeY), dTimeZ);

        MTime kTime(dMinTime, MTime::kSeconds);


        // Handle the Extra key created by a stepped tangent
        if (CheckForExtraSteppedKey( kAnimCurveX, kAnimCurveY, kAnimCurveZ,
                uiX, uiY, uiZ, dTimeX, dTimeY, dTimeZ, dMinTime))
        {
            // Find the Value of each Animation curve at the minimum time
            if (bAnimX)
            {
                InterpolateAnimCurve(kAnimCurveX, kTime, dX, dInX, dOutX, 
                    true);
            }
            
            if (bAnimY)
            {
                InterpolateAnimCurve(kAnimCurveY, kTime, dY, dInY, dOutY, 
                    true);
            }
            
            if (bAnimZ)
            {
                InterpolateAnimCurve(kAnimCurveZ, kTime, dZ, dInZ, dOutZ, 
                    true);
            }
            
            // Create a Keys at the minimum time
            pkBezKeys[iCurrentKey].SetTime((float)(dMinTime - 0.001));
            pkBezKeys[iCurrentKey].SetPos(NiPoint3((float)dX, (float)dY, 
                (float)dZ));

            pkBezKeys[iCurrentKey].SetInTan(NiPoint3((float)dInX, 
                (float)dInY, (float)dInZ));

            pkBezKeys[iCurrentKey].SetOutTan(NiPoint3((float)dOutX, 
                (float)dOutY, (float)dOutZ));

            iCurrentKey++;
        }


        // Find the Value of each Animation curve at the minimum time
        if (bAnimX)
        {
            InterpolateAnimCurve(kAnimCurveX, kTime, dX, dInX, dOutX, false);
        }

        if (bAnimY)
        {
            InterpolateAnimCurve(kAnimCurveY, kTime, dY, dInY, dOutY, false);
        }

        if (bAnimZ)
        {
            InterpolateAnimCurve(kAnimCurveZ, kTime, dZ, dInZ, dOutZ, false);
        }

        // Create a Keys at the minimum time
        pkBezKeys[iCurrentKey].SetTime((float)dMinTime);
        pkBezKeys[iCurrentKey].SetPos(NiPoint3((float)dX, (float)dY, 
            (float)dZ));

        pkBezKeys[iCurrentKey].SetInTan(NiPoint3((float)dInX, (float)dInY, 
            (float)dInZ));

        pkBezKeys[iCurrentKey].SetOutTan(NiPoint3((float)dOutX, (float)dOutY, 
            (float)dOutZ));

        iCurrentKey++;


        // Increment the current key for each animation if it is close to the 
        // minimum time.
        if (DoubleCloseTo(dTimeX, dMinTime))
        {
            uiX++;

            if (uiX == uiNumKeysX)
            {
                dTimeX = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeY, dMinTime))
        {
            uiY++;

            if (uiY == uiNumKeysY)
            {
                dTimeY = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeZ, dMinTime))
        {
            uiZ++;

            if (uiZ == uiNumKeysZ)
            {
                dTimeZ = NI_INFINITY;
            }
        }
    }


    // Fill the Derived values for the Keys
    NiPosKey::FillDerivedValsFunction pDerived;
    pDerived = NiPosKey::GetFillDerivedFunction(NiPosKey::BEZKEY);
    NIASSERT(pDerived);
    (*pDerived)((NiAnimationKey *)pkBezKeys, uiNumKeys, sizeof(NiBezPosKey));

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
bool DoubleCloseTo(double dA, double dB, double dEpsilon)
{
    return ( fabs(dA - dB) <= dEpsilon);
}
//---------------------------------------------------------------------------
bool DoubleCloseTo(double dA, double dB)
{
    double dEpsilon = dA * 0.00001;

    return ( fabs(dA - dB) <= dEpsilon);
}
//---------------------------------------------------------------------------
void GetCurveKey(const MFnAnimCurve& kAnimCurve, unsigned int uiIndex,
                 double& dValue, double& dInSlope, double& dOutSlope )
{
    MStatus kStatus;
    float fMaxTan = 5729577.9485111479f;

    dValue = kAnimCurve.value(uiIndex, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // Create the tangent values
    float fInX;
    float fInY;
    float fOutX;
    float fOutY;


    // Compute the In Tangent slopes
    kStatus = kAnimCurve.getTangent(uiIndex, fInX, fInY, true);
    NIASSERT(kStatus == MS::kSuccess);

    if (DoubleCloseTo((double)fInX, 0.0))
        dInSlope = fMaxTan;
    else
        dInSlope = fInY / fInX;

    // Check the Tangent Types
    MFnAnimCurve::TangentType kOutType = 
        kAnimCurve.outTangentType(uiIndex, &kStatus);

    NIASSERT(kStatus == MS::kSuccess);

    if (kOutType == MFnAnimCurve::kTangentStep)
    {
        dOutSlope = 0.0;
    }
    else
    {
        // Compute the Out Tangent slopes
        kStatus = kAnimCurve.getTangent(uiIndex, fOutX, fOutY, false);
        NIASSERT(kStatus == MS::kSuccess);
        
        if (DoubleCloseTo((double)fOutX, 0.0))
            dOutSlope = fMaxTan;
        else
            dOutSlope = fOutY / fOutX;
    }
}
//---------------------------------------------------------------------------
void InterpolateAnimCurve(const MFnAnimCurve& kAnimCurve, 
        MTime kTime, double& dValue, double& dInSlope, double& dOutSlope,
        bool bCreatingSteppedKey)
{
    MStatus kStatus;

    unsigned int uiLower = kAnimCurve.findClosest(kTime, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // Check for the time match
    if (DoubleCloseTo( kTime.as(MTime::kSeconds), 
        kAnimCurve.time(uiLower, &kStatus).as(MTime::kSeconds)))
    {
        if (!bCreatingSteppedKey)
        {
            // Just get the regular key
            GetCurveKey(kAnimCurve, uiLower, dValue, dInSlope, dOutSlope);
            return;
        }

        // Check for Creating a stepped key
        if ((uiLower > 0) &&
            (kAnimCurve.outTangentType(uiLower-1) == 
                MFnAnimCurve::kTangentStep))
        {
            dValue = kAnimCurve.value(uiLower-1, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);
            dInSlope = 0.0;
            dOutSlope = 0.0;
            return;
        }

    }


    // Roll Time back a little to create the stepped key
    if (bCreatingSteppedKey)
    {
        kTime -= MTime(0.001, MTime::kSeconds);
    }


    // Check for Before the beginning of the curve
    if (kTime < kAnimCurve.time(0, &kStatus))
    {
        dValue = kAnimCurve.value(0, &kStatus);
        
        // Clear the Slopes to Flat
        dInSlope = 0.0;
        dOutSlope = 0.0;
        return;
    }

    // Check for After the End of the Curve
    if (kTime > kAnimCurve.time( kAnimCurve.numKeyframes() - 1, &kStatus))
    {
        dValue = kAnimCurve.value(kAnimCurve.numKeyframes() - 1, &kStatus);
        
        // Clear the Slopes to Flat
        dInSlope = 0.0;
        dOutSlope = 0.0;
        return;
    }

    // Check for Before the Closest time
    if (kTime < kAnimCurve.time(uiLower, &kStatus))
    {
        uiLower--;
    }

    unsigned int uiUpper = uiLower + 1;

    // Get the Key Values
    double dLTime = kAnimCurve.time(uiLower, &kStatus).as(MTime::kSeconds);
    NIASSERT(kStatus == MS::kSuccess);

    double dUTime = kAnimCurve.time(uiUpper, &kStatus).as(MTime::kSeconds);
    NIASSERT(kStatus == MS::kSuccess);

    double dLValue;
    double dLInSlope;
    double dLOutSlope;
    GetCurveKey(kAnimCurve, uiLower, dLValue, dLInSlope, dLOutSlope);

    double dUValue;
    double dUInSlope;
    double dUOutSlope;
    GetCurveKey(kAnimCurve, uiUpper, dUValue, dUInSlope, dUOutSlope);

    // Calculate Delta Values
    double dOldDeltaX = dUValue - dLValue;
    double dOldDeltaT = dUTime - dLTime;
    double dNewDeltaTA = kTime.as(MTime::kSeconds) - dLTime;
    double dNewDeltaTB = dUTime - kTime.as(MTime::kSeconds);
    NI_UNUSED_ARG(dNewDeltaTB);

    // Calculate Normalized Time
    double t = dNewDeltaTA / dOldDeltaT;

    // Premultiply the Slopes because the algorithm only works with 
    // Normalized tangents
    dLOutSlope *= dOldDeltaT;
    dUInSlope *= dOldDeltaT;

    double a = -2.0 * dOldDeltaX + dLOutSlope + dUInSlope;
    double b = 3.0 * dOldDeltaX - 2.0 * dLOutSlope - dUInSlope;
    double c = dLOutSlope;
    double d = dLValue;

    // Calculate the new Result
    dValue = ((a * t + b) * t + c) * t + d;

    // Calculate the Tangents
    dInSlope = (((3.0 * a * t + 2.0 * b) * t + dLOutSlope) / dOldDeltaT);
    dOutSlope = dInSlope;

}
//---------------------------------------------------------------------------
MStatus ConvertColorAlphaAnim(const MPlug& kPlugR, const MPlug& kPlugG, 
     const MPlug& kPlugB, const MPlug& kPlugA, unsigned int& uiNumKeys, 
     NiColorKey*& pkKeys, NiAnimationKey::KeyType& eType)
{

    uiNumKeys = 0;
    pkKeys = 0;
    eType = NiAnimationKey::NOINTERP;


    double dDefaultR;
    double dDefaultG;
    double dDefaultB;
    double dDefaultA;

    kPlugR.getValue(dDefaultR);
    kPlugG.getValue(dDefaultG);
    kPlugB.getValue(dDefaultB);
    kPlugA.getValue(dDefaultA);

    // ConvertFloatAnim converts a keyed MFnNumericAttrbute to a set of 
    // NiFloatKeys

    bool bAnimR = false;
    bool bAnimG = false;
    bool bAnimB = false;
    bool bAnimA = false;

    MFnAnimCurve kAnimCurveR;
    MFnAnimCurve kAnimCurveG;
    MFnAnimCurve kAnimCurveB;
    MFnAnimCurve kAnimCurveA;

    if (ConvertPlugToAnimCurve(kPlugR, bAnimR, kAnimCurveR) == MS::kFailure)
        return MS::kFailure;

    if (ConvertPlugToAnimCurve(kPlugG, bAnimG, kAnimCurveG) == MS::kFailure)
        return MS::kFailure;

    if (ConvertPlugToAnimCurve(kPlugB, bAnimB, kAnimCurveB) == MS::kFailure)
        return MS::kFailure;

    if (ConvertPlugToAnimCurve(kPlugA, bAnimA, kAnimCurveA) == MS::kFailure)
        return MS::kFailure;


    bool bLinear = (!bAnimR || AllKeysAreLinear(kAnimCurveR));
    bLinear &= (!bAnimG || AllKeysAreLinear(kAnimCurveG));
    bLinear &= (!bAnimB || AllKeysAreLinear(kAnimCurveB));
    bLinear &= (!bAnimA || AllKeysAreLinear(kAnimCurveA));

    // Default to true because there are no Bez Color Keys
    bLinear = true;

    if (bLinear)
    {

        eType = NiAnimationKey::LINKEY;
        return ConvertLinColorAAnim(kAnimCurveR, kAnimCurveG, kAnimCurveB, 
                    kAnimCurveA, bAnimR, bAnimG, bAnimB, bAnimA,
                    dDefaultR, dDefaultG, dDefaultB, dDefaultA,
                    pkKeys, uiNumKeys);
    }
    else
    {
/*
        eType = NiAnimationKey::BEZKEY;
        return ConvertBezPosAnim(kAnimCurveX, kAnimCurveY, kAnimCurveZ, 
                    bAnimX, bAnimY, bAnimZ, dDefaultX, dDefaultY, dDefaultZ,
                    pkKeys, uiNumKeys);
*/
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
MStatus ConvertColorAnim(const MPlug& kPlugR, const MPlug& kPlugG, 
     const MPlug& kPlugB, unsigned int& uiNumKeys, 
     NiColorKey*& pkKeys, NiAnimationKey::KeyType& eType)
{

    uiNumKeys = 0;
    pkKeys = 0;
    eType = NiAnimationKey::NOINTERP;


    double dDefaultR;
    double dDefaultG;
    double dDefaultB;
    double dDefaultA = 1.0;

    kPlugR.getValue(dDefaultR);
    kPlugG.getValue(dDefaultG);
    kPlugB.getValue(dDefaultB);

    // ConvertFloatAnim converts a keyed MFnNumericAttrbute to a set of 
    // NiFloatKeys

    bool bAnimR = false;
    bool bAnimG = false;
    bool bAnimB = false;
    bool bAnimA = false;

    MFnAnimCurve kAnimCurveR;
    MFnAnimCurve kAnimCurveG;
    MFnAnimCurve kAnimCurveB;
    MFnAnimCurve kAnimCurveA;

    if (ConvertPlugToAnimCurve(kPlugR, bAnimR, kAnimCurveR) == MS::kFailure)
        return MS::kFailure;

    if (ConvertPlugToAnimCurve(kPlugG, bAnimG, kAnimCurveG) == MS::kFailure)
        return MS::kFailure;

    if (ConvertPlugToAnimCurve(kPlugB, bAnimB, kAnimCurveB) == MS::kFailure)
        return MS::kFailure;



    bool bLinear = (!bAnimR || AllKeysAreLinear(kAnimCurveR));
    bLinear &= (!bAnimG || AllKeysAreLinear(kAnimCurveG));
    bLinear &= (!bAnimB || AllKeysAreLinear(kAnimCurveB));

    // Default to true because there are no Bez Color Keys
    bLinear = true;

    if (bLinear)
    {

        eType = NiAnimationKey::LINKEY;
        return ConvertLinColorAAnim(kAnimCurveR, kAnimCurveG, kAnimCurveB, 
                    kAnimCurveA, bAnimR, bAnimG, bAnimB, bAnimA,
                    dDefaultR, dDefaultG, dDefaultB, dDefaultA,
                    pkKeys, uiNumKeys);
    }
    else
    {
/*
        eType = NiAnimationKey::BEZKEY;
        return ConvertBezPosAnim(kAnimCurveX, kAnimCurveY, kAnimCurveZ, 
                    bAnimX, bAnimY, bAnimZ, dDefaultX, dDefaultY, dDefaultZ,
                    pkKeys, uiNumKeys);
*/
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
bool CheckForExtraSteppedKey(const MFnAnimCurve& kAnimCurveR, 
                             const MFnAnimCurve& kAnimCurveG,
                             const MFnAnimCurve& kAnimCurveB,
                             const MFnAnimCurve& kAnimCurveA,
                             unsigned int uiR,
                             unsigned int uiG,
                             unsigned int uiB,
                             unsigned int uiA,
                             double dTimeR,
                             double dTimeG,
                             double dTimeB,
                             double dTimeA,
                             double dMinTime)
{
    // Check for Stepped Tangents
    if ((uiR > 0) && 
        DoubleCloseTo(dTimeR, dMinTime) &&
        (kAnimCurveR.outTangentType(uiR-1) == MFnAnimCurve::kTangentStep))
    {
        return true;
    }

    if ((uiG > 0) && 
        DoubleCloseTo(dTimeG, dMinTime) &&
        (kAnimCurveG.outTangentType(uiG-1) == MFnAnimCurve::kTangentStep))
    {
        return true;
    }

    if ((uiB > 0) && 
        DoubleCloseTo(dTimeB, dMinTime) &&
        (kAnimCurveB.outTangentType(uiB-1) == MFnAnimCurve::kTangentStep))
    {
        return true;
    }

    if ((uiA > 0) && 
        DoubleCloseTo(dTimeA, dMinTime) &&
        (kAnimCurveA.outTangentType(uiA-1) == MFnAnimCurve::kTangentStep))
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
unsigned int GetNumUnifiedKeys(const MFnAnimCurve& kAnimCurveR, 
                               const MFnAnimCurve& kAnimCurveG,
                               const MFnAnimCurve& kAnimCurveB,
                               const MFnAnimCurve& kAnimCurveA,
                               bool bAnimR,
                               bool bAnimG,
                               bool bAnimB,
                               bool bAnimA)
{
    // Count the number of Unique keys
    unsigned int uiNumKeysR = 0;
    unsigned int uiNumKeysG = 0;
    unsigned int uiNumKeysB = 0;
    unsigned int uiNumKeysA = 0;

    if (bAnimR)
    {
        uiNumKeysR = kAnimCurveR.numKeyframes();
    }

    if (bAnimG)
    {
        uiNumKeysG = kAnimCurveG.numKeyframes();
    }

    if (bAnimB)
    {
        uiNumKeysB = kAnimCurveB.numKeyframes();
    }

    if (bAnimA)
    {
        uiNumKeysA = kAnimCurveA.numKeyframes();
    }

    double dMinTime;
    double dTimeR = NI_INFINITY;
    double dTimeG = NI_INFINITY;
    double dTimeB = NI_INFINITY;
    double dTimeA = NI_INFINITY;

    MStatus kStatus;
    MTime kTime;
    unsigned int uiNumKeys = 0;

    unsigned int uiR = 0;
    unsigned int uiG = 0;
    unsigned int uiB = 0;
    unsigned int uiA = 0;

    while ((uiR < uiNumKeysR) || (uiG < uiNumKeysG) || (uiB < uiNumKeysB) || 
        (uiA < uiNumKeysA))
    {
        if ((bAnimR) && (uiR < uiNumKeysR))
        {
            dTimeR = kAnimCurveR.time(uiR, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimG) && (uiG < uiNumKeysG))
        {
            dTimeG = kAnimCurveG.time(uiG, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimB) && (uiB < uiNumKeysB))
        {
            dTimeB = kAnimCurveB.time(uiB, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimA) && (uiA < uiNumKeysA))
        {
            dTimeA = kAnimCurveA.time(uiA, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        dMinTime = min( min( min(dTimeR, dTimeG), dTimeB), dTimeA);

        uiNumKeys++;

            // Check for the extra key created by a stepped tangent
        if (CheckForExtraSteppedKey(kAnimCurveR, kAnimCurveG, kAnimCurveB, 
            kAnimCurveA, uiR, uiG, uiB, uiA, dTimeR, dTimeG, dTimeB, 
            dTimeA, dMinTime))
        {
            uiNumKeys++;
        }

        if (DoubleCloseTo(dTimeR, dMinTime))
        {
            uiR++;

            if (uiR == uiNumKeysR)
            {
                dTimeR = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeG, dMinTime))
        {
            uiG++;

            if (uiG == uiNumKeysG)
            {
                dTimeG = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeB, dMinTime))
        {
            uiB++;

            if (uiB == uiNumKeysB)
            {
                dTimeB = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeA, dMinTime))
        {
            uiA++;

            if (uiA == uiNumKeysA)
            {
                dTimeA = NI_INFINITY;
            }
        }
    }

    return uiNumKeys;
}
//---------------------------------------------------------------------------
MStatus ConvertLinColorAAnim(const MFnAnimCurve& kAnimCurveR, 
                          const MFnAnimCurve& kAnimCurveG,
                          const MFnAnimCurve& kAnimCurveB,
                          const MFnAnimCurve& kAnimCurveA,
                          bool bAnimR,
                          bool bAnimG,
                          bool bAnimB,
                          bool bAnimA,
                          double dDefaultR,
                          double dDefaultG,
                          double dDefaultB,
                          double dDefaultA,
                          NiColorKey*& pkKeys,
                          unsigned int& uiNumKeys)
{

// Examine the curves and determine the total number of unique unified keys
    uiNumKeys = GetNumUnifiedKeys(kAnimCurveR, kAnimCurveG, kAnimCurveB, 
        kAnimCurveA, bAnimR, bAnimG, bAnimB, bAnimA);

    // Allocate the Keys
    pkKeys = NiNew NiLinColorKey[uiNumKeys];

    // Get the number of keys for each axis
    unsigned int uiNumKeysR = 0;
    unsigned int uiNumKeysG = 0;
    unsigned int uiNumKeysB = 0;
    unsigned int uiNumKeysA = 0;

    if (bAnimR)
    {
        uiNumKeysR = kAnimCurveR.numKeyframes();
    }

    if (bAnimG)
    {
        uiNumKeysG = kAnimCurveG.numKeyframes();
    }

    if (bAnimB)
    {
        uiNumKeysB = kAnimCurveB.numKeyframes();
    }

    if (bAnimA)
    {
        uiNumKeysA = kAnimCurveA.numKeyframes();
    }

    // Initialize the default Times and Values
    double dMinTime;
    double dTimeR = NI_INFINITY;
    double dTimeG = NI_INFINITY;
    double dTimeB = NI_INFINITY;
    double dTimeA = NI_INFINITY;

    float fR = (float)dDefaultR;
    float fG = (float)dDefaultG;
    float fB = (float)dDefaultB;
    float fA = (float)dDefaultA;

    MStatus kStatus;
    MTime kTime;


    int iCurrentKey = 0;
    unsigned int uiR = 0;
    unsigned int uiG = 0;
    unsigned int uiB = 0;
    unsigned int uiA = 0;

    while ((uiR < uiNumKeysR) || (uiG < uiNumKeysG) || (uiB < uiNumKeysB) || 
        (uiA < uiNumKeysA))
    {
        if ((bAnimR) && (uiR < uiNumKeysR))
        {
            dTimeR = kAnimCurveR.time(uiR, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimG) && (uiG < uiNumKeysG))
        {
            dTimeG = kAnimCurveG.time(uiG, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimB) && (uiB < uiNumKeysB))
        {
            dTimeB = kAnimCurveB.time(uiB, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        if ((bAnimA) && (uiA < uiNumKeysA))
        {
            dTimeA = kAnimCurveA.time(uiA, &kStatus).as(MTime::kSeconds);
            NIASSERT(kStatus);
        }

        // Determine the minimum time for all Animation Curve
        dMinTime = min( min( min(dTimeR, dTimeG), dTimeB), dTimeA);

        MTime kTime(dMinTime, MTime::kSeconds);

        // Find the Value of each Animation curve at the minimum time
        if (bAnimR)
        {
            fR = (float)kAnimCurveR.evaluate(kTime, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);
        }

        if (bAnimG)
        {
            fG = (float)kAnimCurveG.evaluate(kTime, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);
        }

        if (bAnimB)
        {
            fB = (float)kAnimCurveB.evaluate(kTime, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);
        }

        if (bAnimA)
        {
            fA = (float)kAnimCurveA.evaluate(kTime, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);
        }

        // Create a Keys at the minimum time
        pkKeys[iCurrentKey].SetTime((float)dMinTime);
        pkKeys[iCurrentKey++].SetColor(NiColorA(fR, fG, fB, fA));

        // Increment the current key for each animation if it is close to 
        // the minimum time.
        if (DoubleCloseTo(dTimeR, dMinTime))
        {
            uiR++;

            if (uiR == uiNumKeysR)
            {
                dTimeR = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeG, dMinTime))
        {
            uiG++;

            if (uiG == uiNumKeysG)
            {
                dTimeG = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeB, dMinTime))
        {
            uiB++;

            if (uiB == uiNumKeysB)
            {
                dTimeB = NI_INFINITY;
            }
        }

        if (DoubleCloseTo(dTimeA, dMinTime))
        {
            uiA++;

            if (uiA == uiNumKeysA)
            {
                dTimeA = NI_INFINITY;
            }
        }
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
bool CreateFloatInterpolator(MFnDependencyNode& kObject, 
    const char* pcAttributeName, NiFloatInterpolator*& pkInterp,
    float fScale/* = 1.0f*/)
{
    MStatus kStatus;

    // Read the Plug
    MPlug kMPlug = kObject.findPlug(MString(pcAttributeName), &kStatus);

    // Create any animation
    if (!PlugIsConnected(kMPlug))
        return false;


    unsigned int uiNumKeys;
    NiFloatKey* pkKeys;
    NiAnimationKey::KeyType eType;
    if (ConvertFloatAnim(kMPlug, uiNumKeys, pkKeys, eType, fScale) != 
            MS::kSuccess)
        return false;


    NiFloatData* pkData = NiNew NiFloatData;
    pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);
    
    // Create the Interpolator
    pkInterp = NiNew NiFloatInterpolator(pkData);
    pkInterp->Collapse();

    return true;
}
//---------------------------------------------------------------------------
bool CreateFloatInterpolator(MObject& kObject, 
    const char* pcAttributeName, NiFloatInterpolator*& pkInterp,
    float fScale/* = 1.0f*/)
{
    MStatus kStatus;

    MFnDependencyNode kNode(kObject, &kStatus);
    if (kStatus != MS::kSuccess)
        return false;

    return CreateFloatInterpolator(kNode, pcAttributeName, pkInterp, fScale);
}
//---------------------------------------------------------------------------

NiTimeController::CycleType GetAnimationPostCycleType(MPlug& kPlug,
    MStatus& kStat)
{
    NiTimeController::CycleType kReturnValue = 
        NiTimeController::CLAMP;

    MPlugArray kPlugArray;

    kPlug.connectedTo(kPlugArray, true, false, &kStat);
    NIASSERT(kStat == MStatus::kSuccess);

    unsigned int plugArrayLength = kPlugArray.length();

    if(plugArrayLength == 1)
    {
		MPlug kAnimationCurvePlug = kPlugArray[0];
		MObject kAnimationCurveNode = kAnimationCurvePlug.node(&kStat);
		NIASSERT(kStat == MStatus::kSuccess);

		if(kAnimationCurveNode.hasFn(MFn::kAnimCurve))
		{
			MFnAnimCurve kMFnAnimCurve(kAnimationCurveNode);
			MFnAnimCurve::InfinityType ePostInfinityType = 
				kMFnAnimCurve.postInfinityType(&kStat);
			NIASSERT(kStat == MStatus::kSuccess);
			switch(ePostInfinityType)
			{
			case MFnAnimCurve::kConstant:
				kReturnValue = NiTimeController::CLAMP;
				break;

			case MFnAnimCurve::kLinear:
			{
				//bad type
				kReturnValue = NiTimeController::CLAMP;

				MString kWarningString = "Exporter does not support post ";
				kWarningString += "infinity cycle type \"Linear\". ";
				kWarningString += "Default to Constant(NiTimeController::CLAMP) ";
	            
				MObject kAnimatedObject = kPlug.node();

				MFnDependencyNode kFnDependencyNode(kAnimatedObject);

				kWarningString += "for node " + kFnDependencyNode.name(&kStat);
				NIASSERT(kStat == MStatus::kSuccess);
				kWarningString += " attribute " + kPlug.name(&kStat);
				NIASSERT(kStat == MStatus::kSuccess);

				MGlobal::displayWarning(kWarningString);
				break;
			}
	         

			case MFnAnimCurve::kCycle:
				kReturnValue = NiTimeController::LOOP;
				break;

			case MFnAnimCurve::kCycleRelative:
			{
				//bad type
				kReturnValue = NiTimeController::CLAMP;

				MString kWarningString = "Exporter does not support post ";
				kWarningString += "infinity cycle type \"Cycle Relative\". ";
				kWarningString += "Default to Constant(NiTimeController::CLAMP) ";
	            
				MObject kAnimatedObject = kPlug.node();

				MFnDependencyNode kFnDependencyNode(kAnimatedObject);

				kWarningString += "for node " + kFnDependencyNode.name(&kStat);
				NIASSERT(kStat == MStatus::kSuccess);
				kWarningString += " attribute " + kPlug.name(&kStat);
				NIASSERT(kStat == MStatus::kSuccess);

				MGlobal::displayWarning(kWarningString);
				break;
			}

			case MFnAnimCurve::kOscillate:
				kReturnValue = NiTimeController::REVERSE;
				break;
			}
		}
	}

    return kReturnValue;
}
//---------------------------------------------------------------------------
