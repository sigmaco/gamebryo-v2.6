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

#include "NiTerrainPhysXPCH.h"
#include "NiTerrainPhysXContactModify.h"
#include "NiTerrainPhysXUtils.h"
#include <NiPhysXTypes.h>
#include <NiTerrainInteractor.h>
#include <NiTerrain.h>
#include <NiMetaData.h>

NiTerrainPhysXContactModify::NiTerrainPhysXContactModify()
{    
}
//---------------------------------------------------------------------------
NiTerrainPhysXContactModify::~NiTerrainPhysXContactModify()
{    
}
//---------------------------------------------------------------------------
bool NiTerrainPhysXContactModify::onContactConstraint(NxU32& changeFlags,
    const NxShape* shape0, const NxShape* shape1, const NxU32 featureIndex0,
    const NxU32 featureIndex1, NxContactCallbackData& data)
{
    NI_UNUSED_ARG(featureIndex1);
    NI_UNUSED_ARG(featureIndex0);

    if (shape0->isHeightField())
    {
        NiTerrainInteractor* pkInteractor = (NiTerrainInteractor*)
            (shape0->userData);

        if (pkInteractor)
        {
            OnContactTerrain(pkInteractor, changeFlags, shape0, data, true);
        }
        return true;
    }
    
    if (shape1->isHeightField())
    {
        NiTerrainInteractor* pkInteractor = (NiTerrainInteractor*)
            (shape1->getActor().userData);

        if (pkInteractor)
        {
            OnContactTerrain(pkInteractor, changeFlags, shape1, data, false);
        }
        return true;
    }

    return true;

}
//---------------------------------------------------------------------------
void NiTerrainPhysXContactModify::OnContactTerrain(
    NiTerrainInteractor* pkInteractor, 
    NxU32& changeFlags,
    const NxShape* pkCollidingShape,
    NxContactCallbackData& data,
    bool bTerrainIs0)
{
    NiPoint3 kPosition;
    NiPhysXTypes::NxVec3ToNiPoint3(pkCollidingShape->getGlobalPosition(), 
        kPosition);

    kPosition = kPosition * m_fScaleFactor;

    // Transform into Terrain Space so the ray is cast according to the
    // terrain's rotation. 
    NiTransform kTerrainTransform = 
        pkInteractor->GetTerrain()->GetWorldTransform();
    NiTransform kInvTerrainTransform;
    kTerrainTransform.Invert(kInvTerrainTransform);

    NiPoint3 kTerrainPosition = kInvTerrainTransform * kPosition;
    NiRay kTerrainRay(
        NiPoint3(kTerrainPosition.x, kTerrainPosition.y, NI_INFINITY),
        NiPoint3::UNIT_Z);

    // Transform the terrain space ray into world space
    NiRay kWorldRay(kTerrainTransform * kTerrainRay.GetOrigin(),
        kTerrainTransform * kTerrainRay.GetDirection());

    NiMetaData kBlendedMetaData;
    NiTFixedStringMap<NiMetaData> kMetaData;
    pkInteractor->GetBlendedMetaData(kWorldRay, kMetaData, kBlendedMetaData);

    float fStaticFriction;
    float fDynFriction;
    float fRestitution;
    float fWeight;

    if (kBlendedMetaData.Get(NiTerrainPhysXUtils::ms_kStaticFriction, 
        fStaticFriction, fWeight))
    {
        if (bTerrainIs0)
        {
            data.staticFriction0 = fStaticFriction;
            changeFlags |= NX_CCC_STATICFRICTION0;
        }
        else
        {
            data.staticFriction1 = fStaticFriction;
            changeFlags |= NX_CCC_STATICFRICTION1;
        }
    }

    if (kBlendedMetaData.Get(NiTerrainPhysXUtils::ms_kDynamicFriction, 
        fDynFriction, fWeight))
    {
        if (bTerrainIs0)
        {
            data.dynamicFriction0 = fDynFriction;
            changeFlags |= NX_CCC_DYNAMICFRICTION0;
        }
        else
        {
            data.dynamicFriction1 = fDynFriction;
            changeFlags |= NX_CCC_DYNAMICFRICTION1;
        }
    }
    
    if (kBlendedMetaData.Get(NiTerrainPhysXUtils::ms_kRestitution, 
        fRestitution, fWeight))
    {
        data.restitution = fRestitution;
        changeFlags |= NX_CCC_RESTITUTION;
    }

}
//---------------------------------------------------------------------------
void NiTerrainPhysXContactModify::SetScaleFactor(float fScale)
{
    m_fScaleFactor = fScale;
}
//---------------------------------------------------------------------------
float NiTerrainPhysXContactModify::GetScaleFactor()
{
    return m_fScaleFactor;
}
