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
MyiRotData::MyiRotData(int iComponentID)
{
    m_pEuler = NULL;
    m_pNiKeys[0] = m_pNiKeys[1] = m_pNiKeys[2] = NULL;

    // 1. Store the number of keys for each axis.
    m_iKeys[0] = gAnimDataGetNumKeyFrames( iComponentID, 3); // 3=>RotateX
    m_iKeys[1] = gAnimDataGetNumKeyFrames( iComponentID, 4); // 4=>RotateY
    m_iKeys[2] = gAnimDataGetNumKeyFrames( iComponentID, 5); // 5=>RotateZ

    //Try to get quaternion w component. If it exists, then
    m_iKeys[3] = gAnimDataGetNumKeyFrames( iComponentID, 10); // 10=>Quaternion

    if(m_iKeys[3] != 0)
    {
        //this needs a quaternion curve

        //quats should have equal amount of keys for each axis
        NIASSERT(m_iKeys[0] == m_iKeys[1] && m_iKeys[1] == m_iKeys[2] &&
            m_iKeys[2] == m_iKeys[3]);

        int iNumKeys = m_iKeys[3];

        m_pQuaternion = NiNew NiLinRotKey[iNumKeys];
        NIASSERT(m_pQuaternion);

        for (int iKeyIndex = 0; iKeyIndex < iNumKeys; iKeyIndex++)
        {

            float fX, fY, fZ, fW;

            fX = (float)gAnimDataGetKeyFrameValue( iComponentID, 3, 
                iKeyIndex);
            fY = (float)gAnimDataGetKeyFrameValue( iComponentID, 4, 
                iKeyIndex);
            fZ = (float)gAnimDataGetKeyFrameValue( iComponentID, 5, 
                iKeyIndex);
            fW = (float)gAnimDataGetKeyFrameValue( iComponentID, 10, 
                iKeyIndex);

            float fTime = gAnimDataGetKeyFrameTime( iComponentID, 3, 
                iKeyIndex);
#if NIDEBUG
            float fTimeY = gAnimDataGetKeyFrameTime( iComponentID, 4, 
                iKeyIndex);
            float fTimeZ = gAnimDataGetKeyFrameTime( iComponentID, 5, 
                iKeyIndex);
            float fTimeW = gAnimDataGetKeyFrameTime( iComponentID, 10, 
                iKeyIndex);

            NIASSERT(fTime == fTimeY && fTimeY == fTimeZ && fTimeZ == fTimeW);
#endif

            m_pQuaternion[iKeyIndex].SetTime(fTime);
            m_pQuaternion[iKeyIndex].SetQuaternion(NiQuaternion(fW, fX, fY, 
                fZ));
            if (iKeyIndex > 0 && NiQuaternion::Dot(
                m_pQuaternion[iKeyIndex].GetQuaternion(), 
                m_pQuaternion[iKeyIndex-1].GetQuaternion()) < 0)
            {
                m_pQuaternion[iKeyIndex].SetQuaternion(
                    NiQuaternion(-fW, -fX, -fY, -fZ));
            }
        }
        SetStatus(MYIOBJ_SUCCESS);
        return;
    }

    // Get non animated Rotations in case we need them for the bug fix. 
    float fRotate[3];
    int result = gMDtObjectGetRotationWOOrientation( iComponentID, 
        &fRotate[0], &fRotate[1], &fRotate[2]);
    NI_UNUSED_ARG(result);

    float fFirstKeyTime = FirstKeyTime(iComponentID);

    // 2. Make a Curve based on these frames.
    for (int i = 0; i < 3; i++)
    {
        m_pNiKeys[i] = NULL;
        if (m_iKeys[i] > 0)
        {   
            if (!MakeCurve(iComponentID, i))
            {
                SetStatus(MYIOBJ_ERROR);
                return;
            }
        }
        else
        {
            // Bug fix.
            // When animations did not occur on a channel (ie this else 
            // statement) that channels rotations are ignored even if non
            // zero. As a fix, we will create a 1 frame animation for  
            // these constant channels.
            m_iKeys[i] = 1;
            m_pNiKeys[i] = NiNew NiBezFloatKey[2];

            NiBezFloatKey* pkBezFloatKey = (NiBezFloatKey*)
                m_pNiKeys[i]->GetKeyAt(0, sizeof(NiBezFloatKey));

            pkBezFloatKey->SetTime(fFirstKeyTime);
            pkBezFloatKey->SetValue(fRotate[i]);
            pkBezFloatKey->SetInTan(0.0f);
            pkBezFloatKey->SetOutTan(0.0f);
        }
    }

    m_pEuler = NiNew NiEulerRotKey[1];

    m_pEuler->Initialize(m_pNiKeys[0], m_iKeys[0], NiFloatKey::BEZKEY,
            m_pNiKeys[1], m_iKeys[1], NiFloatKey::BEZKEY,
            m_pNiKeys[2], m_iKeys[2], NiFloatKey::BEZKEY );


    // Fill the Derived Values
    NiAnimationKey::FillDerivedValsFunction pkFunc = 
        NiAnimationKey::GetFillDerivedFunction(NiAnimationKey::ROTKEY, 
            NiAnimationKey::EULERKEY);

    pkFunc(m_pEuler, 1, sizeof(NiEulerRotKey));

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiRotData::~MyiRotData()
{
}
//---------------------------------------------------------------------------
bool MyiRotData::MakeCurve(int iComponentID, int axis)
{
#ifdef NIDEBUG
    const char* componentName = gMDtObjectGetName(iComponentID);
    NI_UNUSED_ARG(componentName);
#endif
    
    int k, iFirstFrame, iLastFrame, iKey;
    float fTime;

    // AGAIN - Assuming all axis the same for now...
    iFirstFrame = 0;
    iLastFrame  = m_iKeys[axis]-1;

    m_iKeys[axis] = 1 + iLastFrame - iFirstFrame;

    m_pNiKeys[axis] = NiNew NiBezFloatKey[m_iKeys[axis]];
    if (m_pNiKeys[axis] == NULL)
        return false;
    
    for (iKey = 0, k = iFirstFrame; k <= iLastFrame; k++, iKey++)
    {
        
        // Get the Value for this k frame on this axis
        float fValue;
        fValue = (float)gAnimDataGetKeyFrameValue( iComponentID, 3+axis, k);

        // Get the Time for this frame on this axis
        //(3=rotx, 4=roty, 5=rotz)
        fTime = gAnimDataGetKeyFrameTime( iComponentID, 3+axis, k); 

        // if not bez
        //m_ppNiKeys[axis][iKey] = new NiLinFloatKey(fTime, fValue);
        
        // if bez
        float fSlope[2];
        // Get Slopes...
        gAnimDataGetSlopeInOut(iComponentID, axis+3, k, &fSlope[0], 
            &fSlope[1]);

        // slope needs to be modified by deltatime
        float fPrevTime;
        float fNextTime;
        int iNumKeyFrames = gAnimDataGetNumKeyFrames(iComponentID, axis+3 );

        if (k>0)
            fPrevTime = fTime - gAnimDataGetKeyFrameTime( 
                iComponentID, 3+axis, k-1);
        else
            fPrevTime = 0.0; // Figure out how maya handles this....

        if (k<iNumKeyFrames-1)
            fNextTime = gAnimDataGetKeyFrameTime( iComponentID, 3+axis, k+1)
                - fTime;
        else
            fNextTime = 0.0; // Figure out how maya handles this

        fSlope[0]*=(fPrevTime);
        fSlope[1]*=(fNextTime);

        NiBezFloatKey* pkBezFloatKey = (NiBezFloatKey*)
            m_pNiKeys[axis]->GetKeyAt(iKey, sizeof(NiBezFloatKey));

        pkBezFloatKey->SetTime(fTime);
        pkBezFloatKey->SetValue(fValue);
        pkBezFloatKey->SetInTan(fSlope[0]);
        pkBezFloatKey->SetOutTan(fSlope[1]);

        //float fValue[3];
        //gAnimDataGetCKeyFrameValue( iShapeID,1, k, fValue);
        //fTime = gAnimDataGetCKeyFrameTime(iShapeID,1, k);
        //m_ppNiKeys[axis][iKey] = new NiLinFloatKey(fTime, fValue[axis]);

    }
    return true;
}
//---------------------------------------------------------------------------
bool MyiRotData::IsQuaternion()
{
    if(m_iKeys[3] != 0)
    {
        //There are quat w keys
        return true;
    }
    else
    {
        return false;
    }
}

//---------------------------------------------------------------------------
float MyiRotData::FirstKeyTime(int iComponentID)
{
    float fMinTime = 1000000.0f;

    if (gAnimDataGetNumKeyFrames( iComponentID, 3) > 0)
        fMinTime = min(fMinTime,  gAnimDataGetKeyFrameTime( iComponentID, 3, 
            0)); 

    if (gAnimDataGetNumKeyFrames( iComponentID, 4) > 0)
        fMinTime = min(fMinTime,  gAnimDataGetKeyFrameTime( iComponentID, 4, 
            0)); 

    if (gAnimDataGetNumKeyFrames( iComponentID, 5) > 0)
        fMinTime = min(fMinTime,  gAnimDataGetKeyFrameTime( iComponentID, 5, 
            0)); 

    if (fMinTime == 1000000.0f)
        return 0.0;

    return fMinTime;
}
//---------------------------------------------------------------------------
