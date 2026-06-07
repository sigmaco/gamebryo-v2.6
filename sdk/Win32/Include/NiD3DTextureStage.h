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

#ifndef NID3DTEXTURESTAGE_H
#define NID3DTEXTURESTAGE_H

#include "NiD3DDefines.h"
#include "NiDX9RenderState.h"
#include "NiD3DTextureStageGroup.h"
#include "NiGlobalConstantEntry.h"
#include <NiTexturingProperty.h>
#include <NiCriticalSection.h>
#include <NiTextureStage.h>

NiSmartPointer(NiD3DTextureStage);

class NiD3DPass;

// NiD3DTextureStage
// The texture stage class represents a single stage within a texture pass.
// 

class NID3D_ENTRY NiD3DTextureStage : public NiTextureStage
{
protected:
    // Do not set these to anything. Use them only for accessing required
    // functionality. You may assume they will ALWAYS be valid.
    static D3DDevicePtr ms_pkD3DDevice;
    static NiD3DRenderer* ms_pkD3DRenderer;
    static NiD3DRenderState* ms_pkD3DRenderState;

    // *** begin Emergent internal use only ***
    static void SetD3DDevice(D3DDevicePtr pkD3DDevice);
    static void SetD3DRenderState(NiD3DRenderState* pkRS);

public:
    static void SetD3DRenderer(NiD3DRenderer* pkD3DRenderer);
    // *** end Emergent internal use only ***

public:
    NiD3DTextureStage();
    virtual ~NiD3DTextureStage();

    static NiD3DTextureStagePtr CreateNewStage();

    //*** Access functions
    inline unsigned int GetStage();
    inline NiTexture* GetTexture();
    inline unsigned int GetTextureFlags();
    inline unsigned int GetTextureTransformFlags();
    inline bool GetUseIndexFromMap();
    inline unsigned short GetObjTextureFlags() const;

    inline NiD3DTextureStageGroup* GetTextureStageGroup();

    inline D3DMATRIX& GetTextureTransformation();

    inline void SetStage(unsigned int uiStage);
    inline void SetTexture(NiTexture* pkTexture);
    void SetTextureTransformFlags(unsigned int uiFlags, 
        const char* pszGlobalName = 0);
    inline void SetNDLMap(unsigned int uiNDLMap);
    inline void SetDecalMap(unsigned int uiDecal);
    inline void SetShaderMap(unsigned int uiShaderMap);
    inline void SetUseIndexFromMap(bool bUse);

    inline void SetStageState(unsigned int uiState, unsigned int uiValue,
        bool bSave = false);
    inline void SetSamplerState(unsigned int uiState, unsigned int uiValue,
        bool bSave = false);

    inline void RemoveStageState(unsigned int uiState);
    inline void RemoveSamplerState(unsigned int uiState);

    inline void SetTextureTransformation(D3DMATRIX& kD3DMat);
    inline void SetTextureTransformation(bool bUseTransform);
    inline bool IsTextureTransformation() const;

    // These will override the values in the NiD3DTextureStageGroup,
    // not necessarily on the device
    void SetClampMode(NiTexturingProperty::ClampMode eClamp);
    void SetFilterMode(NiTexturingProperty::FilterMode eFilter);
    void SetMaxAnisotropy(unsigned short usMaxAnisotropyValue);

    //*** Pipeline Functions
    bool ConfigureStage();
    void DisableStage();
    static void DisableStage(unsigned int uiStage);

    // Apply functions
    void ApplyTexture();
    void ApplyTextureTransform();

    // *** begin Emergent internal use only ***
    inline void SetTextureFlags(unsigned int uiFlags);
    inline void SetObjTextureFlags(unsigned short usObjTextureFlags);

    inline void SetRendererOwned();
    inline bool IsRendererOwned() const;

    static void InitializePools();
    static void ShutdownPools();

    // For use with NiSmartPointers
    virtual void DeleteThis();

    inline unsigned int GetTexCoordIndex() const;
    inline bool GetSaveTexCoordIndex() const;
    // *** end Emergent internal use only ***

protected:
    bool CalculateTransformationMatrix(unsigned int uiCalculationMethod, 
        const D3DMATRIX* pkD3DMatSource);

    void ReturnStageToPool();

    // This will change the values on the device
    void ModifyFilterMode(bool bMipmap, bool bAllowMiplerp);
    void ModifyClampMode(bool bNonPow2);

    unsigned int m_uiStage;
    NiTexture* m_pkTexture;       //  Texture image (if used)
    unsigned int m_uiTextureFlags;

    NiD3DTextureStageGroup* m_pkTextureStageGroup;

    // Texture transformation members
    unsigned int m_uiTextureTransformFlags;
    NiGlobalConstantEntry* m_pkGlobalEntry;
    D3DMATRIX m_kTextureTransformation;
    static D3DXMATRIX ms_kCalculatedTextureTransformation;

    // Object texture flags.
    unsigned short m_usObjTextureFlags;

    // Internal flags
    bool m_bTextureTransform;
    bool m_bRendererOwned;
    unsigned int m_uiRefCount;

    static NiCriticalSection ms_kTextureStagePoolCriticalSection;
    static NiTObjectPool<NiD3DTextureStage>* ms_pkTextureStagePool;
};

#include "NiD3DTextureStage.inl"

#endif  //#ifndef NID3DTEXTURESTAGE_H
