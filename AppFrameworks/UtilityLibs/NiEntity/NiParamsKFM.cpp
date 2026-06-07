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
#include "NiParamsKFM.h"

//---------------------------------------------------------------------------
NiImplementRTTI(NiParamsKFM, NiExternalAssetParams);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiParamsKFM::NiParamsKFM(bool bAccum)
{
    SetAccumulated(bAccum);
}
//---------------------------------------------------------------------------
NiParamsKFM::~NiParamsKFM()
{
}
//---------------------------------------------------------------------------
void NiParamsKFM::SetAccumulated(bool bAccum) 
{
    m_bAccumulated = bAccum; 
}
//---------------------------------------------------------------------------
bool NiParamsKFM::GetAccumulated() 
{
    return m_bAccumulated; 
}
//---------------------------------------------------------------------------
NiActorManager* NiParamsKFM::GetActor()
{
    return m_pkActor;
}
//---------------------------------------------------------------------------
void NiParamsKFM::SetActor(NiActorManager* pkActor)
{
    m_pkActor = pkActor;
}
//---------------------------------------------------------------------------
