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

#include "NSBD3D10Implementation.h"
#include "NSBD3D10Pass.h"
#include "NSBD3D10Utility.h"
#include "NSBD3D10Shader.h"

#include <NiD3D10BinaryShader.h>
#include <NiD3D10Renderer.h>

//---------------------------------------------------------------------------
NSBD3D10Implementation::NSBD3D10Implementation() :
    m_pcName(0), 
    m_pcDesc(0), 
    m_pcClassName(0), 
    m_uiIndex(0xffffffff), 
    m_pkRequirements(0), 
    m_pkRenderStateGroup(0), 
    m_pcPackingDef(0), 
    m_bSoftwareVP(false) // Unused for D3D10; maintained for legacy streaming.
{
    m_kVertexShaderConstantMaps.RemoveAll();
    m_kGeometryShaderConstantMaps.RemoveAll();
    m_kPixelShaderConstantMaps.RemoveAll();

    m_kPasses.RemoveAll();
    m_kPasses.SetSize(4);
}
//---------------------------------------------------------------------------
NSBD3D10Implementation::~NSBD3D10Implementation()
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
        NSBD3D10ConstantMap* pkCMap = m_kVertexShaderConstantMaps.GetAt(i);
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
        NSBD3D10ConstantMap* pkCMap = m_kGeometryShaderConstantMaps.GetAt(i);
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
        NSBD3D10ConstantMap* pkCMap = m_kPixelShaderConstantMaps.GetAt(i);
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
        NSBD3D10Pass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiDelete pkPass;
            m_kPasses.SetAt(i, NULL);
        }
    }
    m_kPasses.RemoveAll();
}
//---------------------------------------------------------------------------
void NSBD3D10Implementation::SetName(const char* pcName)
{
    NSBD3D10Utility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
void NSBD3D10Implementation::SetDesc(const char* pcDesc)
{
    NSBD3D10Utility::SetString(m_pcDesc, 0, pcDesc);
}
//---------------------------------------------------------------------------
void NSBD3D10Implementation::SetClassName(const char* pcClassName)
{
    NSBD3D10Utility::SetString(m_pcClassName, 0, pcClassName);
}
//---------------------------------------------------------------------------
void NSBD3D10Implementation::SetPackingDef(const char* pcPackingDef)
{
    NSBD3D10Utility::SetString(m_pcPackingDef, 0, pcPackingDef);
}
//---------------------------------------------------------------------------
NSBD3D10Requirements* NSBD3D10Implementation::GetRequirements()
{
    if (m_pkRequirements == 0)
        m_pkRequirements = NiNew NSBD3D10Requirements();

    return m_pkRequirements;
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup* NSBD3D10Implementation::GetRenderStateGroup()
{
    if (m_pkRenderStateGroup == 0)
        m_pkRenderStateGroup = NiNew NSBD3D10StateGroup();

    return m_pkRenderStateGroup;
}
//---------------------------------------------------------------------------
NSBD3D10ConstantMap* NSBD3D10Implementation::GetVertexConstantMap(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kVertexShaderConstantMaps.GetSize());

    if (uiIndex == m_kVertexShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkVertexConstantMap = NiNew NSBD3D10ConstantMap();
        NIASSERT(pkVertexConstantMap);
        pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
        m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
    }
    else if (uiIndex < m_kVertexShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkVertexConstantMap
            = m_kVertexShaderConstantMaps.GetAt(uiIndex);
        if (pkVertexConstantMap == NULL)
        {
            pkVertexConstantMap = NiNew NSBD3D10ConstantMap();
            NIASSERT(pkVertexConstantMap);
            pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
            m_kVertexShaderConstantMaps.ReplaceAt(uiIndex,
                pkVertexConstantMap);
        }
    }

    return m_kVertexShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NSBD3D10ConstantMap* NSBD3D10Implementation::GetGeometryConstantMap(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kGeometryShaderConstantMaps.GetSize());

    if (uiIndex == m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkGeometryConstantMap
            = NiNew NSBD3D10ConstantMap();
        NIASSERT(pkGeometryConstantMap);
        pkGeometryConstantMap->SetProgramType(NiGPUProgram::PROGRAM_GEOMETRY);
        m_kGeometryShaderConstantMaps.Add(pkGeometryConstantMap);
    }
    else if (uiIndex < m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkGeometryConstantMap
            = m_kGeometryShaderConstantMaps.GetAt(uiIndex);
        if (pkGeometryConstantMap == NULL)
        {
            pkGeometryConstantMap = NiNew NSBD3D10ConstantMap();
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
NSBD3D10ConstantMap* NSBD3D10Implementation::GetPixelConstantMap(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kPixelShaderConstantMaps.GetSize());

    if (uiIndex == m_kPixelShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkPixelConstantMap = NiNew NSBD3D10ConstantMap();
        NIASSERT(pkPixelConstantMap);
        pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);
    }
    else if (uiIndex < m_kPixelShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkPixelConstantMap
            = m_kPixelShaderConstantMaps.GetAt(uiIndex);
        if (pkPixelConstantMap == NULL)
        {
            pkPixelConstantMap = NiNew NSBD3D10ConstantMap();
            NIASSERT(pkPixelConstantMap);
            pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
            m_kPixelShaderConstantMaps.ReplaceAt(uiIndex, pkPixelConstantMap);
        }
    }

    return m_kPixelShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Implementation::GetPassCount()
{
    return m_kPasses.GetEffectiveSize();
}
//---------------------------------------------------------------------------
NSBD3D10Pass* NSBD3D10Implementation::GetPass(unsigned int uiIndex,
    bool bCreate)
{
    NSBD3D10Pass* pkPass = 0;
    
    if (m_kPasses.GetSize() > uiIndex)
        pkPass = m_kPasses.GetAt(uiIndex);
    if (!pkPass)
    {
        if (bCreate)
        {
            pkPass = NiNew NSBD3D10Pass();
            NIASSERT(pkPass);

            m_kPasses.SetAtGrow(uiIndex, pkPass);
        }
    }

    return pkPass;
}
//---------------------------------------------------------------------------
bool NSBD3D10Implementation::SaveBinary(NiBinaryStream& kStream)
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
    if (!NSBD3D10Utility::SaveBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;

    unsigned int uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBD3D10Utility::SaveBinaryConstantMap(kStream,
            m_kPixelShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBD3D10Utility::SaveBinaryConstantMap(kStream,
            m_kVertexShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBD3D10Utility::SaveBinaryConstantMap(kStream,
            m_kGeometryShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    if (!SaveBinaryPasses(kStream))
        return false;

    // Not used for D3D10; maintained for legacy streaming.
    unsigned int uiSoftwareVP = m_bSoftwareVP ? 1 : 0;
    NiStreamSaveBinary(kStream, uiSoftwareVP);

    // Version 2.00 added support for semantic adapter tables
    m_kAdapterTable.SaveBinary(kStream);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Implementation::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    m_pcDesc = kStream.ReadCString();

    if (NSBD3D10Shader::GetReadVersion() >= 0x00010005)
    {
        // Version 1.5 added support for user-defined class names
        m_pcClassName = kStream.ReadCString();
    }
    NiStreamLoadBinary(kStream, m_uiIndex);

    if (!LoadBinaryRequirements(kStream))
        return false;

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

    if (!LoadBinaryPackingDef(kStream))
        return false;

    NiDelete m_pkRenderStateGroup;
    m_pkRenderStateGroup = 0;
    if (!NSBD3D10Utility::LoadBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;
    
    m_kVertexShaderConstantMaps.RemoveAll();
    m_kGeometryShaderConstantMaps.RemoveAll();
    m_kPixelShaderConstantMaps.RemoveAll();

    if (NSBD3D10Shader::GetReadVersion() < 0x00010013)
    {
        // Before 1.13, there was exactly one constant map for vertex and
        // pixel shaders.
        NSBD3D10ConstantMap* pkPixelConstantMap = NULL;
        if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
            pkPixelConstantMap))
        {
            return false;
        }
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);

        NSBD3D10ConstantMap* pkVertexConstantMap = NULL;
        if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
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
            NSBD3D10ConstantMap* pkPixelConstantMap = NULL;
            if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
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
            NSBD3D10ConstantMap* pkVertexConstantMap = NULL;
            if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
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
            NSBD3D10ConstantMap* pkGeometryConstantMap = NULL;
            if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
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
    // It's not used for D3D10, but is maintained for legacy streaming.
    if (NSBD3D10Shader::GetReadVersion() >= 0x00010007)
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
    if (NSBD3D10Shader::GetReadVersion() >= 0x00020000)
    {
        m_kAdapterTable.LoadBinary(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Implementation::SetupNiD3D10BinaryShader(
    NiD3D10BinaryShader& kShader, NiShaderDesc* pkShaderDesc,
    NSBD3D10Shader* pkShader)
{
    // Requirements
    if (m_pkRequirements)
    {
        kShader.SetUsesNiRenderState(
            m_pkRequirements->UsesNiRenderState());
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
        kShader.SetBonesPerPartition(0);
        kShader.SetBinormalTangentMethod(
            NiShaderRequirementDesc::NBT_METHOD_NONE);
        kShader.SetBinormalTangentUVSource(
            NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT);
        kShader.SetBoneMatrixRegisters(0);
        kShader.SetBoneCalcMethod(NiD3D10BinaryShader::BONECALC_SKIN);
    }

    // Setup the 'global' constant mappings

    // Vertex
    unsigned int uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBD3D10ConstantMap* pkCM = m_kVertexShaderConstantMaps.GetAt(i);
            if (pkCM)
            {
                kShader.SetVertexConstantMap(i,
                    pkCM->GetVertexConstantMap(pkShaderDesc));
            }
            else
            {
                kShader.SetVertexConstantMap(i, NULL);
            }
        }
    }

    // Geometry
    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBD3D10ConstantMap* pkCM
                = m_kGeometryShaderConstantMaps.GetAt(i);
            if (pkCM)
            {
                kShader.SetGeometryConstantMap(i,
                    pkCM->GetGeometryConstantMap(pkShaderDesc));
            }
            else
            {
                kShader.SetGeometryConstantMap(i, NULL);
            }
        }
    }

    // Stream Out
    kShader.SetOutputStreamDescriptors(pkShader->GetOutputStreamDescriptors());

    // Pixel
    uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBD3D10ConstantMap* pkCM
                = m_kPixelShaderConstantMaps.GetAt(i);
            if (pkCM)
            {
                kShader.SetPixelConstantMap(i,
                    pkCM->GetPixelConstantMap(pkShaderDesc));
            }
            else
            {
                kShader.SetPixelConstantMap(i, NULL);
            }
        }
    }

    // 'Global' render states
    NiD3D10RenderStateGroup* pkRSGroup = 0;
    if (m_pkRenderStateGroup)
    {
        if (m_pkRenderStateGroup->GetStateCount())
        {
            pkRSGroup = NiNew NiD3D10RenderStateGroup();
            
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
    NSBD3D10Pass* pkNSBD3D10Pass;
    NiD3D10PassPtr spPass;
    unsigned int uiCurrPass = 0;
    unsigned int uiPassCount = m_kPasses.GetSize();
    for (unsigned int ui = 0; ui < uiPassCount; ui++)
    {
        pkNSBD3D10Pass = m_kPasses.GetAt(ui);
        if (pkNSBD3D10Pass)
        {

            if (m_bSoftwareVP)
                pkNSBD3D10Pass->SetSoftwareVertexProcessing(true);

            spPass = NiNew NiD3D10Pass();
            if (!pkNSBD3D10Pass->SetupNiD3D10Pass(*spPass, pkShaderDesc))
            {
                // FAILED!
                return false;
            }
            else
            {
                kShader.InsertPass(uiCurrPass, spPass);
                uiCurrPass++;
            }

            // Confirm that a packing definition is present if any pass
            // has a vertex shader
            if (bNeedPackingDef && pkNSBD3D10Pass->GetShaderPresent(
                NiGPUProgram::PROGRAM_VERTEX))
            {
                NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    false, "* ERROR:  "
                    "NSBD3D10Implementation::SetupNiD3D10BinaryShader\nFailed"
                    " to find packing definition for implementation that uses"
                    " vertex shaders.\nShader %s, implementation %s\n", 
                    kShader.GetName(), m_pcName);
                return false;
            }
        }
    }

    m_spShader = &kShader;

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Implementation::SaveBinaryRequirements(NiBinaryStream& kStream)
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
bool NSBD3D10Implementation::SaveBinaryPackingDef(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcPackingDef);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Implementation::SaveBinaryPasses(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kPasses.GetEffectiveSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBD3D10Pass* pkPass;
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
bool NSBD3D10Implementation::LoadBinaryRequirements(NiBinaryStream& kStream)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 1)
    {
        m_pkRequirements = NiNew NSBD3D10Requirements();
        NIASSERT(m_pkRequirements);

        if (!m_pkRequirements->LoadBinary(kStream))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Implementation::LoadBinaryPackingDef(NiBinaryStream& kStream)
{
    m_pcPackingDef = kStream.ReadCString();
    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Implementation::LoadBinaryPasses(NiBinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    m_kPasses.SetSize(uiCount);

    NSBD3D10Pass* pkPass;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPass = NiNew NSBD3D10Pass();
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
void NSBD3D10Implementation::Dump(FILE* pf)
{
    NSBD3D10Utility::Dump(pf, true, "Implementation %2d - %s\n", m_uiIndex, 
        m_pcName);

    NSBD3D10Utility::IndentInsert();
    
    NSBD3D10Utility::Dump(pf, true, "Desc: %s", m_pcDesc);
    NSBD3D10Utility::Dump(pf, false, "\n");

    NSBD3D10Utility::Dump(pf, true, "ClassName: %s", m_pcClassName);
    NSBD3D10Utility::Dump(pf, false, "\n");

    NSBD3D10Utility::Dump(pf, true, "Software Vertex Processing: %s\n", 
        m_bSoftwareVP ? "True" : "False");
    NSBD3D10Utility::Dump(pf, true, "\n");

    if (m_pkRequirements)
        m_pkRequirements->Dump(pf);
    else
        NSBD3D10Utility::Dump(pf, true, "*** NO REQUIREMENTS ***\n");

    NSBD3D10Utility::Dump(pf, true, "PackingDef: %s\n", m_pcPackingDef);

    if (m_pkRenderStateGroup)
    {
        NSBD3D10Utility::Dump(pf, true, "RenderStateGroup\n");
        NSBD3D10Utility::IndentInsert();
        m_pkRenderStateGroup->Dump(pf, NSBD3D10StateGroup::DUMP_RENDERSTATES);
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "***   NO RS GROUP   ***\n");
    }

    unsigned int uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBD3D10Utility::Dump(pf, true, "VertexConstantMaps\n");
        NSBD3D10Utility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kVertexShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "*** NO VERTEX CMAPS  ***\n");
    }

    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBD3D10Utility::Dump(pf, true, "GeometryConstantMaps\n");
        NSBD3D10Utility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kGeometryShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "*** NO GEOMETRY CMAPS  ***\n");
    }

    uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBD3D10Utility::Dump(pf, true, "PixelConstantMaps\n");
        NSBD3D10Utility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kPixelShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "***  NO PIXEL CMAPS  ***\n");
    }

    unsigned int uiCount = m_kPasses.GetSize();
    NSBD3D10Utility::Dump(pf, true, "%d Passes\n", 
        m_kPasses.GetEffectiveSize());
    NSBD3D10Utility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBD3D10Pass* pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
            pkPass->Dump(pf);
    }
    NSBD3D10Utility::IndentRemove();

    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
