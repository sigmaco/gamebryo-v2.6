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

#ifndef NIFACTORIES_H
#define NIFACTORIES_H

#include <NiFixedString.h>
#include "NiTFactory.h"
#include "NiExternalAssetHandler.h"
#include "NiEntityStreamingAscii.h"
#include "NiEntityPropertyInterface.h"

class NIENTITY_ENTRY NiFactories
{
public:
    static NiTFactory<NiEntityStreaming*>* GetStreamingFactory();
    static NiTFactory<NiExternalAssetHandler*>* GetAssetFactory();
    static NiTFactory<NiEntityPropertyInterface*>* GetEntityCompFactory();
    
    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    static void _SDMRegister();
    // *** end Emergent internal use only ***

private:
    static NiTFactory<NiEntityStreaming*>* ms_pkStreamingFactory;
    static NiTFactory<NiExternalAssetHandler*>* ms_pkAssetFactory;
    static NiTFactory<NiEntityPropertyInterface*>* ms_pkEntityCompFactory;
};

// Factory Registration Macros
#define NiFactoryRegisterStreaming(streamingclass) \
    NiFactoryRegisterPersistent(streamingclass, \
    NiFactories::GetStreamingFactory())

#define NiFactoryRegisterAssetHandler(assetclass) \
    NiFactoryRegisterPersistent(assetclass, NiFactories::GetAssetFactory())

#define NiFactoryRegisterPropIntf(propintfclass) \
    NiFactoryRegister(propintfclass, NiFactories::GetEntityCompFactory())

// Factory Declaration Macros
#define NiFactoryDeclareStreaming(streamingclass) \
class streamingclass##Creator \
{   \
public: \
    static void RegisterPersistent( \
        NiTFactory<NiEntityStreaming*>* pkFactory) \
    {   \
        NiFixedString kName(streamingclass::STREAMING_EXTENSION); \
        pkFactory->RegisterPersistent( \
            kName, \
            streamingclass##Creator::Create, \
            streamingclass##Creator::Destroy); \
    }   \
private:    \
    static NiEntityStreaming* Create(const char*)   \
    {   \
        return NiNew streamingclass;    \
    }   \
    static void Destroy(NiEntityStreaming* pkDelete) \
    {   \
        NiDelete ((streamingclass*)pkDelete);  \
    }   \
}; 

#define NiFactoryDeclareAssetHandler(assetclass) \
class assetclass##Creator \
{   \
public: \
    static void RegisterPersistent( \
        NiTFactory<NiExternalAssetHandler*>* pkFactory) \
    {   \
        NiFixedString kName(assetclass::T_ASSETTYPE); \
        pkFactory->RegisterPersistent( \
            kName, \
            assetclass##Creator::Create, \
            assetclass##Creator::Destroy); \
    }   \
private:    \
    static NiExternalAssetHandler* Create(const char*)  \
    {   \
        return NiNew assetclass;    \
    }   \
    static void Destroy(NiExternalAssetHandler* pkDelete) \
    {   \
        NiDelete ((assetclass*)pkDelete);  \
    }   \
}; 

#define NiFactoryDeclarePropIntf(propintfclass) \
class propintfclass##Creator \
{   \
public: \
    static void Register(NiTFactory<NiEntityPropertyInterface*>* pkFactory) \
    {   \
        NiFixedString kName(#propintfclass); \
        pkFactory->Register( \
            kName, \
            propintfclass##Creator::Create); \
    }   \
private:    \
    static NiEntityPropertyInterface* Create(const char*)   \
    {   \
        return NiNew propintfclass; \
    }   \
}; 

#endif // NITFACTORIES_H
