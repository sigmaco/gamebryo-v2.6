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
#include "NiBinaryShaderLibPCH.h"

#include <NiShaderDesc.h>
#include <NiShaderFactory.h>
#include <NiD3DTextureStage.h>
#include <NiD3DRendererHeaders.h>
#include <NiD3DShaderFactory.h>

#include "NSBShader.h"
#include "NSBUtility.h"
#include "NSBPass.h"
#include "NSBTextureStage.h"
#include "NSBAttributeDesc.h"

//---------------------------------------------------------------------------
unsigned int NSBShader::ms_uiReadVersion = NSBShader::NSB_VERSION;
NSBShader::CREATENIBINARYSHADER NSBShader::ms_pfnCreateNiBinaryShader = 
    NSBShader::DefaultCreateNiBinaryShader;
//---------------------------------------------------------------------------
NSBShader::NSBShader() :
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
NSBShader::~NSBShader()
{
    NILOG(NIMESSAGE_GENERAL_1, "Killing NSBShader %s\n", m_pcName);

    m_spShaderDesc = 0;
    NiFree(m_pcName);
    NiFree(m_pcDescription);

    const char* pcName;
    NiTMapIterator kIter;

    NSBPackingDef* pkPackingDef;
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

    NSBImplementation* pkImplementation;
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
NiBinaryShader* NSBShader::DefaultCreateNiBinaryShader(
    const char* pcClassName)
{
    if (pcClassName && strcmp(pcClassName, ""))
    {
        NiRenderer::Warning("NSBShader::DefaultCreateNiBinaryShader "
            "called for class name %s\n"
            "   Did you intend to have a callback set for this???\n",
            pcClassName);
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NSBShader::GetPackingDefCount()
{
    return m_kPackingDefMap.GetCount();
}
//---------------------------------------------------------------------------
NSBPackingDef* NSBShader::GetPackingDef(const char* pcName, 
    bool bCreate)
{
    NSBPackingDef* pkPackingDef = 0;

    if (!m_kPackingDefMap.GetAt(pcName, pkPackingDef))
    {
        if (bCreate)
        {
            pkPackingDef = NiNew NSBPackingDef();
            NIASSERT(pkPackingDef);

            pkPackingDef->SetName(pcName);
            m_kPackingDefMap.SetAt(pcName, pkPackingDef);
        }
    }

    return pkPackingDef;
}
//---------------------------------------------------------------------------
unsigned int NSBShader::GetImplementationCount()
{
    return m_kImplementationArray.GetEffectiveSize();
}
//---------------------------------------------------------------------------
NSBImplementation* NSBShader::GetImplementation(const char* pcName, 
    bool bCreate, unsigned int uiNextIndex)
{
    NSBImplementation* pkImplementation = 0;

    pkImplementation = GetImplementationByName(pcName);
    if (!pkImplementation)
    {
        if (bCreate)
        {
            pkImplementation = NiNew NSBImplementation();
            NIASSERT(pkImplementation);

            pkImplementation->SetName(pcName);
            pkImplementation->SetIndex(uiNextIndex);
            m_kImplementationArray.SetAtGrow(uiNextIndex, pkImplementation);
        }
    }

    return pkImplementation;
}
//---------------------------------------------------------------------------
NSBImplementation* NSBShader::GetImplementationByName(const char* pcName)
{
    unsigned int uiSize = m_kImplementationArray.GetSize();

    NSBImplementation* pkImplementation;
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
NSBImplementation* NSBShader::GetImplementationByIndex(unsigned int uiIndex)
{
    return m_kImplementationArray.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiBinaryShader* NSBShader::GetBinaryShader(unsigned int uiImplementation)
{
    NSBImplementation* pkImplementation = 0;

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
    NiBinaryShader* pkShader = ms_pfnCreateNiBinaryShader(
        pkImplementation->GetClassName());
    if (!pkShader)
    {
        pkShader = NiNew NiBinaryShader();
    }
    // We better have a shader by now!
    NIASSERT(pkShader);

    NiD3DRenderer* pkRenderer =
        NiVerifyStaticCast(NiD3DRenderer, NiRenderer::GetRenderer());
    pkShader->SetD3DRenderer(pkRenderer);
    pkShader->SetName(GetName());
    pkShader->SetImplementation(pkImplementation->GetIndex());
    pkShader->SetIsBestImplementation(bBestImplementation);

    pkShader->SetUserDefinedDataSet(m_spUserDefinedDataSet);
    pkShader->SetImplementationUserDefinedDataSet(
        pkImplementation->GetUserDefinedDataSet());
    NSBPass* pkPass;
    for (unsigned int ui = 0; ui < pkImplementation->GetPassCount(); ui++)
    {
        pkPass = pkImplementation->GetPass(ui, false);
        if (pkPass)
        {
            NSBUserDefinedDataSet* pkUDDSet = 
                pkPass->GetUserDefinedDataSet();
            pkShader->SetPassUserDefinedDataSet(ui, pkUDDSet);
        }
    }

    if (!RegisterTextureStageGlobals(pkImplementation))
    {
        // Determine how to handle the error case
        NiRenderer::Warning("%s - %s - Failed "
            "to register global texture stage variables.\n",
            __FUNCTION__, GetName());
    }

    // Setup the passes and potentially create a shader declaration
    if (!pkImplementation->SetupNiBinaryShader(*pkShader, m_spShaderDesc))
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
        NSBPackingDef* pkPackingDef = GetPackingDef(
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
                GetShaderDeclaration(NiShader::NISHADER_AGNOSTIC);
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

#if !defined(_PS3)
            // Determine if the shader is requesting BlendIndices as colors.
            // If this is the case, then set the flag on the NiD3DShader
            // to enable this conversion.
            bool bConvert = false;
            unsigned int uiStreamCount = spShaderDecl->GetStreamCount();
            for (unsigned int uiS = 0; uiS < uiStreamCount; uiS++)
            {
                unsigned int uiEntryCount = spShaderDecl->GetEntryCount(uiS);
                for (unsigned int uiE = 0; uiE < uiEntryCount; uiE++)
                {
                    const NiShaderDeclaration::ShaderRegisterEntry* pkEntry =
                        spShaderDecl->GetEntry(uiE, uiS);

                    if (pkEntry->m_eType == 
                        NiShaderDeclaration::SPTYPE_UBYTECOLOR && 
                        NiShaderDeclaration::StringToUsage(
                        pkEntry->m_kUsage) ==
                        NiShaderDeclaration::SPUSAGE_BLENDINDICES)
                    {
                        bConvert = true;
                        break;
                    }
                }
            }

            pkShader->SetConvertBlendIndicesToD3DColor(bConvert);
#endif
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

    if (!ReleaseTextureStageGlobals(pkImplementation))
    {
        // Determine how to handle the error case
        NiRenderer::Warning("%s - %s - Failed to release global texture "
            "stage variables.\n",
            __FUNCTION__, GetName());
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
NiShaderDesc* NSBShader::GetShaderDesc()
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

    NSBAttributeDesc* pkAttribDesc = m_kAttribTable.GetFirstAttribute();
    while (pkAttribDesc)
    {
        NiShaderAttributeDesc* pkNewDesc = 
            pkAttribDesc->GetShaderAttributeDesc();
        NIASSERT(pkNewDesc);
        
        m_spShaderDesc->AddAttribute(pkNewDesc);

        pkAttribDesc = m_kAttribTable.GetNextAttribute();
    }

    NSBImplementation* pkImplementation;
    m_spShaderDesc->SetNumberOfImplementations(GetImplementationCount());
    for (unsigned int ui = 0; ui < m_kImplementationArray.GetSize(); ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            NSBRequirements* pkReqs = pkImplementation->GetRequirements();
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
void NSBShader::SetName(const char* pcName)
{
    NiRendererUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
void NSBShader::SetDescription(const char* pcDescription)
{
    NiRendererUtility::SetString(m_pcDescription, 0, pcDescription);
}
//---------------------------------------------------------------------------
bool NSBShader::Save(const char* pcFilename, bool bEndianSwap)
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
bool NSBShader::SaveBinary(NiBinaryStream& kStream, bool bEndianSwap)
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

    unsigned int uiNSBVersion = NSB_VERSION;
    char acNSBVersion[4];
    acNSBVersion[0] = (char)(uiNSBVersion & 0x000000ff);
    acNSBVersion[1] = (char)((uiNSBVersion & 0x0000ff00) >> 8);
    acNSBVersion[2] = (char)((uiNSBVersion & 0x00ff0000) >> 16);
    acNSBVersion[3] = (char)((uiNSBVersion & 0xff000000) >> 24);

    NiStreamSaveBinary(kStream, acNSBVersion, 4);

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
bool NSBShader::Load(const char* pcFilename)
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
bool NSBShader::LoadBinary(NiBinaryStream& kStream)
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

    if (ms_uiReadVersion > NSBShader::NSB_VERSION)
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
    if (NSBShader::GetReadVersion() >= 0x00010006)
    {
        unsigned int uiUDDSetPresent;
        NiStreamLoadBinary(kStream, uiUDDSetPresent);
        if (uiUDDSetPresent)
        {
            m_spUserDefinedDataSet = NiNew NSBUserDefinedDataSet();
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
bool NSBShader::RegisterTextureStageGlobals(
    NSBImplementation* pkImplementation)
{
    bool bResult = true;

#if !defined(_PS3)
    // If texture stages use any globals for the transform, we need to
    // register them w/ the renderer so that things line up correctly
    // when we are converting them.
    unsigned int uiPassCount = pkImplementation->GetPassCount();
    for (unsigned int ui = 0; ui < uiPassCount; ui++)
    {
        NSBPass* pkPass = pkImplementation->GetPass(ui, false);
        if (pkPass)
        {
            unsigned int uiStageCount = pkPass->GetStageCount();
            for (unsigned int uj = 0; uj < uiStageCount; uj++)
            {
                NSBTextureStage* pkStage = pkPass->GetStage(uj, false);
                if (pkStage)
                {
                    // See if it have a global transformation
                    unsigned int uiFlags = 
                        pkStage->GetTextureTransformFlags();
                    if ((uiFlags & NiD3DTextureStage::TSTTF_SOURCE_MASK) == 
                        NiD3DTextureStage::TSTTF_GLOBAL)
                    {
                        // Grab the entry for the global table
                        NSBAttributeDesc* pkDesc = 
                            m_kGlobalAttribTable.GetAttributeByName(
                            (char*)pkStage->GetGlobalName());
                        if (!pkDesc)
                        {
                            // Log this error???
                            bResult = false;
                            continue;
                        }

                        NIASSERT(NiShaderConstantMapEntry::IsMatrix4(
                            pkDesc->GetType()));

                        float afData[16];
                        float* pfData = afData;
                        pkDesc->GetValue_Matrix4(pfData, 16 * sizeof(float));
                        // Register it w/ the renderer
                        NiD3DShaderFactory::RegisterGlobalShaderConstant(
                            pkDesc->GetName(), pkDesc->GetType(),
                            sizeof(float) * 16, afData);
                    }
                }
            }
        }
    }
#endif

    return bResult;
}
//---------------------------------------------------------------------------
bool NSBShader::ReleaseTextureStageGlobals(
    NSBImplementation* pkImplementation)
{
    bool bResult = true;

#if !defined(_PS3)
    // This function will release the globals in texture stages. It is called
    // after the shader is setup, so that the entries will properly be
    // released when the texture stage is released.
    unsigned int uiPassCount = pkImplementation->GetPassCount();
    for (unsigned int ui = 0; ui < uiPassCount; ui++)
    {
        NSBPass* pkPass = pkImplementation->GetPass(ui, false);
        if (pkPass)
        {
            unsigned int uiStageCount = pkPass->GetStageCount();
            for (unsigned int uj = 0; uj < uiStageCount; uj++)
            {
                NSBTextureStage* pkStage = pkPass->GetStage(uj, false);
                if (pkStage)
                {
                    // See if it have a global transformation
                    unsigned int uiFlags = 
                        pkStage->GetTextureTransformFlags();
                    if ((uiFlags & NiD3DTextureStage::TSTTF_SOURCE_MASK) == 
                        NiD3DTextureStage::TSTTF_GLOBAL)
                    {
                        // Grab the entry for the global table
                        NSBAttributeDesc* pkDesc = 
                            m_kGlobalAttribTable.GetAttributeByName(
                            (char*)pkStage->GetGlobalName());
                        if (!pkDesc)
                        {
                            // Log this error???
                            bResult = false;
                            continue;
                        }

                        NIASSERT(NiShaderConstantMapEntry::IsMatrix4(
                            pkDesc->GetType()));

                        // Release it
                        NiFixedString kName = pkDesc->GetName();
                        NiD3DShaderFactory::ReleaseGlobalShaderConstant(
                            kName);
                    }
                }
            }
        }
    }
#endif

    return bResult;
}
//---------------------------------------------------------------------------
// Helper functions for retrieving the best implementation
//---------------------------------------------------------------------------
typedef NiTPointerList<NSBImplementation*> NSBImplementationList;

bool ImplementationInList(NSBImplementation* pkImplementation, 
    NSBImplementationList* pkList)
{
    NSBImplementation* pkCheckImp;

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
void GetRequirementValues(NSBRequirements* pkReqs, unsigned int& uiVSVers, 
    unsigned int& uiGSVers, unsigned int& uiPSVers, unsigned int& uiUser)
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
NSBImplementation* NSBShader::GetBestImplementation()
{
    NSBImplementation* pkImplementation = 0;
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
    NSBRequirements* pkReqs;
    
    NiTPointerList<NSBImplementation*> kValidList;
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

                if ((uiVSVers >= kVersionInfo.m_uiVS_Min) && 
                    (uiGSVers <= kVersionInfo.m_uiGS_Req) && 
                    (uiGSVers >= kVersionInfo.m_uiGS_Min) &&
                    (uiPSVers <= kVersionInfo.m_uiPS_Req) && 
                    (uiPSVers >= kVersionInfo.m_uiPS_Min) &&
                    (uiUser <= kVersionInfo.m_uiUser_Req) &&
                    (uiUser >= kVersionInfo.m_uiUser_Min))
                {
                    if (uiVSVers <= kVersionInfo.m_uiVS_Req ||
                        (kVersionInfo.m_bSoftwareVSCapable_Sys &&
                        pkImplementation->GetSoftwareVP()))
                    {
                        // We have found a workable implementation!
                        bFound = true;
                        break;
                    }
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
void NSBShader::SetupVersionInfo(VersionInfo& kVersionInfo)
{
    NiD3DRenderer* pkRenderer =
        NiVerifyStaticCast(NiD3DRenderer, NiRenderer::GetRenderer());
    const NiD3DShaderLibraryVersion* pkSLVersion = 
        pkRenderer->GetShaderLibraryVersion();
    NIASSERT(pkSLVersion);

    kVersionInfo.m_uiVS_Sys = pkSLVersion->GetSystemVertexShaderVersion();
    kVersionInfo.m_uiGS_Sys = 0xfffd0400;
    kVersionInfo.m_uiPS_Sys = pkSLVersion->GetSystemPixelShaderVersion();
    kVersionInfo.m_uiUser_Sys = pkSLVersion->GetSystemUserVersion();

    kVersionInfo.m_uiVS_Min = pkSLVersion->GetMinVertexShaderVersion();
    kVersionInfo.m_uiGS_Min = 0xfffd0000;
    kVersionInfo.m_uiPS_Min = pkSLVersion->GetMinPixelShaderVersion();
    kVersionInfo.m_uiUser_Min = pkSLVersion->GetMinUserVersion();

    kVersionInfo.m_uiVS_Req = pkSLVersion->GetVertexShaderVersionRequest();
    kVersionInfo.m_uiGS_Req = 0xfffd0400;
    kVersionInfo.m_uiPS_Req = pkSLVersion->GetPixelShaderVersionRequest();
    kVersionInfo.m_uiUser_Req = pkSLVersion->GetUserVersionRequest();

    kVersionInfo.m_uiPlatform = pkSLVersion->GetPlatform();

    kVersionInfo.m_bSoftwareVSCapable_Sys = false;

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
NSBShader::ValidityFlag NSBShader::IsImplementationValid(
    NSBImplementation* pkImplementation, VersionInfo& kVersionInfo)
{
    if (!pkImplementation)
        return INVALID;

    // Check the versions vs. the request ones...
    // NOTE: This assumes that implementations are listed
    // in a hardware-need order. ie, the highest hardware
    // requirements will be first in the list!
    NSBRequirements* pkReqs = pkImplementation->GetRequirements();
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
    
    if (pkReqs->GetSoftwareVPRequired())
    {
        if (kVersionInfo.m_bSoftwareVSCapable_Sys)
        {
            pkImplementation->SetSoftwareVP(true);
            bSoftwareVP = true;
        }
        else
        {
            return INVALID;
        }
    }

    if (!bSoftwareVP && uiVSVers > kVersionInfo.m_uiVS_Sys)
    {
        if (pkReqs->GetSoftwareVPAcceptable() &&
            kVersionInfo.m_bSoftwareVSCapable_Sys)
        {
            pkImplementation->SetSoftwareVP(true);
        }
        else
        {
            return INVALID;
        }
    }

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
bool NSBShader::SaveBinaryPackingDefs(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kPackingDefMap.GetCount();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    const char* pcName;
    NSBPackingDef* pkPackingDef;
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
bool NSBShader::LoadBinaryPackingDefs(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    NSBPackingDef* pkPackingDef;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPackingDef = NiNew NSBPackingDef();
        NIASSERT(pkPackingDef);

        if (!pkPackingDef->LoadBinary(kStream))
            return false;

        m_kPackingDefMap.SetAt(pkPackingDef->GetName(), pkPackingDef);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBShader::SaveBinaryImplementations(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kImplementationArray.GetEffectiveSize();
    unsigned int uiSize = m_kImplementationArray.GetSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    NSBImplementation* pkImplementation;

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
bool NSBShader::LoadBinaryImplementations(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    NSBImplementation* pkImplementation;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkImplementation = NiNew NSBImplementation();
        NIASSERT(pkImplementation);

        if (!pkImplementation->LoadBinary(kStream))
            return false;

        NSBRequirements* pkReqs = pkImplementation->GetRequirements();
        if (pkReqs)
        {
            AddPixelShaderVersion(pkReqs->GetPSVersion());
            AddVertexShaderVersion(pkReqs->GetVSVersion());

            if (NSBShader::GetReadVersion() >= 0x00010013)
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
bool NSBShader::SaveBinaryOutputStreamDescriptors(NiBinaryStream& kStream)
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
bool NSBShader::LoadBinaryOutputStreamDescriptors(NiBinaryStream& kStream)
{
    m_kOutputStreamDescriptors.RemoveAll();

    // Version 2.2 added support for stream output...
    if (NSBShader::GetReadVersion() >= 0x00020002)
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
void NSBShader::Dump()
{
    char acName[NI_MAX_PATH];

    NiSprintf(acName, NI_MAX_PATH, "%s.log", m_pcName);
    FILE* pf;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    fopen_s(&pf, acName, "wt");
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    pf = fopen(acName, "wt");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    NSBUtility::Dump(pf, true, "Dumping BinaryShader %s\n", m_pcName);

    NSBUtility::Dump(pf, true, "    Desc: %s", m_pcDescription);
    NSBUtility::Dump(pf, false, "\n");
#if defined(WIN32) || defined(_XENON)
    NSBUtility::Dump(pf, true, "    Min VS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMinVertexShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMinVertexShaderVersion));
    NSBUtility::Dump(pf, true, "    Max VS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMaxVertexShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMaxVertexShaderVersion));
    NSBUtility::Dump(pf, true, "    Min GS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMinGeometryShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMinGeometryShaderVersion));
    NSBUtility::Dump(pf, true, "    Max GS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMaxGeometryShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMaxGeometryShaderVersion));
    NSBUtility::Dump(pf, true, "    Min PS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMinPixelShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMinPixelShaderVersion));
    NSBUtility::Dump(pf, true, "    Max PS Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMaxPixelShaderVersion),
        D3DSHADER_VERSION_MINOR(m_uiMaxPixelShaderVersion));
    NSBUtility::Dump(pf, true, "    Min US Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMinUserVersion),
        D3DSHADER_VERSION_MINOR(m_uiMinUserVersion));
    NSBUtility::Dump(pf, true, "    Max US Version: %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiMaxUserVersion),
        D3DSHADER_VERSION_MINOR(m_uiMaxUserVersion));
#else
    NSBUtility::Dump(pf, true, "    Min VS Version: %d\n",
        m_uiMinVertexShaderVersion);
    NSBUtility::Dump(pf, true, "    Max VS Version: %d\n",
        m_uiMaxVertexShaderVersion);
    NSBUtility::Dump(pf, true, "    Min GS Version: %d\n",
        m_uiMinGeometryShaderVersion);
    NSBUtility::Dump(pf, true, "    Max GS Version: %d\n",
        m_uiMaxGeometryShaderVersion);
    NSBUtility::Dump(pf, true, "    Min PS Version: %d\n",
        m_uiMinPixelShaderVersion);
    NSBUtility::Dump(pf, true, "    Max PS Version: %d\n",
        m_uiMaxPixelShaderVersion);
    NSBUtility::Dump(pf, true, "    Min US Version: %d\n",
        m_uiMinUserVersion);
    NSBUtility::Dump(pf, true, "    Max US Version: %d\n",
        m_uiMaxUserVersion);
#endif

    NSBUtility::Dump(pf, true, "          Platform: 0x%08x\n", m_uiPlatform);
    NSBUtility::Dump(pf, true, "\n");

    NSBUtility::Dump(pf, true, "Global Attribute Table:\n");
    NSBUtility::IndentInsert();
    m_kGlobalAttribTable.Dump(pf);
    NSBUtility::IndentRemove();
    NSBUtility::Dump(pf, true, "\n");

    NSBUtility::Dump(pf, true, "Local Attribute Table:\n");
    NSBUtility::IndentInsert();
    m_kAttribTable.Dump(pf);
    NSBUtility::IndentRemove();
    NSBUtility::Dump(pf, true, "\n");

    NSBUtility::Dump(pf, true, "PackingDef Count: %d\n",
        GetPackingDefCount());
    NSBUtility::Dump(pf, true, "\n");
//    NiTStringPointerMap<NSBPackingDef*> m_kPackingDefMap;

    NSBUtility::Dump(pf, true, "%2d Implementations\n", 
        m_kImplementationArray.GetEffectiveSize());
    for (unsigned int ui = 0; ui < m_kImplementationArray.GetSize(); ui++)
    {
        NSBImplementation* pkImpl = m_kImplementationArray.GetAt(ui);
        if (pkImpl)
        {
            NSBUtility::IndentInsert();
            pkImpl->Dump(pf);
            NSBUtility::IndentRemove();
            NSBUtility::Dump(pf, true, "\n");
        }
    }

    if (pf)
        fclose(pf);
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
