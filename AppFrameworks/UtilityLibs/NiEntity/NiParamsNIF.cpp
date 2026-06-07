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
#include "NiEntityPCH.h"
#include "NiParamsNIF.h"

//---------------------------------------------------------------------------
NiImplementRTTI(NiParamsNIF, NiExternalAssetParams);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiParamsNIF::NiParamsNIF()
{
    m_spSceneRoot = 0;
}
//---------------------------------------------------------------------------
NiParamsNIF::~NiParamsNIF()
{
    m_spSceneRoot = 0;
}
//---------------------------------------------------------------------------
NiBool NiParamsNIF::GetSceneRoot(NiAVObject*& pkAVObject)
{
    if (!m_spSceneRoot)
        return false;

    pkAVObject = m_spSceneRoot;
    return true;
}
//---------------------------------------------------------------------------
void NiParamsNIF::SetSceneRoot(NiAVObject* pkAVObject)
{
    m_spSceneRoot = pkAVObject;
}
//---------------------------------------------------------------------------
