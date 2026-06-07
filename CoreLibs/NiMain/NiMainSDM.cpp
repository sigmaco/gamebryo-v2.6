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
#include "NiMainPCH.h"

#include "NiMain.h"
#include "NiMainSDM.h"

#include "NiGlobalStringTable.h"
#include "NiCommonSemantics.h"
#include "NiRenderObject.h"

#include "NiAdditionalGeometryData.h"
#include "NiOldScreenSpace.h"
#include "NiTriShapeDynamicData.h"
#include "NiTriStrips.h"
#include "NiTriStripsData.h"

NiImplementSDMConstructor(NiMain, ""); // All libs depend on NiSystem

//---------------------------------------------------------------------------
#ifdef NIMAIN_EXPORT
NiImplementDllMain(NiMain);
#endif // NIMAIN_EXPORT
//---------------------------------------------------------------------------
void NiMainSDM::Init()
{
    NiImplementSDMInitCheck();
    NiGlobalStringTable::_SDMInit();
    NiCommonSemantics::_SDMInit();
    NiRenderObjectMaterialOption::_SDMInit();

    // init default properties
    NiShaderConstantMap::_SDMInit();
    NiTextureEffect::_SDMInit();
    NiAlphaProperty::_SDMInit();
    NiDitherProperty::_SDMInit();
    NiFogProperty::_SDMInit();
    NiMaterialProperty::_SDMInit();
    NiMetricsLayer::_SDMInit();
    NiRendererSpecificProperty::_SDMInit();
    NiShadeProperty::_SDMInit();
    NiShaderConstantMapEntry::_SDMInit();
    NiShaderDeclaration::_SDMInit();
    NiSpecularProperty::_SDMInit();
    NiStencilProperty::_SDMInit();
    NiTexturingProperty::_SDMInit();
    NiVertexColorProperty::_SDMInit();
    NiWireframeProperty::_SDMInit();
    NiZBufferProperty::_SDMInit();

    NiImageConverter::_SDMInit();

    NiStream::_SDMInit();

    NiRegisterStream(NiAlphaAccumulator);
    NiRegisterStream(NiAlphaProperty);
    NiRegisterStream(NiAmbientLight);

    // NiAutoNormalParticles has been removed from the engine
    // Because it never added any data to its base class, we can
    // simply instantiate the base class in streaming.
//    NiRegisterStream(NiAutoNormalParticles);
    NiStream::RegisterLoader("NiAutoNormalParticles", 
        NiParticles::CreateObject);

    // NiAutoNormalParticlesData has been removed from the engine
    // Because it never added any data to its base class, we can
    // simply instantiate the base class in streaming.
//    NiRegisterStream(NiAutoNormalParticlesData);
    NiStream::RegisterLoader("NiAutoNormalParticlesData", 
        NiParticlesData::CreateObject);

    NiRegisterStream(NiBillboardNode);
    NiRegisterStream(NiBinaryExtraData);
    NiRegisterStream(NiBooleanExtraData);
    NiRegisterStream(NiBSPNode);
    NiRegisterStream(NiCamera);

    // NiClusterAccumulator has been removed from the engine
//    NiRegisterStream(NiClusterAccumulator);
    NiStream::RegisterLoader("NiClusterAccumulator", 
        NiAlphaAccumulator::CreateObject);
    
    // NiCollisionSwitch has been removed from the engine
    // Its behavior can be achieved with collision propagation flags
    // in NiAVObject.
//    NiRegisterStream(NiCollisionSwitch);
    NiStream::RegisterLoader("NiCollisionSwitch", NiNode::CreateObject);

    NiRegisterStream(NiColorExtraData);
    NiRegisterStream(NiDefaultAVObjectPalette);
    NiRegisterStream(NiDirectionalLight);
    NiRegisterStream(NiDitherProperty);
    NiRegisterStream(NiExtraData);
    NiRegisterStream(NiFloatExtraData);
    NiRegisterStream(NiFloatsExtraData);
    NiRegisterStream(NiFogProperty);
    NiRegisterStream(NiIntegerExtraData);
    NiRegisterStream(NiIntegersExtraData);
    NiRegisterStream(NiLODNode);
    NiRegisterStream(NiMaterialProperty);
    NiRegisterStream(NiNode);
    NiRegisterStream(NiPalette);

    NiRegisterStream(NiPersistentSrcTextureRendererData);
    NiRegisterStream(NiPixelData);
    NiRegisterStream(NiPointLight);
    NiRegisterStream(NiRangeLODData);
    NiRegisterStream(NiRendererSpecificProperty);
    NiRegisterStream(NiScreenLODData);
    NiRegisterStream(NiShadeProperty);

    NiRegisterStream(NiSortAdjustNode);
    NiRegisterStream(NiSourceTexture);
    NiRegisterStream(NiSourceCubeMap);
    NiRegisterStream(NiSpecularProperty);
    NiRegisterStream(NiSpotLight);
    NiRegisterStream(NiStencilProperty);
    NiRegisterStream(NiStringExtraData);
    NiRegisterStream(NiStringsExtraData);
    NiRegisterStream(NiSwitchNode);
    NiRegisterStream(NiSwitchStringExtraData);
    NiRegisterStream(NiTextureEffect);
    NiRegisterStream(NiTexturingProperty);

    NiRegisterStream(NiVectorExtraData);
    NiRegisterStream(NiVertexColorProperty);
    NiRegisterStream(NiVertWeightsExtraData);
    NiRegisterStream(NiWireframeProperty);
    NiRegisterStream(NiZBufferProperty);

    // Deprecated classes that can still be loaded
    NiRegisterStream(NiAdditionalGeometryData);

    NiRegisterStream(NiLines);
    NiRegisterStream(NiLinesData);

    NiRegisterStream(NiParticles);
    NiRegisterStream(NiParticlesData);

    NiRegisterStream(NiScreenElements);
    NiRegisterStream(NiScreenElementsData);
    NiRegisterStream(NiScreenGeometry);
    NiRegisterStream(NiScreenGeometryData);
    NiRegisterStream(NiScreenPolygon);
    NiRegisterStream(NiScreenSpaceCamera);
    NiRegisterStream(NiScreenTexture);
    NiRegisterStream(NiSkinData);
    NiRegisterStream(NiSkinInstance);
    NiRegisterStream(NiSkinPartition);

    NiRegisterStream(NiTriShape);
    NiRegisterStream(NiTriShapeData);
    NiRegisterStream(NiTriShapeDynamicData);
    NiRegisterStream(NiTriStrips);
    NiRegisterStream(NiTriStripsData);

    //
    NiScreenGeometryData::_SDMInit();

    NiTexture::RendererData::_SDMInit();

    NiPoint3::_SDMInit();

    NiMaterialLibrary::_SDMInit();
    NiSingleShaderMaterialLibrary::_SDMInit();
    NiMaterial::_SDMInit();

    NiRenderClick::_SDMInit();
    NiRenderFrame::_SDMInit();
    NiRenderStep::_SDMInit();
    NiRenderView::_SDMInit();
    NiViewRenderClick::_SDMInit();

    NiShadowMap::_SDMInit();
    NiShadowCubeMap::_SDMInit();
}
//---------------------------------------------------------------------------
void NiMainSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiAlphaAccumulator);
    NiUnregisterStream(NiAlphaProperty);
    NiUnregisterStream(NiAmbientLight);
    NiUnregisterStream(NiAutoNormalParticles);
    NiUnregisterStream(NiAutoNormalParticlesData);
    NiUnregisterStream(NiBillboardNode);
    NiUnregisterStream(NiBinaryExtraData);
    NiUnregisterStream(NiBooleanExtraData);
    NiUnregisterStream(NiBSPNode);
    NiUnregisterStream(NiCamera);
    NiUnregisterStream(NiClusterAccumulator);
    NiUnregisterStream(NiCollisionSwitch);
    NiUnregisterStream(NiColorExtraData);
    NiUnregisterStream(NiDefaultAVObjectPalette);
    NiUnregisterStream(NiDirectionalLight);
    NiUnregisterStream(NiDitherProperty);
    NiUnregisterStream(NiExtraData);
    NiUnregisterStream(NiFloatExtraData);
    NiUnregisterStream(NiFloatsExtraData);
    NiUnregisterStream(NiFogProperty);
    NiUnregisterStream(NiIntegerExtraData);
    NiUnregisterStream(NiIntegersExtraData);
    NiUnregisterStream(NiLODNode);
    NiUnregisterStream(NiMaterialProperty);
    NiUnregisterStream(NiNode);
    NiUnregisterStream(NiPalette);
    NiUnregisterStream(NiPersistentSrcTextureRendererData);
    NiUnregisterStream(NiPixelData);
    NiUnregisterStream(NiPointLight);
    NiUnregisterStream(NiRangeLODData);
    NiUnregisterStream(NiRendererSpecificProperty);
    NiUnregisterStream(NiScreenLODData);
    NiUnregisterStream(NiShadeProperty);
    NiUnregisterStream(NiSortAdjustNode);
    NiUnregisterStream(NiSourceTexture);
    NiUnregisterStream(NiSourceCubeMap);
    NiUnregisterStream(NiSpecularProperty);
    NiUnregisterStream(NiSpotLight);
    NiUnregisterStream(NiStencilProperty);
    NiUnregisterStream(NiStringExtraData);
    NiUnregisterStream(NiStringsExtraData);
    NiUnregisterStream(NiSwitchNode);
    NiUnregisterStream(NiSwitchStringExtraData);
    NiUnregisterStream(NiTextureEffect);
    NiUnregisterStream(NiTexturingProperty);
    NiUnregisterStream(NiVectorExtraData);
    NiUnregisterStream(NiVertexColorProperty);
    NiUnregisterStream(NiVertWeightsExtraData);
    NiUnregisterStream(NiWireframeProperty);
    NiUnregisterStream(NiZBufferProperty);

    // Deprecated classes that can still be loaded
    NiUnregisterStream(NiAdditionalGeometryData);

    NiUnregisterStream(NiLines);
    NiUnregisterStream(NiLinesData);

    NiUnregisterStream(NiParticles);
    NiUnregisterStream(NiParticlesData);

    NiUnregisterStream(NiScreenElements);
    NiUnregisterStream(NiScreenElementsData);
    NiUnregisterStream(NiScreenGeometry);
    NiUnregisterStream(NiScreenGeometryData);
    NiUnregisterStream(NiScreenPolygon);
    NiUnregisterStream(NiScreenSpaceCamera);
    NiUnregisterStream(NiScreenTexture);
    NiUnregisterStream(NiSkinData);
    NiUnregisterStream(NiSkinInstance);
    NiUnregisterStream(NiSkinPartition);

    NiUnregisterStream(NiTriShape);
    NiUnregisterStream(NiTriShapeData);
    NiUnregisterStream(NiTriShapeDynamicData);
    NiUnregisterStream(NiTriStrips);
    NiUnregisterStream(NiTriStripsData);

    //
    NiShaderConstantMap::_SDMShutdown();
    NiTextureEffect::_SDMShutdown();
    NiAlphaProperty::_SDMShutdown();
    NiDitherProperty::_SDMShutdown();
    NiFogProperty::_SDMShutdown();
    NiMaterialProperty::_SDMShutdown();
    NiMetricsLayer::_SDMShutdown();
    NiRendererSpecificProperty::_SDMShutdown();
    NiShadeProperty::_SDMShutdown();
    NiShaderDeclaration::_SDMShutdown();
    NiSpecularProperty::_SDMShutdown();
    NiStencilProperty::_SDMShutdown();
    NiTexturingProperty::_SDMShutdown();
    NiVertexColorProperty::_SDMShutdown();
    NiWireframeProperty::_SDMShutdown();
    NiZBufferProperty::_SDMShutdown();

    NiImageConverter::_SDMShutdown();
    NiStream::_SDMShutdown();
    NiRenderer::_SDMShutdown();

    NiScreenGeometryData::_SDMShutdown();
    NiTexture::RendererData::_SDMShutdown();

    NiPoint3::_SDMShutdown();

    NiMaterial::_SDMShutdown();   
    NiSingleShaderMaterialLibrary::_SDMShutdown();
    NiMaterialLibrary::_SDMShutdown();

    NiRenderClick::_SDMShutdown();
    NiRenderFrame::_SDMShutdown();
    NiRenderStep::_SDMShutdown();
    NiRenderView::_SDMShutdown();
    NiViewRenderClick::_SDMShutdown();

    NiShadowMap::_SDMShutdown();
    NiShadowCubeMap::_SDMShutdown();

    // Shutdown shared memory pool
    NiAllocatorShutdown(size_t);

    NiRenderObjectMaterialOption::_SDMShutdown();
    NiCommonSemantics::_SDMShutdown();
    NiGlobalStringTable::_SDMShutdown();
}
//---------------------------------------------------------------------------
