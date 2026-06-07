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
MyiPosData::MyiPosData( int iComponentID )
{
    // Get the combined keys
    m_iKeys = gAnimDataGetCKeyFrames(iComponentID,0);

    if (!FillNiKeys( iComponentID ))
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }
    
    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiPosData::~MyiPosData( void )
{
}
//---------------------------------------------------------------------------
bool MyiPosData::FillNiKeys( int iComponentID )
{
    int k;

    if (m_iKeys == 0)
    {
        m_pNiKeys = NULL;
        return true;
    }

    m_pNiKeys = NiNew NiBezPosKey[m_iKeys];
    if (m_pNiKeys == NULL)
        return false;

    float fPivot[3];
    float fPivotTranslation[3];
    gMDtObjectGetPivot(iComponentID, &fPivot[0], &fPivot[1], &fPivot[2] );
    gMDtObjectGetPivotTranslation(iComponentID, &fPivotTranslation[0], 
        &fPivotTranslation[1], &fPivotTranslation[2] );

    float fScale[3];
    fScale[0] = fScale[1] = fScale[2] = 1.0f;

    // CHECK FOR SCALING
    int iParent = gMDtObjectGetParentID( iComponentID );
    if (iParent != -1)
    {
        // This shape has a parent.. we will modify translations to 
        // compensate for all the (possibly non-uniform) scaling.
        gMDtObjectGetFinalNonAnimatingNotUniformScale( iComponentID, 
            &fScale[0], &fScale[1], &fScale[2] );
        
        // Remove the current NOT animated Scale
        if(!gMDtObjectGetAnimatedScale(iComponentID))
        {
            float fS[3];
            
            gMDtObjectGetScale( iComponentID, &fS[0], &fS[1], &fS[2] );
            
                // Only Remove Non Uniform Scale for NonUniformScale
            float fEpsilon = fS[0] * 0.001f;
    
            if( !NiOptimize::CloseTo(fS[0], fS[1], fEpsilon) ||
                !NiOptimize::CloseTo(fS[0], fS[2], fEpsilon) )
            {
                if (fS[0] != 0) fScale[0] /= fS[0];
                if (fS[1] != 0) fScale[1] /= fS[1];
                if (fS[2] != 0) fScale[2] /= fS[2];
            }
        }
    }


    for (k = 0; k < m_iKeys; k++)
    {
        float fValue[3];
        float fTime;
        fTime = gAnimDataGetCKeyFrameTime(iComponentID,0, k);
        gAnimDataGetCKeyFrameValue( iComponentID,0, k, fValue);


        // Modify by the pivot values
        fValue[0] += fPivotTranslation[0] + fPivot[0];
        fValue[1] += fPivotTranslation[1] + fPivot[1];
        fValue[2] += fPivotTranslation[2] + fPivot[2];

        // Multiply by the parents scaling
        fValue[0] *= fScale[0];
        fValue[1] *= fScale[1];
        fValue[2] *= fScale[2];

        // Check for the current working units
        if(gExport.m_bUseCurrentWorkingUnits)
        {
            fValue[0] *= gExport.m_fLinearUnitMultiplier;
            fValue[1] *= gExport.m_fLinearUnitMultiplier;
            fValue[2] *= gExport.m_fLinearUnitMultiplier;
        }

        float In[3], Out[3];

        gAnimDataGetCSlopeIn( iComponentID,0, k, In);
        gAnimDataGetCSlopeOut( iComponentID,0, k, Out);


        // Check for the current working units
        if(gExport.m_bUseCurrentWorkingUnits)
        {
            In[0] *= gExport.m_fLinearUnitMultiplier;
            In[1] *= gExport.m_fLinearUnitMultiplier;
            In[2] *= gExport.m_fLinearUnitMultiplier;
            Out[0] *= gExport.m_fLinearUnitMultiplier;
            Out[1] *= gExport.m_fLinearUnitMultiplier;
            Out[2] *= gExport.m_fLinearUnitMultiplier;
        }

        //Weight - Needed to translate to Ni form of bezier
        int iNumKeyFrames = gAnimDataGetCKeyFrames( iComponentID, 0);
        float fPrevTime = 1.0;
        float fNextTime = 1.0;

        if (k>0)
        {
            float fv = gAnimDataGetCKeyFrameTime( iComponentID, 0, k-1);
            fPrevTime = fTime - fv; 
        }
        // else Figure out how maya handles this....

        if (k<iNumKeyFrames-1)
        {
            float fv = gAnimDataGetCKeyFrameTime( iComponentID, 0, k+1);
            fNextTime =  fv - fTime;
        }
        else
            fNextTime = fPrevTime; // Figure out how maya handles this

        In[0]*=(fPrevTime);
        In[1]*=(fPrevTime);
        In[2]*=(fPrevTime);
        Out[0]*=(fNextTime);
        Out[1]*=(fNextTime);
        Out[2]*=(fNextTime);
        // End Weight


        NiBezPosKey* pkBezPosKey = (NiBezPosKey*) m_pNiKeys->GetKeyAt(k, 
            sizeof(NiBezPosKey));

        pkBezPosKey->SetTime(fTime);
        pkBezPosKey->SetPos(NiPoint3(fValue[0], fValue[1], fValue[2]));
        pkBezPosKey->SetInTan(NiPoint3(In[0], In[1], In[2]));
        pkBezPosKey->SetOutTan(NiPoint3(Out[0], Out[1], Out[2]));
    }

    NiAnimationKey::FillDerivedValsFunction pkFunc = 
        NiAnimationKey::GetFillDerivedFunction(NiAnimationKey::POSKEY, 
        NiAnimationKey::BEZKEY);

    pkFunc(m_pNiKeys, m_iKeys, sizeof(NiBezPosKey));
    return true;
}
//---------------------------------------------------------------------------
