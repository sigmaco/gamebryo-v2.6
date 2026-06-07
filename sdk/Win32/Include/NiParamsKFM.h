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

#ifndef NIPARAMSKFM_H
#define NIPARAMSKFM_H

#include "NiEntityLibType.h"
#include "NiExternalAssetParams.h"
#include <NiRTTI.h>
#include <NiSystem.h>

class NiActorManager;

class NIENTITY_ENTRY NiParamsKFM : public NiExternalAssetParams
{
    NiDeclareRTTI;

public:
    NiParamsKFM(bool bAccum = false);
    virtual ~NiParamsKFM();

    void SetAccumulated(bool bAccum);
    bool GetAccumulated();

    NiActorManager* GetActor();
    
    // *** begin Emergent internal use only ***
    void SetActor(NiActorManager* pkActor);
    // *** end Emergent internal use only ***

protected:
    bool m_bAccumulated;
    NiActorManager* m_pkActor;
};

#endif // NIPARAMSKFM_H
