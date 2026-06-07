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

#ifndef NILIGHTPROXYCOMPONENT_H
#define NILIGHTPROXYCOMPONENT_H

#include "NiProxyComponent.h"

class NiLightProxyComponent : public NiProxyComponent
{
public:
    NiLightProxyComponent();
    NiLightProxyComponent(NiEntityInterface* pkMasterEntity,
        const NiFixedString& kAttachmentPointName);

    static NiFixedString ClassName();

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    bool SetLightType(const NiFixedString& kLightType);

    NiFixedString m_kLightType;

    static NiFixedString ms_kLightProxyComponentName;
    static NiFixedString ms_kLightProxyComponentClassName;
    static NiFixedString ms_kLightTypeName;
    static NiFixedString ms_kSwitchNodeName;
    static NiFixedString ms_kPointLightName;
    static NiFixedString ms_kDirectionalLightName;
    static NiFixedString ms_kSpotLightName;
    static NiFixedString ms_kAmbientLightName;

public:
    // NiProxyComponent overrides
    virtual NiBool SetTemplateID(const NiUniqueID& kTemplateID);
    virtual NiUniqueID GetTemplateID();
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties);
    virtual NiFixedString GetName() const;
    virtual NiFixedString GetClassName() const;
    virtual void Update(NiEntityPropertyInterface* pkParentEntity,
        float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);
};

NiSmartPointer(NiLightProxyComponent);

#endif // #ifndef NILIGHTPROXYCOMPONENT_H
