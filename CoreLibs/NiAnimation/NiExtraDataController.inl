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

#include <NiObjectNET.h>

//---------------------------------------------------------------------------
inline const NiFixedString& NiExtraDataController::GetExtraDataName()
{
    return m_kExtraDataName;
}
//---------------------------------------------------------------------------
inline NiExtraData* NiExtraDataController::GetExtraDataPointer()
{
    return m_spExtraData;
}
//---------------------------------------------------------------------------
inline void NiExtraDataController::GetExtraDataPointerFromName()
{
    NIASSERT(m_pkTarget && m_kExtraDataName);
    m_spExtraData = m_pkTarget->GetExtraData(m_kExtraDataName);
    NIASSERT(m_spExtraData);
}
//---------------------------------------------------------------------------
inline void NiExtraDataController::SetExtraDataName(const NiFixedString& kName)
{
    m_kExtraDataName = kName;
}
//---------------------------------------------------------------------------
