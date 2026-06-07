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

#ifndef NIEXTERNALASSETPARAMS_H
#define NIEXTERNALASSETPARAMS_H

#include "NiEntityLibType.h"

#include <NiBool.h>
#include <NiFixedString.h>
#include <NiRTTI.h>
#include <NiSystem.h>

class NIENTITY_ENTRY NiExternalAssetParams : public NiMemObject
{
    NiDeclareRootRTTI(NiExternalAssetParams);

public:
    NiExternalAssetParams();
    virtual ~NiExternalAssetParams();

    NiFixedString GetAssetPath() const;
    NiBool SetAssetPath(const NiFixedString& kAssetPath);  

protected:
    NiFixedString m_kAssetPath;  
};

#endif // NIEXTERNALASSETPARAMS_H
