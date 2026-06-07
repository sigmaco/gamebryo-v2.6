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

#ifndef NIPROXYCOMPONENT_H
#define NIPROXYCOMPONENT_H

class NiProxyComponent : public NiTransformationComponent
{
public:
    NiProxyComponent();
    NiProxyComponent(
        NiEntityInterface* pkSourceEntity,
        NiFixedString kAttachmentPointName = NULL);

    void UpdateScale(NiCamera* pkCamera);
    static NiFixedString ClassName();

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    // For re-scaling based off camera.
    NiEntityPropertyInterface* m_pkParentEntity;

    static NiFixedString ms_kProxyComponentName;
    static NiFixedString ms_kProxyComponentClassName;
    static float ms_fDefaultDistance;

public:
    // NiTransformationComponent overrides
    virtual NiBool SetTemplateID(const NiUniqueID& kTemplateID);
    virtual NiUniqueID GetTemplateID();
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties);
    virtual NiFixedString GetName() const;
    virtual NiFixedString GetClassName() const;
    virtual void Update(NiEntityPropertyInterface* pkParentEntity,
        float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);
    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
        NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiProxyComponent);

#endif // #ifndef NIPROXYCOMPONENT_H
