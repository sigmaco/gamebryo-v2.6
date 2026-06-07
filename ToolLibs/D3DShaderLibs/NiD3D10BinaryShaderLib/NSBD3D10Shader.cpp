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
// Precompiled Header
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10Shader.h"
#include "NSBD3D10Utility.h"
#include "NSBD3D10Pass.h"
#include "NSBD3D10TextureStage.h"
#include "NSBD3D10AttributeDesc.h"

#include <NiShaderDesc.h>
#include <NiD3D10Renderer.h>
#include <NiD3D10ShaderFactory.h>

//---------------------------------------------------------------------------
unsigned int NSBD3D10Shader::ms_uiReadVersion = NSBD3D10Shader::NSB_VERSION;
NSBD3D10Shader::CREATENID3D10BINARYSHADER
    NSBD3D10Shader::ms_pfnCreateNiD3D10BinaryShader = 
    NSBD3D10Shader::DefaultCreateNiD3D10BinaryShader;
//---------------------------------------------------------------------------
NSBD3D10Shader::NSBD3D10Shader() :
    m_pcName(0),
    m_pcDescription(0), 
    m_uiMinVertexShaderVersionRequest(0), 
    m_uiMaxVertexShaderVersionRequest(0), 
    m_uiMinGeometryShaderVersionRequest(0), 
    m_uiMaxGeometryShaderVersionRequest(0), 
    m_uiMinPixelShaderVersionRequest(0), 
    m_uiMaxPixelShaderVersionRequest(0), 
    m_uiMinUserVersionRequest(0), 
    m_uiMaxUserVersionRequest(0), 
    m_uiPlatformRequest(0),
    m_uiMinVertexShaderVersion(0xffffffff), 
    m_uiMaxVertexShaderVersion(0), 
    m_uiMinGeometryShaderVersion(0xffffffff), 
    m_uiMaxGeometryShaderVersion(0), 
    m_uiMinPixelShaderVersion(0xffffffff), 
    m_uiMaxPixelShaderVersion(0), 
    m_uiMinUserVersion(0xffffffff), 
    m_uiMaxUserVersion(0), 
    m_uiPlatform(0), 
    m_spShaderDesc(0)
{
    m_kPackingDefMap.RemoveAll();
    m_kImplementationArray.RemoveAll();
}
//---------------------------------------------------------------------------
NSBD3D10Shader::~NSBD3D10Shader()
{
    NSBD3D10Utility::Log("Killing NSBD3D10Shader %s\n", m_pcName);

    m_spShaderDesc = 0;
    NiFree(m_pcName);
    NiFree(m_pcDescription);

    const char* pcName;
    NiTMapIterator kIter;

    NSBD3D10PackingDef* pkPackingDef;
    kIter = m_kPackingDefMap.GetFirstPos();
    while (kIter)
    {
        m_kPackingDefMap.GetNext(kIter, pcName, pkPackingDef);
        if (pkPackingDef)
        {
            NiDelete pkPackingDef;
        }
    }
    m_kPackingDefMap.RemoveAll();

    NSBD3D10Implementation* pkImplementation;
    for (unsigned int ui = 0; ui < m_kImplementationArray.GetSize(); ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            NiDelete pkImplementation;
            m_kImplementationArray.SetAt(ui, 0);
        }
    }
    m_kImplementationArray.RemoveAll();

    m_kOutputStreamDescriptors.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3D10BinaryShader* NSBD3D10Shader::DefaultCreateNiD3D10BinaryShader(
    const char* pcClassName)
{
    if (pcClassName && strcmp(pcClassName, ""))
    {
        NiD3D10Renderer::Warning(
            "NSBD3D10Shader::DefaultCreateNiD3D10BinaryShader called for "
            "class name %s\n  Did you intend to have a callback set for this "
            "class??\n", pcClassName);
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Shader::GetPackingDefCount()
{
    return m_kPackingDefMap.GetCount();
}
//---------------------------------------------------------------------------
NSBD3D10PackingDef* NSBD3D10Shader::GetPackingDef(const char* pcName, 
    bool bCreate)
{
    NSBD3D10PackingDef* pkPackingDef = 0;

    if (!m_kPackingDefMap.GetAt(pcName, pkPackingDef))
    {
        if (bCreate)
        {
            pkPackingDef = NiNew NSBD3D10PackingDef();
            NIASSERT(pkPackingDef);

            pkPackingDef->SetName(pcName);
            m_kPackingDefMap.SetAt(pcName, pkPackingDef);
        }
    }

    return pkPackingDef;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Shader::GetImplementationCount()
{
    return m_kImplementationArray.GetEffectiveSize();
}
//---------------------------------------------------------------------------
NSBD3D10Implementation* NSBD3D10Shader::GetImplementation(const char* pcName, 
    bool bCreate, unsigned int uiNextIndex)
{
    NSBD3D10Implementation* pkImplementation = 0;

    pkImplementation = GetImplementationByName(pcName);
    if (!pkImplementation)
    {
        if (bCreate)
        {
            pkImplementation = NiNew NSBD3D10Implementation();
            NIASSERT(pkImplementation);

            pkImplementation->SetName(pcName);
            pkImplementation->SetIndex(uiNextIndex);
            m_kImplementationArray.SetAtGrow(uiNextIndex, pkImplementation);
        }
    }

    return pkImplementation;
}
//---------------------------------------------------------------------------
NSBD3D10Implementation* NSBD3D10Shader::GetImplementationByName(
    const char* pcName)
{
    unsigned int uiSize = m_kImplementationArray.GetSize();

    NSBD3D10Implementation* pkImplementation;
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            if (strcmp(pkImplementation->GetName(), pcName) == 0)
                return pkImplementation;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
NSBD3D10Implementation* NSBD3D10Shader::GetImplementationByIndex(
    unsigned int uiIndex)
{
    return m_kImplementationArray.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiD3D10BinaryShader* NSBD3D10Shader::GetD3D10Shader(
    unsigned int uiImplementation)
{
    NSBD3D10Implementation* pkImplementation = 0;

    VersionInfo kVersionInfo;
    SetupVersionInfo(kVersionInfo);

    bool bBestImplementation = false;

    if (uiImplementation != NiShader::DEFAULT_IMPLEMENTATION &&
        uiImplementation < m_kImplementationArray.GetSize())
    {
        // Grab the proper implementation
        pkImplementation = m_kImplementationArray.GetAt(uiImplementation);
        if (!IsImplementationValid(pkImplementation, kVersionInfo))
        {
            NiRenderer::Warning("%s - %s - "
                "Requested implementation (%d) invalid on operating "
                "hardware.\n", __FUNCTION__, GetName(), uiImplementation);
            pkImplementation = 0;
        }
        else
        {
            if (pkImplementation == GetBestImplementation())
            {
                bBestImplementation = true;
            }
        }
    }

    if (!pkImplementation)
    {
        // Get the best for the hardware
        pkImplementation = GetBestImplementation();
        if (!pkImplementation)
        {
            NiRenderer::Warning("%s - %s - Unable "
                "to find valid implementation for hardware.\n",
                __FUNCTION__, GetName());
            return 0;
        }
        bBestImplementation = true;
    }

    // This will ALWAYS be set - at least to the default.
    NiD3D10BinaryShader* pkShader = ms_pfnCreateNiD3D10BinaryShader(
        pkImplementation->GetClassName());
    if (!pkShader)
    {
        pkShader = NiNew NiD3D10BinaryShader();
    }
    // We better have a shader by now!
    NIASSERT(pkShader);

    pkShader->SetName(GetName());
    pkShader->SetImplementation(pkImplementation->GetIndex());
    pkShader->SetIsBestImplementation(bBestImplementation);

    pkShader->SetUserDefinedDataSet(m_spUserDefinedDataSet);
    pkShader->SetImplementationUserDefinedDataSet(
        pkImplementation->GetUserDefinedDataSet());
    
    NSBD3D10Pass* pkPass;
    for (unsigned int ui = 0; ui < pkImplementation->GetPassCount(); ui++)
    {
        pkPass = pkImplementation->GetPass(ui, false);
        if (pkPass)
        {
            NSBD3D10UserDefinedDataSet* pkUDDSet = 
                pkPass->GetUserDefinedDataSet();
            pkShader->SetPassUserDefinedDataSet(ui, pkUDDSet);
        }
    }

    if (!pkImplementation->SetupNiD3D10BinaryShader(*pkShader, m_spShaderDesc,
        this))
    {
        NiRenderer::Error("%s - %s - Failed to setup the binary shader.\n",
            __FUNCTION__, GetName());
        NiDelete pkShader;
        return 0;
    }

    // If the implementation has both a valid semantic adapter table (SAT)
    // and a packing defintion, then the SAT wins, otherwise, the packing
    // def is used to construct a NiShaderDeclaration, which is in turn used
    // to create a SAT.
    if (pkShader->GetSemanticAdapterTable().GetNumFilledTableEntries() > 0)
    {
        // Already found a SAT, do we have a PackingDef too?
        if (pkImplementation->GetPackingDef())
        {
            NiRenderer::Warning("%s - %s - Both a SemanticAdapterTable and "
                "a PackingDef are present in the '%s' implementation. "
                "The PackingDef will be ignored.\n",
                __FUNCTION__, pkImplementation->GetName(), GetName());
        }
    }
    else if (pkImplementation->GetPackingDef())
    {
        // No SAT, but we have a PackingDef reference
        // Get the packing def from its name
        NSBD3D10PackingDef* pkPackingDef = GetPackingDef(
            pkImplementation->GetPackingDef(), false);
        if (!pkPackingDef)
        {
            NiRenderer::Error("%s - %s - Failed "
                "to find packing definition for implementation.\n",
                __FUNCTION__, GetName());
            NIASSERT(!"Failed to find packing def!");
            NiDelete pkShader;
            return 0;
        }
        else
        {
            // Create the shader declaration from the packing def
            NiShaderDeclarationPtr spShaderDecl = pkPackingDef->
                GetVertexDescription();
            if (!spShaderDecl)
            {
                NiRenderer::Error("%s - %s - "
                    "Failed to convert packing definition to "
                    "NiShaderDeclaration.\n", __FUNCTION__, GetName());
                NIASSERT(!"Failed to convert packing def!");
                NiDelete pkShader;
                return 0;
            }

            // Create the semantic adapter table from the shader
            // declaration
            pkShader->SetSemanticAdapterTableFromShaderDeclaration(
                spShaderDecl);
        }
    }
    else
    {
        NiRenderer::Warning("%s - %s - Neither a PackingDef or a "
            "SemanticAdapterTable was present, constructing a default "
            "SemanticAdapterTable containing only POSITION0.\n",
            __FUNCTION__, GetName());

        // There is no decl, so we rely on the default behavior and
        // create a semantic adapter table that needs only position
        pkShader->SetSemanticAdapterTableFromShaderDeclaration(NULL);
    }

    if (!pkShader->Initialize())
    {
        NiRenderer::Warning("%s - %s -  Failed to initialize shader.\n",
            __FUNCTION__, GetName());
        NiDelete pkShader;
        return NULL;
    }

    return pkShader;
}
//---------------------------------------------------------------------------
NiShaderDesc* NSBD3D10Shader::GetShaderDesc()
{
    if (m_spShaderDesc)
        return m_spShaderDesc;

    m_spShaderDesc = NiNew NiShaderDesc();
    NIASSERT(m_spShaderDesc);

    m_spShaderDesc->SetName(m_pcName);
    m_spShaderDesc->SetDescription(m_pcDescription);

    m_spShaderDesc->AddVertexShaderVersion(m_uiMinVertexShaderVersion);
    m_spShaderDesc->AddVertexShaderVersion(m_uiMaxVertexShaderVersion);
    m_spShaderDesc->AddGeometryShaderVersion(m_uiMinGeometryShaderVersion);
    m_spShaderDesc->AddGeometryShaderVersion(m_uiMaxGeometryShaderVersion);
    m_spShaderDesc->AddPixelShaderVersion(m_uiMinPixelShaderVersion);
    m_spShaderDesc->AddPixelShaderVersion(m_uiMaxPixelShaderVersion);
    m_spShaderDesc->AddUserDefinedVersion(m_uiMinUserVersion);
    m_spShaderDesc->AddUserDefinedVersion(m_uiMaxUserVersion);
    m_spShaderDesc->AddPlatformFlags(m_uiPlatform);

    NSBD3D10AttributeDesc* pkAttribDesc = m_kAttribTable.GetFirstAttribute();
    while (pkAttribDesc)
    {
        NiShaderAttributeDesc* pkNewDesc = 
            pkAttribDesc->GetShaderAttributeDesc();
        NIASSERT(pkNewDesc);
        
        m_spShaderDesc->AddAttribute(pkNewDesc);

        pkAttribDesc = m_kAttribTable.GetNextAttribute();
    }

    NSBD3D10Implementation* pkImplementation;
    m_spShaderDesc->SetNumberOfImplementations(GetImplementationCount());
    for (unsigned int ui = 0; ui < m_kImplementationArray.GetSize(); ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            NSBD3D10Requirements* pkReqs = pkImplementation->GetRequirements();
            NIASSERT(pkReqs);
            // Currently, implementation 0 defines the bones per partition
            // as well as the Binormal Tangent method.
            if (ui == 0)
            {
                m_spShaderDesc->SetBonesPerPartition(
                    pkReqs->GetBonesPerPartition());
                m_spShaderDesc->SetBinormalTangentMethod(
                    pkReqs->GetBinormalTangentMethod());
                m_spShaderDesc->SetBinormalTangentUVSource(
                    (unsigned short)pkReqs->GetBinormalTangentUVSource());
            }

            NiShaderRequirementDesc* pkReqDesc = 
                NiNew NiShaderRequirementDesc();
            NIASSERT(pkReqDesc);

            pkReqDesc->SetName(pkImplementation->GetName());
            pkReqDesc->SetDescription(pkImplementation->GetDesc());
            pkReqDesc->AddVertexShaderVersion(pkReqs->GetVSVersion());
            pkReqDesc->AddGeometryShaderVersion(pkReqs->GetGSVersion());
            pkReqDesc->AddPixelShaderVersion(pkReqs->GetPSVersion());
            pkReqDesc->AddUserDefinedVersion(pkReqs->GetUserVersion());
            pkReqDesc->AddPlatformFlags(pkReqs->GetPlatformFlags());
            pkReqDesc->SetBonesPerPartition(pkReqs->GetBonesPerPartition());
            pkReqDesc->SetBinormalTangentMethod(
                pkReqs->GetBinormalTangentMethod());
            pkReqDesc->SetBinormalTangentUVSource(
                (unsigned short)pkReqs->GetBinormalTangentUVSource());
            pkReqDesc->SetSoftwareVPAcceptable(
                pkReqs->GetSoftwareVPAcceptable());
            pkReqDesc->SetSoftwareVPRequired(
                pkReqs->GetSoftwareVPRequired());
            
            m_spShaderDesc->SetImplementationDescription(ui, pkReqDesc);
        }
    }

    return m_spShaderDesc;
}
//---------------------------------------------------------------------------
void NSBD3D10Shader::SetName(const char* pcName)
{
    NSBD3D10Utility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
void NSBD3D10Shader::SetDescription(const char* pcDescription)
{
    NSBD3D10Utility::SetString(m_pcDescription, 0, pcDescription);
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::Save(const char* pcFilename, bool bEndianSwap)
{
    // Open a binary stream
    NiFile* pkFile = NiFile::GetFile(pcFilename, NiFile::WRITE_ONLY);
    if (!pkFile)
        return false;

    bool bResult = SaveBinary(*pkFile, bEndianSwap);
    NiDelete pkFile;

    return bResult;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::SaveBinary(NiBinaryStream& kStream, bool bEndianSwap)
{
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kStream.SetEndianSwap(!bPlatformLittle);

    // File header and version number must be streamed in/out little endian!
    unsigned int uiMagicNumber = MAGIC_NUMBER;
    char acMagicNumber[4];
    acMagicNumber[0] = (char)(uiMagicNumber & 0x000000ff);
    acMagicNumber[1] = (char)((uiMagicNumber & 0x0000ff00) >> 8);
    acMagicNumber[2] = (char)((uiMagicNumber & 0x00ff0000) >> 16);
    acMagicNumber[3] = (char)((uiMagicNumber & 0xff000000) >> 24);

    NiStreamSaveBinary(kStream, acMagicNumber, 4);

    unsigned int uiNSBD3D10Version = NSB_VERSION;
    char acNSBD3D10Version[4];
    acNSBD3D10Version[0] = (char)(uiNSBD3D10Version & 0x000000ff);
    acNSBD3D10Version[1] = (char)((uiNSBD3D10Version & 0x0000ff00) >> 8);
    acNSBD3D10Version[2] = (char)((uiNSBD3D10Version & 0x00ff0000) >> 16);
    acNSBD3D10Version[3] = (char)((uiNSBD3D10Version & 0xff000000) >> 24);

    NiStreamSaveBinary(kStream, acNSBD3D10Version, 4);

    // Version 1.10 added endianness support.
    bool bFileLittle = (bPlatformLittle != bEndianSwap);
    NiStreamSaveBinary(kStream, bFileLittle);
    kStream.SetEndianSwap(bEndianSwap);

    kStream.WriteCString(m_pcName);
    kStream.WriteCString(m_pcDescription);
 
    // Version 1.6 added support for user-defined data...
    unsigned int uiUDDSetPresent = 0;
    if (m_spUserDefinedDataSet)
    {
        uiUDDSetPresent = 1;
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
        if (!m_spUserDefinedDataSet->SaveBinary(kStream))
            return false;
    }
    else
    {
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
    }

    if (!m_kGlobalAttribTable.SaveBinary(kStream))
        return false;

    if (!m_kAttribTable.SaveBinary(kStream))
        return false;

    if (!SaveBinaryPackingDefs(kStream))
        return false;

    if (!SaveBinaryImplementations(kStream))
        return false;

    if (!SaveBinaryOutputStreamDescriptors(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::Load(const char* pcFilename)
{
    // Open a binary stream
    NiFile* pkFile = NiFile::GetFile(pcFilename, NiFile::READ_ONLY);
    if (!pkFile)
        return false;

    bool bResult = LoadBinary(*pkFile);
    NiDelete pkFile;

#if defined(_DEBUG_DUMP_LOADED_SHADER_)
    if (bResult)
        Dump();
#endif  //#if defined(_DEBUG_DUMP_LOADED_SHADER_)

    return bResult;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::LoadBinary(NiBinaryStream& kStream)
{
    // Header is in little endian.
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kStream.SetEndianSwap(!bPlatformLittle);

    char acMagicNumber[4];
    NiStreamLoadBinary(kStream, acMagicNumber, 4);
    unsigned int uiMagicNumber = 
        acMagicNumber[0] << 0 |
        acMagicNumber[1] << 8 |
        acMagicNumber[2] << 16 |
        acMagicNumber[3] << 24;

    if (uiMagicNumber != MAGIC_NUMBER)
        return false;

    char acReadVersion[4];
    NiStreamLoadBinary(kStream, acReadVersion, 4);
    ms_uiReadVersion = 
        acReadVersion[0] << 0 |
        acReadVersion[1] << 8 |
        acReadVersion[2] << 16 |
        acReadVersion[3] << 24;

    if (ms_uiReadVersion > NSBD3D10Shader::NSB_VERSION)
        return false;

    // Get endianness if version 1.10 or later.
    // Old files by default are little endian.
    bool bLittle = true;
    if (ms_uiReadVersion >= 0x00010010)
        NiStreamLoadBinary(kStream, bLittle);
    kStream.SetEndianSwap(bLittle != bPlatformLittle);

    m_pcName = kStream.ReadCString();
    m_pcDescription = kStream.ReadCString();

    // Version 1.6 added support for user-defined data...
    if (NSBD3D10Shader::GetReadVersion() >= 0x00010006)
    {
        unsigned int uiUDDSetPresent;
        NiStreamLoadBinary(kStream, uiUDDSetPresent);
        if (uiUDDSetPresent)
        {
            m_spUserDefinedDataSet = NiNew NSBD3D10UserDefinedDataSet();
            NIASSERT(m_spUserDefinedDataSet);
            if (!m_spUserDefinedDataSet->LoadBinary(kStream))
                return false;
        }
        else
        {
            m_spUserDefinedDataSet = 0;
        }
    }

    if (!m_kGlobalAttribTable.LoadBinary(kStream))
        return false;

    if (!m_kAttribTable.LoadBinary(kStream))
        return false;

    if (!LoadBinaryPackingDefs(kStream))
        return false;

    if (!LoadBinaryImplementations(kStream))
        return false;

    if (!LoadBinaryOutputStreamDescriptors(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
// Helper functions for retrieving the best implementation
//---------------------------------------------------------------------------
typedef NiTPointerList<NSBD3D10Implementation*> NSBD3D10ImplementationList;

bool ImplementationInList(NSBD3D10Implementation* pkImplementation, 
    NSBD3D10ImplementationList* pkList)
{
    NSBD3D10Implementation* pkCheckImp;

    NiTListIterator pos = pkList->GetHeadPos();
    while (pos)
    {
        pkCheckImp = pkList->GetNext(pos);
        if (pkCheckImp == pkImplementation)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void GetRequirementValues(NSBD3D10Requirements* pkReqs,
    unsigned int& uiVSVers, unsigned int& uiGSVers, unsigned int& uiPSVers,
    unsigned int& uiUser)
{
    NIASSERT(pkReqs);

    uiVSVers = pkReqs->GetVSVersion();
    if (uiVSVers == 0)
        uiVSVers = D3DVS_VERSION(0, 0);
    uiGSVers = pkReqs->GetGSVersion();
    if (uiGSVers == 0)
        uiGSVers = D3DGS_VERSION(0, 0);
    uiPSVers = pkReqs->GetPSVersion();
    if (uiPSVers == 0)
        uiPSVers = D3DPS_VERSION(0, 0);
    uiUser = pkReqs->GetUserVersion();
    if (uiUser == 0)
        uiUser = D3DVS_VERSION(0, 0);
}
//---------------------------------------------------------------------------
NSBD3D10Implementation* NSBD3D10Shader::GetBestImplementation()
{
    NSBD3D10Implementation* pkImplementation = 0;
    bool bFound = false;

    VersionInfo kVersionInfo;

    // Grab the version info from the renderer, adjusting parameters as
    // necessary to prevent illegal requests, etc. (For example, a request
    // that is higher than the system.)
    SetupVersionInfo(kVersionInfo);

    unsigned int uiVSVers;
    unsigned int uiGSVers;
    unsigned int uiPSVers;
    unsigned int uiUser;
    NSBD3D10Requirements* pkReqs;
    
    NiTPointerList<NSBD3D10Implementation*> kValidList;
    kValidList.RemoveAll();

    unsigned int uiCount = m_kImplementationArray.GetSize();
    unsigned int ui;

    for (ui = 0; ui < uiCount; ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            // Check the versions vs. the request ones...
            // NOTE: This assumes that implementations are listed
            // in a hardware-need order. ie, the highest hardware
            // requirements will be first in the list!
            if (IsImplementationValid(pkImplementation, kVersionInfo))
            {
                if (!ImplementationInList(pkImplementation, &kValidList))
                    kValidList.AddTail(pkImplementation);
            }
        }
    }

    // Now, check the valid list for the closest match to the requested
    // version.
    if (!bFound)
    {
        NiTListIterator pos;
        // Now, how do we decide on the 'closest' to the requested...
        // We will grab the first one that is <= the requested ones
        pos = kValidList.GetHeadPos();
        while (pos)
        {
            pkImplementation = kValidList.GetNext(pos);
            if (pkImplementation)
            {
                pkReqs = pkImplementation->GetRequirements();
                NIASSERT(pkReqs);
                GetRequirementValues(pkReqs, uiVSVers, uiGSVers, uiPSVers,
                    uiUser);

                if ((uiVSVers <= kVersionInfo.m_uiVS_Req) &&
                    (uiVSVers >= kVersionInfo.m_uiVS_Min) && 
                    (uiGSVers <= kVersionInfo.m_uiGS_Req) && 
                    (uiGSVers >= kVersionInfo.m_uiGS_Min) &&
                    (uiPSVers <= kVersionInfo.m_uiPS_Req) && 
                    (uiPSVers >= kVersionInfo.m_uiPS_Min) &&
                    (uiUser <= kVersionInfo.m_uiUser_Req) &&
                    (uiUser >= kVersionInfo.m_uiUser_Min))
                {
                    // A workable implementation has been found.
                    bFound = true;
                    break;
                }
            }
        }
    }

    kValidList.RemoveAll();

    if (!bFound)
    {
        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "* ERROR: Unable to find usable implementation for %s\n",
            GetName());
        return 0;
    }
    return pkImplementation;
}
//---------------------------------------------------------------------------
void NSBD3D10Shader::SetupVersionInfo(VersionInfo& kVersionInfo)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    const NiD3D10ShaderLibraryVersion* pkSLVersion = 
        pkRenderer->GetShaderLibraryVersion();
    NIASSERT(pkSLVersion);

    kVersionInfo.m_uiVS_Sys = pkSLVersion->GetSystemVertexShaderVersion();
    kVersionInfo.m_uiGS_Sys = pkSLVersion->GetSystemGeometryShaderVersion();
    kVersionInfo.m_uiPS_Sys = pkSLVersion->GetSystemPixelShaderVersion();
    kVersionInfo.m_uiUser_Sys = pkSLVersion->GetSystemUserVersion();

    kVersionInfo.m_uiVS_Min = pkSLVersion->GetMinVertexShaderVersion();
    kVersionInfo.m_uiGS_Min = pkSLVersion->GetMinGeometryShaderVersion();
    kVersionInfo.m_uiGS_Min = 0xfffd0000;  //(makes having a GS optional)
    kVersionInfo.m_uiPS_Min = pkSLVersion->GetMinPixelShaderVersion();
    kVersionInfo.m_uiUser_Min = pkSLVersion->GetMinUserVersion();

    kVersionInfo.m_uiVS_Req = pkSLVersion->GetVertexShaderVersionRequest();
    kVersionInfo.m_uiGS_Req = pkSLVersion->GetGeometryShaderVersionRequest();
    kVersionInfo.m_uiPS_Req = pkSLVersion->GetPixelShaderVersionRequest();
    kVersionInfo.m_uiUser_Req = pkSLVersion->GetUserVersionRequest();

    kVersionInfo.m_uiPlatform = pkSLVersion->GetPlatform();

    // Do some quick checks to reduce the time spent in this function.

    // If the requested versions are GREATER THAN that the system, then we 
    // can bump down the request right now.
    if (kVersionInfo.m_uiVS_Sys < kVersionInfo.m_uiVS_Req)
        kVersionInfo.m_uiVS_Req = kVersionInfo.m_uiVS_Sys;
    if (kVersionInfo.m_uiGS_Sys < kVersionInfo.m_uiGS_Req)
        kVersionInfo.m_uiGS_Req = kVersionInfo.m_uiGS_Sys;
    if (kVersionInfo.m_uiPS_Sys < kVersionInfo.m_uiPS_Req)
        kVersionInfo.m_uiPS_Req = kVersionInfo.m_uiPS_Sys;
    if (kVersionInfo.m_uiUser_Sys < kVersionInfo.m_uiUser_Req)
        kVersionInfo.m_uiUser_Req = kVersionInfo.m_uiUser_Sys;
}
//---------------------------------------------------------------------------
NSBD3D10Shader::ValidityFlag NSBD3D10Shader::IsImplementationValid(
    NSBD3D10Implementation* pkImplementation, VersionInfo& kVersionInfo)
{
    if (!pkImplementation)
        return INVALID;

    // Check the versions vs. the request ones...
    // NOTE: This assumes that implementations are listed
    // in a hardware-need order. ie, the highest hardware
    // requirements will be first in the list!
    NSBD3D10Requirements* pkReqs = pkImplementation->GetRequirements();
    NIASSERT(pkReqs);
    unsigned int uiVSVers;
    unsigned int uiGSVers;
    unsigned int uiPSVers;
    unsigned int uiUser;
    GetRequirementValues(pkReqs, uiVSVers, uiGSVers, uiPSVers, uiUser);

    // Check the platform first.
    if ((pkReqs->GetPlatformFlags() != 0) &&
        ((pkReqs->GetPlatformFlags() & kVersionInfo.m_uiPlatform) == 0))
    {
        return INVALID;
    }

    // Check the system versions to make sure the hardware 
    // can handle the implementation
    if (uiPSVers > kVersionInfo.m_uiPS_Sys)
        return INVALID;

    bool bSoftwareVP = false;

    // D3D10 not capable of doing software VP.
    if (pkReqs->GetSoftwareVPRequired())
        return INVALID;
    if (!bSoftwareVP && uiVSVers > kVersionInfo.m_uiVS_Sys)
        return INVALID;

    // Check the minimum versions to make sure it is acceptable
    // to the user
    if ((uiVSVers < kVersionInfo.m_uiVS_Min) ||
        (uiGSVers < kVersionInfo.m_uiGS_Min) ||
        (uiPSVers < kVersionInfo.m_uiPS_Min)) 
    {
        return INVALID;
    }

    // Check the user version.
    if ((uiUser > kVersionInfo.m_uiUser_Sys) || 
        (uiUser < kVersionInfo.m_uiUser_Min))
    {
        return INVALID;
    }

    if ((uiVSVers == kVersionInfo.m_uiVS_Req) &&
        (uiGSVers == kVersionInfo.m_uiGS_Req) &&
        (uiPSVers == kVersionInfo.m_uiPS_Req) &&
        (uiUser == kVersionInfo.m_uiUser_Req))
    {
        return VALID_REQUESTED;
    }

    return VALID;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::SaveBinaryPackingDefs(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kPackingDefMap.GetCount();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    const char* pcName;
    NSBD3D10PackingDef* pkPackingDef;
    NiTMapIterator iter = m_kPackingDefMap.GetFirstPos();
    while (iter)
    {
        m_kPackingDefMap.GetNext(iter, pcName, pkPackingDef);
        if (pcName && pkPackingDef)
        {
            // Stream the packing def
            if (!pkPackingDef->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::LoadBinaryPackingDefs(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    NSBD3D10PackingDef* pkPackingDef;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPackingDef = NiNew NSBD3D10PackingDef();
        NIASSERT(pkPackingDef);

        if (!pkPackingDef->LoadBinary(kStream))
            return false;

        m_kPackingDefMap.SetAt(pkPackingDef->GetName(), pkPackingDef);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::SaveBinaryImplementations(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kImplementationArray.GetEffectiveSize();
    unsigned int uiSize = m_kImplementationArray.GetSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    NSBD3D10Implementation* pkImplementation;

    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            // Stream the packing def
            if (!pkImplementation->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::LoadBinaryImplementations(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    NSBD3D10Implementation* pkImplementation;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkImplementation = NiNew NSBD3D10Implementation();
        NIASSERT(pkImplementation);

        if (!pkImplementation->LoadBinary(kStream))
            return false;

        NSBD3D10Requirements* pkReqs = pkImplementation->GetRequirements();
        if (pkReqs)
        {
            AddPixelShaderVersion(pkReqs->GetPSVersion());
            AddVertexShaderVersion(pkReqs->GetVSVersion());

            if (NSBD3D10Shader::GetReadVersion() >= 0x00010013)
            {
                // Version 1.13 added D3D10 and geometry shader support.
                AddGeometryShaderVersion(pkReqs->GetGSVersion());
            }

            AddUserVersion(pkReqs->GetUserVersion());
            AddPlatform(pkReqs->GetPlatformFlags());
        }

        NIASSERT(ui == pkImplementation->GetIndex());
        m_kImplementationArray.SetAtGrow(ui, pkImplementation);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::SaveBinaryOutputStreamDescriptors(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kOutputStreamDescriptors.GetSize();
    NiStreamSaveBinary(kStream, uiCount);  // number of screen polygons
    
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        m_kOutputStreamDescriptors[ui].SaveBinary(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Shader::LoadBinaryOutputStreamDescriptors(NiBinaryStream& kStream)
{
    m_kOutputStreamDescriptors.RemoveAll();

    // Version 2.2 added support for stream output...
    if (NSBD3D10Shader::GetReadVersion() >= 0x00020002)
    {
        unsigned int uiCount = 0;
        NiStreamLoadBinary(kStream, uiCount);
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            NiOutputStreamDescriptor kDesc;
            kDesc.LoadBinary(kStream);
            m_kOutputStreamDescriptors.Add(kDesc);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10Shader::Dump()
{
    char acName[NI_MAX_PATH];

    NiSprintf(acName, NI_MAX_PATH, "%s.log", m_pcName);
    FILE* pf;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    fopen_s(&pf, acName, "wt");
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    pf = fopen(acName, "wt");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    NSBD3D10Utility::Dump(pf, true, "Dumping BinaryShader %s\n", m_pcName);

    NSBD3D10Utility::Dump(pf, true, "    Desc: %s", m_pcDescription);
    NSBD3D10Utility::Dump(pf, false, "\n");
    NSBD3D10Utility::Dump(pf, true, "    Min VS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMinVertexShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMinVertexShaderVersion));
    NSBD3D10Utility::Dump(pf, true, "    Max VS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMaxVertexShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMaxVertexShaderVersion));
    NSBD3D10Utility::Dump(pf, true, "    Min GS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMinGeometryShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMinGeometryShaderVersion));
    NSBD3D10Utility::Dump(pf, true, "    Max GS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMaxGeometryShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMaxGeometryShaderVersion));
    NSBD3D10Utility::Dump(pf, true, "    Min PS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMinPixelShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMinPixelShaderVersion));
    NSBD3D10Utility::Dump(pf, true, "    Max PS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMaxPixelShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMaxPixelShaderVersion));
    NSBD3D10Utility::Dump(pf, true, "    Min US Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMinUserVersion),
        D3DSHADER_VERSION_MINOR(m_uiMinUserVersion));
    NSBD3D10Utility::Dump(pf, true, "    Max US Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMaxUserVersion),
        D3DSHADER_VERSION_MINOR(m_uiMaxUserVersion));
    NSBD3D10Utility::Dump(pf, true, "          Platform: 0x%08x\n",
        m_uiPlatform);
    NSBD3D10Utility::Dump(pf, true, "\n");

    NSBD3D10Utility::Dump(pf, true, "Global Attribute Table:\n");
    NSBD3D10Utility::IndentInsert();
    m_kGlobalAttribTable.Dump(pf);
    NSBD3D10Utility::IndentRemove();
    NSBD3D10Utility::Dump(pf, true, "\n");

    NSBD3D10Utility::Dump(pf, true, "Local Attribute Table:\n");
    NSBD3D10Utility::IndentInsert();
    m_kAttribTable.Dump(pf);
    NSBD3D10Utility::IndentRemove();
    NSBD3D10Utility::Dump(pf, true, "\n");

    NSBD3D10Utility::Dump(pf, true, "PackingDef Count: %d\n",
        GetPackingDefCount());
    NSBD3D10Utility::Dump(pf, true, "\n");
//    NiTStringPointerMap<NSBD3D10PackingDef*> m_kPackingDefMap;

    NSBD3D10Utility::Dump(pf, true, "%2d Implementations\n", 
        m_kImplementationArray.GetEffectiveSize());
    for (unsigned int ui = 0; ui < m_kImplementationArray.GetSize(); ui++)
    {
        NSBD3D10Implementation* pkImpl = m_kImplementationArray.GetAt(ui);
        if (pkImpl)
        {
            NSBD3D10Utility::IndentInsert();
            pkImpl->Dump(pf);
            NSBD3D10Utility::IndentRemove();
            NSBD3D10Utility::Dump(pf, true, "\n");
        }
    }

    if (pf)
        fclose(pf);
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
