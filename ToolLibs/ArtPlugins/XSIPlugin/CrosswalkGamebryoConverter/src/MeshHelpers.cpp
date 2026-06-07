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

#include "CrosswalkGamebryoMeshHelpers.h"
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoSettings.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoNodeMap.h"
#include "CrosswalkGamebryoMatMap.h"
#include "CrosswalkGamebryoTexMap.h"
#include "CrosswalkGamebryoEffectMap.h"

#include "Model.h"
#include "Mesh.h"
#include "XSIMesh.h"
#include "XSITriangleStripList.h"
#include "XSITriangleList.h"
#include "XSIPolygonList.h"
#include "XSIShape.h"
#include "XSIVertexList.h"
#include "TriangleList.h"
#include "BaseShape.h"
#include "GlobalMaterial.h"

#include "NiMesh.h"
#include "NiNode.h"
#include "NiDataStreamLock.h"
#include "NiMaterialProperty.h"
#include "NiVertexColorProperty.h"
#include "NiSourceTexture.h"
#include "NiTexturingProperty.h"
#include "NiNBTGenerator.h"
#include "NiMorphMeshModifier.h"
#include "NiToolDataStream.h"
#include "NiSpecularProperty.h"

#include <NiStringExtraData.h>

namespace
{
    //---------------------------------------------------------------------------
    // Convert Crosswalk number of element to Gamebryo equivalent enum.
    //---------------------------------------------------------------------------
    NiDataStreamElement::Format ConvertPerElementCount(int in_Count)
    {
        switch (in_Count)
        {
            case 1: return NiDataStreamElement::F_FLOAT32_1;
            case 2: return NiDataStreamElement::F_FLOAT32_2;
            case 3: return NiDataStreamElement::F_FLOAT32_3;
            case 4: return NiDataStreamElement::F_FLOAT32_4;
        }

        NIASSERT("Unsupported number of elements per data");
        return NiDataStreamElement::F_UNKNOWN;
    }

    //---------------------------------------------------------------------------
    // Generate binormal and tangent if needed.
    //---------------------------------------------------------------------------
    bool GenerateNBT(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITriangleList& in_Triangles,
        NiMesh& in_Mesh)
    {
        if (NiProperty* pProp = in_Mesh.GetProperty(NiTexturingProperty::GetType()))
        {
            NiTexturingProperty* pTexProp = NiDynamicCast(NiTexturingProperty,pProp);
            if (!pTexProp)
            {
                return io_Context.Logf(
                    epg::LOG_ERROR,
                    "Mesh \"%s\" texturing property is invalid.",
                    in_Model.Name().GetText());
            }

            typedef NiTexturingProperty::Map TexMap;
            if (TexMap* pMap = pTexProp->GetNormalMap())
            {
                if (!NiNBTGenerator::SetupBinormalTangentData(
                    &in_Mesh, pMap->GetTextureIndex(), NiShaderRequirementDesc::NBT_METHOD_NDL))
                {
                    return io_Context.Logf(
                        epg::LOG_ERROR,
                        "Cannot create binormal and tangents for normal map of mesh \"%s\".",
                        in_Model.Name().GetText());
                }
            }

            if (TexMap* pMap = pTexProp->GetParallaxMap())
            {
                if (!NiNBTGenerator::SetupBinormalTangentData(
                    &in_Mesh, pMap->GetTextureIndex(), NiShaderRequirementDesc::NBT_METHOD_NDL))
                {
                    return io_Context.Logf(
                        epg::LOG_ERROR,
                        "Cannot create binormal and tangents for parallax map of mesh \"%s\".",
                        in_Model.Name().GetText());
                }
            }

            // Create NBTs for bump map on WII platform only.
            if (io_Context.GetSettings().GetTargetPlatform() == epg::WII_PLATFORM)
            {
                if (TexMap* pMap = pTexProp->GetBumpMap())
                {
                    if (!NiNBTGenerator::SetupBinormalTangentData(
                        &in_Mesh, pMap->GetTextureIndex(),
                        NiShaderRequirementDesc::NBT_METHOD_NDL))
                    {
                        return io_Context.Logf(
                            epg::LOG_ERROR,
                            "Cannot create binormal and tangents for bump map of mesh \"%s\".",
                            in_Model.Name().GetText());
                    }
                }
            }
        }

        CSLBaseMaterial* pBaseMat = in_Triangles.GetMaterial();
        NiMaterial* pMat = pBaseMat ? io_Context.GetMaterialMap().FindMaterial(*pBaseMat) : 0;
        if (pMat)
        {
            const int textureIndex = 0;
            if (!NiNBTGenerator::SetupBinormalTangentData(
                &in_Mesh, textureIndex, NiShaderRequirementDesc::NBT_METHOD_NDL))
            {
                return io_Context.Logf(
                    epg::LOG_ERROR,
                    "Cannot create binormal and tangents for FX shader of mesh \"%s\".",
                    in_Model.Name().GetText());
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert a Crosswalk triangle list's attribute list to a buffer of float.
    //---------------------------------------------------------------------------
    bool ConvertAttributeIndicesToValues(
        epg::Context&  io_Context,
        CSLXSIShape& in_Shape,
        CSLXSITriangleList& in_Triangles,
        CSLXSISubComponentAttributeList* in_pAttrList,
        epg::CSLIntArray* in_pIndices,
        std::vector<float>& out_Buffer,
        int& out_ValuesPerElement)

    {
        out_Buffer.clear();

        if (!in_pAttrList)
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "Missing attribute list in triangle list \"%s\".",
                in_Triangles.Name().GetText());
        }

        if (!in_pIndices)
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "Missing indices in triangle list \"%s\".",
                in_Triangles.Name().GetText());
        }

        SI_Float* pValues = in_pAttrList->GetAttributeArray()->ArrayPtr();
        if (!pValues)
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "Missing attribute list value array in triangle list \"%s\".",
                in_Triangles.Name().GetText());
        }

        SI_Int* pIndices = in_pIndices->ArrayPtr();
        if (!pIndices)
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "Missing indices value array in triangle list \"%s\".",
                in_Triangles.Name().GetText());
        }

        const int triangleCount = in_Triangles.GetTriangleCount();
        static const int VERTICES_PER_TRIANGLE = 3;
        int elementsPerVertex = in_pAttrList->GetAttributeType();

        // Note: indexed attributes only contain a subset of all values.
        //       We convert them to a full set of values for simplicity.

        std::vector<SI_Float> fullValues;
        if (CSLXSIShape::XSI_INDEXED == in_Shape.GetShapeType())
        {
            const int elementsPerValues = elementsPerVertex--;

            // This initializes the vector to all zeroes.
            // This is grossly over-allocated, , basically assuming that all
            // triangles have their own vertices, no vertex shared.
            fullValues.resize(triangleCount * VERTICES_PER_TRIANGLE * elementsPerVertex);

            // Now fill existing values.
            SI_Int count = in_pAttrList->GetCount();
            for (SI_Int i = 0; i < count; ++i)
            {
                int index = (int) ::floor(pValues[i * elementsPerValues] + 0.001f);
                for (int iaxis = 0; iaxis < elementsPerVertex; ++iaxis)
                {
                    // Make sure our prediction was correct.
                    const int fullValueIndex = index * elementsPerVertex + iaxis;
                    if (fullValueIndex >= (int) fullValues.size())
                        fullValues.resize(fullValueIndex+1);

                    fullValues[fullValueIndex] = pValues[i * elementsPerValues + 1 + iaxis];
                }
            }

            pValues = &fullValues[0];
        }

        out_Buffer.reserve(triangleCount * VERTICES_PER_TRIANGLE * elementsPerVertex);

        for (int it = 0; it < triangleCount; ++it)
        {
            const int triangleBasedOffset = it * VERTICES_PER_TRIANGLE;
            for (int iv = 0; iv < VERTICES_PER_TRIANGLE; ++iv)
            {
                const int vertexBasedOffset = triangleBasedOffset + iv;
                NIASSERT(vertexBasedOffset < in_pIndices->GetUsed());

                const int index = pIndices[vertexBasedOffset];
                for (int iaxis = 0; iaxis < elementsPerVertex; ++iaxis)
                {
                    const int valueIndex = index * elementsPerVertex + iaxis;
                    NIASSERT((CSLXSIShape::XSI_INDEXED == in_Shape.GetShapeType())
                          || (valueIndex < in_pAttrList->GetAttributeArray()->GetUsed()));
                    out_Buffer.push_back(pValues[valueIndex]);
                }
            }
        }

        out_ValuesPerElement = elementsPerVertex;

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the triangle list vertex positions to a stream.
    //---------------------------------------------------------------------------
    bool ConvertTrianglesPositions(
        epg::Context&  io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_Shape,
        epg::ShapeAttributeMap& in_ShapeAttrMap,
        CSLXSITriangleList& in_Triangles,
        NiMesh& io_Mesh,
        std::vector<float> out_Buffer)
    {
        // Note: if semantic mapper returns an empty string it means
        //       this stream should be skipped.
        CSLXSISubComponentAttributeList* pPosAttr = in_Shape.GetVertexPositionList();
        const NiFixedString& semantic = in_ShapeAttrMap.SemanticMap[pPosAttr];
        if (!semantic.GetLength())
            return true;

        int valuesPerElement = 0;
        if (!ConvertAttributeIndicesToValues(
            io_Context,
            in_Shape,
            in_Triangles,
            pPosAttr,
            in_Triangles.GetVertexIndices(),
            out_Buffer,
            valuesPerElement))
        {
            return false;
        }

        // Note: must be done after ConvertTransform()
        //      (which is called inside ConvertCommonProperties())
        //       because that may set the irregular scale of the node.
        const CSIBCVector3D* irregularScale = io_Context.GetNodeMap().FindModelScale(in_Model);
        if (irregularScale)
            epg::ScalePoints(*irregularScale, out_Buffer);

        epg::AddStreamToMesh(
            io_Context,
            io_Mesh,
            semantic,
            -1,
            valuesPerElement,
            out_Buffer);

        // Create a dummy index buffer if this is a pure POSITION
        // stream (avoid it for base poses: POSITION_BP, MORPH_POSITION_BP, etc).
        if (NiCommonSemantics::POSITION() == semantic)
        {
            std::vector<NiUInt32> indices;
            for (NiUInt32 i = 0; i < out_Buffer.size() / valuesPerElement; i++)
                indices.push_back(i);
            epg::AddIndexStreamToMesh(
                io_Context,
                io_Mesh,
                NiCommonSemantics::INDEX(),
                -1,
                1,
                indices);
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the triangle list attribute values to a stream.
    //---------------------------------------------------------------------------
    bool ConvertTrianglesAttribute(
        epg::Context&  io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_Shape,
        CSLXSITriangleList& in_Triangles,
        SI_Char* in_Name,
        const NiFixedString& in_Semantic,
        CSLXSISubComponentAttributeList* in_pkAttrList,
        epg::CSLIntArray* in_pkIndices,
        NiMesh& io_Mesh,
        std::vector<float> out_Buffer)
    {
        int valuesPerElement = 0;
        if (!ConvertAttributeIndicesToValues(
            io_Context,
            in_Shape,
            in_Triangles,
            in_pkAttrList,
            in_pkIndices,
            out_Buffer,
            valuesPerElement))
        {
            return false;
        }

        // Note: UV texture coordinate have the second coordinate in opposite
        //       directions in XSI vs. Gamebryo, so invert them here.
        //
        //       Also, we skip generating UV if texture are not exported.
        //
        //       On top of that, for texture coordinate, we need to retrieve
        //       the stream index that was used in the texture property map.
        if (strcmp(in_pkAttrList->GetSemantic(), "TEXCOORD") == 0)
        {
            if (!io_Context.GetSettings().GetExportTextures())
                return true;

            for (size_t i = 1; i < out_Buffer.size(); i += 2)
            {
                out_Buffer[i] = 1.0f - out_Buffer[i];
            }

            // Note: we assume that if any of the following fails,
            //       it's because the texture is not being exported.

            CSLBaseMaterial* mat = in_Triangles.GetMaterial();
            if (!mat)
                return true;

            epg::TextureInfo* texInfo = io_Context.GetTextureMap().FindTextureInfo(*mat);
            if (!texInfo)
                return true;

            // Take care of UV repetitions: in XSI it is a simple parameter
            // but in Gamebryo we need to scale the UV values.
            typedef epg::TextureInfo::RepeatUV RepeatUV;
            typedef epg::TextureInfo::TextureCoordinates TexCoordinates;
            typedef TexCoordinates::const_iterator iter;
            const TexCoordinates& streams = texInfo->FindStreamIndexes(in_Name).empty()
                                          ? texInfo->FindStreamIndexes("*")
                                          : texInfo->FindStreamIndexes(in_Name);
            for (iter pos = streams.begin(); pos != streams.end(); ++pos)
            {
                const RepeatUV repeatUV = pos->first;
                const int streamIndex   = pos->second;
                if (repeatUV.first == 1.0f && repeatUV.second == 1.0f)
                {
                    epg::AddStreamToMesh(
                        io_Context,
                        io_Mesh,
                        in_Semantic,
                        streamIndex,
                        valuesPerElement,
                        out_Buffer);
                }
                else
                {
                    std::vector<float> scaledUVs(out_Buffer);
                    for (size_t i = 0; i < scaledUVs.size(); i+= 2)
                    {
                        scaledUVs[i+0] *= repeatUV.first;
                        scaledUVs[i+1] *= repeatUV.second;
                    }
                    epg::AddStreamToMesh(
                        io_Context,
                        io_Mesh,
                        in_Semantic,
                        streamIndex,
                        valuesPerElement,
                        scaledUVs);
                }
            }
        }
        else
        {
            epg::AddStreamToMesh(
                io_Context,
                io_Mesh,
                in_Semantic,
                -1,
                valuesPerElement,
                out_Buffer);
            if (strcmp(in_pkAttrList->GetSemantic(), "COLOR") == 0)
            {
                NiVertexColorPropertyPtr pkVertexColor = NiNew NiVertexColorProperty;
                pkVertexColor->SetSourceMode(NiVertexColorProperty::SOURCE_AMB_DIFF);
                pkVertexColor->SetLightingMode(NiVertexColorProperty::LIGHTING_E_A_D);
                io_Mesh.AttachProperty(pkVertexColor);
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Create the Gamebryo mesh with its material properties.
    //---------------------------------------------------------------------------
    NiMeshPtr CreateGamebryoMesh(
        Context& io_Context,
        CSLModel& in_Model,
        NiNode& io_Node,
        CSLBaseMaterial* in_pBaseMat)
    {
        // Prepare the Gamebryo mesh object.
        NiMeshPtr pkMesh = NiNew NiMesh;
        pkMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);
        io_Node.AttachChild(pkMesh);

        // Build a unique name for the mesh using the address of the mesh.
        CSIBCString name = in_Model.Name();
        name.Concat( "-Tri");
        // Note: the following substraction is to avoid a compiler warning.
        name.Concat((int) (pkMesh - (NiMesh*)0));
        pkMesh->SetName(name.GetText());

        CopyVisibility(in_Model, *pkMesh);

        // Propagate the selective update flags so that animations work.
        if (io_Node.GetSelectiveUpdate())
        {
            pkMesh->SetSelectiveUpdate(true);
            if (io_Node.GetSelectiveUpdateTransforms())
                pkMesh->SetSelectiveUpdateTransforms(true);
        }

        // Now set the visual aspect of the mesh: color, texture, materials, etc.
        if (io_Context.GetSettings().GetExportTextures() && in_pBaseMat)
        {
            MaterialMap& matMap = io_Context.GetMaterialMap();
            NiProperty* specProp = matMap.FindSpecularProperty(*in_pBaseMat);
            if (specProp)
                pkMesh->AttachProperty(specProp);

            NiProperty* matProp = matMap.FindMaterialProperty(*in_pBaseMat);
            if (matProp)
                pkMesh->AttachProperty(matProp);

            NiMaterial* mat = matMap.FindMaterial(*in_pBaseMat);
            if (mat)
            {             
                const NiFixedString& shaderName = mat->GetName();
                pkMesh->ApplyAndSetActiveMaterial(shaderName);

                // In this case, we have a shader, so add extra data, if any.
                ExtraDataGroup* pExtraDataGroup = matMap.FindExtraDataGroup(*in_pBaseMat);
                if (pExtraDataGroup)
                {
                    for (int i = 0; i < pExtraDataGroup->GetEntryCount(); ++i)
                    {
                        NiExtraData* pExtraData = pExtraDataGroup->GetEntry(i);
                        if (!pExtraData)
                            continue;
                        pkMesh->AddExtraData(pExtraData);
                    }                    
                }
            }

            TextureMap& texMap = io_Context.GetTextureMap();
            TextureInfo* texInfo = texMap.FindTextureInfo(*in_pBaseMat);
            if (texInfo)
                pkMesh->AttachProperty(&texInfo->GetProperty());

            EffectMap& effMap = io_Context.GetEffectMap();
            const bool hasEffects = effMap.AddAffectedObject(*in_pBaseMat, io_Node);

            if (!matProp && !texInfo && !hasEffects && !mat)
            {
                io_Context.Logf(
                    LOG_WARNING,
                    "Mesh \"%s\" material \"%s\" Gamebryo equivalent could not be found.",
                    in_Model.Name().GetText(),
                    in_pBaseMat->Name().GetText());
            }
        }

        //Add the mesh profile extra data
        const char *pcProfilePropertyName = "MeshProfileName";
        NiStringExtraData* pkMeshProfileExtraData;
        if (NiStricmp(io_Context.GetSettings().GetMeshProfile(), "") != 0)
        {
            pkMeshProfileExtraData = 
                NiNew NiStringExtraData(io_Context.GetSettings().GetMeshProfile());
        }
        else
        {
            pkMeshProfileExtraData = 
                NiNew NiStringExtraData("Default");
        }

        pkMeshProfileExtraData->SetName(pcProfilePropertyName);

        pkMesh->AddExtraData(pkMeshProfileExtraData);

        return pkMesh;
    }

    //---------------------------------------------------------------------------
    // Add the given float buffer as a stream in a Gamebryo mesh.
    //---------------------------------------------------------------------------
    int AddStreamToMesh(
        Context&  io_Context,
        NiMesh& io_Mesh,
        const NiFixedString& in_Semantic,
        int in_StreamIndex,
        int in_PerElementCount,
        const std::vector<float>& in_Values)
    {
        if (in_Semantic.GetLength() == 0)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Unsupported stream semantic \"%s\" in mesh \"%s\".",
                (const char *)in_Semantic,
                (const char *)io_Mesh.GetName());
            return -1;
        }

        NiDataStreamElement::Format format = ConvertPerElementCount(in_PerElementCount);
        if (NiDataStreamElement::F_UNKNOWN == format)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Unsupported stream data format in mesh \"%s\".",
                (const char *)io_Mesh.GetName());
            return -1;
        }

        if (in_StreamIndex < 0)
            in_StreamIndex = io_Mesh.GetSemanticCount(in_Semantic);

        NiDataStreamRef* pPreviousRef = io_Mesh.FindStreamRef(
            in_Semantic, in_StreamIndex, format);
        if (pPreviousRef)
        {
            io_Mesh.RemoveStreamRef(pPreviousRef);
        }

        NiDataStreamRef* pRef = io_Mesh.AddStream(
            in_Semantic,
            in_StreamIndex,
            format,
            in_Values.size() / in_PerElementCount,
            NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX,
            &(in_Values[0]));

        return in_StreamIndex;
    }

    //---------------------------------------------------------------------------
    // Add the given index buffer as a stream in a Gamebryo mesh.
    //---------------------------------------------------------------------------
    int AddIndexStreamToMesh(
        Context&  io_Context,
        NiMesh& io_Mesh,
        const NiFixedString& in_Semantic,
        int in_StreamIndex,
        int in_PerElementCount,
        const std::vector<NiUInt32>& in_Values)
    {
        if (in_Semantic.GetLength() == 0)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Unsupported stream semantic \"%s\" in mesh \"%s\".",
                (const char *)in_Semantic,
                (const char *)io_Mesh.GetName());
            return -1;
        }

        NiDataStreamElement::Format format = NiDataStreamElement::F_UINT32_1;
        if (NiDataStreamElement::F_UNKNOWN == format)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Unsupported stream data format in mesh \"%s\".",
                (const char *)io_Mesh.GetName());
            return -1;
        }

        if (in_StreamIndex < 0)
            in_StreamIndex = io_Mesh.GetSemanticCount(in_Semantic);

        NiDataStreamRef* pPreviousRef = io_Mesh.FindStreamRef(
            in_Semantic, in_StreamIndex, format);
        if (pPreviousRef)
        {
            io_Mesh.RemoveStreamRef(pPreviousRef);
        }

        NiDataStreamRef* pRef = io_Mesh.AddStream(
            in_Semantic,
            in_StreamIndex,
            format,
            in_Values.size() / in_PerElementCount,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX,
            &(in_Values[0]));

        return in_StreamIndex;
    }

    //---------------------------------------------------------------------------
    // Type-safe or'ing of stream access flags.
    //---------------------------------------------------------------------------
    NiDataStream::Access operator|(NiDataStream::Access lhs, NiDataStream::Access rhs)
    {
        return (NiDataStream::Access)((int)lhs | (int)rhs);
    }

    //---------------------------------------------------------------------------
    // Modify a data stream access and usage flags.
    //---------------------------------------------------------------------------
    bool SetStreamFlags(
        Context&  io_Context,
        CSLModel& in_Model,
        NiMesh& io_Mesh,
        const NiFixedString& in_Semantic,
        NiDataStream::Access in_NewAccess,
        NiDataStream::Usage  in_NewUsage)
    {
        // By using NiToolDataStream, we can change the stream access flag in-place,
        // avoiding a copy of the stream data.
        NiDataStreamRef* pStreamRef = io_Mesh.FindStreamRef(in_Semantic);
        NiDataStream* pStream = pStreamRef->GetDataStream();
        if (NiToolDataStream* pkToolStream = NiDynamicCast(NiToolDataStream,pStream))
        {
            pkToolStream->SetAccessMask(in_NewAccess);
            pkToolStream->SetUsage(in_NewUsage);
        }
        else
        {
            NiDataStream* pNewStream = pStream->CreateFullCopyEx(
                in_NewAccess, in_NewUsage, true, true);
            if (!pNewStream)
            {
                return io_Context.Logf(
                    epg::LOG_ERROR,
                    "Cannot set stream access flags on \"%s\".",
                    in_Model.Name().GetText());
            }

            pStreamRef->SetDataStream(pNewStream);
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert a triangle list and its attributes to a new mesh, including
    // an optional shape animation.
    //---------------------------------------------------------------------------
    NiMesh* ConvertTriangleListToMesh(
        Context&  io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_Shape,
        ShapeAttributeMap& in_ShapeAttrMap,
        CSLXSITriangleList& in_Triangles,
        NiNode&   io_Node)
    {
        NiMeshPtr pkMesh = CreateGamebryoMesh(
            io_Context, in_Model, io_Node, in_Triangles.GetMaterial());
        if (!ConvertTriangleListToMesh(
            io_Context, in_Model, in_Shape, in_ShapeAttrMap, in_Triangles, *pkMesh))
        {
            return 0;
        }
        return pkMesh;
    }

    //---------------------------------------------------------------------------
    // Convert a triangle list and its attributes into an existing mesh, including
    // an optional shape animation.
    //---------------------------------------------------------------------------
    bool ConvertTriangleListToMesh(
        Context&  io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_Shape,
        ShapeAttributeMap& in_ShapeAttrMap,
        CSLXSITriangleList& in_Triangles,
        NiMesh& io_Mesh)
    {
        // Temporary storage for the various streams associated with the mesh.
        std::vector<float> buffer;

        // Note: for obvious reasons (i.e. making the API harder than it should)
        //       the vertices position indices array is not kept nor provided with the
        //       other arrays. Even though it's kept in an identical fashion
        //       on-disk, it has it's own slightly different API. Yay.
        bool success = ConvertTrianglesPositions(
                          io_Context,
                          in_Model,
                          in_Shape,
                          in_ShapeAttrMap,
                          in_Triangles,
                          io_Mesh,
                          buffer);
        if (!success)
            return false;

        // Convert all other triangle list attribute arrays:
        // normals, texture coordinates, etc.
    	typedef CSLArrayProxy<SI_Char*, SI_Char*, 1> CSLStringArray;
        CSIBCArray<CSLIntArray*>* pkIndicesArray = in_Triangles.GetAttributeIndicesArray();
        CSLStringArray* pkNameArray = in_Triangles.GetAttributeNameArray();

        if (pkIndicesArray && pkNameArray)
        {
            const SI_Int count = in_Triangles.GetAttributeCount();
            for (SI_Int i = 0; i < count; ++i)
            {
                CSLIntArray* pkIndices = (*pkIndicesArray)[i];
                SI_Char* name = (*pkNameArray)[i];
                if (!name || !pkIndices)
                {
                    return io_Context.Logf(
                        epg::LOG_ERROR,
                        "Missing attribute list #%d in triangle list \"%s\".",
                        (int)i+1,
                        in_Triangles.Name().GetText());
                }


                // Note: if semantic mapper returns an empty string it means
                //       this stream should be skipped.
                CSLXSISubComponentAttributeList* pkAttrList = in_ShapeAttrMap.NameMap[name];
                if (!pkAttrList)
                    continue;
                const NiFixedString& semantic = in_ShapeAttrMap.SemanticMap[pkAttrList];
                if (!semantic.GetLength())
                    continue;

                success = ConvertTrianglesAttribute(
                             io_Context,
                             in_Model,
                             in_Shape,
                             in_Triangles,
                             name,
                             semantic,
                             pkAttrList,
                             pkIndices,
                             io_Mesh,
                             buffer);
                if (!success)
                    return false;
            }
        }

        if (!GenerateNBT(io_Context, in_Model, in_Triangles, io_Mesh))
            return false;

        io_Mesh.SetSubmeshCount(1);
        io_Mesh.RecomputeBounds();

        return true;
    }

    //---------------------------------------------------------------------------
}
