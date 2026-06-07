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

#include <NiBinaryShader.h>
#include <NiShaderFactory.h>

#include "NSBImplementation.h"
#include "NSBPass.h"
#include "NSBUtility.h"
#include "NSBShader.h"

#include <NiBinaryShader.h>

#if defined(WIN32) || defined(_XENON)
#include <NiD3DRendererHeaders.h>
#endif

//---------------------------------------------------------------------------
NSBImplementation::NSBImplementation() :
    m_pcName(0), 
    m_pcDesc(0), 
    m_pcClassName(0), 
    m_uiIndex(0xffffffff), 
    m_pkRequirements(0), 
    m_pcPackingDef(0), 
    m_pkRenderStateGroup(0), 
    m_bSoftwareVP(false)
{
    m_kVertexShaderConstantMaps.RemoveAll();
    m_kGeometryShaderConstantMaps.RemoveAll();
    m_kPixelShaderConstantMaps.RemoveAll();

    m_kPasses.RemoveAll();
    m_kPasses.SetSize(4);
}
//---------------------------------------------------------------------------
NSBImplementation::~NSBImplementation()
{
    NiDelete m_pkRenderStateGroup;
    NiDelete m_pkRequirements;
    NiFree(m_pcPackingDef);
    NiFree(m_pcClassName);
    NiFree(m_pcDesc);
    NiFree(m_pcName);

    // Clean up the shader constant maps.

    unsigned int uiSize = m_kVertexShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kVertexShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kVertexShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kVertexShaderConstantMaps.RemoveAll();
    
    uiSize = m_kGeometryShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kGeometryShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kGeometryShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kGeometryShaderConstantMaps.RemoveAll();

    uiSize = m_kPixelShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kPixelShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kPixelShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kPixelShaderConstantMaps.RemoveAll();

    // Clean up the passes.
    uiSize = m_kPasses.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBPass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiDelete pkPass;
            m_kPasses.SetAt(i, NULL);
        }
    }
    m_kPasses.RemoveAll();
}
//---------------------------------------------------------------------------
void NSBImplementation::SetName(const char* pcName)
{
    NiRendererUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
void NSBImplementation::SetDesc(const char* pcDesc)
{
    NiRendererUtility::SetString(m_pcDesc, 0, pcDesc);
}
//---------------------------------------------------------------------------
void NSBImplementation::SetClassName(const char* pcClassName)
{
    NiRendererUtility::SetString(m_pcClassName, 0, pcClassName);
}
//---------------------------------------------------------------------------
void NSBImplementation::SetPackingDef(const char* pcPackingDef)
{
    NiRendererUtility::SetString(m_pcPackingDef, 0, pcPackingDef);
}
//---------------------------------------------------------------------------
NSBRequirements* NSBImplementation::GetRequirements()
{
    if (m_pkRequirements == 0)
        m_pkRequirements = NiNew NSBRequirements();

    return m_pkRequirements;
}
//---------------------------------------------------------------------------
NSBStateGroup* NSBImplementation::GetRenderStateGroup()
{
    if (m_pkRenderStateGroup == 0)
        m_pkRenderStateGroup = NiNew NSBStateGroup();

    return m_pkRenderStateGroup;
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBImplementation::GetVertexConstantMap(unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kVertexShaderConstantMaps.GetSize());

    if (uiIndex == m_kVertexShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkVertexConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkVertexConstantMap);
        pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
        m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
    }
    else if (uiIndex < m_kVertexShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkVertexConstantMap
            = m_kVertexShaderConstantMaps.GetAt(uiIndex);
        if (pkVertexConstantMap == NULL)
        {
            pkVertexConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkVertexConstantMap);
            pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
            m_kVertexShaderConstantMaps.ReplaceAt(uiIndex,
                pkVertexConstantMap);
        }
    }

    return m_kVertexShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBImplementation::GetGeometryConstantMap(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kGeometryShaderConstantMaps.GetSize());

    if (uiIndex == m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkGeometryConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkGeometryConstantMap);
        pkGeometryConstantMap->SetProgramType(NiGPUProgram::PROGRAM_GEOMETRY);
        m_kGeometryShaderConstantMaps.Add(pkGeometryConstantMap);
    }
    else if (uiIndex < m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkGeometryConstantMap
            = m_kGeometryShaderConstantMaps.GetAt(uiIndex);
        if (pkGeometryConstantMap == NULL)
        {
            pkGeometryConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkGeometryConstantMap);
            pkGeometryConstantMap->SetProgramType(
                NiGPUProgram::PROGRAM_GEOMETRY);
            m_kGeometryShaderConstantMaps.ReplaceAt(uiIndex,
                pkGeometryConstantMap);
        }
    }

    return m_kGeometryShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBImplementation::GetPixelConstantMap(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kPixelShaderConstantMaps.GetSize());

    if (uiIndex == m_kPixelShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkPixelConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkPixelConstantMap);
        pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);
    }
    else if (uiIndex < m_kPixelShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkPixelConstantMap
            = m_kPixelShaderConstantMaps.GetAt(uiIndex);
        if (pkPixelConstantMap == NULL)
        {
            pkPixelConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkPixelConstantMap);
            pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
            m_kPixelShaderConstantMaps.ReplaceAt(uiIndex, pkPixelConstantMap);
        }
    }

    return m_kPixelShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NSBImplementation::GetPassCount()
{
    return m_kPasses.GetEffectiveSize();
}
//---------------------------------------------------------------------------
NSBPass* NSBImplementation::GetPass(unsigned int uiIndex, bool bCreate)
{
    NSBPass* pkPass = 0;
    
    if (m_kPasses.GetSize() > uiIndex)
        pkPass = m_kPasses.GetAt(uiIndex);
    if (!pkPass)
    {
        if (bCreate)
        {
            pkPass = NiNew NSBPass();
            NIASSERT(pkPass);

            m_kPasses.SetAtGrow(uiIndex, pkPass);
        }
    }

    return pkPass;
}
//---------------------------------------------------------------------------
bool NSBImplementation::SaveBinary(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    kStream.WriteCString(m_pcDesc);

    // Version 1.5 added a user-defined class name to the implementation
    kStream.WriteCString(m_pcClassName);
    NiStreamSaveBinary(kStream, m_uiIndex);
    if (!SaveBinaryRequirements(kStream))
        return false;

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

    if (!SaveBinaryPackingDef(kStream))
        return false;
    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;

    unsigned int uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kPixelShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kVertexShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kGeometryShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    if (!SaveBinaryPasses(kStream))
        return false;

    unsigned int uiSoftwareVP = m_bSoftwareVP ? 1 : 0;
    NiStreamSaveBinary(kStream, uiSoftwareVP);

    // Version 2.00 added support for semantic adapter tables
    m_kAdapterTable.SaveBinary(kStream);

    return true;
}
//---------------------------------------------------------------------------
bool NSBImplementation::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    m_pcDesc = kStream.ReadCString();

    if (NSBShader::GetReadVersion() >= 0x00010005)
    {
        // Version 1.5 added support for user-defined class names
        m_pcClassName = kStream.ReadCString();
    }
    NiStreamLoadBinary(kStream, m_uiIndex);

    if (!LoadBinaryRequirements(kStream))
        return false;

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

    if (!LoadBinaryPackingDef(kStream))
        return false;

    NiDelete m_pkRenderStateGroup;
    m_pkRenderStateGroup = 0;
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;
    
    m_kVertexShaderConstantMaps.RemoveAll();
    m_kGeometryShaderConstantMaps.RemoveAll();
    m_kPixelShaderConstantMaps.RemoveAll();

    if (NSBShader::GetReadVersion() < 0x00010013)
    {
        // Before 1.13, there was exactly one constant map for vertex and
        // pixel shaders.
        NSBConstantMap* pkPixelConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkPixelConstantMap))
        {
            return false;
        }
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);

        NSBConstantMap* pkVertexConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkVertexConstantMap))
        {
            return false;
        }
        m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
    }
    else
    {
        // Version 1.13 added support for multiple constant maps, D3D10, and
        // geometry shaders.

        unsigned int uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkPixelConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkPixelConstantMap))
            {
                return false;
            }
            m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);
        }
        
        uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkVertexConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkVertexConstantMap))
            {
                return false;
            }
            m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
        }
        
        uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkGeometryConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkGeometryConstantMap))
            {
                return false;
            }
            m_kGeometryShaderConstantMaps.Add(pkGeometryConstantMap);
        }
    }

    if (!LoadBinaryPasses(kStream))
        return false;

    // Version 1.7 added better support for software vertex processing.
    if (NSBShader::GetReadVersion() >= 0x00010007)
    {
        unsigned int uiSoftwareVP = 0;
        NiStreamLoadBinary(kStream, uiSoftwareVP);
        m_bSoftwareVP = (uiSoftwareVP != 0);
    }
    else
    {
        m_bSoftwareVP = false;
    }

    // Version 2.00 added support for semantic adapter tables
    if (NSBShader::GetReadVersion() >= 0x00020000)
    {
        m_kAdapterTable.LoadBinary(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBImplementation::SetupNiBinaryShader(NiBinaryShader& kShader, 
    NiShaderDesc* pkShaderDesc, NSBShader*)
{
    // Requirements
    if (m_pkRequirements)
    {
        kShader.SetUsesNiRenderState(
            m_pkRequirements->UsesNiRenderState());
        kShader.SetUsesNiLightState(
            m_pkRequirements->UsesNiLightState());
        kShader.SetBonesPerPartition(
            m_pkRequirements->GetBonesPerPartition());
        kShader.SetBinormalTangentMethod(
            m_pkRequirements->GetBinormalTangentMethod());
        kShader.SetBinormalTangentUVSource(
            m_pkRequirements->GetBinormalTangentUVSource());
        kShader.SetBoneMatrixRegisters(
            m_pkRequirements->GetBoneMatrixRegisters());
        kShader.SetBoneCalcMethod(
            m_pkRequirements->GetBoneCalcMethod());
    }
    else
    {
        kShader.SetUsesNiRenderState(false);
        kShader.SetUsesNiLightState(false);
        kShader.SetBonesPerPartition(0);
        kShader.SetBinormalTangentMethod(
            NiShaderRequirementDesc::NBT_METHOD_NONE);
        kShader.SetBinormalTangentUVSource(
            NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT);
        kShader.SetBoneMatrixRegisters(0);
        kShader.SetBoneCalcMethod(NiBinaryShader::BONECALC_SKIN);
    }

    // Setup the 'global' constant mappings

    // Vertex
    if (m_kVertexShaderConstantMaps.GetSize() > 0)
    {
        NSBConstantMap* pkCM = m_kVertexShaderConstantMaps.GetAt(0);
        if (pkCM)
        {
            kShader.SetVertexConstantMap(pkCM->GetVertexConstantMap(
                pkShaderDesc));
        }
        else
        {
            kShader.SetVertexConstantMap(NULL);
        }
    }
    else
    {
        kShader.SetVertexConstantMap(NULL);
    }

    // Pixel
    if (m_kPixelShaderConstantMaps.GetSize() > 0)
    {
        NSBConstantMap* pkCM = m_kPixelShaderConstantMaps.GetAt(0);
        if (pkCM)
        {
            kShader.SetPixelConstantMap(pkCM->GetPixelConstantMap(
                pkShaderDesc));
        }
        else
        {
            kShader.SetPixelConstantMap(NULL);
        }
    }
    else
    {
        kShader.SetPixelConstantMap(NULL);
    }

    // 'Global' render states
    NiD3DRenderStateGroup* pkRSGroup = 0;
    if (m_pkRenderStateGroup)
    {
        if (m_pkRenderStateGroup->GetStateCount())
        {
            pkRSGroup = NiNew NiD3DRenderStateGroup();
            
            NIASSERT(pkRSGroup);

            if (!m_pkRenderStateGroup->SetupRenderStateGroup(*pkRSGroup))
            {
                NiDelete pkRSGroup;
                pkRSGroup = 0;
                return false;
            }
        }
    }
    kShader.SetRenderStateGroup(pkRSGroup);


    bool bNeedPackingDef = m_pcPackingDef == NULL;

    // Semantic adapter table support
    NiSemanticAdapterTable& kTable = GetSemanticAdapterTable();
    if (kTable.GetNumFilledTableEntries())
    {
        // Valid semantic adapter table present, copy it to the shader
        // and ensure that we don't create/require a shader declaration
        kShader.GetSemanticAdapterTable() = kTable;
        bNeedPackingDef = false;
    }

    // Now, create the passes
    NSBPass* pkNSBPass;
    NiPointer<NiPlatformShaderPass> spPass;
    unsigned int uiCurrPass = 0;
    unsigned int uiPassCount = m_kPasses.GetSize();
    for (unsigned int ui = 0; ui < uiPassCount; ui++)
    {
        pkNSBPass = m_kPasses.GetAt(ui);
        if (pkNSBPass)
        {

            if (m_bSoftwareVP)
                pkNSBPass->SetSoftwareVertexProcessing(true);

            spPass = NiNew NiPlatformShaderPass();
            if (!pkNSBPass->SetupShaderPass(*spPass, pkShaderDesc))
            {
                // FAILED!
                return false;
            }
            else
            {
                kShader.InsertPass(uiCurrPass, spPass);
                uiCurrPass++;
            }

            // If we have neither a PackingDef, nor a SemanticAdapterTable, the
            // PS3 generates a SAT, while the other platforms error out
            if (bNeedPackingDef && pkNSBPass->GetShaderPresent(
                NiGPUProgram::PROGRAM_VERTEX))
            {
#if defined(_PS3)
                // Create new packing definition
                NIASSERT(pkShader);
                SetPackingDef("GENERATED_PACKING_DEF");
                NSBPackingDef* pkPackingDef = 
                    pkShader->GetPackingDef(m_pcPackingDef, true); 

                // Get shader declaration from vertex shader
                // using the Vertex Program's parameters
                NiPS3CgShaderProgram* pkVP = spPass->GetVertexShader();
                if (pkVP)
                {
                    // Autogenerate it
                    pkPackingDef->GenerateDeclarationFromVertexShader(pkVP);

                    NiShaderDeclarationPtr spShaderDecl =
                        pkPackingDef->GetShaderDeclaration(
                            NiShader::NISHADER_AGNOSTIC);
                       
                    // Conver the declaration to a semantic adapter table
                    kShader.SetSemanticAdapterTableFromShaderDeclaration(
                        spShaderDecl);

                    bNeedPackingDef = false;
                }
#else
                // Confirm that a packing definition is present if any pass
                // has a vertex shader
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    false, "* ERROR: NSBImplementation::SetupNiBinaryShader\n"
                    "    Failed to find packing definition or semantic "
                    "adapter table for an implementation that uses vertex "
                    "shaders.\nShader %s, implementation %s\n", 
                    kShader.GetName(), m_pcName);
                return false;
#endif
            }
        }
    }

    m_spShader = &kShader;

    return true;
}
//---------------------------------------------------------------------------
bool NSBImplementation::SaveBinaryRequirements(NiBinaryStream& kStream)
{
    unsigned int uiValue;

    if (m_pkRequirements)
    {
        uiValue = 1;
        NiStreamSaveBinary(kStream, uiValue);

        if (!m_pkRequirements->SaveBinary(kStream))
            return false;
    }
    else
    {
        uiValue = 0;
        NiStreamSaveBinary(kStream, uiValue);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBImplementation::SaveBinaryPackingDef(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcPackingDef);

    return true;
}
//---------------------------------------------------------------------------
bool NSBImplementation::SaveBinaryPasses(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kPasses.GetEffectiveSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBPass* pkPass;
    for (unsigned int ui = 0; ui < m_kPasses.GetSize(); ui++)
    {
        pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
        {
            if (!pkPass->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBImplementation::LoadBinaryRequirements(NiBinaryStream& kStream)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 1)
    {
        m_pkRequirements = NiNew NSBRequirements();
        NIASSERT(m_pkRequirements);

        if (!m_pkRequirements->LoadBinary(kStream))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBImplementation::LoadBinaryPackingDef(NiBinaryStream& kStream)
{
    m_pcPackingDef = kStream.ReadCString();
    return true;
}
//---------------------------------------------------------------------------
bool NSBImplementation::LoadBinaryPasses(NiBinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    m_kPasses.SetSize(uiCount);

    NSBPass* pkPass;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPass = NiNew NSBPass();
        NIASSERT(pkPass);

        if (!pkPass->LoadBinary(kStream))
            return false;

        m_kPasses.SetAt(ui, pkPass);

        // If any pass requires SW Vertex processing, then the entire
        // implementation does.
        if (pkPass->GetSoftwareVertexProcessing())
            m_pkRequirements->SetSoftwareVPRequired(true);
    }

    return true;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBImplementation::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Implementation %2d - %s\n", m_uiIndex, 
        m_pcName);

    NSBUtility::IndentInsert();
    
    NSBUtility::Dump(pf, true, "Desc: %s", m_pcDesc);
    NSBUtility::Dump(pf, false, "\n");

    NSBUtility::Dump(pf, true, "ClassName: %s", m_pcClassName);
    NSBUtility::Dump(pf, false, "\n");

    NSBUtility::Dump(pf, true, "Software Vertex Processing: %s\n", 
        m_bSoftwareVP ? "True" : "False");
    NSBUtility::Dump(pf, true, "\n");

    if (m_pkRequirements)
        m_pkRequirements->Dump(pf);
    else
        NSBUtility::Dump(pf, true, "*** NO REQUIREMENTS ***\n");

    NSBUtility::Dump(pf, true, "PackingDef: %s\n", m_pcPackingDef);

    if (m_pkRenderStateGroup)
    {
        NSBUtility::Dump(pf, true, "RenderStateGroup\n");
        NSBUtility::IndentInsert();
        m_pkRenderStateGroup->Dump(pf, NSBStateGroup::DUMP_RENDERSTATES);
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***   NO RS GROUP   ***\n");
    }

    unsigned int uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "VertexConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kVertexShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "*** NO VERTEX CMAPS  ***\n");
    }

    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "GeometryConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kGeometryShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "*** NO GEOMETRY CMAPS  ***\n");
    }

    uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "PixelConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kPixelShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***  NO PIXEL CMAPS  ***\n");
    }

    unsigned int uiCount = m_kPasses.GetSize();
    NSBUtility::Dump(pf, true, "%d Passes\n", 
        m_kPasses.GetEffectiveSize());
    NSBUtility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBPass* pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
            pkPass->Dump(pf);
    }
    NSBUtility::IndentRemove();

    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
