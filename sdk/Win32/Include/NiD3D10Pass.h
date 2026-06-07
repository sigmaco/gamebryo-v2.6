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

#ifndef NID3D10PASS_H
#define NID3D10PASS_H

#include "NiRefObject.h"
#include "NiSmartPointer.h"

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"
#include "NiD3D10ShaderConstantMap.h"
#include "NiStreamOutSettings.h"

#include <NiTArray.h>

class NiBound;
class NiDynamicEffectState;
class NiRenderObject;
class NiPropertyState;
class NiTransform;

#include "NiD3D10RenderStateGroup.h"
#include "NiD3D10VertexShader.h"
#include "NiD3D10GeometryShader.h"
#include "NiD3D10PixelShader.h"

NiSmartPointer(NiD3D10Pass);

class NID3D10RENDERER_ENTRY NiD3D10Pass : public NiRefObject
{
public:
    NiD3D10Pass();
    virtual ~NiD3D10Pass();

    static bool CreateNewPass(NiD3D10PassPtr& spNewPass);

    // Name
    inline const char* GetName() const;
    inline void SetName(char* pcName);

    // Render State Group
    inline NiD3D10RenderStateGroup* GetRenderStateGroup() const;
    inline void SetRenderStateGroup(NiD3D10RenderStateGroup* pkRenderStateGroup);

    // Vertex shader
    inline unsigned int GetVertexConstantMapCount() const;
    inline NiD3D10ShaderConstantMap* GetVertexConstantMap(unsigned int uiIndex) const;
    inline void SetVertexConstantMap(unsigned int uiIndex, 
        NiD3D10ShaderConstantMap* pkSCMVertex);
    inline NiD3D10VertexShader* GetVertexShader() const;
    void SetVertexShader(NiD3D10VertexShader* pkVertexShader);
    NiTexture* GetVertexTexture(unsigned int uiTexture) const;
    void SetVertexTexture(unsigned int uiTexture, NiTexture* pkTexture);

    inline const char* GetVertexShaderProgramFileName() const;
    inline void SetVertexShaderProgramFileName(const char* pcProgramFile);
    inline const char* GetVertexShaderProgramEntryPoint() const;
    inline void SetVertexShaderProgramEntryPoint(const char* pcEntryPoint);
    inline const char* GetVertexShaderProgramShaderTarget() const;
    inline void SetVertexShaderProgramShaderTarget(const char* pcShaderTarget);

    // Geometry shader
    unsigned int GetGeometryConstantMapCount() const;
    NiD3D10ShaderConstantMap* GetGeometryConstantMap(unsigned int uiIndex)
        const;
    void SetGeometryConstantMap(unsigned int uiIndex, 
        NiD3D10ShaderConstantMap* pkSCMGeometry);
    NiD3D10GeometryShader* GetGeometryShader() const;
    void SetGeometryShader(NiD3D10GeometryShader* pkGeometryShader);
    NiTexture* GetGeometryTexture(unsigned int uiTexture) const;
    void SetGeometryTexture(unsigned int uiTexture, NiTexture* pkTexture);
    inline const char* GetGeometryShaderProgramFileName() const;
    inline void SetGeometryShaderProgramFileName(const char* pcProgramFile);
    inline const char* GetGeometryShaderProgramEntryPoint() const;
    inline void SetGeometryShaderProgramEntryPoint(const char* pcEntryPoint);
    inline const char* GetGeometryShaderProgramShaderTarget() const;
    inline void SetGeometryShaderProgramShaderTarget(const char* pcShaderTarget);

    // Pixel shader
    unsigned int GetPixelConstantMapCount() const;
    NiD3D10ShaderConstantMap* GetPixelConstantMap(unsigned int uiIndex) const;
    void SetPixelConstantMap(unsigned int uiIndex, 
        NiD3D10ShaderConstantMap* pkSCMPixel);
    NiD3D10PixelShader* GetPixelShader() const;
    void SetPixelShader(NiD3D10PixelShader* pkPixelShader);
    NiTexture* GetPixelTexture(unsigned int uiTexture) const;
    void SetPixelTexture(unsigned int uiTexture, NiTexture* pkTexture);
    inline const char* GetPixelShaderProgramFileName() const;
    inline void SetPixelShaderProgramFileName(const char* pcProgramFile);
    inline const char* GetPixelShaderProgramEntryPoint() const;
    inline void SetPixelShaderProgramEntryPoint(const char* pcEntryPoint);
    inline const char* GetPixelShaderProgramShaderTarget() const;
    inline void SetPixelShaderProgramShaderTarget(const char* pcShaderTarget);

    bool LinkUpConstantMaps();
    bool LinkUpShaderResources();


    //*** Pipeline functions
    // Set the shader constants
    virtual unsigned int ApplyShaderPrograms(
        const NiRenderCallContext& kRCC);

    virtual unsigned int ApplyShaderConstants(const NiRenderCallContext& kRCC);

    virtual unsigned int ApplyTextures(const NiRenderCallContext& kRCC);

    // Setup the pass for rendering the given object
    virtual unsigned int SetupRenderingPass(const NiRenderCallContext& kRCC);

    // End the pass
    virtual unsigned int PostProcessRenderingPass(unsigned int uiPass = 0);

    // Support for automatically configuring Gamebryo textures
    enum GamebryoMaps
    {
        GB_MAP_IGNORE       = 0x00000000,
        // NiTexturingProperty Map to use
        GB_MAP_NONE         = 0x00100000,
        GB_MAP_BASE         = 0x00200000,
        GB_MAP_DARK         = 0x00300000,
        GB_MAP_DETAIL       = 0x00400000,
        GB_MAP_GLOSS        = 0x00500000,
        GB_MAP_GLOW         = 0x00600000,
        GB_MAP_BUMP         = 0x00700000,
        GB_MAP_NORMAL       = 0x00800000,
        GB_MAP_PARALLAX     = 0x00900000,
        GB_MAP_DECAL        = 0x00A00000,
        GB_MAP_SHADER       = 0x00B00000,
        GB_MAP_TYPEMASK     = 0x00F00000,
        // Index for decals and shader maps
        GB_MAP_INDEX_MASK   = 0x0000FFFF
    };

    enum ObjectTextureFlags
    {
        OTF_IGNORE         = 0x0000,
        OTF_INDEX_MASK     = 0x00FF,
        OTF_TYPE_MASK      = 0xFF00,
        OTF_TYPE_SHIFT     = 8
    };

    void ResetTexturesAndSamplers();

    unsigned int GetFirstUnassignedTexture();
    unsigned int GetActiveTextureCount() const;
    unsigned int GetActiveSamplerCount() const;

    // Texture settings
    bool SetTextureName(unsigned int uiTexture, 
        const NiFixedString& kTextureName);
    bool SetGamebryoMap(unsigned int uiTexture, 
        const NiFixedString& kTextureName, unsigned int uiGBMap, 
        unsigned short usInstance, unsigned short usObjectFlags);
    unsigned int GetGamebryoMapFlags(unsigned int uiTexture) const;
    unsigned short GetObjectTextureFlags(unsigned int uiTexture) const;

    // Sampler settings
    bool SetSamplerName(unsigned int uiSampler, 
        const NiFixedString& kSamplerName);

    // StreamOutSettings
    inline void SetStreamOutSettings(const NiStreamOutSettings& kStreamOutSettings);
    inline NiStreamOutSettings& GetStreamOutSettings();

    // *** begin Emergent internal use only

    ID3D10Blob* GetInputSignature() const;
    void SetInputSignature(ID3D10Blob* pkInputSignature);

    // *** begin Emergent internal use only

protected:


    enum 
    {
        MAX_NAME_LENGTH = 16
    };

    char m_acName[MAX_NAME_LENGTH];

    NiD3D10RenderStateGroupPtr m_spRenderStateGroup;

    NiTObjectArray<NiD3D10ShaderConstantMapPtr> m_kVertexShaderConstantMaps;
    NiTObjectArray<NiD3D10ShaderConstantMapPtr> m_kGeometryShaderConstantMaps;
    NiTObjectArray<NiD3D10ShaderConstantMapPtr> m_kPixelShaderConstantMaps;

    NiD3D10VertexShaderPtr m_spVertexShader;
    NiD3D10GeometryShaderPtr m_spGeometryShader;
    NiD3D10PixelShaderPtr m_spPixelShader;

    ID3D10Blob* m_pkInputSignature;

    // NiD3D10TextureData will contain an ID3D10ShaderResourceView*
    NiTexturePtr m_aspVertexTextures[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    NiTexturePtr m_aspGeometryTextures[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    NiTexturePtr m_aspPixelTextures[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    char* m_pcVertexShaderProgramFile;
    char* m_pcVertexShaderEntryPoint;
    char* m_pcVertexShaderTarget;

    char* m_pcGeometryShaderProgramFile;
    char* m_pcGeometryShaderEntryPoint;
    char* m_pcGeometryShaderTarget;

    char* m_pcPixelShaderProgramFile;
    char* m_pcPixelShaderEntryPoint;
    char* m_pcPixelShaderTarget;

    // Auto-configuration
    unsigned int m_uiActiveTextureCount;
    unsigned int m_uiActiveSamplerCount;

    NiFixedString m_akTextureNames[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    NiFixedString m_akSamplerNames[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    unsigned char m_aucShaderResourceRemapping
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    unsigned char m_aucSamplerRemapping
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    // Per-texture settings
    unsigned int m_auiGamebryoMapFlags[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    unsigned short m_ausObjectTextureFlags
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    // Stream Out settings for the pass
    NiStreamOutSettings m_kStreamOutSettings;

    bool m_bConstantMapsLinked;
    bool m_bShaderResourcesLinked;
};

#include "NiD3D10Pass.inl"

#endif  //#ifndef NID3D10PASS_H
