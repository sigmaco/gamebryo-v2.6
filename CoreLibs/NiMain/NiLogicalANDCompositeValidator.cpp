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

#include "NiLogicalANDCompositeValidator.h"

NiImplementRTTI(NiLogicalANDCompositeValidator, NiCompositeValidator);

//---------------------------------------------------------------------------
bool NiLogicalANDCompositeValidator::ValidateClick(
    NiRenderClick* pkRenderClick, unsigned int uiFrameID)
{
    // Iterate over validator list, returning false if any validator fails.
    NiTListIterator kIter = m_kValidators.GetHeadPos();
    while (kIter)
    {
        NiRenderClickValidator* pkValidator = m_kValidators.GetNext(kIter);
        if (!pkValidator->ValidateClick(pkRenderClick, uiFrameID))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
