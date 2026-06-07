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

#ifndef NSBD3D10SHADER_H
#define NSBD3D10SHADER_H

#include "NiD3D10BinaryShaderLibLibType.h"
#include "NSBD3D10AttributeTable.h"
#include "NSBD3D10ObjectTable.h"
#include "NSBD3D10PackingDef.h"
#include "NSBD3D10Implementation.h"
#include "NSBD3D10UserDefinedDataSet.h"
#include <NiOutputStreamDescriptor.h>

#include <NiRefObject.h>
#include <NiSmartPointer.h>

class NiD3D10BinaryShader;
NiSmartPointer(NiShaderDesc);

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10Shader : public NiRefObject
{
protected:
    enum
    {
        MAGIC_NUMBER = 0x0062736E,  // NSB0
        // MMDDYY - version number change information
        // ------   -----------------------------------------------------------
        // 041003 - Bumped version number to 1.1 to reflect addition of
        //          UsesNiLightState requirement. (Shipped in 1.0.1)
        // 101403 - Bumped version to 1.2 to add shader targets, shader 
        //          entry points, and variable names
        // 102303 - Bumped version to 1.3 to add software vertex processing
        // 033004 - Bumped version to 1.4 to add streaming of 
        //          platform-specific constant map entries
        // 033104 - Bumped version to 1.5 to add streaming of a
        //          user-defined class name for implementations.
        // 010404 - Bumped version to 1.6 to add UserDefinedDataBlocks
        // 020404 - Bumped version to 1.7 to improve software vertex processing
        // 031505 - Bumped version to 1.8 to remove DX8 support
        // 031605 - Bumped version to 1.9 to add obj texture flags
        // 032805 - Bumped version to 1.10 to make NSB files endian-aware
        // 020806 - Bumped version to 1.11 to allow for NBT uv source spec.
        // 050106 - Bumped version to 1.12 to add PS3 support
        // 120106 - Bumped version to 1.13 to add D3D10 and geometry shader
        //          support.
        // 011607 - Bumped version to 1.14 to add separate NSBTexture objects.
        // 031907 - Bumped version to 1.15 to add shadow light object effect
        //          types.
        // 010208 - Bumped version to 2.00 to add semantic adapter table
        //          entries.
        // 033108 - Bumped version to 2.1 to add platform-specific shader
        //          program entries.
        // 061608 - Bumped version to 2.2 to add stream out capabilities
        NSB_VERSION     = 0x00020002    // 2.2
    };

public:
    NSBD3D10Shader();
    ~NSBD3D10Shader();

    // This will allow for customers to derived their own 
    // NiD3D10BinaryShader-based classes, allowing them to extend the 
    // functionality while still gaining the benefit of having the
    // default NSF parsing do the file handling.
    //
    // This is the prototype of the CreateNiD3D10BinaryShaderFunction
    typedef NiD3D10BinaryShader* (*CREATENID3D10BINARYSHADER)(
        const char* pcClassName);

    static CREATENID3D10BINARYSHADER SetCreateNiD3D10BinaryShaderCallback(
        CREATENID3D10BINARYSHADER pfnCallback);
    static CREATENID3D10BINARYSHADER GetCreateNiD3D10BinaryShaderCallback();
    
    static NiD3D10BinaryShader* DefaultCreateNiD3D10BinaryShader(
        const char* pcClassName);

    // Name
    inline const char* GetName();
    inline const char* GetDescription();

    // Shader Versions
    inline unsigned int GetMinVertexShaderVersion();
    inline unsigned int GetMaxVertexShaderVersion();
    unsigned int GetMinGeometryShaderVersion();
    unsigned int GetMaxGeometryShaderVersion();
    inline unsigned int GetMinPixelShaderVersion();
    inline unsigned int GetMaxPixelShaderVersion();
    inline unsigned int GetMinUserVersion();
    inline unsigned int GetMaxUserVersion();
    inline unsigned int GetPlatform();

    // Access to the attribute table
    inline NSBD3D10AttributeTable* GetGlobalAttributeTable();
    inline NSBD3D10AttributeTable* GetAttributeTable();

    // Access to the object table
    inline NSBD3D10ObjectTable* GetObjectTable();

    // The packing definitions
    unsigned int GetPackingDefCount();
    NSBD3D10PackingDef* GetPackingDef(const char* pcName,
        bool bCreate = false);

    // The StreamOutputDescriptors
    inline void AddOutputStreamDescriptor(
        const NiOutputStreamDescriptor& kDescriptor);
    inline void ClearOutputStreamDescriptors();
    inline const NiOutputStreamDescriptorArray& GetOutputStreamDescriptors();

    // The implementations
    unsigned int GetImplementationCount();
    NSBD3D10Implementation* GetImplementation(const char* pcName, 
        bool bCreate = false, unsigned int uiNextIndex = 0);
    NSBD3D10Implementation* GetImplementationByName(const char* pcName);
    NSBD3D10Implementation* GetImplementationByIndex(unsigned int uiIndex);

    inline void SetMinVertexShaderVersionRequest(unsigned int uiVersion);
    inline void SetMaxVertexShaderVersionRequest(unsigned int uiVersion);
    void SetMinGeometryShaderVersionRequest(unsigned int uiVersion);
    void SetMaxGeometryShaderVersionRequest(unsigned int uiVersion);
    inline void SetMinPixelShaderVersionRequest(unsigned int uiVersion);
    inline void SetMaxPixelShaderVersionRequest(unsigned int uiVersion);
    inline void SetMinUserVersionRequest(unsigned int uiVersion);
    inline void SetMaxUserVersionRequest(unsigned int uiVersion);
    inline void SetPlatformRequest(unsigned int uiPlatform);
    
    // Get the BINARY shader
    NiD3D10BinaryShader* GetD3D10Shader(
        unsigned int uiImplementation = NiShader::DEFAULT_IMPLEMENTATION);

    // *** begin Emergent internal use only
    // User defined data set
    inline NSBD3D10UserDefinedDataSet* GetUserDefinedDataSet();
    inline void SetUserDefinedDataSet(NSBD3D10UserDefinedDataSet* pkUDDSet);

    NiShaderDesc* GetShaderDesc();

    // These functions are used during the creation
    void SetName(const char* pcName);
    void SetDescription(const char* pcDescription);

    inline void AddVertexShaderVersion(unsigned int uiVersion);
    inline void AddGeometryShaderVersion(unsigned int uiVersion);
    inline void AddPixelShaderVersion(unsigned int uiVersion);
    inline void AddUserVersion(unsigned int uiVersion);
    inline void AddPlatform(unsigned int uiPlatformFlag);

    // By default, NSB shaders are saved to match the endianness of the
    // platform they are written out on (and can be loaded in on any
    // platform.)
    bool Save(const char* pcFilename, bool bSwapEndian = false);
    bool SaveBinary(NiBinaryStream& kStream, bool bSwapEndian = false);
    bool Load(const char* pcFilename);
    bool LoadBinary(NiBinaryStream& kStream);

    static unsigned int GetReadVersion();

#if defined(NIDEBUG)
    void Dump();
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    NSBD3D10Implementation* GetBestImplementation();

    enum ValidityFlag
    {
        INVALID = 0,
        VALID = 1,
        VALID_REQUESTED = 2
    };

    class VersionInfo : public NiMemObject
    {
    public:
        unsigned int m_uiVS_Sys;
        unsigned int m_uiVS_Min;
        unsigned int m_uiVS_Req;
        unsigned int m_uiGS_Sys;
        unsigned int m_uiGS_Min;
        unsigned int m_uiGS_Req;
        unsigned int m_uiPS_Sys;
        unsigned int m_uiPS_Min;
        unsigned int m_uiPS_Req;
        unsigned int m_uiUser_Sys;
        unsigned int m_uiUser_Min;
        unsigned int m_uiUser_Req;
        unsigned int m_uiPlatform;
        bool m_bSoftwareVSCapable_Sys;
    };

    void SetupVersionInfo(VersionInfo& kVersionInfo);
    ValidityFlag IsImplementationValid(
        NSBD3D10Implementation* pkImplementation, VersionInfo& kVersionInfo);

    bool SaveBinaryPackingDefs(NiBinaryStream& kStream);
    bool LoadBinaryPackingDefs(NiBinaryStream& kStream);
    bool SaveBinaryImplementations(NiBinaryStream& kStream);
    bool LoadBinaryImplementations(NiBinaryStream& kStream);
    bool SaveBinaryOutputStreamDescriptors(NiBinaryStream& kStream);
    bool LoadBinaryOutputStreamDescriptors(NiBinaryStream& kStream);

    char* m_pcName;
    char* m_pcDescription;

    unsigned int m_uiMinVertexShaderVersionRequest;
    unsigned int m_uiMaxVertexShaderVersionRequest;
    unsigned int m_uiMinGeometryShaderVersionRequest;
    unsigned int m_uiMaxGeometryShaderVersionRequest;
    unsigned int m_uiMinPixelShaderVersionRequest;
    unsigned int m_uiMaxPixelShaderVersionRequest;
    unsigned int m_uiMinUserVersionRequest;
    unsigned int m_uiMaxUserVersionRequest;
    unsigned int m_uiPlatformRequest;

    unsigned int m_uiMinVertexShaderVersion;
    unsigned int m_uiMaxVertexShaderVersion;
    unsigned int m_uiMinGeometryShaderVersion;
    unsigned int m_uiMaxGeometryShaderVersion;
    unsigned int m_uiMinPixelShaderVersion;
    unsigned int m_uiMaxPixelShaderVersion;
    unsigned int m_uiMinUserVersion;
    unsigned int m_uiMaxUserVersion;
    unsigned int m_uiPlatform;

    // User defined data set
    NSBD3D10UserDefinedDataSetPtr m_spUserDefinedDataSet;

    NSBD3D10AttributeTable m_kGlobalAttribTable;
    NSBD3D10AttributeTable m_kAttribTable;
    NSBD3D10ObjectTable m_kObjectTable;
    NiTStringPointerMap<NSBD3D10PackingDef*> m_kPackingDefMap;
    NiTPrimitiveArray<NSBD3D10Implementation*> m_kImplementationArray;
    NiOutputStreamDescriptorArray m_kOutputStreamDescriptors;

    NiShaderDescPtr m_spShaderDesc;

    static unsigned int ms_uiReadVersion;
    static CREATENID3D10BINARYSHADER ms_pfnCreateNiD3D10BinaryShader;
};

typedef NiPointer<NSBD3D10Shader> NSBD3D10ShaderPtr;

#include "NSBD3D10Shader.inl"

#endif  //NSBD3D10SHADER_H
