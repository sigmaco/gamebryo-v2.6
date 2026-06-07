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
#include "NiD3DRendererPCH.h"

#include "NiDX9MeshMaterialBinding.h"

#include "NiDX9Renderer.h"
#include <NiMesh.h>
#include <NiMeshMaterialBinding.h>
#include <NiRenderObject.h>
#include <NiTArray.h>

//---------------------------------------------------------------------------
NiDX9MeshMaterialBinding::NiDX9MeshMaterialBinding() :
    m_pkVertexDecl(NULL),
    m_pkIndexStreamRef(NULL)
{
}
//---------------------------------------------------------------------------
NiDX9MeshMaterialBinding::~NiDX9MeshMaterialBinding()
{
    Invalidate();
}
//---------------------------------------------------------------------------
void NiDX9MeshMaterialBinding::Invalidate()
{
    if (m_pkVertexDecl)
    {
        if (NiDX9Renderer::GetRenderer())
            NiDX9Renderer::GetRenderer()->GetRenderState()->ClearDeclaration(
            m_pkVertexDecl);

        NiDX9Renderer::ReleaseVertexDecl(m_pkVertexDecl);
        m_pkVertexDecl = 0;
    }
}
//---------------------------------------------------------------------------
struct DX9CreateBindingContext : 
    public NiMeshMaterialBinding::CreateBindingBaseContext
{
    D3DVERTEXELEMENT9 m_akVertexElements[MAXD3DDECLLENGTH+1]; // +1 for end
    NiTPrimitiveArray<NiUInt16>* m_pkStreamsToSet;

    virtual bool CallBack_EndOfElementLoop(
        NiUInt32 uiStream,
        const NiDataStreamRef* pkStreamRef,
        const NiDataStreamElement kElement,
        NiDataStreamElement::Format ePackedDataFormat, 
        const NiFixedString& kRendererSemantic, 
        const NiUInt8 uiRendererSemanticIndex, 
        NiUInt32 uiPackedDataFormatComponentCount, 
        NiUInt32 uiPackedOffset);
};
//---------------------------------------------------------------------------
bool DX9CreateBindingContext::CallBack_EndOfElementLoop(NiUInt32 uiStream,
    const NiDataStreamRef*, const NiDataStreamElement kElement,
    NiDataStreamElement::Format ePackedDataFormat,
    const NiFixedString& kRendererSemantic,
    const NiUInt8 uiRendererSemanticIndex, 
    NiUInt32 uiPackedDataFormatComponentCount, 
    NiUInt32 uiPackedOffset)
{
    // Add new entry
    D3DDECLTYPE eD3DType;
    D3DDECLUSAGE eD3DUsage;

    if (NiDX9MeshMaterialBinding::D3DType_From_Format(
            ePackedDataFormat, eD3DType) 
        &&
        NiDX9MeshMaterialBinding::D3DUsage_From_Semantic(
            kRendererSemantic, eD3DUsage))
    {
        if (NiDX9MeshMaterialBinding::NumComponents_From_D3DDECLTYPE(
            eD3DType) < uiPackedDataFormatComponentCount)
        {
            NiRenderer::Warning(
                __FUNCTION__ "> "
                "Malformed semantic adapter table for mesh %s: "
                "not enough components in vertex element %s for stream %d.\n"
                "    D3D will zero out the remaining components incoming "
                "to the shader.",
                m_pkMesh->GetName(), 
                kElement.GetFormatString(),
                m_uiCurrentStream);
        }       
        
        D3DVERTEXELEMENT9& kCurrentElement = 
            m_akVertexElements[m_uiCurrentElement];
        kCurrentElement.Stream = (WORD)m_uiCurrentStream;
        kCurrentElement.Offset = (WORD)uiPackedOffset;
        kCurrentElement.Type = (BYTE)eD3DType;
        kCurrentElement.Method = D3DDECLMETHOD_DEFAULT;
        kCurrentElement.Usage = (BYTE)eD3DUsage;
        kCurrentElement.UsageIndex = uiRendererSemanticIndex;

        // Store the streams that will need to be set to device.
        // For each device stream that will be set, 
        // associate our stream index from the NiMesh
        if (m_pkStreamsToSet->GetSize() == m_uiCurrentStream)
        {
            // First time we are adding an element to this stream
            m_pkStreamsToSet->SetAtGrow(m_uiCurrentStream,
                (unsigned short)uiStream);
        }
        else if (m_pkStreamsToSet->GetSize() == m_uiCurrentStream + 1)
        {
            // Already added this stream, check that its contents are the same
            if (m_pkStreamsToSet->GetAt(m_uiCurrentStream) != uiStream)
            {
                NiRenderer::Warning(
                    __FUNCTION__ "> "
                    "m_uiCurrentStream already added to m_pkStreamsToSet, "
                    "but uiStream did not match.");
                return false;
            }
        }
        else
        {
            NiRenderer::Warning(
                __FUNCTION__ "> "
                "m_pkStreamsToSet expected to be same size or +1 of "
                "m_uiCurrentStream, but wasn't. Was %d vs %d.",
                m_pkStreamsToSet->GetSize(), 
                m_uiCurrentStream);
            return false;
        }
    }
    else
    {
        NiRenderer::Warning(
            __FUNCTION__ "> "
            "Malformed semantic adapter table for mesh %s: Data type "
            "and/or renderer semantic name %s aren't supported by "
            "DX9.\n"
            "    Vertex declaration cannot be created.",
            m_pkMesh->GetName(), 
            kRendererSemantic);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiDX9MeshMaterialBindingPtr NiDX9MeshMaterialBinding::Create(
    NiMesh* pkMesh,
    const NiSemanticAdapterTable& kAdapterTable,
    bool bConvertBlendIndicesToD3DColor)
{
    NiDX9MeshMaterialBindingPtr spMMB = NiNew NiDX9MeshMaterialBinding();

    NiDataStreamRef* pkIndexRef = pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    NiDataStreamRef* pkDLRef = pkMesh->FindStreamRef(NiCommonSemantics::DISPLAYLIST());
    if (pkDLRef && !pkIndexRef)
    {
        NiRenderer::Warning(
            "%s> Failed to create a vertex declaration for "
            "mesh (pointer: 0x%X, name: %s) because the INDEX stream "
            "was removed during a platform-specific export.",
            __FUNCTION__,
            pkMesh, 
            (const char*) pkMesh->GetName());
        spMMB = NULL;
        return NULL;
    }

    DX9CreateBindingContext kContext;
    kContext.m_pkMesh = pkMesh;
    kContext.m_pkStreamsToSet = &spMMB->m_kStreamsToSet;

    // Init
    kContext.m_pkStreamsToSet->RemoveAll();

    if (!NiMeshMaterialBinding::CreateBinding<DX9CreateBindingContext>
        (kContext, kAdapterTable))
    {
        spMMB = NULL;
        return NULL;
    }

    // GeForce3 cards don't support F_UINT8_4 and so we may optionally
    // want to convert streams of blend indices to be D3DCOLOR instead
    // of integers.  DX9 will take care of this under the hood for us
    // and no data conversion is necessary at this point.  The only
    // difference is that the data needs to be interpreted as a color
    // in the shader itself.
    if (bConvertBlendIndicesToD3DColor)
    {
        for (unsigned int i = 0; i < kContext.m_uiCurrentElement; i++)
        {
            if (kContext.m_akVertexElements[i].Usage != 
                D3DDECLUSAGE_BLENDINDICES)
            {
                continue;
            }

            D3DDECLTYPE eD3DType = 
                (D3DDECLTYPE)kContext.m_akVertexElements[i].Type;
            if (eD3DType != D3DDECLTYPE_UBYTE4 &&
                eD3DType != D3DDECLTYPE_D3DCOLOR)
            {
                NiRenderer::Warning(
                    __FUNCTION__ "> "
                    "Element of semantic E_BLENDINDICES is being "
                    "converted to type F_NORMUINT8_4_BGRA, but is not "
                    "of expected type F_UINT8_4. ");
            }

            kContext.m_akVertexElements[i].Type = D3DDECLTYPE_D3DCOLOR;
        }
    }

    // Terminate list
    D3DVERTEXELEMENT9 akEnd[] = { D3DDECL_END() };
    kContext.m_akVertexElements[kContext.m_uiCurrentElement] = akEnd[0];

    // Enforce that Position0 is on the first set stream
    // This is required by fixed function pipeline.
    // Also solves having per-vertex data (not per instance) always set first.
    {
        unsigned int uiPos0Stream = 0;
        NIVERIFY(FindPosition0Stream(
            kContext.m_akVertexElements, uiPos0Stream));

        if (uiPos0Stream > 0)
        {
            NIASSERT(uiPos0Stream < spMMB->m_kStreamsToSet.GetSize());

            // swap the values of the streams so that 
            // wherever position0 is becomes stream 0, 
            // and what was was there replaces position0
            NIVERIFY(SwapStreamValues(
                kContext.m_akVertexElements,
                uiPos0Stream,
                0));

            NiUInt16 uiIndex0 = spMMB->m_kStreamsToSet.GetAt(0);
            NiUInt16 uiIndex1 = spMMB->m_kStreamsToSet.GetAt(uiPos0Stream);
            spMMB->m_kStreamsToSet.SetAt(0, uiIndex1);
            spMMB->m_kStreamsToSet.SetAt(uiPos0Stream, uiIndex0);
        }
    }

    HRESULT eResult = NiDX9Renderer::GetRenderer()->GetD3DDevice()->
        CreateVertexDeclaration(kContext.m_akVertexElements, 
        &spMMB->m_pkVertexDecl);

    if (FAILED(eResult))
    {
        NiRenderer::Warning(
            "%s> "
            "Failed to create vertex declaration for "
            "mesh (pointer: 0x%X, name: %s)."
            __FUNCTION__,
            pkMesh, 
            (const char*) pkMesh->GetName());
        return NULL;
    }

    // Set the index stream
    spMMB->m_pkIndexStreamRef = pkIndexRef;

    return spMMB;
}
//---------------------------------------------------------------------------
bool NiDX9MeshMaterialBinding::FindPosition0Stream( 
    D3DVERTEXELEMENT9* pkD3DVertexElements,
    unsigned int& uiPos0StreamIndex)
{
    NIASSERT(pkD3DVertexElements);
    D3DVERTEXELEMENT9 akEnd[] = { D3DDECL_END() };

    uiPos0StreamIndex = 0;
    while (pkD3DVertexElements->Stream != akEnd->Stream)
    {
        if (pkD3DVertexElements->Usage == D3DDECLUSAGE_POSITION
            &&
            pkD3DVertexElements->UsageIndex == 0)
        {
            uiPos0StreamIndex = pkD3DVertexElements->Stream;
            return true;
        }
        pkD3DVertexElements++;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiDX9MeshMaterialBinding::SwapStreamValues( 
    D3DVERTEXELEMENT9* pkD3DVertexElements,  
    unsigned int uiValue0,
    unsigned int uiValue1)
{
    NIASSERT(pkD3DVertexElements);
    D3DVERTEXELEMENT9 akEnd[] = { D3DDECL_END() };

    bool bReplacementMade = false;
    while (pkD3DVertexElements->Stream != akEnd->Stream)
    {
        int iPreviousValue = pkD3DVertexElements->Stream;
        if (iPreviousValue == (int)uiValue0)
        {
            pkD3DVertexElements->Stream = (WORD)uiValue1;
            bReplacementMade = true;
        }
        if (iPreviousValue == (int)uiValue1)
        {
            pkD3DVertexElements->Stream = (WORD)uiValue0;
            bReplacementMade = true;
        }
        pkD3DVertexElements++;
    }
    return bReplacementMade;
}
//---------------------------------------------------------------------------
LPDIRECT3DVERTEXDECLARATION9 NiDX9MeshMaterialBinding::
    GetD3DDeclaration() const
{
    return m_pkVertexDecl;
}
//---------------------------------------------------------------------------
NiUInt8 NiDX9MeshMaterialBinding::NumComponents_From_D3DDECLTYPE(
    D3DDECLTYPE eType)
{
/*
// Copied from d3d9types.h
// Declarations for _Type fields
//
typedef enum _D3DDECLTYPE
{
D3DDECLTYPE_FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
D3DDECLTYPE_FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
D3DDECLTYPE_FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
D3DDECLTYPE_FLOAT4    =  3,  // 4D float
D3DDECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. ran
// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
D3DDECLTYPE_UBYTE4    =  5,  // 4D unsigned byte
D3DDECLTYPE_SHORT2    =  6,  // 2D signed short expanded to (value, value, 0., 
D3DDECLTYPE_SHORT4    =  7,  // 4D signed short

// The following types are valid only with vertex shaders >= 2.0


D3DDECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 25
D3DDECLTYPE_SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/3
D3DDECLTYPE_SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/3
D3DDECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]
D3DDECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]
D3DDECLTYPE_UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value,
D3DDECLTYPE_DEC3N     = 14,  // 3D signed 10 10 10 format normalized and expand
D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (
D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
D3DDECLTYPE_UNUSED    = 17,  // When the type field in a decl is unused.
} D3DDECLTYPE;
*/

    NiUInt8 auiComponentCounts[] = 
    {
        1, // D3DDECLTYPE_FLOAT1
        2, // D3DDECLTYPE_FLOAT2
        3, // D3DDECLTYPE_FLOAT3
        4, // D3DDECLTYPE_FLOAT4
        4, // D3DDECLTYPE_D3DCOLOR
        4, // D3DDECLTYPE_UBYTE4
        2, // D3DDECLTYPE_SHORT2
        4, // D3DDECLTYPE_SHORT4
        4, // D3DDECLTYPE_UBYTE4N
        2, // D3DDECLTYPE_SHORT2N
        4, // D3DDECLTYPE_SHORT4N
        2, // D3DDECLTYPE_USHORT2N
        4, // D3DDECLTYPE_USHORT4N
        3, // D3DDECLTYPE_UDEC3
        3, // D3DDECLTYPE_DEC3N
        2, // D3DDECLTYPE_FLOAT16_2
        4  // D3DDECLTYPE_FLOAT16_4
    };

    return auiComponentCounts[(NiUInt32)eType];
}
//---------------------------------------------------------------------------
bool NiDX9MeshMaterialBinding::D3DType_From_Format(
    const NiDataStreamElement::Format eFormat, D3DDECLTYPE& eD3DType)
{
/*
// Copied from d3d9types.h
// Declarations for _Type fields
//
typedef enum _D3DDECLTYPE
{
D3DDECLTYPE_FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
D3DDECLTYPE_FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
D3DDECLTYPE_FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
D3DDECLTYPE_FLOAT4    =  3,  // 4D float
D3DDECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. ran
// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
D3DDECLTYPE_UBYTE4    =  5,  // 4D unsigned byte
D3DDECLTYPE_SHORT2    =  6,  // 2D signed short expanded to (value, value, 0., 
D3DDECLTYPE_SHORT4    =  7,  // 4D signed short

// The following types are valid only with vertex shaders >= 2.0


D3DDECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 25
D3DDECLTYPE_SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/3
D3DDECLTYPE_SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/3
D3DDECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]
D3DDECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]
D3DDECLTYPE_UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value,
D3DDECLTYPE_DEC3N     = 14,  // 3D signed 10 10 10 format normalized and expand
D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (
D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
D3DDECLTYPE_UNUSED    = 17,  // When the type field in a decl is unused.
} D3DDECLTYPE;
*/
    char acTable[NiDataStreamElement::F_TYPE_COUNT] =
    {
        -1,                     // F_INVALID
        -1,                     // F_INT8_1
        -1,                     // F_INT8_2
        -1,                     // F_INT8_3
        -1,                     // F_INT8_4
        -1,                     // F_UINT8_1
        -1,                     // F_UINT8_2 
        -1,                     // F_UINT8_3
        D3DDECLTYPE_UBYTE4,     // F_UINT8_4
        -1,                     // F_NORMINT8_1
        -1,                     // F_NORMINT8_2
        -1,                     // F_NORMINT8_3
        -1,                     // F_NORMINT8_4
        -1,                     // F_NORMUINT8_1
        -1,                     // F_NORMUINT8_2
        -1,                     // F_NORMUINT8_3
        D3DDECLTYPE_UBYTE4N,    // F_NORMUINT8_4
        -1,                     // F_INT16_1
        D3DDECLTYPE_SHORT2,     // F_INT16_2
        -1,                     // F_INT16_3
        D3DDECLTYPE_SHORT4,     // F_INT16_4
        -1,                     // F_UINT16_1       
        -1,                     // F_UINT16_2       
        -1,                     // F_UINT16_3       
        -1,                     // F_UINT16_4       
        -1,                     // F_NORMINT16_1    
        D3DDECLTYPE_SHORT2N,    // F_NORMINT16_2    
        -1,                     // F_NORMINT16_3    
        D3DDECLTYPE_SHORT4N,    // F_NORMINT16_4    
        -1,                     // F_NORMUINT16_1   
        D3DDECLTYPE_USHORT2N,   // F_NORMUINT16_2   
        -1,                     // F_NORMUINT16_3   
        D3DDECLTYPE_USHORT4N,   // F_NORMUINT16_4   
        -1,                     // F_INT32_1        
        -1,                     // F_INT32_2        
        -1,                     // F_INT32_3        
        -1,                     // F_INT32_4        
        -1,                     // F_UINT32_1       
        -1,                     // F_UINT32_2       
        -1,                     // F_UINT32_3       
        -1,                     // F_UINT32_4       
        -1,                     // F_NORMINT32_1    
        -1,                     // F_NORMINT32_2    
        -1,                     // F_NORMINT32_3    
        -1,                     // F_NORMINT32_4    
        -1,                     // F_NORMUINT32_1   
        -1,                     // F_NORMUINT32_2   
        -1,                     // F_NORMUINT32_3   
        -1,                     // F_NORMUINT32_4   
        -1,                     // F_FLOAT16_1                
        D3DDECLTYPE_FLOAT16_2,  // F_FLOAT16_2                
        -1,                     // F_FLOAT16_3                
        D3DDECLTYPE_FLOAT16_4,  // F_FLOAT16_4                
        D3DDECLTYPE_FLOAT1,     // F_FLOAT32_1                
        D3DDECLTYPE_FLOAT2,     // F_FLOAT32_2                
        D3DDECLTYPE_FLOAT3,     // F_FLOAT32_3                
        D3DDECLTYPE_FLOAT4,     // F_FLOAT32_4                
        D3DDECLTYPE_UDEC3,      // F_UINT_10_10_10_L1 (4th field is 1)
        D3DDECLTYPE_DEC3N,      // F_NORMINT_10_10_10_L1 (4th field is 1)
        -1,                     // F_NORMINT_11_11_10
        D3DDECLTYPE_D3DCOLOR,   // F_NORMUINT8_4_BGRA
        -1,                     // F_NORMINT_10_10_10_2
    };

    NiUInt8 uiIndex = NiDataStreamElement::IndexOf(eFormat);
    if (uiIndex >= NiDataStreamElement::F_TYPE_COUNT)
    {
        NiRenderer::Warning("Format %x: %s%s_%d not supported by renderer.",
            eFormat,
            NiDataStreamElement::IsNormalized(eFormat) ? "NORM" : "",
            NiDataStreamElement::GetTypeString(
            NiDataStreamElement::GetType(eFormat)), 
            NiDataStreamElement::GetComponentCount(eFormat));
        return false;
    }
    
    char cD3DTypeLookup = acTable[uiIndex];
    if (cD3DTypeLookup < 0)
    {
        NiRenderer::Warning("Format %x: %s%s_%d not supported by renderer.",
            eFormat,
            NiDataStreamElement::IsNormalized(eFormat) ? "NORM" : "",
            NiDataStreamElement::GetTypeString(
            NiDataStreamElement::GetType(eFormat)), 
            NiDataStreamElement::GetComponentCount(eFormat));
        return false;
    }

    eD3DType = (D3DDECLTYPE)cD3DTypeLookup;
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9MeshMaterialBinding::D3DUsage_From_Semantic(
    const NiFixedString& kSemantic, D3DDECLUSAGE& eD3DUsage)
{
    /*
        // Copied from d3d9types.h
        // Vertex element semantics
        //
        typedef enum _D3DDECLUSAGE
        {
            D3DDECLUSAGE_POSITION = 0,
            D3DDECLUSAGE_BLENDWEIGHT,   // 1
            D3DDECLUSAGE_BLENDINDICES,  // 2
            D3DDECLUSAGE_NORMAL,        // 3
            D3DDECLUSAGE_PSIZE,         // 4
            D3DDECLUSAGE_TEXCOORD,      // 5
            D3DDECLUSAGE_TANGENT,       // 6
            D3DDECLUSAGE_BINORMAL,      // 7
            D3DDECLUSAGE_TESSFACTOR,    // 8
            D3DDECLUSAGE_POSITIONT,     // 9
            D3DDECLUSAGE_COLOR,         // 10
            D3DDECLUSAGE_FOG,           // 11
            D3DDECLUSAGE_DEPTH,         // 12
            D3DDECLUSAGE_SAMPLE,        // 13
        } D3DDECLUSAGE;
    */
    char acTable[NiCommonSemantics::E__EnumerationCount] = 
    {
        -1,                         // E__Invalid
        D3DDECLUSAGE_POSITION,      // E_POSITION
        D3DDECLUSAGE_NORMAL,        // E_NORMAL
        D3DDECLUSAGE_BINORMAL,      // E_BINORMAL
        D3DDECLUSAGE_TANGENT,       // E_TANGENT
        D3DDECLUSAGE_TEXCOORD,      // E_TEXCOORD
        D3DDECLUSAGE_BLENDWEIGHT,   // E_BLENDWEIGHT
        D3DDECLUSAGE_BLENDINDICES,  // E_BLENDINDICES
        D3DDECLUSAGE_COLOR,         // E_COLOR
        D3DDECLUSAGE_PSIZE,         // E_PSIZE
        D3DDECLUSAGE_TESSFACTOR,    // E_TESSFACTOR
        D3DDECLUSAGE_DEPTH,         // E_DEPTH
        D3DDECLUSAGE_FOG,           // E_FOG
        D3DDECLUSAGE_POSITIONT,     // E_POSITIONT
        D3DDECLUSAGE_SAMPLE,        // E_SAMPLE
        -1,                         // E_DATASTREAM
        -1,                         // E_INDEX
        -1,                         // E_BONEMATRICES
        -1,                         // E_BONE_PALETTE
        -1,                         // E_UNUSED0
        D3DDECLUSAGE_POSITION,      // E_POSITION_BP
        D3DDECLUSAGE_NORMAL,        // E_NORMAL_BP
        D3DDECLUSAGE_BINORMAL,      // E_BINORMAL_BP
        D3DDECLUSAGE_TANGENT,       // E_TANGENT_BP
        -1,                         // E_MORPHWEIGHTS
        -1,                         // E_NORMALSHAREINDEX
        -1,                         // E_NORMALSHAREGROUP
        -1,                         // E_TRANSFORMS
        -1,                         // E_INSTANCETRANSFORMS
    };

    NiCommonSemantics::Enumeration e = 
        NiCommonSemantics::GetEnumeration(kSemantic);

    char cD3DUsageLookup = acTable[e];
    if (cD3DUsageLookup < 0)
        return false;

    eD3DUsage = (D3DDECLUSAGE)cD3DUsageLookup;
    return true;
}
//---------------------------------------------------------------------------
