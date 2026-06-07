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

// Precompiled Headers
#include "MayaPluginPCH.h"


//---------------------------------------------------------------------------
//
//      TEXTURE EFFECT MANAGER
//
//---------------------------------------------------------------------------
MyiTextureEffectManager::MyiTextureEffectManager()
{
    m_bFail = false;

    // Examine each of the lights looking for a Texture Effect
    int iLightCount;
    DtLightGetCount(&iLightCount);

    for( int iLightID = 0; iLightID < iLightCount; iLightID++)
    {
        if(IsTextureEffect(iLightID))
        {
            AddTextureEffect(iLightID);
        }
    }
}
//---------------------------------------------------------------------------
MyiTextureEffectManager::~MyiTextureEffectManager()
{

    // Delete all of the texture effects
    while(!m_pTextureEffects.IsEmpty())
    {
        NiDelete m_pTextureEffects.RemoveHead();
    }
}
//---------------------------------------------------------------------------
NiTextureEffect *MyiTextureEffectManager::GetTextureEffect(int iLightID)
{
    NiTListIterator kIter = m_pTextureEffects.GetHeadPos();

    while(kIter != NULL)
    {
        // Check for the Light ID
        if(m_pTextureEffects.Get(kIter)->GetLightID() == iLightID)
        {
            return m_pTextureEffects.Get(kIter)->GetTextureEffect();
        }

        m_pTextureEffects.GetNext(kIter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MyiTextureEffectManager::AddTextureEffect(int iLightID)
{
    MyiTextureEffect* pkNewEffect = NiNew MyiTextureEffect(iLightID);

    if(pkNewEffect->GetTextureEffect()->GetEffectTexture() != NULL)
    {
        m_pTextureEffects.AddHead(pkNewEffect);
    }
    else
    {

    }

    
}
//---------------------------------------------------------------------------
bool MyiTextureEffectManager::IsTextureEffect(int iLightID)
{
    MObject Transform;
    MFnDagNode dgNode;

    // Get the transform based on the LightID
    DtExt_LightGetTransform(iLightID, Transform);

    dgNode.setObject(Transform);

    // Check for the Extra attributes
    if(CheckForExtraAttribute(dgNode, "Ni_DynamicTextureMap", true))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//      TEXTURE EFFECTS
//
//---------------------------------------------------------------------------
MyiTextureEffect::MyiTextureEffect()
{
    m_spTextureEffect = NULL;
    m_iLightID = -1;
}
//---------------------------------------------------------------------------
MyiTextureEffect::MyiTextureEffect(int iLightID)
{
    m_spTextureEffect = NULL;
    m_iLightID = iLightID;

    LoadTextureEffect();
}
//---------------------------------------------------------------------------
void MyiTextureEffect::LoadTextureEffect()
{
    MObject Transform;
    MFnDagNode dgNode;

    // Get the transform based on the LightID
    DtExt_LightGetTransform(m_iLightID, Transform);

    dgNode.setObject(Transform);

    // Check for the Extra attributes
    if(!CheckForExtraAttribute(dgNode, "Ni_DynamicTextureMap", true))
    {
        NIASSERT(false);      // We should never get here
        return;
    }

    int iEffectType;
    if(!GetExtraAttribute(dgNode, "Ni_TextureType", true, iEffectType))
    {
        NIASSERT(false);      // We should never get here
        return;
    }

    int iCoordinateType;
    if(!GetExtraAttribute(dgNode, "Ni_CoordinateType", true, iCoordinateType))
    {
        NIASSERT(false);      // We should never get here
        return;
    }

    float fParallelRadius;
    if(!GetExtraAttribute(dgNode, "Ni_ParallelRadius", true, fParallelRadius))
    {
        NIASSERT(false);      // We should never get here
        return;
    }

    float fPerspectiveAngle;
    if(!GetExtraAttribute(dgNode, "Ni_PerspectiveAngle", true, 
        fPerspectiveAngle))
    {
        NIASSERT(false);      // We should never get here
        return;
    }

    int iClampMode;
    if(!GetExtraAttribute(dgNode, "Ni_ClampMode", true, iClampMode))
    {
        NIASSERT(false);      // We should never get here
        return;
    }

    int iFilter;
    if(!GetExtraAttribute(dgNode, "Ni_Filter", true, iFilter))
    {
        NIASSERT(false);      // We should never get here
        return;
    }

    // Default to false to preserve the existing behavior
    bool bUseClippingPlane = false;
    GetExtraAttribute(dgNode, "Ni_Use_Clipping_Plane", true,
        bUseClippingPlane);

    // Find the Light Translation
    float fX, fY, fZ;
    DtLightGetTranslation(m_iLightID, &fX, &fY, &fZ);

    // Find the Light Rotations
    float fRotateX, fRotateY, fRotateZ;
    DtLightGetRotation(m_iLightID, &fRotateX, &fRotateY, &fRotateZ);

    m_spTextureEffect = NiNew NiTextureEffect();
    m_spTextureEffect->SetTextureFilter(
        (NiTexturingProperty::FilterMode)iFilter);

    m_spTextureEffect->SetTextureClamp(
        (NiTexturingProperty::ClampMode)iClampMode);

    m_spTextureEffect->SetTextureType(
        (NiTextureEffect::TextureType)iEffectType);

    m_spTextureEffect->SetTextureCoordGen(
        (NiTextureEffect::CoordGenType)iCoordinateType);

    m_spTextureEffect->SetClippingPlaneEnable(bUseClippingPlane);
    if (bUseClippingPlane)
    {
        NiPlane kPlane(NiPoint3(0.0f, 0.0f, -1.0f), 
            NiPoint3(0.0f, 0.0f, 0.0f));
        m_spTextureEffect->SetModelClippingPlane(kPlane);
    }

    MObject Shape;
    DtExt_LightGetShapeNode(m_iLightID, Shape);
   
    MObject Texture = FollowInputAttribute(Shape, 
        MString("color"));

    NiSourceTexturePtr spTexture = 
        MyiTextureManager::GetTextureManager()->GetTexture(Texture);

    if (spTexture != NULL)
    {
        m_spTextureEffect->SetEffectTexture(spTexture);
    }
    else
    {
        // Find the Texture
        char szTextureName[512];
        szTextureName[0] = 0;
        if(!DtLightGetTextureName(m_iLightID, szTextureName, 512))
            return;

        // Setup the texture
        MyiTexture Texture(szTextureName);
        m_spTextureEffect->SetEffectTexture( Texture.GetTexture() );
    }

    if(iEffectType != NiTextureEffect::ENVIRONMENT_MAP)
    {
        switch(iCoordinateType)
        {
        case NiTextureEffect::WORLD_PARALLEL:
            {
                NiMatrix3 kDirMat(NiMatrix3::ZERO);
                float fEntry = 0.5f/fParallelRadius;
                kDirMat.SetEntry(0,0, fEntry);
                kDirMat.SetEntry(1,1, -fEntry);
                m_spTextureEffect->SetModelProjectionMatrix(kDirMat);
                
                NiPoint3 kDirTrans(0.5f, 0.5f, 0.0f);
                m_spTextureEffect->SetModelProjectionTranslation(kDirTrans);
            }
            break;
        case NiTextureEffect::WORLD_PERSPECTIVE:
            {
                m_spTextureEffect->SetTextureFilter(
                    NiTexturingProperty::FILTER_BILERP);

                // use fallsize as angle of projection
                float fEntry = 0.5f/(float)tan((NI_PI/360.0f) * 
                    fPerspectiveAngle);
                
                NiMatrix3 kSpotMat;
                kSpotMat.SetCol( 0, -fEntry, 0.0f, 0.0f);
                kSpotMat.SetCol( 1, 0.0f, fEntry, 0.0f);
                kSpotMat.SetCol( 2, 0.5f, 0.5f, 1.0f);
                
                m_spTextureEffect->SetModelProjectionMatrix(kSpotMat);
                
                m_spTextureEffect->SetModelProjectionTranslation(
                    NiPoint3::ZERO);
            }
            break;
        case NiTextureEffect::SPHERE_MAP:
        case NiTextureEffect::SPECULAR_CUBE_MAP:
        case NiTextureEffect::DIFFUSE_CUBE_MAP:
        default:
            break;
        }
    }
}
//---------------------------------------------------------------------------
