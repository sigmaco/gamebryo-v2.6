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
//---------------------------------------------------------------------------
#ifndef NIDX9LIGHTMANAGER_H
#define NIDX9LIGHTMANAGER_H

#include "NiDX9Headers.h"
#include "NiDX9RendererLibType.h"
#include <NiTPointerMap.h>
#include <NiTPointerList.h>
#include <NiTexturingProperty.h>
#include <NiVertexColorProperty.h>

NiSmartPointer(NiLight);

class NiDX9RenderState;
class NiDynamicEffectState;

class NIDX9RENDERER_ENTRY NiDX9LightManager : public NiMemObject
{
public:
    NiDX9LightManager(NiDX9RenderState* pkRenderState,
        LPDIRECT3DDEVICE9 pkD3DDevice9);
    ~NiDX9LightManager();

    void NextFrame();
    void SetState(const NiDynamicEffectState* pkState, 
        const NiTexturingProperty* pkTex, 
        const NiVertexColorProperty* pkColor);

    void RemoveLight(NiLight& kLight);

    void ReleaseLights();

    // this value is not valid for a set of states until after the
    // call to UpdateRenderStates() is made
    bool IsLit() const;

    // *** begin Emergent internal use only ***
    void InitRenderModes();
    // *** end Emergent internal use only ***

protected:
    class LightEntry : public NiMemObject
    {
    public:
        LightEntry(const NiLight& kLight, int iIndex);

        // returns true if the material has changed
        bool Update(const NiLight& kLight);

        D3DLIGHT9* GetD3DLight();
        void SetIndex(int iIndex);
        int GetIndex() const;

        void SetRemoval(bool bRemove);
        bool GetRemoval() const;

        void SetActive(bool bActive);
        bool GetActive() const;

    protected:
        D3DLIGHT9 m_kD3DLight;
        unsigned int m_uiRevID;
        int m_iIndex;
        bool m_bNeedsRemoval;
        bool m_bActive;

        static const float ms_fRangeMax;
    };

    void UpdateLights(const NiDynamicEffectState* pkState);

    unsigned int GetNextAvailableIndex();
    inline void MarkIndexAsAvailable(unsigned int uiIndex);

    NiTPointerMap<NiLight*, LightEntry*> m_kLights;

    NiTPointerList<NiLight*> m_kDeviceLights;

    LPDIRECT3DDEVICE9 m_pkD3DDevice9;
    NiDX9RenderState* m_pkRenderState;

    const NiDynamicEffectState* const m_pkInvalidState;
    const NiDynamicEffectState* m_pkLastState;
    bool m_bLastAmbientDiffuse;

    // Vertex Color Members
    bool m_bAmbientDiffuse;
    NiTexturingProperty::ApplyMode m_eApply;

    NiVertexColorProperty::SourceVertexMode m_eSourceMode;

    // Since some cards only support 0-4095 for indices, we need to track 
    // the valid indices. This will prevent overflows when loading level 
    // after level. 
    enum
    {
        LIGHTINDEXARRAYSIZE = 128
    };

    unsigned int m_uiNextAvailableIndex;
    unsigned int m_auiValidIndices[LIGHTINDEXARRAYSIZE];
};

#include "NiDX9LightManager.inl"

#endif  //#ifndef NIDX9LIGHTMANAGER_H
