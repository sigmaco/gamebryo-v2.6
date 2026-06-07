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
MyiScaleData::MyiScaleData( int iComponentID )
{
    // Get the combined keys
    m_iKeys = gAnimDataGetNumKeyFrames( iComponentID, 6); // 9=>Scale X

    // Only Support Uniform Scale
    if( (gAnimDataGetNumKeyFrames( iComponentID, 6) != 
         gAnimDataGetNumKeyFrames( iComponentID, 7)) ||
        (gAnimDataGetNumKeyFrames( iComponentID, 7) != 
         gAnimDataGetNumKeyFrames( iComponentID, 8)) )
    {
        // Show Error
    }

    if (!FillNiKeys( iComponentID ))
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiScaleData::~MyiScaleData( void )
{
}
//---------------------------------------------------------------------------
bool MyiScaleData::FillNiKeys( int iComponentID )
{
    int k;

    if (m_iKeys == 0)
    {
        m_pNiKeys = NULL;
        return true;
    }

    m_pNiKeys = NiNew NiBezFloatKey [m_iKeys];
    if (m_pNiKeys == NULL)
        return false;

    for (k = 0; k < m_iKeys; k++)
    {

        // Use the X Scale to generate our scalings
        float fTime;
        fTime =(float)gAnimDataGetKeyFrameTime(iComponentID,6, k);

        float fValue = (float)gAnimDataGetKeyFrameValue(iComponentID,6, k);

        float fSlopeIn;
        float fSlopeOut;
        gAnimDataGetSlopeInOut(iComponentID, 6, k, &fSlopeIn, &fSlopeOut);

        // Normalize the In Tangent to the range [0,1]
        if(k > 0)
        {
            fSlopeIn *= (float)gAnimDataGetKeyFrameTime(iComponentID,6, k) -
                (float)gAnimDataGetKeyFrameTime(iComponentID,6, k-1);
        }

        // Normalize the Out Tangent to the range [0,1]
        if(k < (m_iKeys-1))
        {
            fSlopeOut *= (float)gAnimDataGetKeyFrameTime(iComponentID,6, k+1)
                - (float)gAnimDataGetKeyFrameTime(iComponentID,6, k);
        }

        m_pNiKeys[k].SetInTan(fSlopeIn);
        m_pNiKeys[k].SetOutTan(fSlopeOut);
        m_pNiKeys[k].SetValue(fValue);
        m_pNiKeys[k].SetTime(fTime);

        
        if(!VerifyUniformScale(iComponentID, k))
        {
            char cError[256];

            NiSprintf(cError, 256,
                "ERROR: Non-Uniform Animated Scale on %s at time:%f.\n", 
                gMDtObjectGetName(iComponentID), fTime);

            DtExt_Err(cError);
        }

    }

    return true;
}
//---------------------------------------------------------------------------
bool MyiScaleData::VerifyUniformScale(int iComponentID, int k)
{
    
    float fTimeX =(float)gAnimDataGetKeyFrameTime(iComponentID,6, k);
    float fTimeY =(float)gAnimDataGetKeyFrameTime(iComponentID,7, k);
    float fTimeZ =(float)gAnimDataGetKeyFrameTime(iComponentID,8, k);

    // -1 indicates that no animated scale existed for that channel.
    if (fTimeX == -1.0f || fTimeY == -1.0f || fTimeZ == -1.0f)
        return false;

    float fEpsilon = NiMax(NiAbs(fTimeX * 0.001f), 0.001f);
      
    if( !NiOptimize::CloseTo(fTimeX, fTimeY, fEpsilon) || 
        !NiOptimize::CloseTo(fTimeY, fTimeZ, fEpsilon))
        return false;

    
    float fValueX = (float)gAnimDataGetKeyFrameValue(iComponentID,6, k);
    float fValueY = (float)gAnimDataGetKeyFrameValue(iComponentID,7, k);
    float fValueZ = (float)gAnimDataGetKeyFrameValue(iComponentID,8, k);

    fEpsilon = NiMax(NiAbs(fValueX * 0.001f), 0.001f);
      
    if( !NiOptimize::CloseTo(fValueX, fValueY, fEpsilon) || 
        !NiOptimize::CloseTo(fValueY, fValueZ, fEpsilon))
        return false;
    
    float fSlopeInX;
    float fSlopeOutX;
    gAnimDataGetSlopeInOut(iComponentID, 6, k, &fSlopeInX, &fSlopeOutX);
    
    float fSlopeInY;
    float fSlopeOutY;
    gAnimDataGetSlopeInOut(iComponentID, 7, k, &fSlopeInY, &fSlopeOutY);

    float fSlopeInZ;
    float fSlopeOutZ;
    gAnimDataGetSlopeInOut(iComponentID, 8, k, &fSlopeInZ, &fSlopeOutZ);

    
    fEpsilon = NiMax(NiAbs(fSlopeInX * 0.001f), 0.001f);

    if( !NiOptimize::CloseTo(fSlopeInX, fSlopeInY, fEpsilon) || 
        !NiOptimize::CloseTo(fSlopeInY, fSlopeInZ, fEpsilon))
        return false;

    fEpsilon = NiMax(NiAbs(fSlopeOutX * 0.001f), 0.001f);

    if( !NiOptimize::CloseTo(fSlopeOutX, fSlopeOutY, fEpsilon) || 
        !NiOptimize::CloseTo(fSlopeOutY, fSlopeOutZ, fEpsilon))
        return false;

    return true;
}
//---------------------------------------------------------------------------
