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
#include "NiParamsKF.h"
#include <NiMain.h>

//---------------------------------------------------------------------------
NiImplementRTTI(NiParamsKF, NiExternalAssetParams);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiParamsKF::NiParamsKF()
{
    m_spSeqData = 0;
}
//---------------------------------------------------------------------------
NiParamsKF::~NiParamsKF()
{
    m_spSeqData = 0;
}
//---------------------------------------------------------------------------
void NiParamsKF::SetAnimIndex(unsigned int uiAnimIndex)
{
    m_uiAnimIndex = uiAnimIndex;    
}
//---------------------------------------------------------------------------
unsigned int NiParamsKF::GetAnimIndex() const
{
    return m_uiAnimIndex;
}
//---------------------------------------------------------------------------
void NiParamsKF::SetSequenceName(const NiFixedString& kSequenceName)
{
    m_kSequenceName = kSequenceName;
}
//---------------------------------------------------------------------------
const NiFixedString& NiParamsKF::GetSequenceName() const
{
    return m_kSequenceName;
}
//---------------------------------------------------------------------------
NiBool NiParamsKF::GetSequenceData(NiSequenceData*& pkSeqData)
{
    pkSeqData = m_spSeqData;
    return true;
}
//---------------------------------------------------------------------------
NiBool NiParamsKF::SetSequenceData(NiSequenceData* pkSeqData)
{
    m_spSeqData = pkSeqData;
    return true;
}
//---------------------------------------------------------------------------
