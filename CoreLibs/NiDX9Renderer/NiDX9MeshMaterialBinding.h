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
#ifndef NIDX9MESHMATERIALBINDING_H
#define NIDX9MESHMATERIALBINDING_H

#include "NiDataStreamElement.h"
#include "NiFixedString.h"
#include "NiRefObject.h"

#include <NiDX9Headers.h>

class NiMesh;
class NiDataStreamRef;
class NiSemanticAdapterTable;

NiSmartPointer(NiDX9MeshMaterialBinding);

//
// Wraps functionality to build D3D Vertex Declarations for NiRenderObject
//
class NID3D_ENTRY NiDX9MeshMaterialBinding : public NiRefObject
{
public:
    virtual ~NiDX9MeshMaterialBinding();

    static NiDX9MeshMaterialBindingPtr Create(
        NiMesh* pkMesh,
        const NiSemanticAdapterTable& kAdapterTable,
        bool bConvertBlendIndicesToD3DColor);
    static bool FindPosition0Stream(
        D3DVERTEXELEMENT9* pkD3DVertexElements,
        unsigned int& uiPosition0StreamIndex);
    static bool SwapStreamValues(
        D3DVERTEXELEMENT9* pkD3DVertexElements,
        unsigned int uiValue0,
        unsigned int uiValue1);
    void Invalidate();

    LPDIRECT3DVERTEXDECLARATION9 GetD3DDeclaration() const;

    static NiUInt8 NumComponents_From_D3DDECLTYPE(D3DDECLTYPE eType);
    static bool D3DType_From_Format(const NiDataStreamElement::Format eFormat, 
        D3DDECLTYPE& eD3DType);
    static bool D3DUsage_From_Semantic(const NiFixedString& kSemantic, 
        D3DDECLUSAGE& eD3DUsage);

    LPDIRECT3DVERTEXDECLARATION9 m_pkVertexDecl;

    // Mapping from the D3D stream index that we will set 
    // to the NiDataStream set at that index.
    // e.g. pkD3DDevice->SetStreamSource(0, ...m_kStreamsToSet.GetAt(0), ...)
    NiTPrimitiveArray<NiUInt16> m_kStreamsToSet;
    const NiDataStreamRef* m_pkIndexStreamRef;

protected:
    NiDX9MeshMaterialBinding();
};

NiSmartPointer(NiDX9MeshMaterialBinding);

#endif  //#ifndef NIDX9MESHMATERIALBINDING_H
