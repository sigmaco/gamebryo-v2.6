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

#ifndef NSBD3D10PASS_H
#define NSBD3D10PASS_H

#include "NiD3D10BinaryShaderLibLibType.h"
#include "NSBD3D10RenderStates.h"
#include "NSBD3D10ConstantMap.h"
#include "NSBD3D10UserDefinedDataSet.h"
#include <NiStreamOutSettings.h>

#include <NiTArray.h>

class NSBD3D10TextureStage;
class NSBD3D10Texture;
class NSBD3D10StateGroup;
class NiD3D10Pass;

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10Pass : public NiMemObject
{
public:
    NSBD3D10Pass();
    ~NSBD3D10Pass();

    // Name
    inline const char* GetName() const;
    void SetName(const char* pcName);

    // RenderStateGroup
    NSBD3D10StateGroup* GetRenderStateGroup();

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
    NSBD3D10ConstantMap* GetVertexConstantMap(unsigned int uiIndex);
    NSBD3D10ConstantMap* GetGeometryConstantMap(unsigned int uiIndex);
    NSBD3D10ConstantMap* GetPixelConstantMap(unsigned int uiIndex);

    // Stages
    unsigned int GetStageCount();
    NSBD3D10TextureStage* GetStage(unsigned int uiIndex, bool bCreate = true);

    // Textures
    unsigned int GetTextureCount();
    NSBD3D10Texture* GetTexture(unsigned int uiIndex, bool bCreate = true);

    // User defined data set
    inline NSBD3D10UserDefinedDataSet* GetUserDefinedDataSet();
    inline void SetUserDefinedDataSet(NSBD3D10UserDefinedDataSet* pkUDDSet);

    // StreamOutSettings
    inline NiStreamOutSettings& GetStreamOutSettings();

    // *** begin Emergent internal use only
    bool SetupNiD3D10Pass(NiD3D10Pass& kPass, NiShaderDesc* pkShaderDesc);

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
    NSBD3D10UserDefinedDataSetPtr m_spUserDefinedDataSet;

    // 'Local' render state settings for the shader
    NSBD3D10StateGroup* m_pkRenderStateGroup;
    // 'Local' vertex shader mapping for the shader
    NiTPrimitiveSet<NSBD3D10ConstantMap*> m_kVertexShaderConstantMaps;
    // 'Local' geometry shader mapping for the shader
    NiTPrimitiveSet<NSBD3D10ConstantMap*> m_kGeometryShaderConstantMaps;
    // 'Local' pixel shader mapping for the shader
    NiTPrimitiveSet<NSBD3D10ConstantMap*> m_kPixelShaderConstantMaps;

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

    NiTPrimitiveArray<NSBD3D10TextureStage*> m_akStages;
    NiTPrimitiveArray<NSBD3D10Texture*> m_akTextures;
};

#include "NSBD3D10Pass.inl"

#endif  // NSBD3D10PASS_H
