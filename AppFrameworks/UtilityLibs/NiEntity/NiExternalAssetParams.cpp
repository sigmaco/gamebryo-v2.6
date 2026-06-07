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
#include "NiExternalAssetParams.h"

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiExternalAssetParams);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiExternalAssetParams::NiExternalAssetParams()
{
    m_kAssetPath = NULL;
}
//---------------------------------------------------------------------------
NiExternalAssetParams::~NiExternalAssetParams()
{
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetParams::SetAssetPath(
    const NiFixedString& kAssetPath)
{
    // Ensure that only absolute paths are registered
    if (!NiPath::IsUniqueAbsolute(kAssetPath))
    {
        return false;
    }

    m_kAssetPath = kAssetPath;

    return true;
}
//---------------------------------------------------------------------------
NiFixedString NiExternalAssetParams::GetAssetPath() const
{
    return m_kAssetPath;
}
//---------------------------------------------------------------------------
