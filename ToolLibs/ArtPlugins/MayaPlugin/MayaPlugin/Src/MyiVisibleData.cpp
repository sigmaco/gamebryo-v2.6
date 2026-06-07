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
MyiVisibleData::MyiVisibleData( int iComponentID )
{
    // Get the combined keys
    m_iKeys = gAnimDataGetNumKeyFrames( iComponentID, 9); // 9=>Visible


    if (!FillNiKeys( iComponentID ))
        return; // Status set

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiVisibleData::~MyiVisibleData( void )
{
}
//---------------------------------------------------------------------------
bool MyiVisibleData::FillNiKeys( int iComponentID )
{
    int k;

    if (m_iKeys == 0)
    {
        m_pNiKeys = NULL;
        SetStatus(MYIOBJ_SUCCESS);
        return true;
    }

    m_pNiKeys = NiNew NiStepBoolKey[m_iKeys];
    if (m_pNiKeys == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }

    for (k = 0; k < m_iKeys; k++)
    {
        float fTime;
        fTime =(float)gAnimDataGetKeyFrameTime(iComponentID,9, k);

        bool bValue = 
            ((float)gAnimDataGetKeyFrameValue(iComponentID,9, k) != 0.0f);

        m_pNiKeys[k].SetTime(fTime);
        m_pNiKeys[k].SetBool(bValue);

        // 0=TranslateX, 1=TranslateY, 2=TranslateZ
        // Times should be the same - but might have to
        // do some preprocessing later - just in case.
        //fTime = gAnimDataGetKeyFrameTime(iShapeID, 0, k); 
        //fValue[0] = (float)gAnimDataGetKeyFrameValue( iShapeID, 0, k);
        //fValue[1] = (float)gAnimDataGetKeyFrameValue( iShapeID, 1, k);
        //fValue[2] = (float)gAnimDataGetKeyFrameValue( iShapeID, 2, k);


        //m_ppNiKeys[k] = new NiLinPosKey( fTime,
        //                    NiPoint3(fValue[0], fValue[1], fValue[2]));

    }

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
