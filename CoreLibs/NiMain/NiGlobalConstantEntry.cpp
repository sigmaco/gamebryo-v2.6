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
#include "NiMainPCH.h"

#include "NiGlobalConstantEntry.h"

//---------------------------------------------------------------------------
void NiGlobalConstantEntry::SetData(unsigned int uiDataSize, 
    const void* pvDataSource)
{
    if (m_uiDataSize < uiDataSize)
    {
        NiFree(m_pvDataSource);
        m_pvDataSource = 0;
        m_uiDataSize = uiDataSize;
    }

    if (!m_pvDataSource)
    {
        //m_uiDataSize is in bytes.  Allocation should be as such.
        m_pvDataSource = NiAlloc(unsigned char, m_uiDataSize);
        NIASSERT(m_pvDataSource);
    }

    if (pvDataSource)
        NiMemcpy(m_pvDataSource, m_uiDataSize, pvDataSource, uiDataSize);
}
//---------------------------------------------------------------------------
