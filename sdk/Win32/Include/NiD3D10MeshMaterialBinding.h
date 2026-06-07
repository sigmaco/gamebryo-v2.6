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

#ifndef NID3D10MESHMATERIALBINDING_H
#define NID3D10MESHMATERIALBINDING_H

#include <NiRefObject.h>

#include "NiD3D10Headers.h"
#include "NiD3D10RendererLibType.h"
#include "NiD3D10DataStream.h"

class NiRenderObject;
class NiSemanticAdapterTable;
class NiDataStreamRef;
NiSmartPointer(NiD3D10MeshMaterialBinding);

class NID3D10RENDERER_ENTRY NiD3D10MeshMaterialBinding : public NiRefObject
{
public:
    virtual ~NiD3D10MeshMaterialBinding();

    static NiD3D10MeshMaterialBindingPtr Create(NiRenderObject* pkMesh, 
        const NiSemanticAdapterTable& kAdapterTable);

    void UpdateInputLayout(void* pvInputSignature, 
        NiUInt32 uiInputSignatureSize);

    inline D3D10_INPUT_ELEMENT_DESC* GetInputElementDescArray() const;
    inline NiUInt32 GetInputElementDescCount() const;

    inline ID3D10InputLayout* GetInputLayout() const;

    inline void ReleaseCachedInputLayouts();
    inline void ReleaseElementArray();

    inline const NiDataStreamRef* GetIndexStreamRef() const;
    inline const NiUInt16* GetStreamsToSetArray() const;

    inline NiUInt16 GetLastValidStream() const;
    // *** end Emergent internal use only ***

protected:
    NiD3D10MeshMaterialBinding();

    bool FillElementDescArray(NiRenderObject* pkMesh, 
        const NiSemanticAdapterTable& kAdapterTable);

    D3D10_INPUT_ELEMENT_DESC* m_pkElementDescArray;

    class InputLayoutEntry : public NiMemObject
    {
    public:
        InputLayoutEntry();
        ~InputLayoutEntry();

        ID3D10InputLayout* m_pkInputLayout;
        void* m_pvInputSignature;
        InputLayoutEntry* m_pkNext;
    };

    InputLayoutEntry* m_pkInputLayouts;
    ID3D10InputLayout* m_pkCurrentInputLayout;

    const NiDataStreamRef* m_pkIndexStreamRef;
    NiUInt16 m_auiStreamsToSet[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

    NiUInt16 m_uiLastValidStream;
    NiUInt16 m_uiElementCount;
};
      
#include "NiD3D10MeshMaterialBinding.inl"

#endif  //#ifndef NID3D10MESHMATERIALBINDING_H