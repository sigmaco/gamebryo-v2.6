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

#include "NiTerrainPCH.h"

#include <NiNode.h>
#include <NiMesh.h>
#include <NiTexture.h>
#include "NiTerrainInteractor.h"
#include "NiWaterMaterial.h"
#include "NiWater.h"

// Include DX9 Renderer for device reset callbacks
#ifdef WIN32
#include <NiDX9Renderer.h>
#endif

#ifdef _WII
#include "NiWiiWaterShader.h"
#endif

NiSourceTexturePtr NiWater::ms_spBlankNormals = 0;

NiImplementRTTI(NiWater, NiNode);

//---------------------------------------------------------------------------
NiWater::NiWater() :
    m_kShallowColor(0.0f,0.5f,0.5f,0.15f),
    m_kDeepColor(0.0f,0.2f,0.4f,1.0f),
    m_kTexTransformVelocity(0.0f, 0.0f),    
    m_uiLength(128),
    m_uiWidth(128),
    m_uiMaxVerticesPerSide(50),
    m_uiNumVerticesLength(50),
    m_uiNumVerticesWidth(50),
    m_fDepthThreshold(50.0f),
    m_fNormalWidth(4.0f),
    m_fNormalLength(4.0f),
    m_fReflectionFactor(0.0f),
    m_fSpecularity(3.0f),
    m_fDisturbance(25.0f),
    m_fFogDensity(1.0f),
    m_fFogDistance(1000.0f),
    m_fFogFallOff(1.0f),
    m_fNormalAnimationSpeed(25.0f),
    m_uiTextureSizeRatio(64),
    m_uiRefMode(REFLECTION_MODE_NONE),    
    m_kNormalMapDir(),
    m_kNormalMapFile("filename_%d.bmp"),    
    m_kEnvMap(),    
    m_kRefractionMap(),        
    m_spAssociatedTerrain(),
    m_kCurrentNormalMap(0),
    m_kTranslation(0,0,0),
    m_fTextureOffsetX(0),
    m_fTextureOffsetY(0),
    m_fPreviousTime(0),  
    m_uiTextureLength(128),
    m_uiTextureWidth(128),
    m_spWaterMesh(),
    m_spWaterMaterial(0),
    m_spTextureMask(),
    m_spEnvironmentMap(ms_spBlankNormals),
    m_spRefractionMap(0),    
    m_bRedrawTexture(true),   
    m_bReloadNormalMaps(true),
    m_bUpdateExtraData(true),
    m_bReloadEnvMap(true),
    m_bReloadRefractionMap(true),
    m_bParsingLeftToRight(true)
{
    SubscribeToDXDeviceResetNotification();
}
//---------------------------------------------------------------------------
NiWater::NiWater(NiWater* pkTemplate):
    m_kShallowColor(pkTemplate->m_kShallowColor),
    m_kDeepColor(pkTemplate->m_kDeepColor),
    m_kTexTransformVelocity(pkTemplate->m_kTexTransformVelocity),    
    m_uiLength(pkTemplate->m_uiLength),
    m_uiWidth(pkTemplate->m_uiWidth),
    m_uiMaxVerticesPerSide(pkTemplate->m_uiMaxVerticesPerSide),
    m_uiNumVerticesLength(pkTemplate->m_uiNumVerticesLength),
    m_uiNumVerticesWidth(pkTemplate->m_uiNumVerticesWidth),
    m_fDepthThreshold(pkTemplate->m_fDepthThreshold),
    m_fNormalWidth(pkTemplate->m_fNormalWidth),
    m_fNormalLength(pkTemplate->m_fNormalLength),
    m_fReflectionFactor(pkTemplate->m_fReflectionFactor),
    m_fSpecularity(pkTemplate->m_fSpecularity),
    m_fDisturbance(pkTemplate->m_fDisturbance),
    m_fFogDensity(pkTemplate->m_fFogDensity),
    m_fFogDistance(pkTemplate->m_fFogDistance),
    m_fFogFallOff(pkTemplate->m_fFogFallOff),
    m_fNormalAnimationSpeed(pkTemplate->m_fNormalAnimationSpeed),
    m_uiTextureSizeRatio(pkTemplate->m_uiTextureSizeRatio),
    m_uiRefMode(pkTemplate->m_uiRefMode),    
    m_kNormalMapDir(pkTemplate->m_kNormalMapDir),
    m_kNormalMapFile(pkTemplate->m_kNormalMapFile),    
    m_kEnvMap(pkTemplate->m_kEnvMap),    
    m_kRefractionMap(pkTemplate->m_kRefractionMap),        
    m_spAssociatedTerrain(pkTemplate->m_spAssociatedTerrain),
    m_kCurrentNormalMap(0),
    m_kTranslation(0,0,0),
    m_fTextureOffsetX(0),
    m_fTextureOffsetY(0),
    m_fPreviousTime(pkTemplate->m_fPreviousTime),  
    m_uiTextureLength(pkTemplate->m_uiTextureLength),
    m_uiTextureWidth(pkTemplate->m_uiTextureWidth),
    m_spWaterMesh(),
    m_spWaterMaterial(pkTemplate->m_spWaterMaterial),
    m_spTextureMask(),
    m_spEnvironmentMap(ms_spBlankNormals),
    m_spRefractionMap(0),    
    m_bRedrawTexture(true),   
    m_bReloadNormalMaps(true),
    m_bUpdateExtraData(true),
    m_bReloadEnvMap(true),
    m_bReloadRefractionMap(true),
    m_bParsingLeftToRight(pkTemplate->m_bParsingLeftToRight)
{
    SubscribeToDXDeviceResetNotification();
}
//---------------------------------------------------------------------------
NiWater::~NiWater()
{
    UnsubscribeToDXDeviceResetNotification();
    DetachChild(m_spWaterMesh);
   
    m_spWaterMaterial = 0;
    m_spAssociatedTerrain = 0;
    m_spTextureMask = 0;
    m_spWaterMesh = 0;

    m_spEnvironmentMap = 0;
    m_spRefractionMap = 0;

    m_kNormalMapList.RemoveAll();
    
    m_kEnvMap = NULL;    
    m_kRefractionMap = NULL;
}
//---------------------------------------------------------------------------
void NiWater::Initialize()
{
    CreateWaterMesh(GetName());  
    
    ComputeMesh();
    ComputeTexture();
    CreateMeshProperties();
    
    // When the mesh is recreated, we want to make sure we recompute the 
    // generated shader
    NiMesh::RecursiveSetMaterialNeedsUpdate(m_spWaterMesh, true);
    
#if defined(_WII)
    m_spWaterMaterial = NiMaterial::GetMaterial("NiWiiWaterShader");
    if (!m_spWaterMaterial)
        m_spWaterMaterial = NiSingleShaderMaterial::Create(NiNew NiWiiWaterShader);
#else
    if (!m_spWaterMaterial)
        m_spWaterMaterial = NiWaterMaterial::Create();
#endif
    
    if (!m_spWaterMesh->IsMaterialApplied(m_spWaterMaterial))
        m_spWaterMesh->ApplyMaterial(m_spWaterMaterial);

    m_spWaterMesh->SetActiveMaterial(m_spWaterMaterial);
}
//---------------------------------------------------------------------------
void NiWater::DoUpdate(NiUpdateProcess& kUpdate)
{   
    static float fNormalAnimTime = 0;
    float fTime = kUpdate.GetTime();
    float fDeltaTime = fTime - m_fPreviousTime;
    m_fPreviousTime = fTime;

    NiTerrainInteractor* pkInteractor = 0;
    NiPoint3 kTerrainTranslate;
    if (m_spAssociatedTerrain)
    {
        pkInteractor = m_spAssociatedTerrain->GetInteractor();
        kTerrainTranslate = m_spAssociatedTerrain->GetTranslate();
    }

    if (!pkInteractor && !m_spWaterMesh)
    {    
        Initialize();
    }
    else if (!m_spWaterMesh) 
    {        
        NiRay kRay = NiRay(NiPoint3(kTerrainTranslate.x, 
            kTerrainTranslate.y , NI_INFINITY), NiPoint3(0.0f, 0.0f, -1.0f));

        if (pkInteractor->Collide(kRay))
           Initialize();
    }

    if (!m_spWaterMesh)
        return;

    // Change the Normal map to animate the water
    NiTexturingPropertyPtr spToChange = 
        NiDynamicCast(NiTexturingProperty, 
        m_spWaterMesh->GetProperty(NiTexturingProperty::GetType()));
    if (!spToChange)
        return;

    bool bComputeMaterial = false;
    if (m_bReloadNormalMaps)
    {
        LoadNormalMaps();
        if (!m_kCurrentNormalMap && m_kNormalMapList.GetHeadPos())
            m_kCurrentNormalMap = m_kNormalMapList.GetHeadPos();

        if (m_kCurrentNormalMap)
        {
            spToChange->GetNormalMap()->SetTexture(
                m_kNormalMapList.GetNext(m_kCurrentNormalMap));
        }
        bComputeMaterial = true;
    }

    if (m_bUpdateExtraData)
    {
        UpdateExtraData();
    }

    if (m_bReloadEnvMap)
    {
        LoadEnvMap();
        bComputeMaterial = true;
    }

    if (m_bReloadRefractionMap)
    {
        LoadRefractionMap();
        bComputeMaterial = true;
    }

    NiSourceTexturePtr spMap;

    if (spToChange->GetNormalMap())
    {
        fNormalAnimTime += fDeltaTime;
        NiUInt32 uiNumFrames = (NiUInt32)(m_fNormalAnimationSpeed
            * fNormalAnimTime);

        if (uiNumFrames != 0)
            fNormalAnimTime = 0;

        for (NiUInt32 ui = 0; ui < uiNumFrames; ++ui)
        {
            if (!m_kCurrentNormalMap && m_kNormalMapList.GetHeadPos())
            {
                if (m_bParsingLeftToRight)
                {
                    m_bParsingLeftToRight = false;
                    m_kCurrentNormalMap = m_kNormalMapList.GetTailPos();
                    m_kCurrentNormalMap = 
                        m_kNormalMapList.GetPrevPos(m_kCurrentNormalMap);
                }
                else
                {
                    m_bParsingLeftToRight = true;
                    m_kCurrentNormalMap = m_kNormalMapList.GetHeadPos();
                    m_kCurrentNormalMap = 
                        m_kNormalMapList.GetNextPos(m_kCurrentNormalMap);
                }
            }

            if (m_kCurrentNormalMap)
            {
                if (m_bParsingLeftToRight)
                {
                    spMap = m_kNormalMapList.GetNext(m_kCurrentNormalMap);
                }
                else
                {
                    spMap = m_kNormalMapList.GetPrev(m_kCurrentNormalMap);
                }
            }
        }

        if (spMap)
            spToChange->GetNormalMap()->SetTexture(spMap);           

        // Translate the normal map to give an impression of motion   
        NiPoint2 kTexTrans = 
            spToChange->GetNormalMap()->GetTextureTransform()->
            GetTranslate();

        kTexTrans.x += GetNormalVelocity().x * fDeltaTime;
        kTexTrans.y += GetNormalVelocity().y * fDeltaTime;

        spToChange->GetNormalMap()->GetTextureTransform()->
            SetTranslate(kTexTrans);
        
    }

    if (m_spEnvironmentMap && spToChange->GetShaderMap(0))
    {
        spToChange->GetShaderMap(0)->SetTexture(m_spEnvironmentMap);
        bComputeMaterial = true;
    }

    // Refraction
    if (m_spRefractionMap && spToChange->GetShaderMap(1))
    {
        spToChange->GetShaderMap(1)->SetTexture(m_spRefractionMap);
        bComputeMaterial = true;
    }
    spToChange->Update(fTime);

    // A texture setting has changed (loaded normals...) make sure we have
    // the right version of the shader.
    if (bComputeMaterial)
        NiMesh::RecursiveSetMaterialNeedsUpdate(m_spWaterMesh, true);

    if (m_spAssociatedTerrain)
    {
        bool bModified = m_spAssociatedTerrain->HasShapeChangedLastUpdate();

        if (bModified)
        {
            m_bRedrawTexture = true;
            m_spAssociatedTerrain->SetShapeChangedLastUpdate(false);
        }
    }

    NiMatrix3 kRotate;
    float fScale;
   
    NiPoint3 kOldTranslate = m_spWaterMesh->GetWorldTranslate();
    NiMatrix3 kOldRotate = m_spWaterMesh->GetWorldRotate();
    float fOldScale = m_spWaterMesh->GetWorldScale();
            
    m_kTranslation = GetTranslate();
    kRotate = GetRotate();
    fScale = GetScale();   
    
    if (kOldTranslate != m_kTranslation ||
        kOldRotate != kRotate ||
        fOldScale != fScale)
    {
        m_bRedrawTexture = true;
    }

    if (m_bRedrawTexture)
    {
        ComputeUVStream();
        ComputeTexture();
    }

}
//---------------------------------------------------------------------------
void NiWater::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{        
    DoUpdate(kUpdate);
    NiNode::UpdateDownwardPass(kUpdate); 
}
//---------------------------------------------------------------------------
void NiWater::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{    
    if (GetSelectiveUpdateTransforms())
    {
        DoUpdate(kUpdate);
    }
    NiNode::UpdateSelectedDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiWater::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    if (GetSelectiveUpdateTransforms())
    {
        DoUpdate(kUpdate);
    } 
    
    NiNode::UpdateRigidDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiWater::SetMaxVerticesPerSide(NiUInt32 uiMaxVerticesPerSide)
{
    float fLongestSide = static_cast<float>(NiMax((int)GetWidth(),
        (int)GetLength()));

    NiUInt32 uiNumVerticesLength = static_cast<NiUInt32>(
        (float)uiMaxVerticesPerSide * ((float)GetLength() / fLongestSide));

    NiUInt32 uiNumVerticesWidth = static_cast<NiUInt32>(
        (float)uiMaxVerticesPerSide * ((float)GetWidth() / fLongestSide));

    if (uiMaxVerticesPerSide > 1 &&
        uiNumVerticesLength > 1 &&
        uiNumVerticesWidth > 1 &&
        (GetMaxVerticesPerSide() != uiMaxVerticesPerSide ||
        GetNumLengthVertices() != uiNumVerticesLength ||
        GetNumWidthVertices() != uiNumVerticesWidth))
    {
        DetachChild(m_spWaterMesh);
        m_spWaterMesh = 0;
        m_spTextureMask = 0;        
        m_uiMaxVerticesPerSide = uiMaxVerticesPerSide;
        m_uiNumVerticesLength = uiNumVerticesLength;
        m_uiNumVerticesWidth = uiNumVerticesWidth;
    }
}
//---------------------------------------------------------------------------
float NiWater::WidthLerp(float fMinValue, float fMaxValue, NiUInt32 uiVertex)
{
    NIASSERT(uiVertex < GetNumWidthVertices());

    float fScale = static_cast<float>(uiVertex) / 
        static_cast<float>(GetNumWidthVertices()-1);

    return fMinValue + fScale *(fMaxValue - fMinValue);
}
//---------------------------------------------------------------------------
float NiWater::LengthLerp(float fMinValue, float fMaxValue, NiUInt32 uiVertex)
{
    NIASSERT(uiVertex < GetNumLengthVertices());

    float fScale = static_cast<float>(uiVertex) / 
        static_cast<float>(GetNumLengthVertices()-1);

    return fMinValue + fScale *(fMaxValue - fMinValue);
}
//---------------------------------------------------------------------------
NiUInt32 NiWater::GetVertexIndex(NiUInt32 uiRow, NiUInt32 uiCol)
{
    NIASSERT(uiCol < GetNumLengthVertices());
    NIASSERT(uiRow < GetNumWidthVertices());

    return uiRow + (uiCol * GetNumWidthVertices());
}
//---------------------------------------------------------------------------
void NiWater::CreateWaterMesh(NiFixedString kEntityName)
{
    m_spWaterMesh = NiNew NiMesh();
    AttachChild(m_spWaterMesh);    

    NiDataStreamRef* pkStreamRef = 0;
        
    NiString kName;
    kName.Format("%s_mesh", (const char*)kEntityName);
    m_spWaterMesh->SetName((const char*)NiFixedString(kName));
       
    m_spWaterMesh->SetSubmeshCount(1);
    m_spWaterMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);
    
    NiUInt32 uiSize = 0;
    if (GetLength() >= GetWidth())
        uiSize = GetLength();
    else
        uiSize = GetWidth();

    NiBound kModelBound;
    kModelBound.SetCenterAndRadius(NiPoint3(0,0,0), 
        NiSqrt(2) * (uiSize / 2.0f));
    m_spWaterMesh->SetModelBound(kModelBound);
        
    NiUInt32 uiNumVerts = 
        (GetNumLengthVertices() * GetNumWidthVertices());
    NIASSERT( uiNumVerts == 
        GetVertexIndex(GetNumWidthVertices()-1, GetNumLengthVertices()-1)+1);

    NiUInt32 uiNumIndices = 
        6 * (GetNumLengthVertices() - 1) * (GetNumWidthVertices() - 1);

    // Create the streams
    // position stream
    pkStreamRef = m_spWaterMesh->AddStream(
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, 
        uiNumVerts,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX);
    
    // Texture stream 
    pkStreamRef = m_spWaterMesh->AddStream(
        NiCommonSemantics::TEXCOORD(), 0,
        NiDataStreamElement::F_FLOAT32_2, 
        uiNumVerts,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX);
    
    // Normal stream 
    pkStreamRef = m_spWaterMesh->AddStream(
        NiCommonSemantics::NORMAL(), 0,
        NiDataStreamElement::F_FLOAT32_3,
        uiNumVerts,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX);

    // Tangent stream
    pkStreamRef = m_spWaterMesh->AddStream(
        NiCommonSemantics::TANGENT(), 0,
        NiDataStreamElement::F_FLOAT32_3,
        uiNumVerts,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX);
   
    // Index stream
    pkStreamRef = m_spWaterMesh->AddStream(
        NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1, 
        uiNumIndices,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX);
}
//---------------------------------------------------------------------------
void NiWater::CreateMeshProperties()
{
    NIASSERT(m_spWaterMesh);
    NIASSERT(m_spTextureMask);
           
    // Create the texturing property
    NiTexturingPropertyPtr spTextProp = NiNew NiTexturingProperty();
    m_spWaterMesh->AttachProperty(spTextProp);

    // Base texture
    NiTexturingProperty::ShaderMap* pkBase = 
        NiNew NiTexturingProperty::ShaderMap();
    pkBase->SetClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    pkBase->SetFilterMode(NiTexturingProperty::FILTER_BILERP);
    pkBase->SetTexture(m_spTextureMask);
    pkBase->SetTextureIndex(0);
    spTextProp->SetBaseMap(pkBase);

    m_spWaterMesh->Update(0.0f);
    m_spWaterMesh->UpdateProperties();
    m_spWaterMesh->UpdateEffects();
    m_spWaterMesh->Update(0.0f);

    // Environment map
    NiTexturingProperty::ShaderMap* pkEnv = 
        NiNew NiTexturingProperty::ShaderMap();
    pkEnv->SetClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    pkEnv->SetFilterMode(NiTexturingProperty::FILTER_BILERP);
    pkEnv->SetTextureIndex(0);
    spTextProp->SetShaderMap(0, pkEnv);

    // Refraction map
    NiTexturingProperty::ShaderMap* pkRefr = 
        NiNew NiTexturingProperty::ShaderMap();
    pkRefr->SetClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    pkRefr->SetFilterMode(NiTexturingProperty::FILTER_BILERP);
    pkRefr->SetTextureIndex(0);
    spTextProp->SetShaderMap(1, pkRefr);

    spTextProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);

    // Normal map
    NiTexturingProperty::ShaderMap* pkMap = 
        NiNew NiTexturingProperty::ShaderMap();
    
    pkMap->SetTextureIndex(3);
    pkMap->SetFilterMode(NiTexturingProperty::FILTER_BILERP);
    pkMap->SetClampMode(NiTexturingProperty::WRAP_S_WRAP_T);
    pkMap->SetTextureTransform(NiNew NiTextureTransform(NiPoint2(0, 0), 0,
        NiPoint2(1,1), NiPoint2(0.5,0.5), 
        NiTextureTransform::MAX_TRANSFORM));
    pkMap->SetTexture(ms_spBlankNormals);
    spTextProp->SetNormalMap(pkMap);
      
    if (m_kCurrentNormalMap != 0)
    {        
        pkMap->SetTexture(m_kNormalMapList.Get(m_kCurrentNormalMap));
    }

    // We then add an alpha property
    NiAlphaPropertyPtr spAlpha = NiNew NiAlphaProperty();
    spAlpha->SetAlphaBlending(true);
    spAlpha->SetNoSorter(false);
    spAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    spAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    spAlpha->SetAlphaTesting(true);
    spAlpha->SetTestMode(NiAlphaProperty::TEST_GREATER);
       

    NiMaterialPropertyPtr spMatProp = NiNew NiMaterialProperty();
    spMatProp->SetAlpha(0.0);
    spMatProp->SetAmbientColor(NiColor(0.9f,0.9f,0.9f));
    spMatProp->SetDiffuseColor(NiColor(0.1f,0.1f,0.1f));
    spMatProp->SetSpecularColor(NiColor(1.0f,1.0f,1.0f));
    spMatProp->SetEmittance(NiColor(0.8f,0.8f,0.8f));
    spMatProp->SetShineness(10);

    // Finally a color property
    NiVertexColorProperty *pkVertexColorProperty = 
        NiNew NiVertexColorProperty();
    pkVertexColorProperty->SetSourceMode
        (NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVertexColorProperty->SetLightingMode
        (NiVertexColorProperty::LIGHTING_E_A_D);

    NiWireframePropertyPtr spWireframe = NiNew NiWireframeProperty;
    spWireframe->SetWireframe(false);
           
    // Attach the properties
    m_spWaterMesh->AttachProperty(pkVertexColorProperty);
    m_spWaterMesh->AttachProperty(spAlpha);
    m_spWaterMesh->AttachProperty(spMatProp);
    m_spWaterMesh->AttachProperty(spWireframe);

    m_spWaterMesh->Update(0.0f);
    m_spWaterMesh->UpdateProperties();
    m_spWaterMesh->UpdateEffects();
    m_spWaterMesh->Update(0.0f);
}
//---------------------------------------------------------------------------
void NiWater::ComputeMesh()
{
    // Create the locks
    NiDataStreamElementLock kPosLock(m_spWaterMesh, 
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_READ |
        NiDataStream::LOCK_WRITE);    
    NiDataStreamElementLock kNormalLock(m_spWaterMesh, 
        NiCommonSemantics::NORMAL(),
        0, NiDataStreamElement::F_FLOAT32_3,NiDataStream::LOCK_READ | 
        NiDataStream::LOCK_WRITE);
    NiDataStreamElementLock kTangentLock(m_spWaterMesh, 
        NiCommonSemantics::TANGENT(),
        0, NiDataStreamElement::F_FLOAT32_3,NiDataStream::LOCK_READ | 
        NiDataStream::LOCK_WRITE);

    // Create the iterators
    NiTStridedRandomAccessIterator<NiPoint3> kPositionIter = 
        kPosLock.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint3> kNormalIter = 
        kNormalLock.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint3> kTangentIter = 
        kTangentLock.begin<NiPoint3>();
    
    // Create Verts
    for(NiUInt32 uiCol = 0; uiCol < GetNumLengthVertices(); ++uiCol)
    {
        NiPoint3 kPos(0.0f, 0.0f, 0.0f);
        kPos.y = LengthLerp(-(GetLength() / 2.0f), 
            (GetLength() / 2.0f), uiCol);

        for(NiUInt32 uiRow = 0; uiRow < GetNumWidthVertices(); ++uiRow)
        {
            NiUInt32 uiVertIndex = GetVertexIndex(uiRow, uiCol);
            kPos.x = WidthLerp(-(GetWidth() / 2.0f), 
                (GetWidth() / 2.0f), uiRow);

            kPositionIter[uiVertIndex] = kPos;
            kNormalIter[uiVertIndex] = NiPoint3(0, 0, 1);
            kTangentIter[uiVertIndex] = NiPoint3(1, 0, 0);
        }
    }

    kPosLock.Unlock();
    kNormalLock.Unlock();
    kTangentLock.Unlock();

    // Setup indices to form polys with the following 
    // vertex layout to form a grid for the water plane:
    // A ---------- B
    // |           /|
    // |         /  |
    // |       /    |
    // |     /      |
    // |   /        |
    // | /          |
    // C ---------- D

    NiDataStreamElementLock kIndexLock(m_spWaterMesh, 
        NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1, NiDataStream::LOCK_READ |
        NiDataStream::LOCK_WRITE);

    NiTStridedRandomAccessIterator<NiUInt16> kIndexIter = 
        kIndexLock.begin<NiUInt16>();

    NiUInt32 uiIndex = 0;
    NIASSERT(GetNumWidthVertices()> 1);
    for(NiUInt32 uiCol = 0; uiCol < GetNumLengthVertices()-1; ++uiCol)
    {
        NIASSERT(GetNumLengthVertices()> 1);
        for(NiUInt32 uiRow = 0; uiRow < GetNumWidthVertices()-1; ++uiRow)
        {
            NiUInt32 uiVertA = GetVertexIndex(uiRow, uiCol);
            NiUInt32 uiVertB = GetVertexIndex(uiRow+1, uiCol);
            NiUInt32 uiVertC = GetVertexIndex(uiRow, uiCol+1);
            NiUInt32 uiVertD = GetVertexIndex(uiRow+1, uiCol+1);

            kIndexIter[uiIndex] = (NiUInt16)uiVertA;
            kIndexIter[uiIndex+1] = (NiUInt16)uiVertB;
            kIndexIter[uiIndex+2] = (NiUInt16)uiVertC;

            kIndexIter[uiIndex+3] = (NiUInt16)uiVertB;
            kIndexIter[uiIndex+4] = (NiUInt16)uiVertD;
            kIndexIter[uiIndex+5] = (NiUInt16)uiVertC;
            uiIndex += 6;
        }
    }

    kIndexLock.Unlock();
    
    // Create and add extra data to mesh to control water appearance
    NiFloatExtraData* pkExtra = 
        NiNew NiFloatExtraData(GetReflectionFactor());
    pkExtra->SetName("Reflection");
    
    NiFloatExtraData* pkExtraSpecularity = 
        NiNew NiFloatExtraData(GetSpecularity());
    pkExtraSpecularity->SetName("Specularity");
    
    NiFloatExtraData* pkExtraDisturbance = 
        NiNew NiFloatExtraData(GetDisturbance());
    pkExtraDisturbance->SetName("Disturbance");    
   
    NiFloatExtraData* pkExtraFogDensity = 
        NiNew NiFloatExtraData(GetFogDensity());
    pkExtraFogDensity->SetName("FogDensity");

    NiFloatExtraData* pkExtraFogDistance = 
        NiNew NiFloatExtraData(GetFogDistance());
    pkExtraFogDistance->SetName("FogDistance");

    NiFloatExtraData* pkExtraFogFallOff = 
        NiNew NiFloatExtraData(GetFogFallOff());
    pkExtraFogFallOff->SetName("FogFallOff");

    float fNormalMaps[2] = {0.0f, 0.0f};
    NiFloatsExtraData* pkExtraNormalMap = 
        NiNew NiFloatsExtraData(2, fNormalMaps);
    pkExtraNormalMap->SetName("NormalMapModifier");
        
    NiIntegerExtraData* pkExtraMode = 
        NiNew NiIntegerExtraData(GetReflectionMode());
    pkExtraMode->SetName("Mode");

    m_spWaterMesh->AddExtraData("Reflection", pkExtra);
    m_spWaterMesh->AddExtraData("Specularity", pkExtraSpecularity);
    m_spWaterMesh->AddExtraData("Disturbance", pkExtraDisturbance);
    m_spWaterMesh->AddExtraData("FogDensity", pkExtraFogDensity);
    m_spWaterMesh->AddExtraData("FogDistance", pkExtraFogDistance);
    m_spWaterMesh->AddExtraData("FogFallOff", pkExtraFogFallOff);
    m_spWaterMesh->AddExtraData("Mode", pkExtraMode);
    m_spWaterMesh->AddExtraData("NormalMapModifier", pkExtraNormalMap);

    // Update UV stream
    ComputeUVStream();
    
    m_spWaterMesh->UpdateEffects();
    m_spWaterMesh->Update(0.0f);
}
//---------------------------------------------------------------------------
void NiWater::ComputeTexture()
{
    NiTerrainInteractor* pkInteractor = 0;
    if (m_spAssociatedTerrain)
    {        
        pkInteractor = m_spAssociatedTerrain->GetInteractor();
    }

    if (!m_spTextureMask)
    {
        NiTexture::FormatPrefs kPrefs;
        kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
        kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
        kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;

#if defined(_WII)
        // Dynamic textures on the Wii must be tiled
        const bool bTiled = true;
#else
        const bool bTiled = false;
#endif

        // Try to get a format that matches the source for top speed
        m_spTextureMask = NiDynamicTexture::Create(
            GetTextureSizeRatio(), GetTextureSizeRatio(), kPrefs, bTiled);
        NIASSERT(m_spTextureMask);
    }

    NiInt32 iPitch = 0;
    NiUInt32* puiPtr = (NiUInt32*)(m_spTextureMask->Lock(iPitch));
    NIASSERT(puiPtr != NULL);

    NiColorA kColor = GetDeepColor();
    NiUInt32 uiPixelValue;

    // Figure out the sequential layout of red, green, blue, and padding
    // channels directly from the pixel format in a device-agnostic way.
    const NiPixelFormat* pkFmt = m_spTextureMask->GetPixelFormat();

#if defined(_WII)
    // The Wii packing logic assumes RGBA32 formatted dynamic texture.
    NIASSERT(pkFmt->FunctionallyIdentical(NiPixelFormat::RGBA32, true));
#endif
    if (m_spAssociatedTerrain)
    {
        NiPoint3 kTerrainTranslate = m_spAssociatedTerrain->GetTranslate();
        
        NiTransform kMeshTransform = m_spWaterMesh->GetWorldTransform();
               
        NiPoint3 kTerrainVertex(0,0,0), kTerrainNormal(0,0,0);
        NiColorA kColorVaration = GetDeepColor() - GetShallowColor();
        NiPoint3 kMeshPoint;
        NiRay kRay;

        for (NiUInt32 y = 0; y < GetTextureSizeRatio(); y++)
        {
            // For each pixel in the texture
            for (NiUInt32 x = 0; x < GetTextureSizeRatio(); x++)
            {
                kColor = GetDeepColor();
                if (y != 0 && x != 0 && 
                    y != (NiUInt32)(GetTextureSizeRatio() - 1) && 
                    x != (NiUInt32)(GetTextureSizeRatio() - 1))
                {
                    // We compute the corresponding mesh point this depends on
                    // the size of the texture (64, 128, 256....), the world 
                    // dimension of the texture (dependent on the size of the 
                    // mesh) and the position of the texture on the mesh 
                    // (given by the offset).
                    kMeshPoint = NiPoint3(x * (m_uiTextureWidth / 
                        (float)GetTextureSizeRatio()) - 
                        (float)(m_uiTextureWidth) / 2.0f - 
                        (m_fTextureOffsetX * m_uiTextureWidth),
                         y * (m_uiTextureLength / 
                        (float)GetTextureSizeRatio()) - 
                        (float)(m_uiTextureLength) / 2.0f - 
                        (m_fTextureOffsetY * m_uiTextureLength), 0.0f);
                                   
                    // We transform this point through the mesh transform to 
                    // obtain world coordinates
                    kTerrainVertex = kMeshTransform * kMeshPoint;
                    
                    kTerrainVertex.z = NI_INFINITY;
                    kRay = NiRay(kTerrainVertex, NiPoint3(0,0,-1));

                    // We now check for collision with the terrain to find the
                    // depth of the water
                    if (pkInteractor && pkInteractor->Collide(kRay))
                    {               
                        kRay.GetIntersection(kTerrainVertex, kTerrainNormal);
                        float fDepth = (kMeshTransform * kMeshPoint).z - 
                            kTerrainVertex.z;                

                        float fDiffRatio = 
                            ((NiAbs(fDepth)) - GetDepthThreshold()) 
                            / GetDepthThreshold();

                        if (fDepth < 0)
                            fDiffRatio = -1;

                        // We now compute the color of the water (
                        // interpolation)
                        if ( fDiffRatio < 0)
                        {
                            kColor = GetShallowColor() + 
                                ((1 - (NiAbs(fDiffRatio))) * kColorVaration);
                        }
                        else
                        {
                            kColor = GetDeepColor();
                        }
                    }
                    
                }
                                              

                // Apply the colour found                
                uiPixelValue = ((NiUInt32)(kColor.r * 255) 
                    << pkFmt->GetShift(NiPixelFormat::COMP_RED)) 
                    & pkFmt->GetMask(NiPixelFormat::COMP_RED);
                uiPixelValue |= ((NiUInt32)(kColor.g * 255)  
                    << pkFmt->GetShift(NiPixelFormat::COMP_GREEN)) 
                    & pkFmt->GetMask(NiPixelFormat::COMP_GREEN);
                uiPixelValue |= ((NiUInt32)(kColor.b * 255)  
                    << pkFmt->GetShift(NiPixelFormat::COMP_BLUE)) 
                    & pkFmt->GetMask(NiPixelFormat::COMP_BLUE);
                uiPixelValue |= ((NiUInt32)(kColor.a * 255)  
                    << pkFmt->GetShift(NiPixelFormat::COMP_ALPHA)) 
                    & pkFmt->GetMask(NiPixelFormat::COMP_ALPHA);

#if !defined(_WII)
                *puiPtr = uiPixelValue;
                puiPtr++;
#else
                // Pack the pixel into the correct tiles for the Wii
                unsigned int uiTile = (y / 4) * (GetTextureSizeRatio() / 2) + (x / 4 * 2);
                unsigned short* pTile = (unsigned short*)((unsigned char*)puiPtr +
                    (uiTile * 32) + 8 * (y % 4) + 2 * (x % 4));

                // Convert RGBA to ARGB since tiling divides colors into AR and GB
                // Asssumes RGBA32 format (see NIASSERT above)
                unsigned long uiARGB = (uiPixelValue>>8) | ((uiPixelValue & 0x00FF)<<24);

                *(pTile) = uiARGB >> 16;
                *(pTile + 16) = uiARGB;
#endif
            }
        }
    }
    else
    {
        for (NiUInt32 y = 0; y < GetTextureSizeRatio(); y++)
        {
            // For each pixel in the texture
            for (NiUInt32 x = 0; x < GetTextureSizeRatio(); x++)
            {
                uiPixelValue = ((NiUInt32)(kColor.r * 255) 
                    << pkFmt->GetShift(NiPixelFormat::COMP_RED)) 
                    & pkFmt->GetMask(NiPixelFormat::COMP_RED);
                uiPixelValue |= ((NiUInt32)(kColor.g * 255)  
                    << pkFmt->GetShift(NiPixelFormat::COMP_GREEN)) 
                    & pkFmt->GetMask(NiPixelFormat::COMP_GREEN);
                uiPixelValue |= ((NiUInt32)(kColor.b * 255)  
                    << pkFmt->GetShift(NiPixelFormat::COMP_BLUE)) 
                    & pkFmt->GetMask(NiPixelFormat::COMP_BLUE);
                uiPixelValue |= ((NiUInt32)(kColor.a * 255)  
                    << pkFmt->GetShift(NiPixelFormat::COMP_ALPHA)) 
                    & pkFmt->GetMask(NiPixelFormat::COMP_ALPHA);

#if !defined(_WII)
                *puiPtr = uiPixelValue;
                puiPtr++;
#else
                // Pack the pixel into the correct tiles for the Wii
                unsigned int uiTile = (y / 4) * (GetTextureSizeRatio() / 2) + (x / 4 * 2);
                unsigned short* pTile = (unsigned short*)((unsigned char*)puiPtr +
                    (uiTile * 32) + 8 * (y % 4) + 2 * (x % 4));

                // Convert RGBA to ARGB since tiling divides colors into AR and GB
                // Asssumes RGBA32 format (see NIASSERT above)
                unsigned long uiARGB = (uiPixelValue>>8) | ((uiPixelValue & 0x00FF)<<24);

                *(pTile) = uiARGB >> 16;
                *(pTile + 16) = uiARGB;
#endif
            }
        }
    }

    m_spTextureMask->UnLock();

    // We now update the texture mask
    if (m_spWaterMesh)
    {
        NiTexturingProperty kTemp;
        NiTexturingPropertyPtr spTexProp;
    
        spTexProp = 
            (NiTexturingProperty*)m_spWaterMesh->GetProperty(kTemp.GetType());

        if (spTexProp)
            spTexProp->GetBaseMap()->SetTexture(m_spTextureMask);
    }

    m_bRedrawTexture = false;
}
//---------------------------------------------------------------------------
void NiWater::LoadNormalMaps()
{
    m_bReloadNormalMaps = false;
    m_kNormalMapList.RemoveAll();
    m_kCurrentNormalMap = 0;

    if(!GetNormalMapDir())
        return;

    char acAbsPath[NI_MAX_PATH];
    if (NiPath::IsRelative(GetNormalMapDir()))
    {
        char acDir[NI_MAX_PATH];
        NiPath::GetCurrentWorkingDirectory(acDir,NI_MAX_PATH);
        NiPath::ConvertToAbsolute(acAbsPath,
            NI_MAX_PATH, GetNormalMapDir(), acDir);
    }
    else
    {
        NiStrcpy(acAbsPath, NI_MAX_PATH, GetNormalMapDir());
    }

    if (NiFile::DirectoryExists(acAbsPath) == false)
    {
        return;
    }

    NiBool bFinished = false;
    NiInt32 i = 1;
    while (!bFinished)
    {
        char acFile[NI_MAX_PATH];
        char acPath[NI_MAX_PATH];

        NiSprintf(acFile, NI_MAX_PATH, GetNormalFile(), i);
        NiSprintf(acPath, NI_MAX_PATH, "%s\\%s", 
            acAbsPath, acFile);
        NiPath::Standardize(acPath);

        if (!NiFile::Access(acPath, NiFile::READ_ONLY))
        {
            bFinished = true;
            break;
        }

        NiSourceTexturePtr spToAdd = NiSourceTexture::Create(acPath);

        if (spToAdd)
        {
            m_kNormalMapList.AddTail(spToAdd);
            i++;
        }        
    }
}
//---------------------------------------------------------------------------
void NiWater::UpdateExtraData()
{
    m_bUpdateExtraData = false;

    if (!m_spWaterMesh)
    {
        return;
    }

    ((NiFloatExtraData*)(m_spWaterMesh->GetExtraData("Reflection")))->
                SetValue(GetReflectionFactor());

    NiFloatExtraData* pkExtraMode = (NiFloatExtraData*)
        m_spWaterMesh->GetExtraData("Specularity");
    pkExtraMode->SetValue(GetSpecularity());

    pkExtraMode = 
        (NiFloatExtraData*)m_spWaterMesh->GetExtraData("Disturbance");
    pkExtraMode->SetValue(GetDisturbance());

    pkExtraMode = (NiFloatExtraData*)
                m_spWaterMesh->GetExtraData("FogDensity");
    pkExtraMode->SetValue(GetFogDensity());

    pkExtraMode = (NiFloatExtraData*)
                m_spWaterMesh->GetExtraData("FogDistance");
    pkExtraMode->SetValue(GetFogDistance());

    pkExtraMode = (NiFloatExtraData*)
                m_spWaterMesh->GetExtraData("FogFallOff");
    pkExtraMode->SetValue(GetFogFallOff());

    NiIntegerExtraData* pkExtraIntMode = (NiIntegerExtraData*)
        m_spWaterMesh->GetExtraData("Mode");
    pkExtraIntMode->SetValue(GetReflectionMode());
}
//---------------------------------------------------------------------------
void NiWater::LoadEnvMap()
{
    m_bReloadEnvMap = false;

    m_spEnvironmentMap = ms_spBlankNormals;

    if (!GetEnvMap())
    {
        return;
    }

    if (NiFile::Access(GetEnvMap(), NiFile::READ_ONLY))
    {
        NiDevImageConverter kImageConverter;

        NiUInt32 uiWidth, uiHeight, uiFaces;
        NiPixelFormat kFormat;
        bool bMipmap;

        if (!kImageConverter.ReadImageFileInfo(
            m_kEnvMap, kFormat, bMipmap, uiWidth, uiHeight, uiFaces))
        {
            // Chosen file is not readable
            NiMessageBox("The selected file is not a supported image "
            "format.\n A blank environment map will be used instead.", 
            "NiWaterComponent::SetEnvMap");
            return;
        }

        if (uiFaces != 6)
        {
            // This is not a valid cube map
            NiMessageBox("The selected image is not a valid cube map image"
            ".\n A valid cube map should have 6 faces.\n A blank"
            " environment map will be used instead.",
            "NiWaterComponent::SetEnvMap");
            return;
        }

        m_spEnvironmentMap = NiSourceCubeMap::Create(GetEnvMap(), 
            NiRenderer::GetRenderer());

        SetReflectionMode(REFLECTION_MODE_CUBE_MAP);
    }
}
//---------------------------------------------------------------------------
void NiWater::LoadRefractionMap()
{
    m_bReloadRefractionMap = false;

    m_spRefractionMap = 0;
    if (!GetRefractionMap())
    {
        return;
    }

    if (NiFile::Access(GetRefractionMap(), NiFile::READ_ONLY))
    {
        m_spRefractionMap = NiSourceCubeMap::Create(
            m_kRefractionMap, 
            NiRenderer::GetRenderer());
    }
    else
    {
        m_spRefractionMap = ms_spBlankNormals;
    }  
}
//---------------------------------------------------------------------------
void NiWater::ComputeUVStream()
{    
    if (!m_spWaterMesh)
        return;

    // We now need to destroy the uv stream and create a new one   
    NiDataStreamRef* pkRef = m_spWaterMesh->FindStreamRef(
        NiCommonSemantics::TEXCOORD(), 0,
        NiDataStreamElement::F_FLOAT32_2);

    NiDataStreamElementSet ntElementSet;
    ntElementSet.AddElement(NiDataStreamElement::F_FLOAT32_2); 
        
    NiDataStream* pkStream = NiDataStream::CreateDataStream(
        ntElementSet, 
        (GetNumLengthVertices() * GetNumWidthVertices()),
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        false);
    pkStream->AddRegion(NiDataStream::Region(0, 2));
    pkRef->SetDataStream(pkStream);    
       
    // Texture stream 
    NiDataStreamElementLock kUVLock(
        m_spWaterMesh, 
        NiCommonSemantics::TEXCOORD(), 0, 
        NiDataStreamElement::F_FLOAT32_2,
	    NiDataStream::LOCK_WRITE);
    
    // Assign the new values
    NiTStridedRandomAccessIterator<NiPoint2> kUVIter = 
        kUVLock.begin<NiPoint2>(); 

    m_uiTextureLength = (NiUInt32)GetLength();
    m_uiTextureWidth = (NiUInt32)GetWidth();
    m_fTextureOffsetX = 0.0f;
    m_fTextureOffsetY = 0.0f;

    float fMinXTexCoord;
    float fMaxXTexCoord;
    float fMinYTexCoord;
    float fMaxYTexCoord;
    CalculateUVTexCoords(fMinXTexCoord, fMaxXTexCoord, fMinYTexCoord, 
        fMaxYTexCoord);

    NiPoint2 kNormalMapModifier;
    kNormalMapModifier.x = m_fNormalWidth / (fMaxXTexCoord - fMinXTexCoord);
    kNormalMapModifier.y = m_fNormalLength / (fMaxYTexCoord - fMinYTexCoord);
    
    NiFloatsExtraData* pkExtraMode = (NiFloatsExtraData*)
        m_spWaterMesh->GetExtraData("NormalMapModifier");
    pkExtraMode->SetValue(0, kNormalMapModifier.x);
    pkExtraMode->SetValue(1, kNormalMapModifier.y);

    NIASSERT(GetNumWidthVertices()> 1);
    for(NiUInt32 uiCol = 0; uiCol < GetNumLengthVertices()-1; ++uiCol)
    {
        NIASSERT(GetNumLengthVertices()> 1);
        for(NiUInt32 uiRow = 0; uiRow < GetNumWidthVertices()-1; ++uiRow)
        {
            NiUInt32 uiVertA = GetVertexIndex(uiRow, uiCol);
            NiUInt32 uiVertB = GetVertexIndex(uiRow+1, uiCol);
            NiUInt32 uiVertC = GetVertexIndex(uiRow, uiCol+1);
            NiUInt32 uiVertD = GetVertexIndex(uiRow+1, uiCol+1);

            kUVIter[uiVertA] = NiPoint2(
                WidthLerp(fMinXTexCoord, fMaxXTexCoord, uiRow),
                LengthLerp(fMinYTexCoord, fMaxYTexCoord, uiCol));
            kUVIter[uiVertB] = NiPoint2(
                WidthLerp(fMinXTexCoord, fMaxXTexCoord, uiRow+1),
                LengthLerp(fMinYTexCoord, fMaxYTexCoord, uiCol));
            kUVIter[uiVertC] = NiPoint2(
                WidthLerp(fMinXTexCoord, fMaxXTexCoord, uiRow),
                LengthLerp(fMinYTexCoord, fMaxYTexCoord, uiCol+1));
            kUVIter[uiVertD] = NiPoint2(
                WidthLerp(fMinXTexCoord, fMaxXTexCoord, uiRow+1),
                LengthLerp(fMinYTexCoord, fMaxYTexCoord, uiCol+1));
        }
    }
    kUVLock.Unlock();
}
//---------------------------------------------------------------------------
void NiWater::CalculateUVTexCoords(float& fMinXTexCoord, float& fMaxXTexCoord, 
    float& fMinYTexCoord, float& fMaxYTexCoord)
{
    fMinXTexCoord = 0.0f;
    fMaxXTexCoord = 1.0f;
    fMinYTexCoord = 0.0f;
    fMaxYTexCoord = 1.0f;

    // If there isn't a terrain associated with the water, we're done.
    // Otherwise, modify the texcoords based on the terrain.
    if (!m_spAssociatedTerrain)
    {
        return;
    }

    NiTerrainInteractor* pkInteractor = 0;
    pkInteractor = m_spAssociatedTerrain->GetInteractor();

    NIASSERT(pkInteractor);
    if (!pkInteractor)
    {
        return;
    }

    NiTerrainPtr spTerrain = pkInteractor->GetTerrain();
    NiUInt32 uiSectorSize = spTerrain->GetCalcSectorSize();
    float fTerrainScale = spTerrain->GetScale();
    
    float fTerrainSize = (float) 
        spTerrain->GetNumLoadedSectors() * (uiSectorSize - 1)
        * fTerrainScale;
        
    // We only need to recompute the UV stream if one of the edge of the 
    // water is bigger than the terrain
    if (GetLength() <= fTerrainSize && GetWidth() <= fTerrainSize)
    {
        return;
    }

    NiTransform kWaterTransform = m_spWaterMesh->GetWorldTransform();
    kWaterTransform.m_Translate = NiPoint3(0,0,0);

    NiPoint3 kTerrainTranslate = m_spAssociatedTerrain->GetTranslate();
    NiMatrix3 kTerrainMatrix = m_spAssociatedTerrain->GetRotate();
   
    float fAngleX, fAngleY, fAngleZ, fTerrainAngleX, fTerrainAngleY;
    float fMaxAngle, fTerrainAngleZ;

    kWaterTransform.m_Rotate.ToEulerAnglesXYZ(fAngleX, fAngleY, fAngleZ);
    kTerrainMatrix.ToEulerAnglesXYZ(fTerrainAngleX, fTerrainAngleY, 
        fTerrainAngleZ);

    fMaxAngle = NiMax(NiAbs(fAngleX - fTerrainAngleX), 
        NiAbs(fAngleY - fTerrainAngleY));
    fMaxAngle = NiMax(fMaxAngle, NiAbs(fAngleZ - fTerrainAngleZ));
    
    if (GetLength() > fTerrainSize)
    {
        m_uiTextureLength = 
            (NiUInt32)(((fTerrainSize + 1) * NiMin(((1.0f + NiMin(
            NiAbs(NiCos(fMaxAngle)), NiAbs(NiSin(fMaxAngle))))), 
            NiSqrt(2.0f))) / kWaterTransform.m_fScale);
        fMinYTexCoord = -0.5f * ((float)GetLength() / 
            ((m_uiTextureLength))) + 0.5f;
        fMaxYTexCoord = 0.5f *(float)GetLength() / 
            ((m_uiTextureLength)) + 0.5f;        
               
        m_fTextureOffsetY = (((kWaterTransform.m_Rotate.Inverse() * 
            ((m_kTranslation - kTerrainTranslate) / 
            kWaterTransform.m_fScale)).y) / (float)m_uiTextureLength) ; 
       
        fMinYTexCoord += m_fTextureOffsetY;
        fMaxYTexCoord += m_fTextureOffsetY;                
    }

    if (GetWidth() > fTerrainSize)
    {
        m_uiTextureWidth = 
            (NiUInt32)(((fTerrainSize + 1) * NiMin(((1.0f + NiMin(
            NiAbs(NiCos(fMaxAngle)), NiAbs(NiSin(fMaxAngle))))), 
            NiSqrt(2.0f))) / kWaterTransform.m_fScale);
        fMinXTexCoord = - 0.5f * ((float)GetWidth() / 
            ((m_uiTextureWidth))) + 0.5f;
        fMaxXTexCoord = 0.5f * (float)GetWidth() / 
            ((m_uiTextureWidth)) + 0.5f;        
        
        m_fTextureOffsetX = (((kWaterTransform.m_Rotate.Inverse() * 
            ((m_kTranslation - kTerrainTranslate) / 
            kWaterTransform.m_fScale)).x) / (float)m_uiTextureWidth) ;
       
        fMinXTexCoord += m_fTextureOffsetX;
        fMaxXTexCoord += m_fTextureOffsetX;      
    }
}
//---------------------------------------------------------------------------
void NiWater::SubscribeToDXDeviceResetNotification()
{
    m_uiDXDeviceResetCallbackIndex = 0;
    m_bRegisteredDXDeviceResetCallback = false;

#ifdef WIN32
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    if (pkRenderer)
    {
        m_uiDXDeviceResetCallbackIndex = pkRenderer->AddResetNotificationFunc(
            (NiDX9Renderer::RESETNOTIFYFUNC)&HandleDXDeviceReset, this);
        m_bRegisteredDXDeviceResetCallback = true;
    }
#endif
}
//---------------------------------------------------------------------------
void NiWater::UnsubscribeToDXDeviceResetNotification()
{
#ifdef WIN32
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    if (pkRenderer && m_bRegisteredDXDeviceResetCallback)
    {
        pkRenderer->RemoveResetNotificationFunc(
            m_uiDXDeviceResetCallbackIndex);
    }
#endif
}
//---------------------------------------------------------------------------
bool NiWater::HandleDXDeviceReset(bool bBeforeReset, void* pkVoid)
{
    if (!bBeforeReset) // Wait until after device reset
    {
        NiWater* pkWater = (NiWater*)pkVoid;
        NIASSERT(pkWater);

        pkWater->m_bRedrawTexture = true;        
        pkWater->ComputeTexture();
    }

    return true;
}
//---------------------------------------------------------------------------
