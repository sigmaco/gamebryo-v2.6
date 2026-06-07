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
#ifndef NIDX9MATERIALMANAGER_H
#define NIDX9MATERIALMANAGER_H

#include "NiDX9Headers.h"
#include "NiDX9RendererLibType.h"

class NiMaterialProperty;

class NIDX9RENDERER_ENTRY NiDX9MaterialManager : public NiMemObject
{
public:
    NiDX9MaterialManager();
    ~NiDX9MaterialManager();

    D3DMATERIAL9* GetCurrentMaterial();
    void SetCurrentMaterial(NiMaterialProperty& kMat);

    void UpdateToDevice(LPDIRECT3DDEVICE9 pkD3DDevice9);
    void PurgeMaterial(NiMaterialProperty* pkMaterial);

    // *** begin Emergent internal use only ***

    void Initialize();

    // *** end Emergent internal use only ***

protected:
    class MaterialEntry : public NiMemObject
    {
    public:
        MaterialEntry(const NiMaterialProperty& kMat);

        // returns true if the material has changed
        bool Update(const NiMaterialProperty& kMat);

        D3DMATERIAL9* GetD3DMaterial() { return &m_kD3DMat; }

    protected:
        D3DMATERIAL9 m_kD3DMat;
        unsigned int m_uiRevID;
    };

    MaterialEntry* m_pkCurrentMaterial;
    bool m_bChanged;
};

#endif  //#ifndef NIDX9MATERIALMANAGER_H
