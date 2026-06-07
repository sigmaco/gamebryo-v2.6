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

#ifndef NSBPASS_H
#define NSBPASS_H

#include "NiBinaryShaderLibLibType.h"
#include "NSBRenderStates.h"
#include "NSBConstantMap.h"
#include "NSBUserDefinedDataSet.h"
#include "NSBUtility.h"
#include <NiStreamOutSettings.h>

#include <NiTArray.h>

class NSBTextureStage;
class NSBTexture;
class NSBStateGroup;

class NIBINARYSHADERLIB_ENTRY NSBPass : public NiMemObject
{
public:
    NSBPass();
    ~NSBPass();

    // Name
    inline const char* GetName() const;
    void SetName(const char* pcName);

    // RenderStateGroup
    NSBStateGroup* GetRenderStateGroup();

    // Shader Programs
    inline const char* GetShaderProgramFile(NiSystemDesc::RendererID eRenderer,
        NiGPUProgram::ProgramType eType) const;
    inline void SetShaderProgramFile(const char* pcProgramFileName, 
        NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType);
    inline const char* GetShaderProgramEntryPoint(
        NiSystemDesc::RendererID eRenderer,
        NiGPUProgram::ProgramType eType) const;
    inline void SetShaderProgramEntryPoint(const char* pcEntryPoint, 
        NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType);
    inline const char* GetShaderProgramShaderTarget(
        NiSystemDesc::RendererID eRenderer, 
        NiGPUProgram::ProgramType eType) const;
    inline void SetShaderProgramShaderTarget(const char* pcShaderTarget, 
        NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType);
    /// Is a shader program of this type present for any renderer?
    bool GetShaderPresent(NiGPUProgram::ProgramType eType) const;

    inline bool GetSoftwareVertexProcessing() const;
    inline void SetSoftwareVertexProcessing(bool bSWVertexProcessing);

    // Constant Maps
    inline unsigned int GetVertexConstantMapCount();
    inline unsigned int GetGeometryConstantMapCount();
    inline unsigned int GetPixelConstantMapCount();
    NSBConstantMap* GetVertexConstantMap(unsigned int uiIndex);
    NSBConstantMap* GetGeometryConstantMap(unsigned int uiIndex);
    NSBConstantMap* GetPixelConstantMap(unsigned int uiIndex);

    // Stages
    unsigned int GetStageCount();
    NSBTextureStage* GetStage(unsigned int uiIndex, bool bCreate = true);

    // Textures
    unsigned int GetTextureCount();
    NSBTexture* GetTexture(unsigned int uiIndex, bool bCreate = true);

    // User defined data set
    inline NSBUserDefinedDataSet* GetUserDefinedDataSet();
    inline void SetUserDefinedDataSet(NSBUserDefinedDataSet* pkUDDSet);

    // StreamOutSettings
    inline NiStreamOutSettings& GetStreamOutSettings();

    // *** begin Emergent internal use only
    bool SetupShaderPass(NiPlatformShaderPass& kPass,
        NiShaderDesc* pkShaderDesc);

    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    bool SaveBinaryShaderProgram(NiBinaryStream& kStream,
        NiGPUProgram::ProgramType eType);
    bool SaveBinaryStages(NiBinaryStream& kStream);
    bool SaveBinaryTextures(NiBinaryStream& kStream);

    bool LoadBinaryVertexShaderProgram(NiBinaryStream& kStream);
    bool LoadBinaryGeometryShaderProgram(NiBinaryStream& kStream);
    bool LoadBinaryPixelShaderProgram(NiBinaryStream& kStream);
    bool LoadBinaryShaderProgram(NiBinaryStream& kStream, 
        NiGPUProgram::ProgramType eType);
    bool LoadBinaryStages(NiBinaryStream& kStream);
    bool LoadBinaryTextures(NiBinaryStream& kStream);

    char* m_pcName;

    // User defined data set
    NSBUserDefinedDataSetPtr m_spUserDefinedDataSet;

    // 'Local' render state settings for the shader
    NSBStateGroup* m_pkRenderStateGroup;
    // 'Local' vertex shader mapping for the shader
    NiTPrimitiveSet<NSBConstantMap*> m_kVertexShaderConstantMaps;
    // 'Local' geometry shader mapping for the shader
    NiTPrimitiveSet<NSBConstantMap*> m_kGeometryShaderConstantMaps;
    // 'Local' pixel shader mapping for the shader
    NiTPrimitiveSet<NSBConstantMap*> m_kPixelShaderConstantMaps;

    struct ShaderProgramInfo
    {
        ShaderProgramInfo();
        ~ShaderProgramInfo();

        char* m_pcProgramFile;
        char* m_pcEntryPoint;
        char* m_pcTarget;
    };
    
    ShaderProgramInfo 
        m_akInfo[NiSystemDesc::RENDERER_NUM][NiGPUProgram::PROGRAM_MAX];

    bool m_bSoftwareVP;

    // Stream Out settings for the implementation
    NiStreamOutSettings m_kStreamOutSettings;

    NiTPrimitiveArray<NSBTextureStage*> m_akStages;
    NiTPrimitiveArray<NSBTexture*> m_akTextures;
};

#include "NSBPass.inl"

#endif  //NSBPASS_H
