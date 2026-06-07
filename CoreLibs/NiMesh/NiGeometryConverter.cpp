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
#include "NiMeshPCH.h"
#include <NiSystem.h>

#include "NiGeometryConverter.h"
#include "NiDataStream.h"
#include "NiToolDataStream.h"
#include "NiLines.h"
#include "NiGeometryData.h"
#include "NiTriShape.h"
#include "NiTriStrips.h"
#include "NiParticles.h"
#include "NiScreenElements.h"
#include "NiCloningProcess.h"
#include "NiAdditionalGeometryData.h"
#include "NiCommonSemantics.h"
#include "NiSkinInstance.h"
#include "NiSkinningMeshModifier.h"
#include "NiMeshScreenElements.h"
#include "NiTSimpleArray.h"

#if defined(_WII)
#include "NiWiiSkinningMeshConverter.h"
#endif

// Simple typedefs to assist in software skinning conversion
typedef NiTSimpleArray<NiInt16, 4> NiBlendIndexArray;
typedef NiTSimpleArray<float, 4> NiBlendWeightArray;
typedef NiTStridedRandomAccessIterator<NiBlendWeightArray> 
    NiBlendWeightIterator;
typedef NiTStridedRandomAccessIterator<NiBlendIndexArray> 
    NiBlendIndexIterator;

//-------------------------------------------------------------------------
// Static variables
//-------------------------------------------------------------------------
bool NiGeometryConverter::ms_bAutoGeometryConversion = true;
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Float to int helper function from NiD3DUtils.h.
// This function was used in the DX9 renderer for packing in earlier versions
// of Gamebryo. It is reproduced here to keep the conversion of assets as 
// identical as possible. Note also that since the code for color conversion
// is templated but not commented out by preprocess, this conversion must 
// compile for all platforms. An implementation is provided for all platforms 
// even though only the DX9 renderer will encounter this code.
//---------------------------------------------------------------------------
static inline int FastFloatToInt(float fValue)
{
#if defined(WIN32)
    int iValue;
    _asm fld fValue
    _asm fistp iValue
    return iValue;
#else
    // Use a standard compiler cast for compilation on other platforms.
    return (int)fValue;
#endif
}

//---------------------------------------------------------------------------
// Static, templated functions for UBC Color Conversion
//---------------------------------------------------------------------------
template <int T> static inline void ColorConvert(const NiColorA* pkInData,
    NiUInt32& uiOut)
{
    // Generic case and D3D10 renderer should fall through here.
    NIASSERT(T == NiSystemDesc::RENDERER_GENERIC || 
        T == NiSystemDesc::RENDERER_D3D10);
    // RGBA
    uiOut =
        ((NiUInt32)(pkInData->a * 255.0f) << 24) |
        ((NiUInt32)(pkInData->b * 255.0f) << 16) |
        ((NiUInt32)(pkInData->g * 255.0f) << 8)  |
        ((NiUInt32)(pkInData->r * 255.0f));
}
//---------------------------------------------------------------------------
template<> static inline void ColorConvert<NiSystemDesc::RENDERER_DX9>(
    const NiColorA* pkInData, NiUInt32& uiOut)
{
    // BGRA
    uiOut =                                             
        ((FastFloatToInt(pkInData->a * 255.0f)) << 24) |
        ((FastFloatToInt(pkInData->r * 255.0f)) << 16) |
        ((FastFloatToInt(pkInData->g * 255.0f)) << 8)  |
         (FastFloatToInt(pkInData->b * 255.0f));          
}
//---------------------------------------------------------------------------
template<> static inline void ColorConvert<NiSystemDesc::RENDERER_PS3>(
    const NiColorA* pkInData, NiUInt32& uiOut)
{
    // RGBA, byte-swapped
    uiOut =
        ((NiUInt32)(pkInData->r * 255.0f) << 24) |
        ((NiUInt32)(pkInData->g * 255.0f) << 16) |
        ((NiUInt32)(pkInData->b * 255.0f) << 8)  |
        ((NiUInt32)(pkInData->a * 255.0f));
}
//---------------------------------------------------------------------------
template<> static inline void ColorConvert<NiSystemDesc::RENDERER_XENON>(
    const NiColorA* pkInData, NiUInt32& uiOut)
{
    // RGBA, byte-swapped
    uiOut =
        ((NiUInt32)(pkInData->r * 255.0f) << 24) |
        ((NiUInt32)(pkInData->g * 255.0f) << 16) |
        ((NiUInt32)(pkInData->b * 255.0f) << 8)  |
        ((NiUInt32)(pkInData->a * 255.0f));
}
//---------------------------------------------------------------------------
template<> static inline void ColorConvert<NiSystemDesc::RENDERER_WII>(
    const NiColorA* pkInData, NiUInt32& uiOut)
{
    // RGBA
    uiOut =
        ((NiUInt32)(pkInData->r * 255.0f) << 24) |
        ((NiUInt32)(pkInData->g * 255.0f) << 16) |
        ((NiUInt32)(pkInData->b * 255.0f) << 8)  |
        ((NiUInt32)(pkInData->a * 255.0f));
}
//---------------------------------------------------------------------------
template <int T>
static bool ConvertColorStreamRenderer(const NiColorA* pkColors, 
    NiUInt32 uiNumColors, NiUInt32* pkConvertedColors, 
    NiDataStreamElement::Format& eFormat)
{
    // Force NORMUINT8_4_BGRA format on DX9. 
    // Conversion functions for that renderer swizzle.

    int iRenderer = NiSystemDesc::RENDERER_DX9;
    if (T == iRenderer)
    {
        eFormat = NiDataStreamElement::F_NORMUINT8_4_BGRA;
    }
    else
    {
        eFormat = NiDataStreamElement::F_NORMUINT8_4;
    }

    for (NiUInt32 i = 0; i < uiNumColors; i++)
    {
        NiUInt32* puiOutData = pkConvertedColors + i;
        ColorConvert<T>(pkColors + i, *puiOutData);
    }

    return true;
}
//--------------------------------------------------------------------------
static bool ConvertColorStream(const NiColorA* pkColors, NiUInt32 uiNumColors,
    NiUInt32* pkConvertedColors, NiDataStreamElement::Format& eFormat)
{
    NiSystemDesc::RendererID eRendererID = NiSystemDesc::RENDERER_GENERIC;
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer)
        eRendererID = pkRenderer->GetRendererID();
    switch (eRendererID)
    {
        case NiSystemDesc::RENDERER_PS3:
            return ConvertColorStreamRenderer<NiSystemDesc::RENDERER_PS3>(
                pkColors, uiNumColors, pkConvertedColors, eFormat);
        case NiSystemDesc::RENDERER_XENON:
            return ConvertColorStreamRenderer<NiSystemDesc::RENDERER_XENON>(
                pkColors, uiNumColors, pkConvertedColors, eFormat);
        case NiSystemDesc::RENDERER_D3D10:
            return ConvertColorStreamRenderer<NiSystemDesc::RENDERER_D3D10>(
                pkColors, uiNumColors, pkConvertedColors, eFormat);
        case NiSystemDesc::RENDERER_WII:
            return ConvertColorStreamRenderer<NiSystemDesc::RENDERER_WII>(
                pkColors, uiNumColors, pkConvertedColors, eFormat);
        case NiSystemDesc::RENDERER_DX9:
            return ConvertColorStreamRenderer<NiSystemDesc::RENDERER_DX9>(
                pkColors, uiNumColors, pkConvertedColors, eFormat);
        default:
            return ConvertColorStreamRenderer<NiSystemDesc::RENDERER_GENERIC>(
                pkColors, uiNumColors, pkConvertedColors, eFormat);
    }
}
//---------------------------------------------------------------------------
static bool FindBoneController(NiSkinInstance* pkInstance, const char* pcName)
{
    if (!pkInstance || !pkInstance->GetSkinData())
        return false;

    NiUInt32 uiBones = pkInstance->GetSkinData()->GetBoneCount();
    if (uiBones == 0)
        return false;

    NiAVObject* pkRootParent = pkInstance->GetRootParent();
    NiAVObject* pkNode = pkInstance->GetBones()[0];
    NiAVObject* pkLastNode = NULL;

    do
    {
        if (!pkNode)
            return false;

        NiTimeController* pkController = pkNode->GetControllers();
        while (pkController)
        {
            if (0 == strcmp(pcName, pkController->GetRTTI()->GetName()))
                return true;

            pkController = pkController->GetNext();
        }

        pkLastNode = pkNode;
        pkNode = pkNode->GetParent();
    }
    while (pkLastNode != pkRootParent);

    return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Top level conversion functions.
//-------------------------------------------------------------------------
void NiGeometryConverter::ConvertToNiMesh(NiStream& kStream,
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 10))
        return;

    if (ms_bAutoGeometryConversion)
    {
        const bool bForce32BitIndices = false;
        const bool bIsRuntime = true;
        const bool bAttachModifiers = kStream.GetPrepareMeshModifiers();
        NiGeometryConverter
            kConverter(bForce32BitIndices, bIsRuntime, bAttachModifiers);
        for (unsigned int i = 0; i < kTopObjects.GetSize(); i++)
        {
            NiObject* pkObject = kTopObjects.GetAt(i);
            kConverter.RecursiveConvert(pkObject, kTopObjects, i, 
                kStream.GetConversionMap());
        }
    }

    NiStream::SetPostProcessFinalFunction(NiGeometryConverter::Finalize);
}
//---------------------------------------------------------------------------
void NiGeometryConverter::RemoveNiGeometryObjects(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiGeometry, pkObject))
    {
        NILOG("NiGeometryConverter WARNING! Failed to convert \"%s\". "
            "Removing from scene graph.\n", (const char*) pkObject->GetName());
        NiNode* pkParent = pkObject->GetParent();
        if (pkParent)
        {
            pkParent->DetachChild(pkObject);
        }
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ++ui)
        {
            RemoveNiGeometryObjects(pkNode->GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiGeometryConverter::Finalize(NiStream& kStream,
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 10))
        return;

    // Remove any NiGeometry objects that remain in the scene graph. Such
    // objects were not converted by previous functions.
    for (NiUInt32 ui = 0; ui < kTopObjects.GetSize(); ++ui)
    {
        NiAVObject* pkSceneRoot = NiDynamicCast(NiAVObject,
            kTopObjects.GetAt(ui));
        if (pkSceneRoot)
        {
            RemoveNiGeometryObjects(pkSceneRoot);
        }
    }
}
//--------------------------------------------------------------------------
NiUInt32 NiGeometryConverter::RecursiveConvert(NiObject* pkObject, 
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects, NiUInt32 uiNodeIndex,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
    NiUInt32 uiCurrentCount)
{
    if (!pkObject)
        return uiCurrentCount;

    if (NiIsKindOf(NiGeometry, pkObject))
    {
        NiGeometry* pkGeom = (NiGeometry*)pkObject;
        NiSkinInstance* pkSkinInstance = pkGeom->GetSkinInstance();
        if (pkSkinInstance)
        {
            // NiSkinningLODController conversion occurs in NiAnimation, 
            // so check for these controllers.
            if (FindBoneController(pkSkinInstance, "NiBoneLODController"))
                return uiCurrentCount;
        }

        bool bScreenElements = NiIsKindOf(NiScreenElements, pkObject);
        NiMeshPtr spMesh = bScreenElements ? 
            NiNew NiMeshScreenElements() : NiNew NiMesh();
        NiNode* pkParent = pkGeom->GetParent();
        ConversionResult eResult = Convert(pkGeom, spMesh);

        if (eResult != FAIL && bScreenElements)
            eResult = ConvertScreenElements();

        if (eResult != FAIL)
        {
            NIASSERT(spMesh->GetSubmeshCount() != 0);
            
            kConversionMap.SetAt(pkGeom, spMesh);

            // Replace geometry object with converted mesh. The mesh needs to
            // be replaced at the same index in its parent node to avoid
            // breaking NiSwitchNode and NiLODNode functionality.

            // Search the parent's children to find the index at which the
            // geometry object resides.
            if (pkParent)
            {
                NiUInt32 uiIndex;
                for (uiIndex = 0; uiIndex < pkParent->GetArrayCount(); 
                    ++uiIndex)
                {
                    if (pkParent->GetAt(uiIndex) == pkGeom)
                    {
                        break;
                    }
                }
                NIASSERT(uiIndex != pkParent->GetArrayCount());

                // Replace the child at the found index with the mesh.
                pkParent->SetAt(uiIndex, spMesh);
            }
            else
            {
                // Replace the top level NiGeometry object with the newly 
                // generated NiMesh object.
                NiObject* pkObj = (NiObject*)spMesh;
                kTopObjects.SetAt(uiNodeIndex, pkObj);
            }

            uiCurrentCount++;
        }
    }

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;
        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            uiCurrentCount = RecursiveConvert(pkNode->GetAt(ui), kTopObjects,
                uiNodeIndex, kConversionMap, uiCurrentCount);
        }
    }

    return uiCurrentCount;
}
//--------------------------------------------------------------------------
NiGeometryConverter::NiGeometryConverter(bool bForce32BitIndices, 
    bool bIsRuntime, bool bAttachModifiers) :
    m_pkMesh(NULL),
    m_pkGeom(NULL), 
    m_pkSkinInstance(NULL), 
    m_pkSkinData(NULL), 
    m_pkSkinPartition(NULL), 
    m_pkPartitions(NULL), 
    m_spCollision(NULL),
    m_uiKeepFlags(0), 
    m_uiConsistencyFlags(0), 
    m_uiRegionCount(1), 
    m_uiPartitionCount(0), 
    m_uiGeomVertexCount(0),
    m_uiMeshVertexCount(0),
    m_bIsRuntime(bIsRuntime), 
    m_bForce32BitIndices(bForce32BitIndices), 
    m_bIsSkinned(false), 
    m_bNiSkinInstance_HardwareSkinned(false), 
    m_bNiSkinningMeshModifier_HardwareSkinned(false),
    m_bIsMorphed(false),
    m_bAttachModifiers(bAttachModifiers)
{
}
//--------------------------------------------------------------------------
NiGeometryConverter::~NiGeometryConverter()
{    
}
//--------------------------------------------------------------------------
NiGeometryConverter::ConversionResult NiGeometryConverter::Convert(
    NiGeometry* pkGeom, NiMesh* pkMesh)
{
    if (NiIsKindOf(NiParticles, pkGeom))
    {
        // NiParticles conversion happens in the NiParticle library,
        // so we skip it here. 
        return FAIL;
    }

    // Setup with new mesh
    m_pkMesh = pkMesh;

    // Setup global state
    m_pkGeom = pkGeom;
    m_uiGeomVertexCount = pkGeom->GetVertexCount();
    m_uiMeshVertexCount = m_uiGeomVertexCount;
    m_pkSkinPartition = NULL;
    m_pkSkinData = NULL;
    m_pkSkinInstance = pkGeom->GetSkinInstance();
    m_uiPartitionCount = 0;

    // Check for skinning
    m_bIsSkinned = m_pkSkinInstance != NULL;
    m_bNiSkinInstance_HardwareSkinned = false;
    if (m_bIsSkinned)
    {
        m_pkSkinPartition = m_pkSkinInstance->GetSkinPartition();
        m_bNiSkinInstance_HardwareSkinned = m_pkSkinPartition != NULL;

        m_pkSkinData = m_pkSkinInstance->GetSkinData();
        NIASSERT(m_pkSkinData);

        if (m_bNiSkinInstance_HardwareSkinned)
        {
            NIASSERT(m_pkSkinPartition);

            m_pkPartitions = m_pkSkinPartition->GetPartitions();
            NIASSERT(m_pkPartitions);

            m_uiPartitionCount = m_pkSkinPartition->GetPartitionCount();
            NIASSERT(m_uiPartitionCount > 0);
        }

        // If NiSkinInstance was using GPU skinning, use GPU skinning
        // on the NiSkinningMeshModifier
        m_bNiSkinningMeshModifier_HardwareSkinned = 
#if defined(_WII)
            false;
#else
            m_bNiSkinInstance_HardwareSkinned;
#endif

    }

    // Pull out the collision object for later conversion.
    m_spCollision = m_pkGeom->GetCollisionObject();
    if (m_spCollision)
        m_pkGeom->SetCollisionObject(NULL);
   
    // Copy the effect state, property state, and bound
    ConvertCore(pkGeom, pkMesh);

    // Cache the NiGeometry keep and consistency flags.
    SetConverterStateFromGeometry();

    // Copy the materials
    ConvertMaterials(pkGeom, pkMesh);

    // If this mesh uses shared data then copy the data stream GROUP.
    // This will share the underlying pointers by default
    NiMesh* pkSharedMesh = NULL;
    NiGeometryData* pkGeomData = m_pkGeom->GetModelData();
    // Don't clone stream refs for bone LOD controllers.
    bool bHasSkinningLODController = FindBoneController(
        m_pkSkinInstance, "NiSkinningLODController");
    if (m_kSharedDataMap.GetAt(pkGeomData, pkSharedMesh) &&
        !bHasSkinningLODController)
    {
        pkMesh->CloneStreamRefs(pkSharedMesh);

        pkMesh->SetPrimitiveType(pkSharedMesh->GetPrimitiveType());

        pkMesh->UpdateCachedPrimitiveCount();

        return SUCCESS_CLONED;
    }
    // Add this mesh to the cache
    m_kSharedDataMap.SetAt(pkGeomData, pkMesh);

    // Set the submesh count
    if (m_uiPartitionCount == 0)
        pkMesh->SetSubmeshCount(1);
    else
        pkMesh->SetSubmeshCount(m_uiPartitionCount);

    if (!ConvertIndexBuffers())
        return FAIL;
    
    if (!ConvertPositions())
        return FAIL;

    if (!ConvertNBTs())
        return FAIL;
    
    if (!ConvertColors())
        return FAIL;

    if (!ConvertTexcoords())
        return FAIL;

    if (!ConvertAGDs())
        return FAIL;

    if (!ConvertSoftwareSkinData())
        return FAIL;

#if defined(_WII)
    // If the object is skinned, transform the generic 
    // streams into Wii specific skinning streams.
    if (m_bIsSkinned)
    {
        NiWiiSkinningMeshConverter kConverter;
        if (!kConverter.ConvertToWiiSoftwareStreams(m_pkMesh))
            return FAIL;
    }
#endif

    if (!ConvertSkinInstance())
        return FAIL;

    if (!ConvertCollision())
        return FAIL;

    // Conversion should have produced a valid NiMesh object.
    NIASSERT(m_pkMesh->IsValid());

    // Finish mesh creation. Submesh count is already set.
    m_pkMesh = NULL;

    pkMesh->UpdateCachedPrimitiveCount();

    // If we got this far then we succeeded.
    return SUCCESS;
}
//--------------------------------------------------------------------------
void NiGeometryConverter::ConvertCore(NiGeometry* pkGeom, NiMesh* pkMesh)
{
    // Clone and copy effects and property state
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    pkGeom->CloneInPlace(pkMesh, kCloning);
    pkMesh->SetEffectState(pkGeom->GetEffectState());
    pkMesh->SetPropertyState(pkGeom->GetPropertyState());

    // Convert bounds

    // Get initial bound from the skinning data, if it exists.
    if (pkGeom->GetSkinInstance())
    {
        NiBound kBound;
        pkGeom->GetSkinInstance()->UpdateModelBound(kBound);
        pkMesh->SetModelBound(kBound);
    }
    else
    {
        NiGeometryData* pkGeomData = pkGeom->GetModelData();
        NIASSERT(pkGeomData);
        pkMesh->SetModelBound(pkGeomData->GetBound());
    }
}
//--------------------------------------------------------------------------
void NiGeometryConverter::SetConverterStateFromGeometry()
{
    // Check for morphing. Have to use RTTI names so we do not introduce
    // a dependency on NiAnimation, and it gets done after we have started
    // converting so that the controllers have been copied.
    m_bIsMorphed = GetIsMorphed(m_pkMesh);

    NiGeometryData* pkGeomData = m_pkGeom->GetModelData();
    NIASSERT(pkGeomData);
    m_uiKeepFlags = pkGeomData->GetKeepFlags();
    m_uiConsistencyFlags = m_pkGeom->GetConsistency();
}
//--------------------------------------------------------------------------
void NiGeometryConverter::ConvertMaterials(NiGeometry* pkGeom, NiMesh* pkMesh)
{
    // Copy materials
    NiUInt32 uiNumMaterials = pkGeom->GetMaterialCount();
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance* pkInst = pkGeom->GetMaterialInstance(i);
        pkMesh->ApplyMaterial(
            pkInst->GetMaterial(), 
            pkInst->GetMaterialExtraData());
    }
    pkMesh->SetActiveMaterial(pkGeom->GetActiveMaterial());

    // Note that we are not propagating the default flag for material needs 
    // update for older geometry.
    pkMesh->SetMaterialNeedsUpdateDefault(false);

    // If there are no materials applied, then we attach the 
    // renderer default material.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (NiRenderObject::GetStreamingAppliesDefaultMaterial() && 
        pkMesh->GetMaterialCount() == 0 && pkRenderer)
    {
        // Attach the renderer default material.
        pkMesh->ApplyAndSetActiveMaterial(pkRenderer->GetDefaultMaterial());
    }
}
//--------------------------------------------------------------------------
NiGeometryConverter::ConversionResult 
    NiGeometryConverter::ConvertScreenElements()
{
    NIASSERT(NiIsKindOf(NiScreenElements, m_pkGeom));
    NiScreenElements* pkSEs = (NiScreenElements*)m_pkGeom;

    NiMeshScreenElements* pkMeshSE = NiSmartPointerCast(
        NiMeshScreenElements, m_pkMesh);
    NIASSERT(NULL != pkMeshSE);

    pkMeshSE->SetMaxPQuantity((NiUInt16) pkSEs->Data()->GetMaxPQuantity());
    pkMeshSE->SetPQuantity((NiUInt16) pkSEs->Data()->GetNumPolygons());
    pkMeshSE->SetPGrowBy(pkSEs->Data()->GetPGrowBy());
    pkMeshSE->SetMaxVQuantity((NiUInt16) pkSEs->Data()->GetMaxVQuantity());
    pkMeshSE->SetVGrowBy(pkSEs->Data()->GetVGrowBy());
    pkMeshSE->SetMaxTQuantity((NiUInt16) pkSEs->Data()->GetMaxTQuantity());
    pkMeshSE->SetTGrowBy(pkSEs->Data()->GetTGrowBy());
    pkMeshSE->ResetCacheData();

    NiUInt32 uiMaxPolys = pkSEs->Data()->GetMaxPQuantity();
    pkMeshSE->SetPolygonArray(pkSEs->Data()->GetPolygonArray(), 
        (NiUInt16)uiMaxPolys);
    pkMeshSE->SetIndexerArray(pkSEs->Data()->GetIndexerArray(), 
        (NiUInt16)uiMaxPolys);
    return SUCCESS;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertIndexBuffers()
{
    if (m_bIsSkinned && m_bNiSkinInstance_HardwareSkinned)
    {
        return CreateHardwareSkinnedStreams();
    }
    else if (NiIsKindOf(NiTriShape, m_pkGeom))
    {
        // Build index buffer from trishape
        m_pkMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);
        return CreateTriShapeIndexStream((NiTriShape*)m_pkGeom);
    }
    else if (NiIsKindOf(NiTriStrips, m_pkGeom))
    {
        // Build index buffer from tristrip
        m_pkMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRISTRIPS);
        return CreateTriStripsIndexStream((NiTriStrips*)m_pkGeom);
    }
    else if (NiIsKindOf(NiLines, m_pkGeom))
    {
        // Build index buffer from lines
        m_pkMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
        return CreateLinesIndexStream((NiLines*)m_pkGeom);
    }

    // Assert on error
    NIASSERT(0 && "Unsupported Geometry Conversion");
    return false;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertPositions()
{
    // Ensure that we at least have positions
    const NiPoint3* pkPositions = m_pkGeom->GetVertices();
    if (!pkPositions)
        return false;

    NiUInt32 uiPosAccessMask = 
        GetAccess(NiGeometryData::KEEP_XYZ) | NiDataStream::ACCESS_GPU_READ;

    if (m_bIsMorphed)
    {
        // Must be mutable to support position read for normal regeneration.
        SetCPUWriteAccess(uiPosAccessMask, 
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE);
        SetCPUReadAccess(uiPosAccessMask);
    }
    else if (m_bIsSkinned && !m_bNiSkinningMeshModifier_HardwareSkinned)
    {
        // Positions must be writable for software skinning
        if (uiPosAccessMask & NiDataStream::ACCESS_CPU_READ)
        {
            SetCPUWriteAccess(uiPosAccessMask,
                NiDataStream::ACCESS_CPU_WRITE_MUTABLE);
        }
        else
        {
            SetCPUWriteAccess(uiPosAccessMask,
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE);
        }
    }
    
    NiDataStreamRef* pkStreamRef = NULL;
    if (!m_bIsSkinned || 
        (m_bIsSkinned && !m_bNiSkinningMeshModifier_HardwareSkinned))
    {
        pkStreamRef = m_pkMesh->AddStream(NiCommonSemantics::POSITION(), 0, 
            NiDataStreamElement::F_FLOAT32_3, m_uiMeshVertexCount, 
            (NiUInt8)uiPosAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
            !m_bIsRuntime, false);
        NIASSERT(pkStreamRef);
        BuildPartitions<NiPoint3>(pkStreamRef, pkPositions);
    }
    
    if (m_bIsSkinned)
    {
        NiUInt32 uiBindPoseAccessMask = 
            GetAccess(NiGeometryData::KEEP_BONEDATA) | 
            NiDataStream::ACCESS_GPU_READ;

        if (!m_bNiSkinningMeshModifier_HardwareSkinned || m_bIsMorphed)
        {
            // CPU read access needed for the bind pose positions
            SetCPUReadAccess(uiBindPoseAccessMask);
        }
        
        pkStreamRef = m_pkMesh->AddStream(NiCommonSemantics::POSITION_BP(),
            0, NiDataStreamElement::F_FLOAT32_3, m_uiMeshVertexCount, 
            (NiUInt8)uiBindPoseAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
            !m_bIsRuntime, false);
        NIASSERT(pkStreamRef);
        BuildPartitions<NiPoint3>(pkStreamRef, pkPositions);
    }
    
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertNBTs()
{
    // Normals: Generate one stream for normals
    const NiPoint3* pkNormals = m_pkGeom->GetNormals();
    if (pkNormals)
    {
        NiUInt32 uiAccessMask = 
            GetAccess(NiGeometryData::KEEP_NORM) |
            NiDataStream::ACCESS_GPU_READ;

        if (m_bIsMorphed)
        {
            // Must be mutable to support read for normal regeneration.
            SetCPUWriteAccess(uiAccessMask,
                NiDataStream::ACCESS_CPU_WRITE_MUTABLE);
            SetCPUReadAccess(uiAccessMask);
        }
        else if (m_bIsSkinned && !m_bNiSkinningMeshModifier_HardwareSkinned)
        {
            // Normals must be writable for software skinning
            if (uiAccessMask & NiDataStream::ACCESS_CPU_READ)
            {
                SetCPUWriteAccess(uiAccessMask,
                    NiDataStream::ACCESS_CPU_WRITE_MUTABLE);
            }
            else
            {
                SetCPUWriteAccess(uiAccessMask,
                    NiDataStream::ACCESS_CPU_WRITE_VOLATILE);
            }
        }
    
        NiUInt32 uiBPAccessMask =
            GetAccess(NiGeometryData::KEEP_BONEDATA) |
            NiDataStream::ACCESS_GPU_READ;
        if (!m_bNiSkinningMeshModifier_HardwareSkinned || m_bIsMorphed)
        {
            // Must be able to read from bind pose normals to do software 
            // skinning
            SetCPUReadAccess(uiBPAccessMask);
        }
            
        NiDataStreamRef* pkStreamRef = NULL;

        if (!m_bIsSkinned || 
            (m_bIsSkinned && !m_bNiSkinningMeshModifier_HardwareSkinned))
        {
            pkStreamRef = m_pkMesh->AddStream(NiCommonSemantics::NORMAL(),
                0, NiDataStreamElement::F_FLOAT32_3, m_uiMeshVertexCount,
                (NiUInt8)uiAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
                !m_bIsRuntime, false);
            NIASSERT(pkStreamRef);
            BuildPartitions<NiPoint3>(pkStreamRef, pkNormals);
        }

        if (m_bIsSkinned)
        {
            pkStreamRef = m_pkMesh->AddStream(NiCommonSemantics::NORMAL_BP(),
                0, NiDataStreamElement::F_FLOAT32_3, m_uiMeshVertexCount,
                (NiUInt8)uiBPAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
                !m_bIsRuntime, false);
            BuildPartitions<NiPoint3>(pkStreamRef, pkNormals);
            NIASSERT(pkStreamRef);
        }

        // Add binormals and tangents if present
        NiShaderRequirementDesc::NBTFlags eNBTMethod =
            m_pkGeom->GetNormalBinormalTangentMethod(); 

        if (eNBTMethod != NiShaderRequirementDesc::NBT_METHOD_NONE)
        {
            // Generate binormals stream
            const NiPoint3* pkBinormals = pkNormals + m_uiGeomVertexCount;
            const NiPoint3* pkTangents = pkNormals + 2 * m_uiGeomVertexCount;
            if (!m_bIsSkinned ||
                (m_bIsSkinned && !m_bNiSkinningMeshModifier_HardwareSkinned))
            {
                pkStreamRef = m_pkMesh->AddStream(
                    NiCommonSemantics::BINORMAL(), 0, 
                    NiDataStreamElement::F_FLOAT32_3, m_uiMeshVertexCount,
                    (NiUInt8)uiAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
                    !m_bIsRuntime, false);
                NIASSERT(pkStreamRef);
                BuildPartitions<NiPoint3>(pkStreamRef, pkBinormals);

                pkStreamRef = m_pkMesh->AddStream(
                    NiCommonSemantics::TANGENT(), 0, 
                    NiDataStreamElement::F_FLOAT32_3, m_uiMeshVertexCount,
                    (NiUInt8)uiAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
                    !m_bIsRuntime, false);
                NIASSERT(pkStreamRef);
                BuildPartitions<NiPoint3>(pkStreamRef, pkTangents);
            }
            
            if (m_bIsSkinned)
            {
                pkStreamRef = m_pkMesh->AddStream(
                    NiCommonSemantics::BINORMAL_BP(), 0, 
                    NiDataStreamElement::F_FLOAT32_3, m_uiMeshVertexCount,
                    (NiUInt8)uiBPAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
                    !m_bIsRuntime, false);
                BuildPartitions<NiPoint3>(pkStreamRef, pkBinormals);
                NIASSERT(pkStreamRef);

                pkStreamRef = m_pkMesh->AddStream(
                    NiCommonSemantics::TANGENT_BP(), 0, 
                    NiDataStreamElement::F_FLOAT32_3, m_uiMeshVertexCount, 
                    (NiUInt8)uiBPAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
                    !m_bIsRuntime, false);
                BuildPartitions<NiPoint3>(pkStreamRef, pkTangents);
                NIASSERT(pkStreamRef);
            }
        }
    }
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertColors()
{
    // Colors: Generate one stream for colors
    const NiColorA* pkColors = m_pkGeom->GetColors();
    if (pkColors)
    {
        NiUInt32* pkConvertedColors = NiAlloc(NiUInt32, m_uiGeomVertexCount);
        NiDataStreamElement::Format eFormat;
        
        ConvertColorStream(pkColors, m_uiGeomVertexCount, pkConvertedColors, 
            eFormat);

        const NiUInt32 uiAccessMask =
            GetAccess(NiGeometryData::KEEP_COLOR) |
            NiDataStream::ACCESS_GPU_READ;

        NiDataStreamRef* pkStreamRef = 
            m_pkMesh->AddStream(NiCommonSemantics::COLOR(), 0,
            eFormat, m_uiMeshVertexCount, (NiUInt8)uiAccessMask,
            NiDataStream::USAGE_VERTEX, NULL, !m_bIsRuntime, false);
        NIASSERT(pkStreamRef);
        BuildPartitions<NiUInt32>(pkStreamRef, pkConvertedColors);
        
        NiFree(pkConvertedColors);
    }
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertTexcoords()
{
    // Texture coordinates: generate one stream per set
    const NiUInt32 uiAccessMask =
        GetAccess(NiGeometryData::KEEP_UV) | NiDataStream::ACCESS_GPU_READ;

    NiUInt16 uiTextureSetCount = m_pkGeom->GetTextureSets();
    for (NiUInt32 uiIndex = 0; uiIndex < uiTextureSetCount; ++uiIndex)
    {
        NiDataStreamRef* pkStreamRef = NULL;
        const NiPoint2* pkTextureSet = 
            m_pkGeom->GetTextureSet((NiUInt16)uiIndex);
        if (pkTextureSet)
        {
            pkStreamRef = m_pkMesh->AddStream(
                NiCommonSemantics::TEXCOORD(), uiIndex, 
                NiDataStreamElement::F_FLOAT32_2, m_uiMeshVertexCount, 
                (NiUInt8) uiAccessMask, NiDataStream::USAGE_VERTEX, NULL, 
                !m_bIsRuntime, false);
            NIASSERT(pkStreamRef);
            BuildPartitions<NiPoint2>(pkStreamRef, pkTextureSet);
        }
    }
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertAGDs()
{
    // Add AGDs
    NiAdditionalGeometryData* pkAgData = m_pkGeom->GetAdditionalGeometryData();
    if (pkAgData)
    {
        NiUInt32 uiAgdStreamCount = pkAgData->GetDataStreamCount();
        for (NiUInt32 i = 0; i< uiAgdStreamCount; ++i)
        {   
            unsigned char* pucData = NULL;
            NiUInt32 uiType = 0;
            NiUInt16 uiCount = 0;
            NiUInt32 uiTotalSize = 0;
            NiUInt32 uiUnitSize = 0;
            NiUInt32 uiStride = 0;
            pkAgData->GetDataStream(i, pucData, uiType, uiCount, uiTotalSize, 
                uiUnitSize, uiStride);

            NiUInt32 uiAccess = 0;
            NiGeometryData::Consistency eConsistency = NiGeometryData::STATIC;
            bool bKeep = false;
            pkAgData->GetConsistency(i, eConsistency);
            pkAgData->GetKeep(i, bKeep);

            switch(eConsistency)
            {
            case NiGeometryData::MUTABLE:   
                uiAccess = NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
                break;
            case NiGeometryData::STATIC:
                uiAccess = NiDataStream::ACCESS_CPU_WRITE_STATIC;
                break;
            case NiGeometryData::VOLATILE:
                uiAccess = NiDataStream::ACCESS_CPU_WRITE_VOLATILE;
                break;
            default:
                NIASSERT(!"Unknown type!");
                uiAccess = NiDataStream::ACCESS_CPU_WRITE_STATIC;
            }
            
            if (bKeep)
            {
                SetCPUReadAccess(uiAccess);
            }
            
            uiAccess |= NiDataStream::ACCESS_GPU_READ;

            if (pucData)
            {
                NiDataStreamElement::Format eFormat;
                switch (uiType)
                {
                case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
                    eFormat = NiDataStreamElement::F_FLOAT32_1;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
                    eFormat = NiDataStreamElement::F_FLOAT32_2;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
                    eFormat = NiDataStreamElement::F_FLOAT32_3;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
                    eFormat = NiDataStreamElement::F_FLOAT32_4;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
                    eFormat = NiDataStreamElement::F_INT32_1;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
                    eFormat = NiDataStreamElement::F_INT32_2;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
                    eFormat = NiDataStreamElement::F_INT32_3;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
                    eFormat = NiDataStreamElement::F_INT32_4;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
                    eFormat = NiDataStreamElement::F_UINT32_1;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
                    eFormat = NiDataStreamElement::F_UINT32_2;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
                    eFormat = NiDataStreamElement::F_UINT32_3;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
                    eFormat = NiDataStreamElement::F_UINT32_4;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
                    eFormat = NiDataStreamElement::F_INT16_1;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
                    eFormat = NiDataStreamElement::F_INT16_2;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
                    eFormat = NiDataStreamElement::F_INT16_3;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
                    eFormat = NiDataStreamElement::F_INT16_4;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
                    eFormat = NiDataStreamElement::F_UINT16_1;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
                    eFormat = NiDataStreamElement::F_UINT16_2;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
                    eFormat = NiDataStreamElement::F_UINT16_3;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
                    eFormat = NiDataStreamElement::F_UINT16_4;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
                    eFormat = NiDataStreamElement::F_INT8_1;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
                    eFormat = NiDataStreamElement::F_INT8_2;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
                    eFormat = NiDataStreamElement::F_INT8_3;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
                    eFormat = NiDataStreamElement::F_INT8_4;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
                    eFormat = NiDataStreamElement::F_UINT8_1;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
                    eFormat = NiDataStreamElement::F_UINT8_2;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
                    eFormat = NiDataStreamElement::F_UINT8_3;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
                    eFormat = NiDataStreamElement::F_UINT8_4;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
                    eFormat = NiDataStreamElement::F_FLOAT32_1;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
                    eFormat = NiDataStreamElement::F_FLOAT32_2;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
                    eFormat = NiDataStreamElement::F_FLOAT32_3;
                    break;
                case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
                    eFormat = NiDataStreamElement::F_FLOAT32_4;
                    break;
                default:
                    NIASSERT(false && "AGD type not known!");
                    eFormat = NiDataStreamElement::F_UINT32_1;
                }
                
                // Add the single element
                NiDataStreamElementSet kElements;
                kElements.AddElement(eFormat);

                // Allocate stream
                NiDataStream* pkStream = NiDataStream::CreateDataStream(
                    kElements, 
                    uiCount, 
                    (NiUInt8)uiAccess, 
                    NiDataStream::USAGE_VERTEX);
                NIASSERT(pkStream && "Allocation of NiDataStream failed!");


                // Allocate streams Region 0
                NiDataStream::Region kRegion(0, uiCount);
                pkStream->AddRegion(kRegion);

                // Copy data to new stream
                void *pvData = pkStream->Lock(NiDataStream::LOCK_WRITE);
                NIASSERT(pvData && "pkStream->Lock failed!");
                NiMemcpy(pvData, pucData, pkStream->GetSize());
                pkStream->Unlock(NiDataStream::LOCK_WRITE);

                // Add stream reference
                m_pkMesh->AddStreamRef(pkStream, 
                    NiCommonSemantics::DATASTREAM(), i);
            }
        }
    }
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertSoftwareSkinData()
{
    // Get skinning instance and skin data
    if (!m_bIsSkinned || m_bNiSkinInstance_HardwareSkinned)
        return true;
    
    // Cache bone count
    NiUInt32 uiBoneCount = m_pkSkinData->GetBoneCount();

    // Compute blend access flags
    NiUInt32 uiBlendAccess = NiDataStream::ACCESS_CPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_STATIC;

    // Create blend weights stream 
    NIVERIFY(m_pkMesh->AddStream(
        NiCommonSemantics::BLENDWEIGHT(), 0, 
        NiDataStreamElement::F_FLOAT32_4,
        m_uiMeshVertexCount,
        (NiUInt8)uiBlendAccess, 
        NiDataStream::USAGE_VERTEX, 
        NULL, !m_bIsRuntime, true));

    NiDataStreamElementLock kBlendWeightsLock(m_pkMesh, 
        NiCommonSemantics::BLENDWEIGHT(), 0, 
        NiDataStreamElement::F_FLOAT32_4, NiDataStream::LOCK_WRITE);

    NIASSERT(kBlendWeightsLock.DataStreamExists());
    NIASSERT(kBlendWeightsLock.IsLocked());

    // Create blend indices stream (need to store all 4)
    NIVERIFY(m_pkMesh->AddStream(
        NiCommonSemantics::BLENDINDICES(), 0, 
        NiDataStreamElement::F_INT16_4, 
        m_uiMeshVertexCount,
        (NiUInt8)uiBlendAccess, 
        NiDataStream::USAGE_VERTEX, 
        NULL, !m_bIsRuntime, true));

    NiDataStreamElementLock kBlendIndicesLock(m_pkMesh, 
        NiCommonSemantics::BLENDINDICES(), 0, 
        NiDataStreamElement::F_INT16_4, NiDataStream::LOCK_WRITE);

    NIASSERT(kBlendIndicesLock.DataStreamExists());
    NIASSERT(kBlendIndicesLock.IsLocked());

    // Initialize the blend weights and iterators
    NiBlendWeightIterator kBWIter = 
        kBlendWeightsLock.begin<NiBlendWeightArray>();

    NiBlendIndexIterator kBIIter = 
        kBlendIndicesLock.begin<NiBlendIndexArray>();

    // Initialize the arrays with default values
    for (NiUInt32 ui = 0; ui < m_uiMeshVertexCount; ui++)
    {
        for (NiUInt32 uiSubIdx = 0; uiSubIdx < 4; uiSubIdx++)
        {
            kBIIter[ui][uiSubIdx] = 0;
            kBWIter[ui][uiSubIdx] = 0.0f;
        }
    }

    NiSkinData::BoneData* pkBoneData = m_pkSkinData->GetBoneData();
    NIASSERT(pkBoneData);

    for (NiUInt32 uiBone = 0; uiBone < uiBoneCount; uiBone++)
    {
        NiSkinData::BoneVertData* pkBoneVertData = 
            pkBoneData[uiBone].m_pkBoneVertData;

        NIASSERT(pkBoneVertData);

        // Try to find this vertex in the bone array.
        for (NiUInt32 uiBoneVert = 0; 
            uiBoneVert < pkBoneData[uiBone].m_usVerts;
            uiBoneVert++)
        {

            NiUInt16 uiBoneIndex = (NiUInt16)uiBone;
            float fBoneWeight = pkBoneVertData[uiBoneVert].m_fWeight;
            NiUInt32 uiVertex = (NiUInt32)pkBoneVertData[uiBoneVert].m_usVert;

            if (fBoneWeight <= 0.0f)
                continue;

            for (NiUInt32 uiSubIdx = 0; uiSubIdx < 4; uiSubIdx++)
            {
                // Perform an insertion sort if the bone weight is greater
                // than the highest value, dropping off any lingering weights.
                if (fBoneWeight > kBWIter[uiVertex][uiSubIdx])
                {
                    NiUInt32 uiBoneIndexTemp = kBIIter[uiVertex][uiSubIdx];
                    float fBoneWeightTemp = kBWIter[uiVertex][uiSubIdx];

                    kBIIter[uiVertex][uiSubIdx] = uiBoneIndex;
                    kBWIter[uiVertex][uiSubIdx] = fBoneWeight;

                    uiBoneIndex = (NiUInt16)uiBoneIndexTemp;
                    fBoneWeight = fBoneWeightTemp;
                }
            }
        }
    }

    for (NiUInt32 uiVertex = 0; uiVertex < m_uiMeshVertexCount; uiVertex++)
    {
        // Now unitize the weights on the four selected bones.
        if (kBWIter[uiVertex][0] == 0.0f)
        {
            // This bone has no weights at all, make sure that at least the
            // base bone has the full weight applied. The other values should
            // still be initialize to 0.0 for the weights from above.
            kBWIter[uiVertex][0] = 1.0f;
            kBIIter[uiVertex][0] = 0;
        }
        else
        {
            // Renormalize the actual weights
            float fFullValue = kBWIter[uiVertex][0] + kBWIter[uiVertex][1] + 
                kBWIter[uiVertex][2] + kBWIter[uiVertex][3];
            NIASSERT(fFullValue <= 1.000001f);
            NIASSERT(fFullValue > 0.0f);

            float fDivisor = 1.0f / fFullValue;

            for (NiUInt32 uiSubIdx = 0; uiSubIdx < 4; uiSubIdx++)
            {
                kBWIter[uiVertex][uiSubIdx] = fDivisor * 
                    kBWIter[uiVertex][uiSubIdx];
            }
        }
    }

    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertSkinInstance()
{
    // Get skinning instance and skin data
    if (!m_bIsSkinned)
        return true;

    // Cache bone count
    NiUInt32 uiBoneCount = m_pkSkinData->GetBoneCount();
    
    // Create a new skinning modifier which we will build and ultimately attach
    // to this mesh
    NiSkinningMeshModifier* pkModifier =
        NiNew NiSkinningMeshModifier(uiBoneCount);
    NIASSERT(pkModifier);

    pkModifier->SetSoftwareSkinned(!m_bNiSkinningMeshModifier_HardwareSkinned);

    // Get the destinations for the converted skinning data
    NiAVObject** apkDestBones = pkModifier->GetBones();
    NIASSERT(apkDestBones);

    NiTransform* akDestSkinToBoneTransforms =
        pkModifier->GetSkinToBoneTransforms();
    NIASSERT(akDestSkinToBoneTransforms);

    pkModifier->EnableDynamicBounds();
    NiBound* akDestBoneBounds = pkModifier->GetBoneBounds();
    NIASSERT(akDestBoneBounds);

    // Set the root bone and its BoneToBindSpace transform
    NiAVObject* pkRootBoneParent = m_pkSkinInstance->GetRootParent();
    pkModifier->SetRootBoneParent(pkRootBoneParent);
    pkModifier->SetRootBoneParentToSkinTransform(
        m_pkSkinData->GetRootParentToSkin());

    // Set each bone and is bind to bone space transforms
    NiSkinData::BoneData* pkBoneData = m_pkSkinData->GetBoneData();
    NiAVObject** apkSourceBones = (NiAVObject**)m_pkSkinInstance->GetBones();
    for (NiUInt32 ui = 0; ui < uiBoneCount; ui++)
    {
        // Add bones, transforms, and bone bounds
        apkDestBones[ui] = apkSourceBones[ui];
        akDestSkinToBoneTransforms[ui] = pkBoneData[ui].m_kSkinToBone;
        akDestBoneBounds[ui] = pkBoneData[ui].m_kBound;
    }

    // Add the modifier to the mesh
    m_pkMesh->AddModifier(pkModifier, m_bAttachModifiers);
    
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::ConvertCollision()
{
    if (m_spCollision)
    {
        m_pkMesh->SetCollisionObject((NiCollisionObject*)
            m_spCollision->Clone());
        m_spCollision = NULL;
    }
    return true;
}
//--------------------------------------------------------------------------
NiDataStreamRef* 
NiGeometryConverter::BuildIndexBuffer(NiUInt32 uiCount, const NiUInt16* pusSrc,
    NiUInt32 uiAccessMask, bool bDefaultRegion)
{
    NiDataStreamRef* pkStreamRef = NULL;

    if (!m_bForce32BitIndices)
    {
        pkStreamRef = m_pkMesh->AddStream(
            NiCommonSemantics::INDEX(), 
            0, 
            NiDataStreamElement::F_UINT16_1, 
            uiCount, 
            (NiUInt8)uiAccessMask,
            NiDataStream::USAGE_VERTEX_INDEX,
            pusSrc,
            !m_bIsRuntime,
            bDefaultRegion);
    }
    else
    {
        // Create copy of index values as NiUInt32s
        NiUInt32* puiTriList = NULL;
        if (pusSrc)
        {
            puiTriList = NiAlloc(NiUInt32, uiCount);
            for (NiUInt32 i = 0; i < uiCount; i++)
                puiTriList[i] = pusSrc[i];
        }

        // Add index stream
        pkStreamRef = m_pkMesh->AddStream(
            NiCommonSemantics::INDEX(), 
            0, 
            NiDataStreamElement::F_UINT32_1,
            uiCount, 
            (NiUInt8)uiAccessMask, 
            NiDataStream::USAGE_VERTEX_INDEX,
            puiTriList,
            !m_bIsRuntime,
            bDefaultRegion);

        if (puiTriList)
            NiFree(puiTriList);
    } 

    return pkStreamRef;
}
//--------------------------------------------------------------------------
NiDataStreamRef* NiGeometryConverter::BuildStriplistIndexBuffer(
    NiUInt32 uiCount, const NiUInt16* pusSrc, const NiUInt16 uiStripCount, 
    const NiUInt16* puiStripLengths, NiUInt32 uiAccessMask, 
    bool bDefaultRegion)
{
    NiDataStreamRef* pkStreamRef = NULL;

    if (m_bIsMorphed)
    {
        // For normal calculations
        SetCPUReadAccess(uiAccessMask);
    }

    // Insert space for degenerates. At max, we'll need 3 degenerate indices
    // to stitch the strips together and restart the winding order.
    uiCount += (uiStripCount - 1) * 3;

    if (!m_bForce32BitIndices)
    {
        // Create copy of index values as NiUInt16s
        NiUInt16* puiTriList = NULL;
        if (pusSrc)
        {
            puiTriList = NiAlloc(NiUInt16, uiCount);
            NiUInt32 uiSrcIdx = 0;
            NiUInt32 uiDestIdx = 0;

            for (NiUInt32 uiStrip = 0; uiStrip < uiStripCount; uiStrip++)
            {
                // Insert degenerates
                if (uiStrip != 0)
                {
                    bool bOddLastIndex = uiDestIdx % 2 != 0;

                    // if even, we only need to add two.
                    puiTriList[uiDestIdx] = puiTriList[uiDestIdx - 1];
                    ++uiDestIdx;
                    puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                    ++uiDestIdx;

                    // if odd, we need to add one more to make the new strip's
                    // winding order work out right
                    if (bOddLastIndex)
                    {
                        puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                        ++uiDestIdx;
                    }
                        
                }

                for (NiUInt32 uj = 0; uj < puiStripLengths[uiStrip]; uj++)
                {
                    puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                    ++uiSrcIdx;
                    ++uiDestIdx;
                }
            }

            // Fill the end with degenerates
            while (uiDestIdx != uiCount)
            {
                puiTriList[uiDestIdx] = pusSrc[uiSrcIdx - 1];
                ++uiDestIdx;
            }
        }

        // Add index stream
        pkStreamRef = m_pkMesh->AddStream(
            NiCommonSemantics::INDEX(), 
            0, 
            NiDataStreamElement::F_UINT16_1,
            uiCount, 
            (NiUInt8)uiAccessMask, 
            NiDataStream::USAGE_VERTEX_INDEX,
            puiTriList,
            !m_bIsRuntime,
            bDefaultRegion);

        NiFree(puiTriList);
    }
    else
    {
        // Create copy of index values as NiUInt32s
        NiUInt32* puiTriList = NULL;
        if (pusSrc)
        {
            puiTriList = NiAlloc(NiUInt32, uiCount);
            NiUInt32 uiSrcIdx = 0;
            NiUInt32 uiDestIdx = 0;

            for (NiUInt32 uiStrip = 0; uiStrip < uiStripCount; uiStrip++)
            {
                // Insert degenerates
                if (uiStrip != 0)
                {
                    bool bOddLastIndex = uiDestIdx % 2 != 0;

                    // if even, we only need to add two.
                    puiTriList[uiDestIdx] = puiTriList[uiDestIdx - 1];
                    ++uiDestIdx;
                    puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                    ++uiDestIdx;

                    // if odd, we need to add one more to make the new strip's
                    // winding order work out right
                    if (bOddLastIndex)
                    {
                        puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                        ++uiDestIdx;
                    }
                }

                for (NiUInt32 uj = 0; uj < puiStripLengths[uiStrip]; uj++)
                {
                    puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                    ++uiSrcIdx;
                    ++uiDestIdx;
                }
            }
            
            // Fill the end with degenerates
            while (uiDestIdx != uiCount)
            {
                puiTriList[uiDestIdx] = pusSrc[uiSrcIdx - 1];
                ++uiDestIdx;
            }
        }

        // Add index stream
        pkStreamRef = m_pkMesh->AddStream(
            NiCommonSemantics::INDEX(), 
            0, 
            NiDataStreamElement::F_UINT32_1,
            uiCount, 
            (NiUInt8)uiAccessMask, 
            NiDataStream::USAGE_VERTEX_INDEX,
            puiTriList,
            !m_bIsRuntime,
            bDefaultRegion);

        NiFree(puiTriList);
    }     

    return pkStreamRef;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::BuildStriplistIndexBuffer(
    NiSkinPartition::Partition* pkPartition, void* pvIndexBuffer, 
    NiUInt32 uiIndexOffset, NiUInt32& uiIndicesAdded)
{
    NiUInt16* pusSrc = pkPartition->m_pusTriList;
    NIASSERT(pusSrc);

    // Add in padding for degenerates to the total index buffer size for this
    // submesh
    NiUInt32 uiCount = pkPartition->GetStripLengthSum() + 
        (pkPartition->m_usStrips - 1)*3;

    NiUInt32 uiSrcIdx = 0;
    NiUInt32 uiDestIdx = uiIndexOffset;

    if (m_bForce32BitIndices)
    {
        NiUInt32* puiTriList = (NiUInt32*)pvIndexBuffer;
    
        for (NiUInt32 uiStrip = 0; uiStrip < pkPartition->m_usStrips; 
            uiStrip++)
        {
            // Insert degenerates to handle situations where we need to stitch
            // together strips in the same submesh.
            if (uiDestIdx != 0 && pkPartition->m_usStrips != 1)
            {
                bool bOddLastIndex = (uiDestIdx - uiIndexOffset) % 2 != 0;

                // if even, we only need to add two.
                puiTriList[uiDestIdx] = puiTriList[uiDestIdx - 1];
                ++uiDestIdx;
                puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                ++uiDestIdx;

                // if odd, we need to add one more to make the new 
                // strip's winding order work out right
                if (bOddLastIndex)
                {
                    puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                    ++uiDestIdx;
                }
            }

            // Copy over the indices 
            NIASSERT((uiDestIdx - uiIndexOffset) % 2 == 0);
            for (NiUInt32 uj = 0; 
                uj < pkPartition->m_pusStripLengths[uiStrip]; uj++)
            {
                puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                ++uiSrcIdx;
                ++uiDestIdx;
            }
        }
        
        // Fill the end with degenerates
        while (uiDestIdx != uiIndexOffset + uiCount)
        {
            puiTriList[uiDestIdx] = pusSrc[uiSrcIdx - 1];
            ++uiDestIdx;
        }
    }
    else
    {
        NiUInt16* puiTriList = (NiUInt16*)pvIndexBuffer;
        
        for (NiUInt32 uiStrip = 0; uiStrip < pkPartition->m_usStrips; 
            uiStrip++)
        {
            // Insert degenerates to handle situations where we need to stitch
            // together strips in the same submesh.
            if (uiDestIdx != 0 && pkPartition->m_usStrips != 1)
            {
                bool bOddLastIndex = (uiDestIdx - uiIndexOffset) % 2 != 0;

                // if even, we only need to add two.
                puiTriList[uiDestIdx] = puiTriList[uiDestIdx - 1];
                ++uiDestIdx;

                puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                ++uiDestIdx;
                
                // if odd, we need to add one more to make the new 
                // strip's winding order work out right
                if (bOddLastIndex)
                {
                    puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                    ++uiDestIdx;
                }
            }

            // Copy over the indices 
            NIASSERT((uiDestIdx - uiIndexOffset) % 2 == 0);
            for (NiUInt32 uj = 0; 
                uj < pkPartition->m_pusStripLengths[uiStrip]; uj++)
            {
                puiTriList[uiDestIdx] = pusSrc[uiSrcIdx];
                ++uiSrcIdx;
                ++uiDestIdx;
            }
        }
        
        // Fill the end with degenerates
        while (uiDestIdx != uiIndexOffset + uiCount)
        {
            puiTriList[uiDestIdx] = pusSrc[uiSrcIdx - 1];
            ++uiDestIdx;
        }
    }

   uiIndicesAdded = uiDestIdx - uiIndexOffset;
   return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::CreateTriShapeIndexStream(NiTriShape* pkTriShape)
{
    const NiUInt32 uiIndexCount = pkTriShape->GetTriListLength();
    const NiUInt16* pusTriList = pkTriShape->GetTriList();

    NiUInt32 uiAccessMask =
        GetAccess(NiGeometryData::KEEP_INDICES) | 
        NiDataStream::ACCESS_GPU_READ;
    if (m_bIsMorphed)
    {
        // For normal calculations
        SetCPUReadAccess(uiAccessMask);
    }

    if (pusTriList && (uiIndexCount > 0))
    {
        BuildIndexBuffer(uiIndexCount, pusTriList, uiAccessMask, true);
    }
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::CreateTriStripsIndexStream(NiTriStrips* pkTriStrip)
{
    // Ensure that there is no GPU skinning information
    // This should always be the case 
    NIASSERT(!m_bIsSkinned || !m_bNiSkinInstance_HardwareSkinned);

    // Grab the model data
    NiTriStripsData* pkTriStripData = 
        (NiTriStripsData*)pkTriStrip->GetModelData();
    NIASSERT(pkTriStripData);

    // Get the number of triangle strips. This will be the number
    // of Regions we need to generate for this mesh
    const NiUInt16 uiStripCount = pkTriStripData->GetStripCount();
    const NiUInt16* puiStripLengths = pkTriStripData->GetStripLengths();
    const unsigned short* pusTriStrip = pkTriStripData->GetStripLists();
    NiUInt32 uiIndexCount = pkTriStripData->GetStripLengthSum();

    if (uiIndexCount > 0)
    {
        NiUInt32 uiAccessMask =
            GetAccess(NiGeometryData::KEEP_INDICES) |
            NiDataStream::ACCESS_GPU_READ;
        if (m_bIsMorphed)
        {
            // For normal calculations
            SetCPUReadAccess(uiAccessMask);
        }
        NIVERIFY(BuildStriplistIndexBuffer(uiIndexCount, pusTriStrip,
            uiStripCount, puiStripLengths, uiAccessMask, true));
    }
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::CreateLinesIndexStream(NiLines* pkLines)
{
    // For the purpose of conversion, no strips will be used.
    // An index buffer will be created using the 
    // connectivity bits.
    const NiBool* pkConnectivity = pkLines->GetFlags();
    NiUInt32 uiConnections = 0;
    for(NiUInt32 ui=0; ui < m_uiGeomVertexCount; ui++)
    {
        if (pkConnectivity[ui])
            uiConnections++;
    }

    NiUInt32 uiAccessMask =
        GetAccess(NiGeometryData::KEEP_INDICES) |
        NiDataStream::ACCESS_GPU_READ;
    if (m_bIsMorphed)
    {
        // For normal calculations
        SetCPUReadAccess(uiAccessMask);
    }

    uiConnections *= 2; // a line segment for each.
    NiDataStreamRef* pkStreamRef =
        BuildIndexBuffer(uiConnections, NULL, uiAccessMask, true);
    NIASSERT(pkStreamRef);

    NiDataStream* pkStream = pkStreamRef->GetDataStream();

    if (m_bForce32BitIndices)
    {
        NiUInt32* pIndices =
            (NiUInt32*)pkStream->Lock(NiDataStream::LOCK_WRITE);

        NiUInt32 uiIndex = 0;
        for(NiUInt32 ui=0; ui < m_uiGeomVertexCount; ui++)
        {
            if (!pkConnectivity[ui])
                continue;

            pIndices[uiIndex++] = ui;

            if ((ui + 1) == m_uiGeomVertexCount)
            {
                pIndices[uiIndex++] = 0;
                break;
            }
            else
            {
                pIndices[uiIndex++] = ui + 1;
            }
        }
    }
    else
    {
        NiUInt16* pIndices =
            (NiUInt16*)pkStream->Lock(NiDataStream::LOCK_WRITE);

        NiUInt32 uiIndex = 0;
        for(NiUInt32 ui=0; ui < m_uiGeomVertexCount; ui++)
        {
            if (!pkConnectivity[ui])
                continue;

            pIndices[uiIndex++] = (NiUInt16)ui;

            if ((ui + 1) == m_uiGeomVertexCount)
            {
                pIndices[uiIndex++] = 0;
                break;
            }
            else
            {
                pIndices[uiIndex++] = (NiUInt16)(ui + 1);
            }
        }
    }

    pkStream->Unlock(NiDataStream::LOCK_WRITE);
    return true;
}
//--------------------------------------------------------------------------
bool NiGeometryConverter::CreateHardwareSkinnedStreams()
{
    // This function depends on m_pkPartitions and expects it to be non-null.
    NIASSERT(m_bNiSkinInstance_HardwareSkinned);

    // Get the first partition. If the strip count in the partition
    // is non-zero then the partitions will contained tristrips.
    // Otherwise the partitions will contain triangles
    // Walk through the partitions and create index buffer
    NiUInt32 usStrips = m_pkPartitions[0].m_usStrips;
    bool bUsesStrips = usStrips > 0;
    m_pkMesh->SetPrimitiveType(bUsesStrips ? 
        NiPrimitiveType::PRIMITIVE_TRISTRIPS :
        NiPrimitiveType::PRIMITIVE_TRIANGLES);

    // Compute total index value count across all partitions
    NiUInt32 uiTotalIndexCount = 0;
    NiUInt32 uiTotalInteractionCount = 0;
    NiUInt32 uiTotalBones = 0;
    m_uiMeshVertexCount = 0;
    for (NiUInt32 ui = 0; ui < m_uiPartitionCount; ui++)
    {
        NiSkinPartition::Partition* pkPartition = &m_pkPartitions[ui];

        NIASSERT(bUsesStrips == (pkPartition->m_usStrips > 0) 
            && "Partitions must all be strips or shapes");

        // Compute the amount our space needed for index data
        if (bUsesStrips)
        {
            // Insert max potential degenerates per strip
            uiTotalIndexCount += pkPartition->GetStripLengthSum() + 
                (pkPartition->m_usStrips - 1)*3;
        }
        else
        {
            uiTotalIndexCount += pkPartition->m_usTriangles * 3;
        }

        // Compute the total number of bone/vertex interactions
        uiTotalInteractionCount += 
            pkPartition->m_usBonesPerVertex * pkPartition->m_usVertices; 

        uiTotalBones += pkPartition->m_usBones;
        m_uiMeshVertexCount += pkPartition->m_usVertices;

        // Code later on in conversion assumes that there are 4 bones per 
        // vertex to construct the correct stream types
        NIASSERT(pkPartition->m_usBonesPerVertex == 4);
    }

    // Create the index buffer stream
    NiUInt32 uiIndexAccessMask =
        GetAccess(NiGeometryData::KEEP_INDICES) |
        NiDataStream::ACCESS_GPU_READ;
    if (m_bIsMorphed)
    {
        // For normal calculations
        SetCPUReadAccess(uiIndexAccessMask);
    }

    NIVERIFY(BuildIndexBuffer(uiTotalIndexCount, NULL, uiIndexAccessMask,
        false));

    // Create bones indices stream. Consumed by shader system, not the GPU,
    // so no GPU_READ access, and USAGE_USER is used because it is not 
    // per-vertex data.
    NiUInt32 uiBoneAccess = GetAccess(NiGeometryData::KEEP_BONEDATA);
    SetCPUReadAccess(uiBoneAccess);

    NiDataStream* pkBonesDataStream = NULL;
    NiDataStreamRef* pkBonesStreamRef = NULL;
    if (m_bNiSkinningMeshModifier_HardwareSkinned)
    {
        NIVERIFY(m_pkMesh->AddStream(
            NiCommonSemantics::BONE_PALETTE(), 0, 
            NiDataStreamElement::F_UINT16_1, 
            uiTotalBones, 
            (NiUInt8)uiBoneAccess,
            NiDataStream::USAGE_USER, 
            NULL, !m_bIsRuntime, false));
    }

    NiUInt32 uiBlendAccess =
        GetAccess(NiGeometryData::KEEP_BONEDATA) |
        NiDataStream::ACCESS_GPU_READ;
    if (!m_bNiSkinningMeshModifier_HardwareSkinned)
    {
        // When software skinning, blend weights must be CPU readable
        SetCPUReadAccess(uiBlendAccess);
        uiBlendAccess |= NiDataStream::ACCESS_CPU_READ;
    }

    // Create blend weights stream (store only 3, 4th is computed as 1-sum)
    NIVERIFY(m_pkMesh->AddStream(
        NiCommonSemantics::BLENDWEIGHT(), 0, 
        NiDataStreamElement::F_FLOAT32_3,
        m_uiMeshVertexCount,
        (NiUInt8)uiBlendAccess, 
        NiDataStream::USAGE_VERTEX, 
        NULL, !m_bIsRuntime, false));
    

    // Create blend indices stream (need to store all 4)
    NIVERIFY(m_pkMesh->AddStream(
        NiCommonSemantics::BLENDINDICES(), 0, 
        NiDataStreamElement::F_UINT8_4, 
        m_uiMeshVertexCount,
        (NiUInt8)uiBlendAccess, 
        NiDataStream::USAGE_VERTEX, 
        NULL, !m_bIsRuntime, false));

    const NiUInt8 eLock = NiDataStream::LOCK_WRITE;

    // Bones buffer - used by GPU skinning only 
    NiUInt16* pusBones;
    if (m_bNiSkinningMeshModifier_HardwareSkinned)
    {
        pkBonesStreamRef = 
            m_pkMesh->FindStreamRef(NiCommonSemantics::BONE_PALETTE(), 0);
        NIASSERT(pkBonesStreamRef);
        pkBonesDataStream = pkBonesStreamRef->GetDataStream();
        NIASSERT(pkBonesDataStream);
        pusBones = (NiUInt16*)pkBonesDataStream->Lock(eLock);
    }
    else
    {
        pusBones = NULL;
    }

    // Index buffer
    NiDataStreamRef* pkIndexStreamRef = 
        m_pkMesh->FindStreamRef(NiCommonSemantics::INDEX(), 0);
    NIASSERT(pkIndexStreamRef);
    NiDataStream* pkIndexDataStream = pkIndexStreamRef->GetDataStream();
    NIASSERT(pkIndexDataStream);
    void* pvIndexBuffer = pkIndexDataStream->Lock(eLock);

    // Blend Indices
    NiDataStreamRef* pkBlendIndicesStreamRef =
        m_pkMesh->FindStreamRef(NiCommonSemantics::BLENDINDICES(), 0);
    NIASSERT(pkBlendIndicesStreamRef);
    NiDataStream* pkBlendIndicesDataStream = 
        pkBlendIndicesStreamRef->GetDataStream();
    NIASSERT(pkBlendIndicesDataStream);
    NiUInt8* pucBlendIdxs = (NiUInt8*)pkBlendIndicesDataStream->Lock(eLock);

    // Blend weights
    NiDataStreamRef* pkBlendWeightStreamRef =
        m_pkMesh->FindStreamRef(NiCommonSemantics::BLENDWEIGHT(), 0);
    NIASSERT(pkBlendWeightStreamRef);
    NiDataStream* pkBlendWeightDataStream = 
        pkBlendWeightStreamRef->GetDataStream();
    NIASSERT(pkBlendWeightDataStream);
    float* pfBlendWeights = (float*)pkBlendWeightDataStream->Lock(eLock);

    // Initialize the region counters
    NiUInt32 uiIndexStart = 0;
    NiUInt32 uiBoneStart = 0;
    NiUInt32 uiBlendStart = 0;
    NiUInt32 uiBlendWeightStart = 0;    
    NiUInt32 uiIndexOffset = 0;
            
    // Iterate over each partition setting data on buffers
    for (NiUInt16 uiPartition = 0; uiPartition < m_uiPartitionCount; 
        uiPartition++)
    {
        // Get next partition
        NiSkinPartition::Partition* pkPartition = &m_pkPartitions[uiPartition];
        NIASSERT(pkPartition);

        NiUInt32 uiPartitionIndexStartOffset = uiIndexOffset;
      
        // Populate index buffer
        if (bUsesStrips)
        {
            NiUInt32 uiIndicesAdded = 0;
            NIVERIFY(BuildStriplistIndexBuffer(pkPartition, pvIndexBuffer,
                uiIndexOffset, uiIndicesAdded));
            uiIndexOffset += uiIndicesAdded;
        }
        else
        {
            // Cache pointers 
            NiUInt16* pusTriList = pkPartition->m_pusTriList;
            NIASSERT(pusTriList);

            NiUInt32 uiIndexCount = pkPartition->m_usTriangles * 3;

            if (m_bForce32BitIndices)
            {
                for (NiUInt32 uiCount = 0; uiCount < uiIndexCount; uiCount++)
                {
                    ((NiUInt32*)pvIndexBuffer)[uiIndexOffset++] =
                        pusTriList[uiCount];
                }
            }
            else
            {
                for (NiUInt32 uiCount = 0; uiCount < uiIndexCount; uiCount++)
                {
                    ((NiUInt16*)pvIndexBuffer)[uiIndexOffset++] =
                        pusTriList[uiCount];
                }
            }
        }


        // Populate blend weights
        // Storing 3 weights, the 4th is implicitly computed by shader or CPU
        NIASSERT(pkPartition->m_usBonesPerVertex == 4);
        float* pkSourceBlendWeights = pkPartition->m_pfWeights;
        for (NiUInt32 ui = 0; ui < pkPartition->m_usVertices; ui++)
        {
            pfBlendWeights[0] = pkSourceBlendWeights[0];
            pfBlendWeights[1] = pkSourceBlendWeights[1];
            pfBlendWeights[2] = pkSourceBlendWeights[2];
            pfBlendWeights += 3; 
            pkSourceBlendWeights += 4; 
        }

        // Populate blend indices
        if (m_bNiSkinningMeshModifier_HardwareSkinned)
        {
            // Copy bones
            NiUInt32 uiBoneCount = pkPartition->m_usBones;
            NiUInt32 uiBonesSize = uiBoneCount * sizeof(NiUInt16);
            NiMemcpy(pusBones, pkPartition->m_pusBones, uiBonesSize); 
            pusBones += uiBoneCount;

            // Verify that all the bone indices are within a valid range
            NiUInt32 uiMaxBoneIndex = 0;
            for (NiUInt32 ui = 0; ui < uiBoneCount; ui++)
            {
                uiMaxBoneIndex = NiMax(uiMaxBoneIndex,
                    pkPartition->m_pusBones[ui]);
            }
            NIASSERT(uiMaxBoneIndex < m_pkSkinData->GetBoneCount());

            // Populate blend indices
            NiUInt32 uiBlendCount = pkPartition->m_usVertices * 4;
            NiUInt32 uiBlendSize = uiBlendCount * sizeof(NiUInt8);

            if (pkPartition->m_pucBonePalette != NULL)
            {
                NiMemcpy(pucBlendIdxs, pkPartition->m_pucBonePalette,
                    uiBlendSize);
            }
            else
            {
                // Implied bone palette of 0,1,2,3 per vertex
                for (unsigned int ui = 0; ui < uiBlendCount; ++ui)
                {
                    pucBlendIdxs[ui] = (NiUInt8)(ui & 3);
                }
            }

            pucBlendIdxs += uiBlendCount;
            
        }
        else
        {
            // Precompute the mapping for all indices
            NiUInt16* pusBoneMap = pkPartition->m_pusBones;

            NiUInt32 uiInteractions = 
                pkPartition->m_usBonesPerVertex * pkPartition->m_usVertices;

            if (pkPartition->m_pucBonePalette)
            {
                for (NiUInt32 ui = 0; ui < uiInteractions; ui++)
                {
                    NiUInt8 uiIndex = pkPartition->m_pucBonePalette[ui];
                    NiUInt16 uiRemapped = pusBoneMap[uiIndex];
                    NIASSERT(uiRemapped < 256);
                    *pucBlendIdxs++ = (NiUInt8)uiRemapped;
                }
            }
            else
            {
                // Implied bone palette of 0,1,2,3 per vertex
                NiUInt32 uiBlendCount = pkPartition->m_usVertices * 4;
                for (unsigned int ui = 0; ui < uiBlendCount; ++ui)
                {
                    NiUInt8 uiIndex = (NiUInt8)(ui & 3);
                    NiUInt16 uiRemapped = pusBoneMap[uiIndex];
                    NIASSERT(uiRemapped < 256);
                    *pucBlendIdxs++ = (NiUInt8)uiRemapped;
                }
            }
        }

        ///////
        // Build the regions
        ///////

        // Tristrips 
        if (bUsesStrips)
        {
            const NiUInt32 uiVertCount = pkPartition->m_usVertices;
            const NiUInt32 uiBlendWeightCount = pkPartition->m_usVertices;

            // Add the region for the index buffer
            pkIndexStreamRef->BindRegionToSubmesh(uiPartition, 
                NiDataStream::Region(uiPartitionIndexStartOffset, 
                uiIndexOffset - uiPartitionIndexStartOffset));

            if (m_bNiSkinningMeshModifier_HardwareSkinned)
            {
                const NiUInt32 uiCount = pkPartition->m_usBones;
                NiDataStream::Region kBonesRegion(uiBoneStart, uiCount);
                pkBonesStreamRef->BindRegionToSubmesh(uiPartition, 
                    kBonesRegion);
                uiBoneStart += uiCount;
            }

            // Add region for blend indices
            NiDataStream::Region kBlendRegion(uiBlendStart, uiVertCount);
            pkBlendIndicesStreamRef->BindRegionToSubmesh(uiPartition,
                kBlendRegion);

            // Add region for blend weights
            NiDataStream::Region kBlendWeightRegion(uiBlendWeightStart,
                uiBlendWeightCount);
            pkBlendWeightStreamRef->BindRegionToSubmesh(uiPartition,
                kBlendWeightRegion);
        
            uiBlendStart += uiVertCount;
            uiBlendWeightStart += uiBlendWeightCount;
        }
        else
        {
            // Add a single index buffer region for this partition
            NiUInt32 uiRange = pkPartition->m_usTriangles * 3;
            NiDataStream::Region kRegion(uiIndexStart, uiRange);
            pkIndexStreamRef->BindRegionToSubmesh(uiPartition, kRegion);
            uiIndexStart += uiRange;
            
            // Add region for bones
            if (m_bNiSkinningMeshModifier_HardwareSkinned)
            {
                const NiUInt32 uiCount = pkPartition->m_usBones;
                NiDataStream::Region kBonesRegion(uiBoneStart, uiCount);
                pkBonesStreamRef->BindRegionToSubmesh(uiPartition, 
                    kBonesRegion);
                uiBoneStart += uiCount;
            }

            // Add region for blend indices
            uiRange = pkPartition->m_usVertices;
            NiDataStream::Region kBlendRegion(uiBlendStart, uiRange);
            pkBlendIndicesStreamRef->BindRegionToSubmesh(uiPartition, 
                kBlendRegion);
            uiBlendStart += uiRange;

            // Add region for blend weights
            NiDataStream::Region kBlendWeightRegion(uiBlendWeightStart,
                uiRange);
            pkBlendWeightStreamRef->BindRegionToSubmesh(uiPartition, 
                kBlendWeightRegion);
            uiBlendWeightStart += uiRange;
        }
    }
    // Unlock streams
    pkIndexDataStream->Unlock(eLock);
    pkBlendWeightDataStream->Unlock(eLock);
    pkBlendIndicesDataStream->Unlock(eLock);
    if (m_bNiSkinningMeshModifier_HardwareSkinned)
        pkBonesDataStream->Unlock(eLock);

    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiGeometryConverter::GetAccess(NiGeometryData::Keep eKeepMask)
{
    NiUInt32 uiAccess = 0;
    
    switch(m_uiConsistencyFlags)
    {
    // DX9 (and possibly other renderers) require that
    // NiDataStream objects with ACCESS_CPU_WRITE_VOLATILE are written 
    // every frame, but all buffers on a NiGeometry object with
    // VOLATILE consistency are not necessarily written every frame.

    // Since we are propagating the NiGeometry consistency flag to all
    // NiMesh data streams, the only safe conversion to apply is to
    // convert NiGeometryData::VOLATILE to 
    // NiDataStream::ACCESS_CPU_WRITE_MUTABLE, which does not have the 
    // must-write-once-per-frame restriction.

    // If performance suffers because of this, it may be worthwhile to 
    // implement more complex logic that produces more optimized set of 
    // NiDataStream consistency flags and warns if those flags differ from 
    // the NiGeometryData consistency.  We could also provide a 
    // "m_bStrictNiGeometryDataConsistency" option to preserve the existing
    // conversion behavior.

    case NiGeometryData::VOLATILE:
    case NiGeometryData::MUTABLE:   
        uiAccess = NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
        break;
    case NiGeometryData::STATIC:
        uiAccess = NiDataStream::ACCESS_CPU_WRITE_STATIC;
        break;
    default:
        NIASSERT(!"Unknown type!");
        uiAccess = NiDataStream::ACCESS_CPU_WRITE_STATIC;
    }
    
    if (m_uiKeepFlags & eKeepMask || 
        m_uiConsistencyFlags == NiGeometryData::MUTABLE)
    {
        if (uiAccess == NiDataStream::ACCESS_CPU_WRITE_VOLATILE)
            uiAccess = NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
        
        uiAccess |= NiDataStream::ACCESS_CPU_READ;
    }
    
    return uiAccess;
}
//---------------------------------------------------------------------------
bool NiGeometryConverter::GetIsMorphed(NiRenderObject* pkMesh)
{
    NiTimeController* pkController = pkMesh->GetControllers();
    while (pkController)
    {
        if (!NiStricmp(pkController->GetRTTI()->GetName(),
            "NiGeomMorpherController"))
        {
            return true;
        }
        
        pkController = pkController->GetNext();
    }
    
    return false;
}
//---------------------------------------------------------------------------

