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

#include <NiUniversalTypes.h>
#include "NiObjectGroup.h"

//---------------------------------------------------------------------------
NiObjectGroup::NiObjectGroup(unsigned int uiBytes)
{
    m_uiSize = uiBytes;

    if (uiBytes)
        m_pvBuffer = NiAlloc(char,uiBytes);
    else
        m_pvBuffer = 0;

    m_pvFree = m_pvBuffer;
    m_uiRefCount = 0;
}
//---------------------------------------------------------------------------
NiObjectGroup::~NiObjectGroup()
{
    NiFree((char*) m_pvBuffer);
}
//---------------------------------------------------------------------------
