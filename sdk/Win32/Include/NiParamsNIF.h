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

#ifndef NIPARAMSNIF_H
#define NIPARAMSNIF_H

#include "NiEntityLibType.h"

#include "NiExternalAssetParams.h"
#include <NiAVObject.h>
#include <NiSystem.h>

class NIENTITY_ENTRY NiParamsNIF : public NiExternalAssetParams
{
    NiDeclareRTTI;

public:
    NiParamsNIF();
    virtual ~NiParamsNIF();

    NiBool GetSceneRoot(NiAVObject*& pkAVObject);
    void SetSceneRoot(NiAVObject* pkAVObject);

protected:
    NiAVObjectPtr m_spSceneRoot;
};

#endif // NIPARAMSNIF_H
