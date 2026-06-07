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

// Precompiled Header
#include "NiAnimationPCH.h"

#include "NiBoolTimelineInterpolator.h"

NiImplementRTTI(NiBoolTimelineInterpolator, NiBoolInterpolator);

//---------------------------------------------------------------------------
NiBoolTimelineInterpolator::NiBoolTimelineInterpolator()
    : NiBoolInterpolator(), m_uiPreviousIdx(0),
    m_bPreviousValue(false)
{
}
//---------------------------------------------------------------------------
NiBoolTimelineInterpolator::NiBoolTimelineInterpolator(NiBoolData* pkBoolData)
    : NiBoolInterpolator(pkBoolData), m_uiPreviousIdx(0),
    m_bPreviousValue(false)
{
}
//---------------------------------------------------------------------------
NiBoolTimelineInterpolator::NiBoolTimelineInterpolator(bool bPoseValue) :
    NiBoolInterpolator(bPoseValue), m_uiPreviousIdx(0),
    m_bPreviousValue(false)
{
}
//---------------------------------------------------------------------------
bool NiBoolTimelineInterpolator::Update(float fTime,
    NiObjectNET*, bool& bValue)
{
    if (!TimeHasChanged(fTime))
    {
        if (m_bBoolValue == INVALID_BOOL)
        {
            bValue = false;
            return false;
        }
        bValue = (m_bBoolValue != 0);
        return true;
    }

    unsigned int uiNumKeys;
    unsigned char ucSize;
    NiAnimationKey::KeyType eType;
    NiBoolKey* pkKeys = GetKeys(uiNumKeys, eType, ucSize);
    if (uiNumKeys > 0)
    {
        m_uiPreviousIdx = m_uiLastIdx;
        bool bInterpValue = NiBoolKey::GenInterp(fTime, pkKeys, eType, 
            uiNumKeys, m_uiLastIdx, ucSize);
        unsigned int uiCurrentIdx = m_uiLastIdx;
        bool bBoolValue = bInterpValue;

        if (bBoolValue == m_bPreviousValue)
        {
            bool bValueToFind = !bBoolValue;

            // Check to see if there was an opposite key between these two
            // keys. This is to prevent missing events in the timeline.
            if (m_uiPreviousIdx != uiCurrentIdx &&
                m_uiPreviousIdx + 1 > uiCurrentIdx)
            {
                // We have looped. Iterate from the previous index to the end
                // index.
                for (unsigned int ui = m_uiPreviousIdx + 1; ui < uiNumKeys;
                    ui++)
                {
                    if (pkKeys->GetKeyAt(ui, ucSize)->GetBool() == 
                        bValueToFind)
                    {
                        bBoolValue = bValueToFind;
                        break;
                    }
                }

                // If true key not found, iterate from the start index to the
                // current index.
                if (bBoolValue != bValueToFind)
                {
                    for (unsigned int ui = 0; ui < uiCurrentIdx; ui++)
                    {
                        if (pkKeys->GetKeyAt(ui, ucSize)->GetBool() == 
                            bValueToFind)
                        {
                            bBoolValue = bValueToFind;
                            break;
                        }
                    }
                }
            }
            else
            {
                // We have not looped. Iterate from previous index to current
                // index looking.
                for (unsigned int ui = m_uiPreviousIdx + 1;
                    ui < uiCurrentIdx && ui < uiNumKeys; ui++)
                {
                    if (pkKeys->GetKeyAt(ui, ucSize)->GetBool() == 
                        bValueToFind)
                    {
                        bBoolValue = bValueToFind;
                        break;
                    }
                }
            }
        }

        m_bBoolValue = (bBoolValue != 0);
        m_bPreviousValue = bInterpValue;
    }
    
    if (m_bBoolValue == INVALID_BOOL)
    {
        bValue = false;
        return false;
    }

    bValue = (m_bBoolValue != 0);
    m_fLastTime = fTime;
    return true;

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBoolTimelineInterpolator);
//---------------------------------------------------------------------------
void NiBoolTimelineInterpolator::CopyMembers(
    NiBoolTimelineInterpolator* pkDest, NiCloningProcess& kCloning)
{
    NiBoolInterpolator::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBoolTimelineInterpolator);

//---------------------------------------------------------------------------
void NiBoolTimelineInterpolator::LoadBinary(NiStream& kStream)
{
    NiBoolInterpolator::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiBoolTimelineInterpolator::LinkObject(NiStream& kStream)
{
    NiBoolInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBoolTimelineInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiBoolInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBoolTimelineInterpolator::SaveBinary(NiStream& kStream)
{
    NiBoolInterpolator::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBoolTimelineInterpolator::IsEqual(NiObject* pkObject)
{
    return NiBoolInterpolator::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBoolTimelineInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBoolInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBoolTimelineInterpolator::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
