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
#include "SceneDesignerFrameworkPCH.h"

#include "NiLightProxyComponent.h"

NiFixedString NiLightProxyComponent::ms_kLightProxyComponentName;
NiFixedString NiLightProxyComponent::ms_kLightProxyComponentClassName;
NiFixedString NiLightProxyComponent::ms_kLightTypeName;
NiFixedString NiLightProxyComponent::ms_kSwitchNodeName;
NiFixedString NiLightProxyComponent::ms_kPointLightName;
NiFixedString NiLightProxyComponent::ms_kDirectionalLightName;
NiFixedString NiLightProxyComponent::ms_kSpotLightName;
NiFixedString NiLightProxyComponent::ms_kAmbientLightName;

//---------------------------------------------------------------------------
NiFixedString NiLightProxyComponent::ClassName()
{
    return ms_kLightProxyComponentClassName;
}
//---------------------------------------------------------------------------
void NiLightProxyComponent::_SDMInit()
{
    ms_kLightProxyComponentName = "Light Proxy Component";
    ms_kLightProxyComponentClassName = "NiLightProxyComponent";
    ms_kLightTypeName = "Light Type";
    ms_kSwitchNodeName = "NiSwitchNode";
    ms_kPointLightName = "Point";
    ms_kDirectionalLightName = "Directional";
    ms_kSpotLightName = "Spot";
    ms_kAmbientLightName = "Ambient";
}
//---------------------------------------------------------------------------
void NiLightProxyComponent::_SDMShutdown()
{
    ms_kLightProxyComponentName = NULL;
    ms_kLightProxyComponentClassName = NULL;
    ms_kLightTypeName = NULL;
    ms_kSwitchNodeName = NULL;
    ms_kPointLightName = NULL;
    ms_kDirectionalLightName = NULL;
    ms_kSpotLightName = NULL;
    ms_kAmbientLightName = NULL;
}
//---------------------------------------------------------------------------
NiLightProxyComponent::NiLightProxyComponent() : NiProxyComponent()
{
}
//---------------------------------------------------------------------------
NiLightProxyComponent::NiLightProxyComponent(
    NiEntityInterface* pkMasterEntity,
    const NiFixedString& kAttachmentPointName) : NiProxyComponent(
    pkMasterEntity, kAttachmentPointName)
{
}
//---------------------------------------------------------------------------
bool NiLightProxyComponent::SetLightType(const NiFixedString& kLightType)
{
    bool bChangedIndex = false;

    // Set the value of the switch node in our scene graph.
    if (m_pkParentEntity)
    {
        NiObject* pkObject;
        m_pkParentEntity->GetPropertyData(ms_kDepSceneRootPointer, pkObject);
        NiAVObject* pkSceneRoot = NiDynamicCast(NiAVObject, pkObject);
        if (pkSceneRoot)
        {
            NiAVObject* pkSwitchObj = pkSceneRoot->GetObjectByName(
                ms_kSwitchNodeName);
            NiSwitchNode* pkSwitchNode = NiDynamicCast(NiSwitchNode,
                pkSwitchObj);
            if (pkSwitchNode)
            {
                // Set the switch state based on the light type.
                if (kLightType == ms_kPointLightName)
                {
                    pkSwitchNode->SetIndex(0);
                    bChangedIndex = true;
                }
                else if (kLightType == ms_kDirectionalLightName)
                {
                    pkSwitchNode->SetIndex(1);
                    bChangedIndex = true;
                }
                else if (kLightType == ms_kSpotLightName)
                {
                    pkSwitchNode->SetIndex(2);
                    bChangedIndex = true;
                }
                else if (kLightType == ms_kAmbientLightName)
                {
                    pkSwitchNode->SetIndex(3);
                    bChangedIndex = true;
                }
            }
        }
    }

    return bChangedIndex;
}
//---------------------------------------------------------------------------
NiBool NiLightProxyComponent::SetTemplateID(const NiUniqueID&)
{
    // This component does not support setting the template ID.
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID NiLightProxyComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID(0xBB, 0x34, 0xF7, 0x89, 0xD8, 0xBB,
        0xA5, 0x4E, 0x87, 0xC2, 0x5A, 0xCC, 0x8, 0x81, 0xE4, 0x3D);
    return kUniqueID;
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiLightProxyComponent::Clone(
    bool)
{
    return NiNew NiLightProxyComponent(m_pkSourceEntity,
        m_kAttachmentPointName);
}
//---------------------------------------------------------------------------
NiFixedString NiLightProxyComponent::GetName() const
{
    return ms_kLightProxyComponentName;
}
//---------------------------------------------------------------------------
NiFixedString NiLightProxyComponent::GetClassName() const
{
    return ms_kLightProxyComponentClassName;
}
//---------------------------------------------------------------------------
void NiLightProxyComponent::Update(NiEntityPropertyInterface* pkParentEntity,
    float fTime, NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager)
{
    NiProxyComponent::Update(pkParentEntity, fTime, pkErrors, pkAssetManager);

    // If the light type property on the source entity has changed, change
    // the light proxy switch node index.
    if (m_pkSourceEntity)
    {
        NiFixedString kLightType;
        NIVERIFY(m_pkSourceEntity->GetPropertyData(ms_kLightTypeName,
            kLightType));
        if (kLightType != m_kLightType)
        {
            if (SetLightType(kLightType))
            {
                m_kLightType = kLightType;
            }
        }
    }
}
//---------------------------------------------------------------------------
