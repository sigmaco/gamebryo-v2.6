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

// Precompiled Header
#include "NiD3D10RendererPCH.h"

#include "NiD3D10ShaderConstantMap.h"

#include "NiD3D10DataStream.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10Shader.h"
#include "NiD3D10ShaderFactory.h"
#include "NiD3D10Utility.h"

#include <NiAmbientLight.h>
#include <NiBooleanExtraData.h>
#include <NiColorExtraData.h>
#include <NiDirectionalLight.h>
#include <NiFloatExtraData.h>
#include <NiFloatsExtraData.h>
#include <NiFogProperty.h>
#include <NiRenderObject.h>
#include <NiIntegerExtraData.h>
#include <NiIntegersExtraData.h>
#include <NiMaterialProperty.h>
#include <NiPointLight.h>
#include <NiSCMExtraData.h>
#include <NiShaderAttributeDesc.h>
#include <NiShadowGenerator.h>
#include <NiShadowManager.h>
#include <NiShadowMap.h>
#include <NiSkinningMeshModifier.h>
#include <NiSpotLight.h>
#include <NiTextureEffect.h>
#include <NiTexturingProperty.h>

// -2 ^ 20. This number is used to create the light position for directional
// lights.
const float NiD3D10ShaderConstantMap::ms_fDirLightDistance = -1048576.0f;

//---------------------------------------------------------------------------
NiD3D10ShaderConstantMap::NiD3D10ShaderConstantMap(
    NiGPUProgram::ProgramType eType) :
    NiShaderConstantMap(eType),
    m_bConstantBufferCurrent(false),
    m_bExternalStream(false)
{
    m_uiShaderConstantBufferIndex = (NiUInt32)BUFFER_INDEX_UNASSIGNED;
    m_aspEntries.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantMap::~NiD3D10ShaderConstantMap()
{
    // We need to release any entries which are global
    for (NiUInt32 ui = 0; ui < m_aspEntries.GetAllocatedSize(); ui++)
    {
        NiShaderConstantMapEntryPtr spEntry = m_aspEntries.GetAt(ui);
        if (spEntry && spEntry->IsGlobal())
        {
            NiFixedString kKey= spEntry->GetKey();
            NiD3D10ShaderFactory::ReleaseGlobalShaderConstant(kKey);
        }
    }

    m_aspEntries.RemoveAll();
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::AddEntry(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiShaderRegister, 
    unsigned int uiRegisterCount, const char* pcVariableName, 
    unsigned int uiDataSize, unsigned int uiDataStride, 
    const void* pvDataSource, bool bCopyData)
{
    NiShaderError eLastError = NISHADERERR_OK;

    // See if the entry is in the list...
    NiShaderConstantMapEntry* pkEntry = GetEntry(pcKey);
    if (pkEntry && pkEntry->GetExtra() == uiExtra &&
        pkEntry->GetFlags() == uiFlags)
    {
        // Was already in the list. Return an error
        eLastError = NISHADERERR_DUPLICATEENTRYKEY;
        return eLastError;
    }

    // Check what the entry is
    if (NiShaderConstantMapEntry::IsAttribute(uiFlags))
    {
        eLastError = AddAttributeEntry(pcKey, uiFlags, uiExtra, 
            uiShaderRegister, uiRegisterCount, pcVariableName, uiDataSize, 
            uiDataStride, pvDataSource, bCopyData);
    }
    else if (NiShaderConstantMapEntry::IsConstant(uiFlags))
    {
        eLastError = AddConstantEntry(pcKey, uiFlags, uiExtra, 
            uiShaderRegister, uiRegisterCount, pcVariableName, uiDataSize, 
            uiDataStride, pvDataSource, bCopyData);
    }
    else if (NiShaderConstantMapEntry::IsDefined(uiFlags))
    {
        eLastError = AddPredefinedEntry(pcKey, uiExtra, 
            uiShaderRegister, pcVariableName);
    }
    else if (NiShaderConstantMapEntry::IsGlobal(uiFlags))
    {
        eLastError = AddGlobalEntry(pcKey, uiFlags, uiExtra, 
            uiShaderRegister, uiRegisterCount, pcVariableName, uiDataSize, 
            uiDataStride, pvDataSource, bCopyData);
    }
    else if (NiShaderConstantMapEntry::IsOperator(uiFlags))
    {
        eLastError = AddOperatorEntry(pcKey, uiFlags, uiExtra, 
            uiShaderRegister, uiRegisterCount, pcVariableName);
    }
    else if (NiShaderConstantMapEntry::IsObject(uiFlags))
    {
        eLastError = AddObjectEntry(pcKey, uiShaderRegister,
            pcVariableName, uiExtra,
            NiShaderConstantMapEntry::GetObjectType(uiFlags));
    }
    else
    {
        NIASSERT(!"ConstantMap AddEntry - Invalid mapping type!");
        eLastError = NISHADERERR_INVALIDMAPPING;
    }

    // Ensure the register count was properly recorded for those cases where
    // the register count is not passed through to the Add*Entry function.
    if (IsRegisterEncoded(uiShaderRegister) &&
        eLastError == NISHADERERR_OK)
    {
        NIASSERT(IsRegisterEncoded(uiRegisterCount));
        NiShaderConstantMapEntry* pkFoundEntry = NULL;

        NiUInt32 uiEntry = m_aspEntries.GetSize();
        // Start looking at the last entry
        while (uiEntry--)
        {
            NiShaderConstantMapEntry* pkEntry = m_aspEntries.GetAt(uiEntry);
            if (pkEntry)
            {
                // Entries do not necessarily have unique keys
                // Verify encoded register, which will be unique
                if (uiShaderRegister == pkEntry->GetShaderRegister())
                {
                    pkFoundEntry = pkEntry;
                    break;
                }
            }
        }
        NIASSERT(pkFoundEntry && 
            NiStricmp(pcKey, pkFoundEntry->GetKey()) == 0);

        pkFoundEntry->SetRegisterCount(uiRegisterCount);
    }

    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::AddPredefinedEntry(const char* pcKey, 
    unsigned int uiExtra, unsigned int uiShaderRegister, 
    const char* pcVariableName)
{
    NiShaderError eLastError = NISHADERERR_OK;

    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    pkEntry->SetKey(pcKey);
    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetVariableName(pcVariableName);

    // Set the flags to just the DEFINED type. The setup predefined call
    // will fill in the position masks
    pkEntry->SetFlags(NiShaderConstantMapEntry::SCME_MAP_DEFINED);

    // Look-up and set the data.
    eLastError = SetupPredefinedEntry(pkEntry);

    // Insert it!
    if (eLastError == NISHADERERR_OK)
    {
        eLastError = InsertEntry(pkEntry);
    }
    else
    {
        NiDelete pkEntry;
    }

    // Check for an already-encoded shader register value.
    NiUInt32 uiRegister;
    NiUInt32 uiElements;
    bool bPacked;
    if (DecodePackedRegisterAndElement(
        uiShaderRegister, 
        uiRegister, 
        uiElements, 
        bPacked))
    {
    }
    else if (uiShaderRegister != SCM_REGISTER_NONE)
    {
        // If it's not encoded, then we need encode it with the
        // buffer offset/size for the data to occupy.
        SetConstantBufferObsolete();
    }

    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::AddAttributeEntry(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiShaderRegister, 
    unsigned int uiRegisterCount, const char* pcVariableName, 
    unsigned int uiDataSize, unsigned int uiDataStride, 
    const void* pvDataSource, bool bCopyData)
{
    NiShaderError eLastError = NISHADERERR_OK;

    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    // Attributes will have to be retrieved from the geometry each time they
    // are set...
    pkEntry->SetKey(pcKey);

    // Make sure the flags are set correctly
    // Clear the old map
    uiFlags &= ~NiShaderConstantMapEntry::SCME_MAP_MASK;
    // Set the confirmed one
    uiFlags |= NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;

    pkEntry->SetFlags(uiFlags);

    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetRegisterCount(uiRegisterCount);
    pkEntry->SetVariableName(pcVariableName);

    pkEntry->SetData(uiDataSize, uiDataStride, (void*)pvDataSource, 
        bCopyData);

    // Insert it!
    eLastError = InsertEntry(pkEntry);
    if (eLastError != NISHADERERR_OK)
    {
        NiDelete pkEntry;
    }

    // Check for an already-encoded shader register value.
    NiUInt32 uiRegister;
    NiUInt32 uiElements;
    bool bPacked;
    if (DecodePackedRegisterAndElement(
        uiShaderRegister, 
        uiRegister, 
        uiElements, 
        bPacked))
    {
        // Ensure that the register count is also encoded.
        NIASSERT(DecodePackedRegisterAndElement(
            uiRegisterCount,
            uiRegister,
            uiElements,
            bPacked));
    }
    else if (uiShaderRegister != SCM_REGISTER_NONE)
    {
        // If it's not encoded, then we need encode it with the
        // buffer offset/size for the data to occupy.
        SetConstantBufferObsolete();
    }

    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::AddConstantEntry(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiShaderRegister, 
    unsigned int uiRegisterCount, const char* pcVariableName, 
    unsigned int uiDataSize, unsigned int uiDataStride, 
    const void* pvDataSource, bool bCopyData)
{
    NiShaderError eLastError = NISHADERERR_OK;

    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    pkEntry->SetKey(pcKey);
    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetRegisterCount(uiRegisterCount);
    pkEntry->SetVariableName(pcVariableName);

    // Clear the old map
    uiFlags &= ~NiShaderConstantMapEntry::SCME_MAP_MASK;
    // Set the confirmed one
    uiFlags |= NiShaderConstantMapEntry::SCME_MAP_CONSTANT;

    if (!IsRegisterEncoded(uiShaderRegister) &&
        uiShaderRegister != SCM_REGISTER_NONE)
    {
        // Store the constant's attribute or register size
        uiFlags &= ~NiShaderConstantMapEntry::GetAttributeMask();
        if (uiDataSize == sizeof(float))
        {
            uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
        }
        if (uiDataSize == 2 * sizeof(float))
        {
            uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
        }
        else if (uiDataSize == 3 * sizeof(float))
        {
            uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
        }
        else if (uiDataSize == 4 * sizeof(float))
        {
            uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        else if (uiDataSize == 8 * sizeof(float))
        {
            uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8;
        }
        else if (uiDataSize == 9 * sizeof(float))
        {
            uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
        }
        else if (uiDataSize == 12 * sizeof(float))
        {
            uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12;
        }
        else if (uiDataSize == 16 * sizeof(float))
        {
            uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        else
        {
            // Encode this odd size - assume fully packed data
            NIASSERT(uiDataSize % sizeof(float) == 0);
            NiUInt32 uiNumFloats = uiDataSize / sizeof(float);

            NiUInt32 uiStartRegister = 0;
            NiUInt32 uiStartElement = 0;
            NiUInt32 uiEncodedStartRegister;
            NiUInt32 uiEncodedRegisterCount;
            CalculatePackingEntry(uiStartRegister, uiStartElement, 
                uiNumFloats / 4, uiNumFloats % 4, uiEncodedStartRegister, 
                uiEncodedRegisterCount, true);
            pkEntry->SetShaderRegister(uiEncodedStartRegister);
            pkEntry->SetRegisterCount(uiEncodedRegisterCount);
            pkEntry->SetVariableHookupValid(true);
        }
    }

    pkEntry->SetFlags(uiFlags);

    pkEntry->SetData(uiDataSize, uiDataStride, (void*)pvDataSource, 
        bCopyData);

    // Insert it!
    eLastError = InsertEntry(pkEntry);
    if (eLastError != NISHADERERR_OK)
    {
        NiDelete pkEntry;
    }

    // Check for an already-encoded shader register value.
    NiUInt32 uiRegister;
    NiUInt32 uiElements;
    bool bPacked;
    if (DecodePackedRegisterAndElement(
        uiShaderRegister, 
        uiRegister, 
        uiElements, 
        bPacked))
    {
        // Ensure that the register count is also encoded.
        NIASSERT(DecodePackedRegisterAndElement(
            uiRegisterCount,
            uiRegister,
            uiElements,
            bPacked));
    }
    else if (uiShaderRegister != SCM_REGISTER_NONE)
    {
        // If it's not encoded, then we need encode it with the
        // buffer offset/size for the data to occupy.
        SetConstantBufferObsolete();
    }

    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::AddGlobalEntry(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiShaderRegister, 
    unsigned int uiRegisterCount, const char* pcVariableName, 
    unsigned int uiDataSize, unsigned int uiDataStride, 
    const void* pvDataSource, bool bCopyData)
{
    NiShaderError eLastError = NISHADERERR_OK;

    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    pkEntry->SetKey(pcKey);

    // Make sure the flags are set correctly
    // Clear the old map
    uiFlags &= ~NiShaderConstantMapEntry::SCME_MAP_MASK;
    // Set the confirmed one
    uiFlags |= NiShaderConstantMapEntry::SCME_MAP_GLOBAL;
    pkEntry->SetFlags(uiFlags);

    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetRegisterCount(uiRegisterCount);
    pkEntry->SetVariableName(pcVariableName);

    pkEntry->SetData(uiDataSize, uiDataStride, (void*)pvDataSource, 
        bCopyData);

    // Insert it!
    eLastError = InsertEntry(pkEntry);
    if (eLastError != NISHADERERR_OK)
    {
        NiDelete pkEntry;
    }
    else
    {
        NiShaderAttributeDesc::AttributeType eType = 
            NiShaderConstantMapEntry::GetAttributeType(uiFlags);
        // Register the shader constant map entry
        if (!NiD3D10ShaderFactory::RegisterGlobalShaderConstant(
            pcKey, eType, uiDataSize, pvDataSource))
        {
            NIASSERT(!"Failed to add global constant!");
        }
    }

    // Check for an already-encoded shader register value.
    NiUInt32 uiRegister;
    NiUInt32 uiElements;
    bool bPacked;
    if (DecodePackedRegisterAndElement(
        uiShaderRegister, 
        uiRegister, 
        uiElements, 
        bPacked))
    {
        // Ensure that the register count is also encoded.
        NIASSERT(DecodePackedRegisterAndElement(
            uiRegisterCount,
            uiRegister,
            uiElements,
            bPacked));
    }
    else if (uiShaderRegister != SCM_REGISTER_NONE)
    {
        // If it's not encoded, then we need encode it with the
        // buffer offset/size for the data to occupy.
        SetConstantBufferObsolete();
    }
    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::AddOperatorEntry(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiShaderRegister, 
    unsigned int uiRegisterCount, const char* pcVariableName)
{
    NiShaderError eLastError = NISHADERERR_OK;

    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    // 
    pkEntry->SetKey(pcKey);

    // Make sure the flags are set correctly
    // Clear the old map
    uiFlags &= ~NiShaderConstantMapEntry::SCME_MAP_MASK;
    // Set the confirmed one
    uiFlags |= NiShaderConstantMapEntry::SCME_MAP_OPERATOR;
    pkEntry->SetFlags(uiFlags);

    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetRegisterCount(uiRegisterCount);
    pkEntry->SetVariableName(pcVariableName);

    // Insert it!
    eLastError = InsertEntry(pkEntry);
    if (eLastError != NISHADERERR_OK)
    {
        NiDelete pkEntry;
    }

    // Check for an already-encoded shader register value.
    NiUInt32 uiRegister;
    NiUInt32 uiElements;
    bool bPacked;
    if (DecodePackedRegisterAndElement(
        uiShaderRegister, 
        uiRegister, 
        uiElements, 
        bPacked))
    {
        // Ensure that the register count is also encoded.
        NIASSERT(DecodePackedRegisterAndElement(
            uiRegisterCount,
            uiRegister,
            uiElements,
            bPacked));
    }
    else if (uiShaderRegister != SCM_REGISTER_NONE)
    {
        // If it's not encoded, then we need encode it with the
        // buffer offset/size for the data to occupy.
        SetConstantBufferObsolete();
    }
    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::AddObjectEntry(const char* pcKey,
    unsigned int uiShaderRegister, const char* pcVariableName,
    unsigned int uiObjectIndex, NiShaderAttributeDesc::ObjectType eObjectType)
{
    NiShaderError eLastError = NISHADERERR_OK;

    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    pkEntry->SetKey(pcKey);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetVariableName(pcVariableName);
    pkEntry->SetExtra(uiObjectIndex);

    // Set the flags to the object type.
    pkEntry->SetFlags(NiShaderConstantMapEntry::SCME_MAP_OBJECT |
        NiShaderConstantMapEntry::GetObjectFlags(eObjectType));

    // Look-up and set the data.
    eLastError = SetupObjectEntry(pkEntry);

    // Insert it!
    if (eLastError == NISHADERERR_OK)
    {
        eLastError = InsertEntry(pkEntry);
    }
    else
    {
        NiDelete pkEntry;
    }

    // Check for an already-encoded shader register value.
    NiUInt32 uiRegister;
    NiUInt32 uiElements;
    bool bPacked;
    if (DecodePackedRegisterAndElement(
        uiShaderRegister, 
        uiRegister, 
        uiElements, 
        bPacked))
    {
    }
    else if (uiShaderRegister != SCM_REGISTER_NONE)
    {
        // If it's not encoded, then we need encode it with the
        // buffer offset/size for the data to occupy.
        SetConstantBufferObsolete();
    }
    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::RemoveEntry(const char* pcKey)
{
    NiShaderError eLastError = NISHADERERR_OK;

    NiUInt32 uiIndex = GetEntryIndex(pcKey);

    if (uiIndex != INVALID_ENTRY_INDEX)
    {
        NiShaderConstantMapEntry* pkEntry = m_aspEntries.GetAt(uiIndex);
        if (pkEntry)
        {
            if (pkEntry->IsGlobal())
            {
                NiFixedString kKey= pkEntry->GetKey();
                NiD3D10ShaderFactory::ReleaseGlobalShaderConstant(kKey);
            }

            if (pkEntry->GetShaderRegister() != SCM_REGISTER_NONE)
                SetConstantBufferObsolete();
        }

        m_aspEntries.SetAt(uiIndex, 0);
    }
    else
    {
        eLastError = NISHADERERR_ENTRYNOTFOUND;
    }

    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderConstantMapEntry* NiD3D10ShaderConstantMap::GetEntry(const char* pcKey)
{
    NiShaderConstantMapEntry* pkFoundEntry = NULL;

    NiUInt32 uiSize = m_aspEntries.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiShaderConstantMapEntry* pkEntry = m_aspEntries.GetAt(ui);
        if (pkEntry)
        {
            if (NiStricmp(pcKey, pkEntry->GetKey()) == 0)
            {
                pkFoundEntry = pkEntry;
                break;
            }
        }
    }

    return pkFoundEntry;
}
//---------------------------------------------------------------------------
NiShaderConstantMapEntry* NiD3D10ShaderConstantMap::GetEntryAtIndex(
    NiUInt32 uiIndex)
{
    if (uiIndex < m_aspEntries.GetSize())
        return m_aspEntries.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10ShaderConstantMap::GetEntryCount()
{
    return m_aspEntries.GetEffectiveSize();
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::UpdateShaderConstants(
    const NiRenderCallContext& kRCC, bool bGlobal)
{
    if (!m_bExternalStream &&
        CreateShaderConstantDataStream() == false)
    {
        return NISHADERERR_UNKNOWN;
    }

    NIASSERT(m_spShaderConstantDataStream || 
        m_aspEntries.GetEffectiveSize() == 0);

    return UpdateShaderConstantValues(kRCC, bGlobal);
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderConstantMap::LinkShaderConstantBuffer(
    ID3D10ShaderReflection* pkShaderReflection)
{
    NiFixedString kTemp = m_kName;
    if (kTemp.GetLength() == 0)
        kTemp = "$Globals";

    ID3D10ShaderReflectionConstantBuffer* pkBufferReflection = 
        pkShaderReflection->GetConstantBufferByName(kTemp);

    if (pkBufferReflection == NULL)
        return false;

    // In previous versions of the DX SDK (at some unknown point earlier than 
    // October 2008), this function would write beyond the bounds of kDesc. 
    // This would cause problems, but it seems to have been fixed.
    D3D10_SHADER_DESC kShaderDesc;
    HRESULT hr = pkShaderReflection->GetDesc(&kShaderDesc);
    if (FAILED(hr))
    {
        return false;
    }

    NiUInt32 uiBufferIndex = UINT_MAX;
    for (NiUInt32 j = 0; j < kShaderDesc.ConstantBuffers; j++)
    {
        ID3D10ShaderReflectionConstantBuffer* pkTemp = 
            pkShaderReflection->GetConstantBufferByIndex(j);
        if (pkTemp == pkBufferReflection)
        {
            uiBufferIndex = j;
            break;
        }
    }

    // If not found, the shader constant buffer may not be needed by this
    // shader.
    if (uiBufferIndex == UINT_MAX)
        return false;

    return CreateShaderConstantDataStream(pkBufferReflection, uiBufferIndex);
}

//---------------------------------------------------------------------------
bool NiD3D10ShaderConstantMap::CreateShaderConstantDataStream(
    ID3D10ShaderReflectionConstantBuffer* pkBufferReflection, 
    NiUInt32 uiBufferIndex)
{
    if (!m_bExternalStream && IsConstantBufferCurrent() == false)
    {
        NiDataStreamElementSet kElementSet;
        UpdateConstantBufferPacking(pkBufferReflection, kElementSet);
        NiUInt32 uiSize = kElementSet.GetStride();
        if (m_spShaderConstantDataStream)
        {
            if (m_spShaderConstantDataStream->GetSize() < uiSize)
            {
                m_spShaderConstantDataStream->Resize(uiSize);
            }
            m_bConstantBufferCurrent = true;
        }
        else if (uiSize > 0)
        {
            // Ensure size is a multiple of 16, which is a requirement of
            // constant buffer data
            uiSize = (uiSize + 15) & 0xFFFFFFF0;
            NiUInt32 uiDiff = uiSize - kElementSet.GetStride();
            NIASSERT(uiDiff % 
                (D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8) 
                == 0);
            if (uiDiff != 0)
            {
                // Pad if necessary
                InsertPadding(uiDiff, kElementSet);
            }

            m_spShaderConstantDataStream = (NiD3D10DataStream*)
                NiDataStream::CreateDataStream(kElementSet, 1, 
                NiDataStream::ACCESS_GPU_READ | 
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE, 
                NiDataStream::USAGE_SHADERCONSTANT);

            if (m_spShaderConstantDataStream == NULL)
            {
                return false;
            }

            m_bConstantBufferCurrent = (m_spShaderConstantDataStream != NULL);
        }
        else
        {
            // No need to create a buffer.
            m_bConstantBufferCurrent = true;
        }

        if (m_spShaderConstantDataStream)
            m_uiShaderConstantBufferIndex = uiBufferIndex;
    }
    NIASSERT(m_bConstantBufferCurrent == false || 
        m_spShaderConstantDataStream != NULL ||
        m_aspEntries.GetEffectiveSize() == 0);

    return m_bConstantBufferCurrent;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantMap::ReleaseShaderConstantDataStream()
{
    m_spShaderConstantDataStream = NULL;
    m_bExternalStream = false;
    m_uiShaderConstantBufferIndex = (NiUInt32) BUFFER_INDEX_UNASSIGNED;

    SetConstantBufferObsolete();
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantMap::SetupTextureTransformMatrix(
    D3DXMATRIXA16& kResult, const NiMatrix3* pkTexMatrix, bool bTrans)
{
    if (pkTexMatrix)
    {
        kResult._11 = pkTexMatrix->GetEntry(0, 0);
        kResult._12 = pkTexMatrix->GetEntry(1, 0);
        kResult._13 = 0.0f; 
        kResult._14 = 0.0f; 
        kResult._21 = pkTexMatrix->GetEntry(0, 1);
        kResult._22 = pkTexMatrix->GetEntry(1, 1);
        kResult._23 = 0.0f; 
        kResult._24 = 0.0f; 
        kResult._31 = 0.0f; 
        kResult._32 = 0.0f; 
        kResult._33 = 0.0f; 
        kResult._34 = 0.0f; 
        kResult._41 = pkTexMatrix->GetEntry(0, 2); 
        kResult._42 = pkTexMatrix->GetEntry(1, 2); 
        kResult._43 = 0.0f; 
        kResult._44 = 0.0f; 

        if (bTrans)
            D3DXMatrixTranspose(&kResult, &kResult);
    }
    else
    {
        D3DXMatrixIdentity(&kResult);
    }
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderConstantMap::UpdateConstantBufferPacking(
    ID3D10ShaderReflectionConstantBuffer* pkBufferReflection, 
    NiDataStreamElementSet& kDataStreamElements)
{
    const NiUInt32 uiComponentSize = 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8;
    if (pkBufferReflection)
    {
        D3D10_SHADER_BUFFER_DESC kBufferDesc;
        HRESULT hr = pkBufferReflection->GetDesc(&kBufferDesc);
        if (FAILED(hr))
        {
            return 0;
        }

        if (kBufferDesc.Type != D3D10_CT_CBUFFER)
        {
            return 0;
        }

        // Iterate over variables in the order they appear in the 
        // constant buffer
        for (NiUInt32 i = 0; i < kBufferDesc.Variables; i++)
        {
            ID3D10ShaderReflectionVariable* pkVariableReflection =
                pkBufferReflection->GetVariableByIndex(i);
            if (pkVariableReflection == NULL)
            {
                return false;
            }

            D3D10_SHADER_VARIABLE_DESC kVariableDesc;
            HRESULT hr = pkVariableReflection->GetDesc(&kVariableDesc);
            if (FAILED(hr))
            {
                return false;
            }

            NiFixedString kVariableName = kVariableDesc.Name;
            // Find entry for this variable, if it exists
            NiShaderConstantMapEntry* pkEntry = NULL;
            for (NiUInt32 j = 0; j < m_aspEntries.GetSize(); j++)
            {
                NiShaderConstantMapEntry* pkLocalEntry = m_aspEntries.GetAt(j);
                if (!pkLocalEntry)
                    continue;
                NiFixedString kVarName = pkLocalEntry->GetVariableName();
                if (!kVarName.Exists())
                    kVarName = pkLocalEntry->GetKey();
                if (kVarName == kVariableName)
                {
                    pkEntry = pkLocalEntry;
                    break;
                }
            }
            if (pkEntry == NULL)
            {
                // Entry for this variable not found - insert appropriate 
                // padding and continue on to next one
                InsertPadding(kVariableDesc.Size, kDataStreamElements);
                continue;
            }

            ID3D10ShaderReflectionType* pkTypeReflection = 
                pkVariableReflection->GetType();
            if (pkTypeReflection == NULL)
            {
                return false;
            }

            D3D10_SHADER_TYPE_DESC kTypeDesc;
            hr = pkTypeReflection->GetDesc(&kTypeDesc);
            if (FAILED(hr))
            {
                return false;
            }

            bool bPackRegisters = true;
            bool bColumnMajor = true;
            NiDataStreamElement::Type eElementType;
            NiUInt32 uiNumRows = 0;
            NiUInt32 uiNumColumns = 0;
            NiUInt32 uiNumElements = 1;
            // Examine types for support
            if (kTypeDesc.Type == D3D10_SVT_FLOAT)
            {
                eElementType = NiDataStreamElement::T_FLOAT32;

                if (kTypeDesc.Class == D3D10_SVC_VECTOR)
                {
                    uiNumRows = kTypeDesc.Rows;
                    uiNumColumns = kTypeDesc.Columns;
                }
                // Check for matrices
                else if (kTypeDesc.Class == D3D10_SVC_MATRIX_ROWS)
                {
                    bColumnMajor = false;
                    uiNumColumns = kTypeDesc.Columns;
                    uiNumRows = kTypeDesc.Rows;
                    if (uiNumColumns != 4)
                        bPackRegisters = false;
                }
                else if (kTypeDesc.Class == D3D10_SVC_MATRIX_COLUMNS)
                {
                    uiNumColumns = kTypeDesc.Rows;
                    uiNumRows = kTypeDesc.Columns;
                    if (uiNumColumns != 4)
                        bPackRegisters = true;
                }
                else
                {
                    uiNumRows = 1;
                    uiNumColumns = 0;
                }
                uiNumElements = kTypeDesc.Elements;
            }
            else if (kTypeDesc.Type == D3D10_SVT_INT || 
                kTypeDesc.Type == D3D10_SVT_BOOL)
            {
                eElementType = NiDataStreamElement::T_INT32;

                // Only scalars supported
                if (kTypeDesc.Class != D3D10_SVC_SCALAR)
                {
                    return false;
                }
                uiNumColumns = 1;
                uiNumRows = 1;
                uiNumElements = kTypeDesc.Elements;
            }
            else
            {
                // We don't support setting the other types, but they're 
                // currently just ignored anyway. Don't even bother 
                // with a warning.
                continue;
            }
            NIASSERT(uiNumElements != 0);

            NiUInt32 uiStartOffset = kVariableDesc.StartOffset / 
                uiComponentSize;
            NiUInt32 uiStartRegister = uiStartOffset / 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
            NiUInt32 uiStartElement = uiStartOffset % 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;

            NiUInt32 uiEncodedStartRegister;
            EncodePackedRegisterAndElement(uiEncodedStartRegister, 
                uiStartRegister, uiStartElement, false);

            NiUInt32 uiFinalOffset = 
                (kVariableDesc.StartOffset + kVariableDesc.Size) / 
                uiComponentSize;
            NiUInt32 uiFinalRegister = uiFinalOffset / 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
            NiUInt32 uiFinalElement = uiFinalOffset % 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
            if (uiStartElement > uiFinalElement)
            {
                uiFinalElement += 
                    D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
                uiFinalRegister--;
            }
            NIASSERT(uiFinalElement >= uiStartElement);
            NIASSERT(uiFinalRegister >= uiStartRegister);

            NiUInt32 uiEncodedRegisterCount;
            EncodePackedRegisterAndElement(uiEncodedRegisterCount, 
                uiFinalRegister - uiStartRegister, 
                uiFinalElement - uiStartElement, bPackRegisters);

            pkEntry->SetShaderRegister(uiEncodedStartRegister);
            pkEntry->SetRegisterCount(uiEncodedRegisterCount);
            pkEntry->SetColumnMajor(bColumnMajor);
            pkEntry->SetVariableHookupValid(true);

            // Add padding if necessary
            NiUInt32 uiPadding = kVariableDesc.StartOffset - 
                kDataStreamElements.GetStride();
            if (uiPadding != 0)
            {
                InsertPadding(uiPadding, kDataStreamElements);
            }

            // Add new data elements
            NiUInt32 uiNumComponents = kVariableDesc.Size / uiComponentSize;
            if (uiNumColumns == 0)
                uiNumColumns = uiNumComponents;
            NiDataStreamElement::Format eFormat = 
                NiDataStreamElement::GetPredefinedFormat(
                eElementType, (NiUInt8)uiNumColumns, false);
            uiPadding = 
                (D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS - 
                uiNumColumns) * uiComponentSize;

            NIASSERT(uiNumRows > 0);
            NiUInt32 uiTotalRows = uiNumRows * uiNumElements;
            // Fill in all rows before last row, including padding if necessary
            for (NiUInt32 j = 0; j < uiTotalRows - 1; j++)
            {
                kDataStreamElements.AddElement(eFormat);
                if (uiPadding != 0)
                    InsertPadding(uiPadding, kDataStreamElements);
            }
            // Fill in last row
            kDataStreamElements.AddElement(eFormat);
        }

        return true;
    }
    else
    {
        // No reflection data - need to try to manually figure out where to
        // place each variable.
        NiUInt32 uiRegister = 0;
        NiUInt32 uiElement = 0;

        for (NiUInt32 i = 0; i < m_aspEntries.GetSize(); i++)
        {
            NiShaderConstantMapEntry* pkEntry = m_aspEntries.GetAt(i);
            if (!pkEntry)
                continue;

            // Allow for skipping of entries if the have a -1 in the 
            // shader register.
            if (pkEntry->GetShaderRegister() == SCM_REGISTER_NONE &&
                pkEntry->GetVariableName() == 0)
            {
                continue;
            }
            // If the shader register is > max constants, skip that too
            //        if (pkEntry->GetShaderRegister() >= ...)
            //            continue;

            NiShaderAttributeDesc::AttributeType eType = 
                pkEntry->GetAttributeType();
            NiUInt32 uiEncodedStartRegister;
            NiUInt32 uiEncodedRegisterCount;
            switch (eType)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                CalculatePackingEntry(uiRegister, uiElement, 0, 1, 
                    uiEncodedStartRegister, uiEncodedRegisterCount, true);
                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                CalculatePackingEntry(uiRegister, uiElement, 0, 2, 
                    uiEncodedStartRegister, uiEncodedRegisterCount, true);
                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                CalculatePackingEntry(uiRegister, uiElement, 0, 3, 
                    uiEncodedStartRegister, uiEncodedRegisterCount, true);
                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                CalculatePackingEntry(uiRegister, uiElement, 1, 0, 
                    uiEncodedStartRegister, uiEncodedRegisterCount, true);
                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
                CalculatePackingEntry(uiRegister, uiElement, 2, 0, 
                    uiEncodedStartRegister, uiEncodedRegisterCount, true);
                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
                CalculatePackingEntry(uiRegister, uiElement, 3, 0, 
                    uiEncodedStartRegister, uiEncodedRegisterCount, true);
                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                // SCM_DEF_BONE_MATRIX_3 was deprecated
                NIASSERT(pkEntry->GetInternal() != SCM_DEF_BONE_MATRIX_3);
                if (pkEntry->GetInternal() == SCM_DEF_SKINBONE_MATRIX_3)
                {
                    // Special case: SCM_DEF_SKINBONE_MATRIX_3
                    const NiUInt32 uiBoneCount = 
                        (pkEntry->GetExtra() & 0xffff0000) >> 16;

                    CalculatePackingEntryArray(uiBoneCount, uiRegister, 
                        uiElement, 3, 0, uiEncodedStartRegister, 
                        uiEncodedRegisterCount, false);
                }
                else if (pkEntry->IsDefined())
                {
                    // Special case: predefined MATRIX3 are actually 3 full
                    // registers, similar to FLOAT12.
                    CalculatePackingEntry(uiRegister, uiElement, 3, 0, 
                        uiEncodedStartRegister, uiEncodedRegisterCount, true);
                }
                else
                {
                    CalculatePackingEntry(uiRegister, uiElement, 2, 3, 
                        uiEncodedStartRegister, uiEncodedRegisterCount, false);
                }
                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                // SCM_DEF_BONE_MATRIX_4 and SCM_DEF_SKINBONE_MATRIX_4 
                // were deprecated
                NIASSERT(pkEntry->GetInternal() != SCM_DEF_BONE_MATRIX_4 &&
                    pkEntry->GetInternal() != SCM_DEF_SKINBONE_MATRIX_4);
                {
                    CalculatePackingEntry(uiRegister, uiElement, 4, 0, 
                        uiEncodedStartRegister, uiEncodedRegisterCount, true);
                }
                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
            {
                const NiUInt32 uiDataSize = pkEntry->GetDataSize();
                const NiUInt32 uiDataStride = pkEntry->GetDataStride();
                NIASSERT(uiDataSize != 0 && uiDataStride != 0);
                const NiUInt32 uiNumArrayElements = 
                    uiDataSize / uiDataStride;
                const NiUInt32 uiNumRegisterElementsPerArrayElement =
                    uiDataStride / sizeof(float);

                // 9 elements per array element - matrix3x3, which is the only
                // non-packed data type - gets 2 registers and 3 elements
                const NiUInt32 uiRegisterCount = 
                    (uiNumRegisterElementsPerArrayElement == 9 ? 2 : 
                    uiNumRegisterElementsPerArrayElement / 4);
                const NiUInt32 uiElementCount = 
                    (uiNumRegisterElementsPerArrayElement == 9 ? 3 : 
                    uiNumRegisterElementsPerArrayElement % 4);
                bool bPackRegisters = 
                    (uiNumRegisterElementsPerArrayElement != 9);

                CalculatePackingEntryArray(uiNumArrayElements, uiRegister, 
                    uiElement, uiRegisterCount, uiElementCount, 
                    uiEncodedStartRegister, uiEncodedRegisterCount, 
                    bPackRegisters);

                pkEntry->SetShaderRegister(uiEncodedStartRegister);
                pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                pkEntry->SetVariableHookupValid(true);
                break;
            }
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
                if (pkEntry->GetVariableHookupValid())
                {
                    // Entry has been packed at some point; can use existing 
                    // encoded register count
                    NiUInt32 uiTempStartRegister;
                    NiUInt32 uiTempStartElement;
                    NiUInt32 uiTempRegisterCount;
                    NiUInt32 uiTempElementCount;
                    bool bPackRegisters;
                    bool bValid = DecodePackedRegisterAndElement(
                        pkEntry->GetShaderRegister(), uiTempStartRegister, 
                        uiTempStartElement, bPackRegisters);
                    NIASSERT(bValid);
                    NIASSERT(bPackRegisters == false);
                    bValid = DecodePackedRegisterAndElement(
                        pkEntry->GetRegisterCount(), uiTempRegisterCount, 
                        uiTempElementCount, bPackRegisters);
                    NIASSERT(bValid);
                    CalculatePackingEntry(uiRegister, uiElement, 
                        uiTempRegisterCount, uiTempElementCount, 
                        uiEncodedStartRegister, uiEncodedRegisterCount, 
                        bPackRegisters);

                    pkEntry->SetShaderRegister(uiEncodedStartRegister);
                    pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                    pkEntry->SetVariableHookupValid(true);
                }
                else
                {
                    // Entry has never been packed, and has no valid type
                    // Must use stored register count; assume packed
                    NiUInt32 uiTempRegisterCount = 
                        pkEntry->GetRegisterCount();
                    NIASSERT(!IsRegisterEncoded(uiTempRegisterCount));

                    CalculatePackingEntry(uiRegister, uiElement, 
                        uiTempRegisterCount, 0, 
                        uiEncodedStartRegister, uiEncodedRegisterCount, true);

                    pkEntry->SetShaderRegister(uiEncodedStartRegister);
                    pkEntry->SetRegisterCount(uiEncodedRegisterCount);
                    pkEntry->SetVariableHookupValid(true);
                }
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            default:
                NIASSERT(false);
                break;
            }
        }

        NiUInt32 uiBufferSize = 
            (uiRegister * D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENTS + 
            uiElement) * 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8;

        NiUInt32 uiUnclaimedBufferSize = uiBufferSize;
        NIASSERT(uiBufferSize % 
            (D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8) == 0);
        while (uiUnclaimedBufferSize > 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENTS * 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8)
        {
            kDataStreamElements.AddElement(NiDataStreamElement::F_FLOAT32_4);
            uiUnclaimedBufferSize -= 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENTS * 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8;
        }
        while (uiUnclaimedBufferSize > 0)
        {
            kDataStreamElements.AddElement(NiDataStreamElement::F_FLOAT32_1);
            uiUnclaimedBufferSize -= 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8;
        }
        NIASSERT(uiBufferSize == kDataStreamElements.GetStride());

        return true;
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantMap::SetShaderConstantDataStream(
    NiD3D10DataStream* pkStream)
{
    m_spShaderConstantDataStream = pkStream;
    m_bExternalStream = true;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderConstantMap::EncodePackedRegisterAndElement(
    NiUInt32& uiEncodedValue, NiUInt32 uiRegister, 
    NiUInt32 uiElement, bool bPackedRegisters)
{
    uiEncodedValue = SCM_REGISTER_ENCODING |
        (uiElement << SCM_REGISTER_ELEMENT_SHIFT) |
        (uiRegister << SCM_REGISTER_SHIFT) |
        (bPackedRegisters ? SCM_REGISTER_PACKED_BIT : 0) ;
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderConstantMap::DecodePackedRegisterAndElement(
    NiUInt32 uiEncodedValue, NiUInt32& uiRegister, 
    NiUInt32& uiElement, bool& bPackedRegisters)
{
    if (!IsRegisterEncoded(uiEncodedValue))
        return false;

    uiRegister = 
        (uiEncodedValue & SCM_REGISTER_MASK) >> SCM_REGISTER_SHIFT;
    uiElement =
        (uiEncodedValue & SCM_REGISTER_ELEMENT_MASK) >> 
        SCM_REGISTER_ELEMENT_SHIFT;
    bPackedRegisters = ((uiEncodedValue & SCM_REGISTER_PACKED_BIT) != 0);

    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10ShaderConstantMap::GetEntryIndex(const char* pcKey)
{
    NiShaderConstantMapEntry* pkEntry;
    NiUInt32 uiSize = m_aspEntries.GetSize();
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        pkEntry = m_aspEntries.GetAt(i);
        if (pkEntry)
        {
            if (NiStricmp(pcKey, pkEntry->GetKey()) == 0)
            {
                return i;
            }
        }
    }

    return (NiUInt32) INVALID_ENTRY_INDEX;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::InsertEntry(
    NiShaderConstantMapEntry* pkEntry)
{
    NiShaderError eLastError = NISHADERERR_OK;

    if (m_aspEntries.AddFirstEmpty(pkEntry) == 0xffffffff)
    {
        // FAILED
        eLastError = NISHADERERR_ENTRYNOTADDED;
    }

    return eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::SetupPredefinedEntry(
    NiShaderConstantMapEntry* pkEntry)
{
    // Look up the key.
    bool bFound = false;

    NiUInt32 uiRegCount = 4;
    NiUInt32 uiMappingID;

    if (!LookUpPredefinedMapping(pkEntry->GetKey(), uiMappingID, uiRegCount))
        return NISHADERERR_INVALIDMAPPING;

    NiShaderAttributeDesc::AttributeType eAttribType = 
        LookUpPredefinedMappingType(uiMappingID, uiRegCount);
    NiUInt32 uiFlags = pkEntry->GetFlags();
    uiFlags &= ~NiShaderConstantMapEntry::GetAttributeMask();
    uiFlags |= NiShaderConstantMapEntry::GetAttributeFlags(eAttribType);
    pkEntry->SetFlags(uiFlags);

    if (uiMappingID != 0)
    {
        // FOUND IT!
        pkEntry->SetInternal(uiMappingID);

        if (uiMappingID == SCM_DEF_CONSTS_TAYLOR_SIN)
        {
            float afTaylorSin[] = 
                { 1.0f, -0.16161616f, 0.0083333f, -0.00019841f };
            pkEntry->SetData(sizeof(afTaylorSin), sizeof(afTaylorSin),
                (void*)afTaylorSin, true);
        }
        else if (uiMappingID == SCM_DEF_CONSTS_TAYLOR_COS)
        {
            float afTaylorCos[] = 
                { -0.5f, -0.041666666f, -0.0013888889f, 0.000024801587f };
            pkEntry->SetData(sizeof(afTaylorCos), sizeof(afTaylorCos),
                (void*)afTaylorCos, true);
        }

        bFound = true;
    }

    if (!bFound)
        return NISHADERERR_INVALIDMAPPING;

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::SetupObjectEntry(
    NiShaderConstantMapEntry* pkEntry)
{
    NiUInt32 uiRegCount = 0;
    NiUInt32 uiMappingID;
    if (!LookUpObjectMapping(pkEntry->GetKey(), uiMappingID))
    {
        return NISHADERERR_INVALIDMAPPING;
    }

    NiUInt32 uiFloatCount;
    NiShaderAttributeDesc::AttributeType eAttribType =
        LookUpObjectMappingType(uiMappingID, uiRegCount, uiFloatCount);
    NiUInt32 uiFlags = pkEntry->GetFlags();
    uiFlags &= ~NiShaderConstantMapEntry::GetAttributeMask();
    uiFlags |= NiShaderConstantMapEntry::GetAttributeFlags(eAttribType);
    pkEntry->SetFlags(uiFlags);

    if (uiMappingID != NiShaderConstantMap::SCM_OBJ_INVALID)
    {
        pkEntry->SetInternal(uiMappingID);
        // No need to set register count, since the attribute type has been
        // stored in the flags.
        //pkEntry->SetRegisterCount(uiRegCount);
    }
    else
    {
        return NISHADERERR_INVALIDMAPPING;
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::UpdateShaderConstantValues(
    const NiRenderCallContext& kRCC, bool bGlobal)
{
    if (m_aspEntries.GetEffectiveSize() == 0)
        return NISHADERERR_OK;

    if (m_spShaderConstantDataStream == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }

    void* pvWriteData = 
        m_spShaderConstantDataStream->Lock(NiDataStream::LOCK_WRITE);
    if (pvWriteData == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }

    bool bSuccess = true;

    // Grab the NiSCMExtraData object which basically holds cached
    // values for extra data pointers so we don't have to call strcmp
    // too much.
    NiRenderObject* pkMesh = kRCC.m_pkMesh;
    NiSCMExtraData* pkShaderData = (NiSCMExtraData*)
        pkMesh->GetExtraData(NiD3D10Shader::GetEmergentShaderMapName());

    for (NiUInt32 i = 0; i < m_aspEntries.GetSize(); i++)
    {
        NiShaderConstantMapEntry* pkEntry = m_aspEntries.GetAt(i);
        if (!pkEntry)
            continue;

        // Allow for skipping of entries if the have a -1 in the 
        // shader register.
        if (pkEntry->GetShaderRegister() == SCM_REGISTER_NONE)
            continue;

        // If the shader register is > max constants, skip that too
        //        if (pkEntry->GetShaderRegister() >= ...)
        //            continue;

        NiShaderError eResult = NISHADERERR_OK;
        if (pkEntry->IsDefined())
        {
            eResult = UpdateDefinedConstantValue(pvWriteData, pkEntry, kRCC);
        }
        else if (pkEntry->IsConstant())
        {
            eResult = UpdateConstantConstantValue(pvWriteData, pkEntry, kRCC);
        }
        else if (pkEntry->IsAttribute())
        {
            eResult = UpdateAttributeConstantValue(pvWriteData, pkEntry, kRCC, 
                bGlobal, pkShaderData);
        }
        else if (pkEntry->IsGlobal())
        {
            eResult = UpdateGlobalConstantValue(pvWriteData, pkEntry, kRCC);
        }
        else if (pkEntry->IsOperator())
        {
            eResult = UpdateOperatorConstantValue(pvWriteData, pkEntry, kRCC, 
                bGlobal, pkShaderData);
        }
        else if (pkEntry->IsObject())
        {
            eResult = UpdateObjectConstantValue(pvWriteData, pkEntry, kRCC);
        }
        else
        {
            eResult = NISHADERERR_INVALIDMAPPING;
        }
        if (eResult != NISHADERERR_OK)
        {
            bSuccess = false;
        }
    }

    m_spShaderConstantDataStream->Unlock(NiDataStream::LOCK_WRITE);

    return bSuccess ? NISHADERERR_OK : NISHADERERR_UNKNOWN;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::UpdateDefinedConstantValue(
    void* pvShaderConstantBuffer, NiShaderConstantMapEntry* pkEntry, 
    const NiRenderCallContext& kRCC)
{
    NiUInt32 uiEncodedShaderRegister = pkEntry->GetShaderRegister();
    NiUInt32 uiEncodedRegisterCount = pkEntry->GetRegisterCount();

    NiUInt32 uiStartRegister = 0;
    NiUInt32 uiStartElement = 0;
    NiUInt32 uiRegisterCount = 0;
    NiUInt32 uiElementCount = 0;
    bool bPackRegisters = false;

    bool bValid1 = DecodePackedRegisterAndElement(uiEncodedShaderRegister, 
        uiStartRegister, uiStartElement, bPackRegisters);
    NIASSERT(bPackRegisters == false);
    bool bValid2 = DecodePackedRegisterAndElement(uiEncodedRegisterCount, 
        uiRegisterCount, uiElementCount, bPackRegisters);
    if (!bValid1 || !bValid2)
    {
        return NISHADERERR_UNKNOWN;
    }

    D3DXMATRIXA16 kTempMatrix;
    const void* pvDataSource = ObtainDefinedConstantValue(pkEntry, kRCC, 
        kTempMatrix);
    if (pvDataSource == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiArrayLength = 1;
    if (pkEntry->IsArray())
    {
        uiArrayLength = pkEntry->GetDataSize() / pkEntry->GetDataStride();
        NiUInt32 uiRegPerEntry = uiRegisterCount / uiArrayLength;
        NiUInt32 uiRemainder = uiRegisterCount % uiArrayLength;
        // The registers will not divide evenly if each array element
        // ends with a partially filled register - in that case, the 
        // remainder will be (uiArrayLength - 1);
        NIASSERT(uiRemainder == 0 || uiRemainder == uiArrayLength - 1);
        if (uiRemainder != 0)
            uiRegPerEntry++;

        uiRegisterCount = uiRegPerEntry;
    }

    // Bone reordering may take place - pass in reorder array!
    const NiUInt16* puiBonePalette = NULL;
    NiDataStream* pkBonePaletteStream = NULL;
    if (pkEntry->GetInternal() == SCM_DEF_SKINBONE_MATRIX_3)
    {
        NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

        // Special-case bones, since they aren't counted as an array
        NiSkinningMeshModifier* pkSkin = 
            NiGetModifier(NiSkinningMeshModifier, pkMesh);

        if (pkSkin)
        {
            NiDataStreamRef* pkBonePaletteRef = 
                pkMesh->FindStreamRef(NiCommonSemantics::BONE_PALETTE());
            NIASSERT(pkBonePaletteRef);

            pkBonePaletteStream = pkBonePaletteRef->GetDataStream();
            NIASSERT(pkBonePaletteStream != NULL);

            const NiDataStream::Region& kRegion =
                pkBonePaletteStream->GetRegion(kRCC.m_uiSubmesh);
#if defined(NIDEBUG)
            // The palette indicies are assumed to be 16 bit unsigned ints,
            // packed in their own stream
            const NiDataStreamElement& kElem =
                pkBonePaletteStream->GetElementDescAt(0);
            NIASSERT(kElem.GetFormat() == NiDataStreamElement::F_UINT16_1);
            NIASSERT(pkBonePaletteStream->GetStride() == sizeof(NiUInt16));
            NIASSERT(kElem.GetOffset() == 0);
#endif
            const NiUInt32 uiSubmeshBoneCount = kRegion.GetRange();

            // Get a pointer to the current region's bone palette
            puiBonePalette =
                (NiUInt16*)pkBonePaletteStream->LockRegion(kRCC.m_uiSubmesh,
                NiDataStream::LOCK_READ);
            NIASSERT(puiBonePalette != NULL);

            // Use lesser of encoded bone count and actual bone count
            NiUInt32 uiEncodedBoneCount = 
                (pkEntry->GetExtra() & 0xffff0000) >> 16;
            NIASSERT(uiEncodedBoneCount < INT_MAX && 
                uiSubmeshBoneCount < INT_MAX);
            uiArrayLength = NiMin((NiInt32)uiEncodedBoneCount, 
                (NiInt32)uiSubmeshBoneCount);

            // Force register count to 3
            uiRegisterCount = 3;
        }
    }

    NiShaderError eErr = FillShaderConstantBuffer(pvShaderConstantBuffer,
        pvDataSource, uiStartRegister, uiStartElement, 
        uiRegisterCount, uiElementCount, uiArrayLength, bPackRegisters,
        puiBonePalette);

    if (puiBonePalette)
    {
        NIASSERT(pkBonePaletteStream);
        pkBonePaletteStream->Unlock(NiDataStream::LOCK_READ);
    }

    return eErr;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::UpdateConstantConstantValue(
    void* pvShaderConstantBuffer, NiShaderConstantMapEntry* pkEntry, 
    const NiRenderCallContext& kRCC)
{
    const void* pvDataSource = ObtainConstantConstantValue(pkEntry, kRCC);
    if (pvDataSource == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiEncodedShaderRegister = pkEntry->GetShaderRegister();
    NiUInt32 uiEncodedRegisterCount = pkEntry->GetRegisterCount();

    NiUInt32 uiStartRegister = 0;
    NiUInt32 uiStartElement = 0;
    NiUInt32 uiRegisterCount = 0;
    NiUInt32 uiElementCount = 0;
    bool bPackRegisters = false;

    bool bValid1 = DecodePackedRegisterAndElement(uiEncodedShaderRegister, 
        uiStartRegister, uiStartElement, bPackRegisters);
    NIASSERT(bPackRegisters == false);
    bool bValid2 = DecodePackedRegisterAndElement(uiEncodedRegisterCount, 
        uiRegisterCount, uiElementCount, bPackRegisters);
    if (!bValid1 || !bValid2)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiArrayLength = 1;
    if (pkEntry->IsArray())
    {
        uiArrayLength = pkEntry->GetDataSize() / pkEntry->GetDataStride();
        NiUInt32 uiRegPerEntry = uiRegisterCount / uiArrayLength;
        NiUInt32 uiRemainder = uiRegisterCount % uiArrayLength;
        // The registers will not divide evenly if each array element
        // ends with a partially filled register - in that case, the 
        // remainder will be (uiArrayLength - 1);
        NIASSERT(uiRemainder == 0 || uiRemainder == uiArrayLength - 1);
        if (uiRemainder != 0)
            uiRegPerEntry++;

        uiRegisterCount = uiRegPerEntry;
    }

    return FillShaderConstantBuffer(pvShaderConstantBuffer,
        pvDataSource, uiStartRegister, uiStartElement, 
        uiRegisterCount, uiElementCount, uiArrayLength, bPackRegisters,
        NULL);
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::UpdateAttributeConstantValue(
    void* pvShaderConstantBuffer, NiShaderConstantMapEntry* pkEntry, 
    const NiRenderCallContext& kRCC, bool bGlobal, NiExtraData* pkExtraData)
{
    if (kRCC.m_pkMesh == NULL)
        return NISHADERERR_UNKNOWN;

    // Grab the attribute from the geometry and set it

    D3DXMATRIXA16 kTempMatrix;
    const void* pvDataSource = ObtainAttributeConstantValue(pkEntry, kRCC, 
        bGlobal, pkExtraData, kTempMatrix);
    if (pvDataSource == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }

    if (pvDataSource == 0)
    {
        if (pkEntry->IsTexture())
        {
            return NISHADERERR_OK;
        }

        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiEncodedShaderRegister = pkEntry->GetShaderRegister();
    NiUInt32 uiEncodedRegisterCount = pkEntry->GetRegisterCount();

    NiUInt32 uiStartRegister = 0;
    NiUInt32 uiStartElement = 0;
    NiUInt32 uiRegisterCount = 0;
    NiUInt32 uiElementCount = 0;
    bool bPackRegisters = true;

    bool bValid1 = DecodePackedRegisterAndElement(uiEncodedShaderRegister, 
        uiStartRegister, uiStartElement, bPackRegisters);
    NIASSERT(bPackRegisters == false);
    bool bValid2 = DecodePackedRegisterAndElement(uiEncodedRegisterCount, 
        uiRegisterCount, uiElementCount, bPackRegisters);
    if (!bValid1 || !bValid2)
    {
        NIASSERT(false);
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiArrayLength = 1;
    if (pkEntry->IsArray())
    {
        uiArrayLength = pkEntry->GetDataSize() / pkEntry->GetDataStride();
        NiUInt32 uiRegPerEntry = uiRegisterCount / uiArrayLength;
        NiUInt32 uiRemainder = uiRegisterCount % uiArrayLength;
        // The registers will not divide evenly if each array element
        // ends with a partially filled register - in that case, the 
        // remainder will be (uiArrayLength - 1);
        NIASSERT(uiRemainder == 0 || uiRemainder == uiArrayLength - 1);
        if (uiRemainder != 0)
            uiRegPerEntry++;

        uiRegisterCount = uiRegPerEntry;
    }

    return FillShaderConstantBuffer(pvShaderConstantBuffer,
        pvDataSource, uiStartRegister, uiStartElement, uiRegisterCount, 
        uiElementCount, uiArrayLength, bPackRegisters, NULL);
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::UpdateGlobalConstantValue(
    void* pvShaderConstantBuffer, NiShaderConstantMapEntry* pkEntry, 
    const NiRenderCallContext& kRCC)
{
    const void* pvDataSource = ObtainGlobalConstantValue(pkEntry, kRCC);
    if (pvDataSource == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiEncodedShaderRegister = pkEntry->GetShaderRegister();
    NiUInt32 uiEncodedRegisterCount = pkEntry->GetRegisterCount();

    NiUInt32 uiStartRegister = 0;
    NiUInt32 uiStartElement = 0;
    NiUInt32 uiRegisterCount = 0;
    NiUInt32 uiElementCount = 0;
    bool bPackRegisters = false;

    bool bValid1 = DecodePackedRegisterAndElement(uiEncodedShaderRegister, 
        uiStartRegister, uiStartElement, bPackRegisters);
    NIASSERT(bPackRegisters == false);
    bool bValid2 = DecodePackedRegisterAndElement(uiEncodedRegisterCount, 
        uiRegisterCount, uiElementCount, bPackRegisters);
    if (!bValid1 || !bValid2)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiArrayLength = 1;
    if (pkEntry->IsArray())
    {
        uiArrayLength = pkEntry->GetDataSize() / pkEntry->GetDataStride();
        NiUInt32 uiRegPerEntry = uiRegisterCount / uiArrayLength;
        NiUInt32 uiRemainder = uiRegisterCount % uiArrayLength;
        // The registers will not divide evenly if each array element
        // ends with a partially filled register - in that case, the 
        // remainder will be (uiArrayLength - 1);
        NIASSERT(uiRemainder == 0 || uiRemainder == uiArrayLength - 1);
        if (uiRemainder != 0)
            uiRegPerEntry++;

        uiRegisterCount = uiRegPerEntry;
    }

    return FillShaderConstantBuffer(pvShaderConstantBuffer,
        pvDataSource, uiStartRegister, uiStartElement, 
        uiRegisterCount, uiElementCount, uiArrayLength, bPackRegisters, NULL);
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::UpdateOperatorConstantValue(
    void* pvShaderConstantBuffer, NiShaderConstantMapEntry* pkEntry, 
    const NiRenderCallContext& kRCC, bool bGlobal, NiExtraData* pkExtraData)
{
    D3DXMATRIXA16 kResult;

    const void* pvDataSource = ObtainOperatorConstantValue(pkEntry, kRCC,
        bGlobal, pkExtraData, kResult);
    if (pvDataSource == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiEncodedShaderRegister = pkEntry->GetShaderRegister();
    NiUInt32 uiEncodedRegisterCount = pkEntry->GetRegisterCount();

    NiUInt32 uiStartRegister = 0;
    NiUInt32 uiStartElement = 0;
    NiUInt32 uiRegisterCount = 0;
    NiUInt32 uiElementCount = 0;
    bool bPackRegisters = false;

    bool bValid1 = DecodePackedRegisterAndElement(uiEncodedShaderRegister, 
        uiStartRegister, uiStartElement, bPackRegisters);
    NIASSERT(bPackRegisters == false);
    bool bValid2 = DecodePackedRegisterAndElement(uiEncodedRegisterCount, 
        uiRegisterCount, uiElementCount, bPackRegisters);
    if (!bValid1 || !bValid2)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiArrayLength = 1;
    if (pkEntry->IsArray())
    {
        uiArrayLength = pkEntry->GetDataSize() / pkEntry->GetDataStride();
        NiUInt32 uiRegPerEntry = uiRegisterCount / uiArrayLength;
        NiUInt32 uiRemainder = uiRegisterCount % uiArrayLength;
        // The registers will not divide evenly if each array element
        // ends with a partially filled register - in that case, the 
        // remainder will be (uiArrayLength - 1);
        NIASSERT(uiRemainder == 0 || uiRemainder == uiArrayLength - 1);
        if (uiRemainder != 0)
            uiRegPerEntry++;

        uiRegisterCount = uiRegPerEntry;
    }

    return FillShaderConstantBuffer(pvShaderConstantBuffer,
        pvDataSource, uiStartRegister, uiStartElement, 
        uiRegisterCount, uiElementCount, uiArrayLength, bPackRegisters, NULL);
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::UpdateObjectConstantValue(
    void* pvShaderConstantBuffer, NiShaderConstantMapEntry* pkEntry, 
    const NiRenderCallContext& kRCC)
{
    D3DXMATRIXA16 kResult;

    const void* pvDataSource = ObtainObjectConstantValue(pkEntry, kRCC,
        kResult);
    if (pvDataSource == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiEncodedShaderRegister = pkEntry->GetShaderRegister();
    NiUInt32 uiEncodedRegisterCount = pkEntry->GetRegisterCount();

    NiUInt32 uiStartRegister = 0;
    NiUInt32 uiStartElement = 0;
    NiUInt32 uiRegisterCount = 0;
    NiUInt32 uiElementCount = 0;
    bool bPackRegisters = false;

    bool bValid1 = DecodePackedRegisterAndElement(uiEncodedShaderRegister, 
        uiStartRegister, uiStartElement, bPackRegisters);
    NIASSERT(bPackRegisters == false);
    bool bValid2 = DecodePackedRegisterAndElement(uiEncodedRegisterCount, 
        uiRegisterCount, uiElementCount, bPackRegisters);
    if (!bValid1 || !bValid2)
    {
        return NISHADERERR_UNKNOWN;
    }

    NiUInt32 uiArrayLength = 1;
    if (pkEntry->IsArray())
    {
        uiArrayLength = pkEntry->GetDataSize() / pkEntry->GetDataStride();
        NiUInt32 uiRegPerEntry = uiRegisterCount / uiArrayLength;
        NiUInt32 uiRemainder = uiRegisterCount % uiArrayLength;
        // The registers will not divide evenly if each array element
        // ends with a partially filled register - in that case, the 
        // remainder will be (uiArrayLength - 1);
        NIASSERT(uiRemainder == 0 || uiRemainder == uiArrayLength - 1);
        if (uiRemainder != 0)
            uiRegPerEntry++;

        uiRegisterCount = uiRegPerEntry;
    }

    return FillShaderConstantBuffer(pvShaderConstantBuffer,
       pvDataSource, uiStartRegister, uiStartElement, 
       uiRegisterCount, uiElementCount, uiArrayLength, bPackRegisters, NULL);
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10ShaderConstantMap::FillShaderConstantBuffer(
    void* pvShaderConstantBuffer, const void* pvSourceData, 
    NiUInt32 uiStartRegister, NiUInt32 uiStartElement, 
    NiUInt32 uiRegisterCount, NiUInt32 uiElementCount, 
    NiUInt32 uiArrayCount, bool bPackRegisters, 
    const NiUInt16* puiReorderArray)
{
    const size_t uiElementSize = 
    D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8;

    NiUInt8* puiTemp = (NiUInt8*)pvShaderConstantBuffer;
    NiUInt8* puiWriteLocation = puiTemp + (uiStartRegister * 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS + 
        uiStartElement) * uiElementSize;
    if (uiElementSize == 4)
    {
        if (uiElementCount != 0)
        {
            // The encoded register count is only _full_ registers, so
            // additional elements go to the next register.
            uiRegisterCount++;
        }
        else  //  uiElementCount == 0
        {
            // If no additional elements are required, then we're going
            // to need to fill in all elements of each register.
            uiElementCount = 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
            bPackRegisters = true;
        }
                                                    
        // If bPackRegisters is true, then we write all 4 elements on all
        // but the last register - on that one, we only write 
        // uiElementCount. If bPackRegisters if false, we write
        // uiElementCount elements in all registers.
        const NiUInt32 uiWriteElements =
            ((bPackRegisters == false) ? 
            uiElementCount : 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS);

        const size_t uiArrayElementSize = 
            uiRegisterCount * uiElementCount * uiElementSize;
        const NiUInt32 uiWriteLocationDelta = uiElementSize * 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENTS;

        
        if (puiReorderArray)
        {
            for (NiUInt32 i = 0; i < uiArrayCount; i++)
            {
                
                NiUInt8* puiSource = (NiUInt8*)pvSourceData + 
                    puiReorderArray[i] * uiArrayElementSize;
              
                NiMemcpyFloatArray( (float*) puiWriteLocation ,
                    (const float*) puiSource ,
                    uiWriteElements*(uiRegisterCount-1)); 
                
                puiSource += (uiRegisterCount-1) * 
                    uiWriteElements*uiElementSize;
                
                puiWriteLocation += (uiRegisterCount-1) * 
                    uiWriteLocationDelta;

                NiMemcpyFloatArray( (float*) puiWriteLocation ,
                    (const float*) puiSource ,uiElementCount); 
                puiSource += uiElementCount*uiElementSize;

                puiWriteLocation += uiWriteLocationDelta;
            }
        }
        else
        {
            for (NiUInt32 i = 0; i < uiArrayCount; i++)
            {
                NiUInt8* puiSource = (NiUInt8*)pvSourceData + 
                    i * uiArrayElementSize;
                  

                for (NiUInt32 j = 0; j < uiRegisterCount-1; j++)
                {
                   
                    NiMemcpyFloatArray( (float*) puiWriteLocation ,
                        (const float*) puiSource ,uiWriteElements); 
                    puiSource += uiWriteElements*uiElementSize;
                
                    puiWriteLocation += uiWriteLocationDelta;
                 
                }

                NiMemcpyFloatArray( (float*) puiWriteLocation ,
                    (const float*) puiSource ,uiElementCount); 
                puiSource += uiElementCount*uiElementSize;

                puiWriteLocation += uiWriteLocationDelta;
            }
        }
    }
    else
    {

        if (uiElementCount != 0)
        {
            // The encoded register count is only _full_ registers, so
            // additional elements go to the next register.
            uiRegisterCount++;
        }
        else
        {
            // If no additional elements are required, then we're going
            // to need to fill in all elements of each register.
            uiElementCount = 
                D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
            bPackRegisters = true;
        }

        const size_t uiArrayElementSize = 
            uiRegisterCount * uiElementCount * uiElementSize;

        for (NiUInt32 i = 0; i < uiArrayCount; i++)
        {
            NiUInt8* puiSource = (NiUInt8*)pvSourceData;
            if (puiReorderArray)
                puiSource += puiReorderArray[i] * uiArrayElementSize;
            else
                puiSource += i * uiArrayElementSize;

            for (NiUInt32 j = 0; j < uiRegisterCount; j++)
            {
                NiUInt8* puiDest = puiWriteLocation;
                // If bPackRegisters is true, then we write all 4 elements on
                // all but the last register - on that one, we only write 
                // uiElementCount. If bPackRegisters if false, we write
                // uiElementCount elements in all registers.
                const NiUInt32 uiWriteElements =
                    ((bPackRegisters == false || j == uiRegisterCount - 1) ? 
                    uiElementCount : 
                    D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS);
                for (NiUInt32 k = 0; k < uiWriteElements; k++)
                {
                    // 4-byte register elements
                    memcpy(puiDest, puiSource, uiElementSize);
                    puiSource += uiElementSize;
                    puiDest += uiElementSize;
                }
                puiWriteLocation += uiElementSize * 
                    D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENTS;
            }
        }
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::ObtainDefinedConstantValue(
    NiShaderConstantMapEntry* pkEntry, const NiRenderCallContext& kRCC, 
    D3DXMATRIXA16& kTempMatrix)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();

    NIASSERT(kRCC.m_pkMesh != NULL && kRCC.m_pkState != NULL);

    NiUInt32 uiInternal = pkEntry->GetInternal();
    switch (uiInternal)
    {
    // Transformations
    case SCM_DEF_PROJ:
    case SCM_DEF_INVPROJ:
    case SCM_DEF_PROJ_T:
    case SCM_DEF_INVPROJ_T:
    {
        bool bInv = false;
        bool bTrans = false;

        if ((uiInternal == SCM_DEF_INVPROJ) ||
            (uiInternal == SCM_DEF_INVPROJ_T))
        {
            bInv = true;
        }
        if ((uiInternal == SCM_DEF_PROJ_T) ||
            (uiInternal == SCM_DEF_INVPROJ_T))
        {
            bTrans = true;
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        kTempMatrix = pkRenderer->GetProjectionMatrix();

        if (bInv)
            D3DXMatrixInverse(&kTempMatrix, 0, &kTempMatrix);
        if (bTrans)
            D3DXMatrixTranspose(&kTempMatrix, &kTempMatrix);

        return &kTempMatrix;
    }
    case SCM_DEF_VIEW:
    case SCM_DEF_INVVIEW:
    case SCM_DEF_VIEW_T:
    case SCM_DEF_INVVIEW_T:
    {
        bool bInv = false;
        bool bTrans = false;

        if ((uiInternal == SCM_DEF_INVVIEW) ||
            (uiInternal == SCM_DEF_INVVIEW_T))
        {
            bInv = true;
        }
        if ((uiInternal == SCM_DEF_VIEW_T) ||
            (uiInternal == SCM_DEF_INVVIEW_T))
        {
            bTrans = true;
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        kTempMatrix = pkRenderer->GetViewMatrix();

        if (bInv)
            D3DXMatrixInverse(&kTempMatrix, 0, &kTempMatrix);
        if (bTrans)
            D3DXMatrixTranspose(&kTempMatrix, &kTempMatrix);

        return &kTempMatrix;
    }
    case SCM_DEF_WORLD:
    case SCM_DEF_INVWORLD:
    case SCM_DEF_WORLD_T:
    case SCM_DEF_INVWORLD_T:
    {
        bool bInv = false;
        bool bTrans = false;

        if ((uiInternal == SCM_DEF_INVWORLD) ||
            (uiInternal == SCM_DEF_INVWORLD_T))
        {
            bInv = true;
        }
        if ((uiInternal == SCM_DEF_WORLD_T) ||
            (uiInternal == SCM_DEF_INVWORLD_T))
        {
            bTrans = true;
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        kTempMatrix = pkRenderer->GetWorldMatrix();

        if (bInv)
            D3DXMatrixInverse(&kTempMatrix, 0, &kTempMatrix);
        if (bTrans)
            D3DXMatrixTranspose(&kTempMatrix, &kTempMatrix);

        return &kTempMatrix;
    }
    case SCM_DEF_WORLDVIEW:
    case SCM_DEF_INVWORLDVIEW:
    case SCM_DEF_WORLDVIEW_T:
    case SCM_DEF_INVWORLDVIEW_T:
    {
        bool bInv = false;
        bool bTrans = false;

        if ((uiInternal == SCM_DEF_INVWORLDVIEW) ||
            (uiInternal == SCM_DEF_INVWORLDVIEW_T))
        {
            bInv = true;
        }
        if ((uiInternal == SCM_DEF_WORLDVIEW_T) ||
            (uiInternal == SCM_DEF_INVWORLDVIEW_T))
        {
            bTrans = true;
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        const D3DXMATRIXA16& kD3DWorld = pkRenderer->GetWorldMatrix();
        const D3DXMATRIXA16& kD3DView = pkRenderer->GetViewMatrix();
        kTempMatrix = kD3DWorld * kD3DView;

        if (bInv)
            D3DXMatrixInverse(&kTempMatrix, 0, &kTempMatrix);
        if (bTrans)
            D3DXMatrixTranspose(&kTempMatrix, &kTempMatrix);

        return &kTempMatrix;
    }
    case SCM_DEF_VIEWPROJ:
    case SCM_DEF_INVVIEWPROJ:
    case SCM_DEF_VIEWPROJ_T:
    case SCM_DEF_INVVIEWPROJ_T:
    {
        bool bInv = false;
        bool bTrans = false;

        if ((uiInternal == SCM_DEF_INVVIEWPROJ) ||
            (uiInternal == SCM_DEF_INVVIEWPROJ_T))
        {
            bInv = true;
        }
        if ((uiInternal == SCM_DEF_VIEWPROJ_T) ||
            (uiInternal == SCM_DEF_INVVIEWPROJ_T))
        {
            bTrans = true;
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        const D3DXMATRIXA16& kD3DView = pkRenderer->GetViewMatrix();
        const D3DXMATRIXA16& kD3DProj = pkRenderer->GetProjectionMatrix();
        kTempMatrix = kD3DView * kD3DProj;

        if (bInv)
            D3DXMatrixInverse(&kTempMatrix, 0, &kTempMatrix);
        if (bTrans)
            D3DXMatrixTranspose(&kTempMatrix, &kTempMatrix);

        return &kTempMatrix;
    }
    case SCM_DEF_WORLDVIEWPROJ:
    case SCM_DEF_INVWORLDVIEWPROJ:
    case SCM_DEF_WORLDVIEWPROJ_T:
    case SCM_DEF_INVWORLDVIEWPROJ_T:
    {
        bool bInv = false;
        bool bTrans = false;

        if ((uiInternal == SCM_DEF_INVWORLDVIEWPROJ) ||
            (uiInternal == SCM_DEF_INVWORLDVIEWPROJ_T))
        {
            bInv = true;
        }
        if ((uiInternal == SCM_DEF_WORLDVIEWPROJ_T) ||
            (uiInternal == SCM_DEF_INVWORLDVIEWPROJ_T))
        {
            bTrans = true;
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        const D3DXMATRIXA16& kD3DWorld = pkRenderer->GetWorldMatrix();
        const D3DXMATRIXA16& kD3DView = pkRenderer->GetViewMatrix();
        const D3DXMATRIXA16& kD3DProj = pkRenderer->GetProjectionMatrix();
        D3DXMATRIXA16 kD3DWorldView = kD3DWorld * kD3DView;
        kTempMatrix = kD3DWorldView * kD3DProj;

        if (bInv)
            D3DXMatrixInverse(&kTempMatrix, 0, &kTempMatrix);
        if (bTrans)
            D3DXMatrixTranspose(&kTempMatrix, &kTempMatrix);

        return &kTempMatrix;
    }
    case SCM_DEF_SKINBONE_MATRIX_3:
    {
        NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

        NiSkinningMeshModifier* pkSkin = 
            NiGetModifier(NiSkinningMeshModifier, pkMesh);

        if (pkSkin)
        {
            return pkSkin->GetBoneMatrices();
        }
        else
        {
            return NULL;
        }
    }
    // Texture transforms
    case SCM_DEF_TEXTRANSFORMBASE:
    case SCM_DEF_INVTEXTRANSFORMBASE:
    case SCM_DEF_TEXTRANSFORMBASE_T:
    case SCM_DEF_INVTEXTRANSFORMBASE_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetBaseMap();
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMBASE) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMBASE_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMBASE_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMBASE_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMDARK:
    case SCM_DEF_INVTEXTRANSFORMDARK:
    case SCM_DEF_TEXTRANSFORMDARK_T:
    case SCM_DEF_INVTEXTRANSFORMDARK_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetDarkMap();
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMDARK) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMDARK_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMDARK_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMDARK_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMDETAIL:
    case SCM_DEF_INVTEXTRANSFORMDETAIL:
    case SCM_DEF_TEXTRANSFORMDETAIL_T:
    case SCM_DEF_INVTEXTRANSFORMDETAIL_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetDetailMap();
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMDETAIL) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMDETAIL_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMDETAIL_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMDETAIL_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMGLOSS:
    case SCM_DEF_INVTEXTRANSFORMGLOSS:
    case SCM_DEF_TEXTRANSFORMGLOSS_T:
    case SCM_DEF_INVTEXTRANSFORMGLOSS_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetGlossMap();
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMGLOSS) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMGLOSS_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMGLOSS_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMGLOSS_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMGLOW:
    case SCM_DEF_INVTEXTRANSFORMGLOW:
    case SCM_DEF_TEXTRANSFORMGLOW_T:
    case SCM_DEF_INVTEXTRANSFORMGLOW_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetGlowMap();
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMGLOW) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMGLOW_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMGLOW_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMGLOW_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMBUMP:
    case SCM_DEF_INVTEXTRANSFORMBUMP:
    case SCM_DEF_TEXTRANSFORMBUMP_T:
    case SCM_DEF_INVTEXTRANSFORMBUMP_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetBumpMap();
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMBUMP) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMBUMP_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMBUMP_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMBUMP_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMNORMAL:
    case SCM_DEF_INVTEXTRANSFORMNORMAL:
    case SCM_DEF_TEXTRANSFORMNORMAL_T:
    case SCM_DEF_INVTEXTRANSFORMNORMAL_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetNormalMap();
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMNORMAL) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMNORMAL_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMNORMAL_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMNORMAL_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMPARALLAX:
    case SCM_DEF_INVTEXTRANSFORMPARALLAX:
    case SCM_DEF_TEXTRANSFORMPARALLAX_T:
    case SCM_DEF_INVTEXTRANSFORMPARALLAX_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetParallaxMap();
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMPARALLAX) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMPARALLAX_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMPARALLAX_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMPARALLAX_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMDECAL:
    case SCM_DEF_INVTEXTRANSFORMDECAL:
    case SCM_DEF_TEXTRANSFORMDECAL_T:
    case SCM_DEF_INVTEXTRANSFORMDECAL_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = 
                pkTexProp->GetDecalMap(pkEntry->GetExtra());
            if (pkMap)
            {
                const NiTextureTransform* pkTextureTransform = 
                    pkMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMDECAL) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMDECAL_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMDECAL_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMDECAL_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix,  bTrans);

        return &kTempMatrix;
    }
    case SCM_DEF_TEXTRANSFORMSHADER:
    case SCM_DEF_INVTEXTRANSFORMSHADER:
    case SCM_DEF_TEXTRANSFORMSHADER_T:
    case SCM_DEF_INVTEXTRANSFORMSHADER_T:
    {
        const NiMatrix3* pkMatrix = NULL;

        bool bTrans = false;

        NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            NiTexturingProperty::Map* pkShaderMap = 
                pkTexProp->GetShaderMap(pkEntry->GetExtra());
            if (pkShaderMap)
            {
                NiTextureTransform* pkTextureTransform = 
                    pkShaderMap->GetTextureTransform();

                if (pkTextureTransform)
                {
                    pkMatrix = pkTextureTransform->GetMatrix();

                    if ((uiInternal == SCM_DEF_INVTEXTRANSFORMSHADER) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMSHADER_T))
                    {
                    }
                    if ((uiInternal == SCM_DEF_TEXTRANSFORMSHADER_T) ||
                        (uiInternal == SCM_DEF_INVTEXTRANSFORMSHADER_T))
                    {
                        bTrans = true;
                    }
                }
            }
        }

        // If it's column major, then we must manually transpose.
        if (pkEntry->GetColumnMajor())
            bTrans = !bTrans;

        SetupTextureTransformMatrix(kTempMatrix, pkMatrix, bTrans);

        return &kTempMatrix;
    }

    // Lighting
    case SCM_DEF_LIGHT_POS_WS:
    case SCM_DEF_LIGHT_DIR_WS:
    case SCM_DEF_LIGHT_POS_OS:
    case SCM_DEF_LIGHT_DIR_OS:
    {
#ifdef NIDEBUG
        char acString[256];
        NiSprintf(acString, 256, "Constant \"%s\" is not supported.\n",
            (const char*)pkEntry->GetKey());
        NiOutputDebugString(acString);
#endif
        break;
    }
    // Materials
    case SCM_DEF_MATERIAL_DIFFUSE:
    {
        NiMaterialProperty* pkMaterial = kRCC.m_pkState->GetMaterial();
        if (pkMaterial)
        {
            kTempMatrix._11 = pkMaterial->GetDiffuseColor().r;
            kTempMatrix._12 = pkMaterial->GetDiffuseColor().g;
            kTempMatrix._13 = pkMaterial->GetDiffuseColor().b;
            kTempMatrix._14 = pkMaterial->GetAlpha();
        }
        return &kTempMatrix;
    }
    case SCM_DEF_MATERIAL_AMBIENT:
    {
        NiMaterialProperty* pkMaterial = kRCC.m_pkState->GetMaterial();
        if (pkMaterial)
        {
            kTempMatrix._11 = pkMaterial->GetAmbientColor().r;
            kTempMatrix._12 = pkMaterial->GetAmbientColor().g;
            kTempMatrix._13 = pkMaterial->GetAmbientColor().b;
            kTempMatrix._14 = pkMaterial->GetAlpha();
        }
        return &kTempMatrix;
    }
    case SCM_DEF_MATERIAL_SPECULAR:
    {
        NiMaterialProperty* pkMaterial = kRCC.m_pkState->GetMaterial();
        if (pkMaterial)
        {
            kTempMatrix._11 = pkMaterial->GetSpecularColor().r;
            kTempMatrix._12 = pkMaterial->GetSpecularColor().g;
            kTempMatrix._13 = pkMaterial->GetSpecularColor().b;
            kTempMatrix._14 = pkMaterial->GetAlpha();
        }
        return &kTempMatrix;
    }
    case SCM_DEF_MATERIAL_EMISSIVE:
    {
        NiMaterialProperty* pkMaterial = kRCC.m_pkState->GetMaterial();
        if (pkMaterial)
        {
            kTempMatrix._11 = pkMaterial->GetEmittance().r;
            kTempMatrix._12 = pkMaterial->GetEmittance().g;
            kTempMatrix._13 = pkMaterial->GetEmittance().b;
            kTempMatrix._14 = pkMaterial->GetAlpha();
        }
        return &kTempMatrix;
    }
    case SCM_DEF_MATERIAL_POWER:
    {
        NiMaterialProperty* pkMaterial = kRCC.m_pkState->GetMaterial();
        if (pkMaterial)
        {
            kTempMatrix._11 = pkMaterial->GetShineness();
            kTempMatrix._12 = pkMaterial->GetShineness();
            kTempMatrix._13 = pkMaterial->GetShineness();
            kTempMatrix._14 = pkMaterial->GetShineness();
        }
        return &kTempMatrix;
    }
    // Eye
    case SCM_DEF_EYE_POS:
    {
        const D3DXMATRIXA16& kViewMat = pkRenderer->GetInverseViewMatrix();

        kTempMatrix._11 = kViewMat._41;
        kTempMatrix._12 = kViewMat._42;
        kTempMatrix._13 = kViewMat._43;
        kTempMatrix._14 = kViewMat._44;
        return &kTempMatrix;
    }
    case SCM_DEF_EYE_DIR:
    {
        const D3DXMATRIXA16& kViewMat = pkRenderer->GetInverseViewMatrix();

        kTempMatrix._11 = kViewMat._31;
        kTempMatrix._12 = kViewMat._32;
        kTempMatrix._13 = kViewMat._33;
        kTempMatrix._14 = kViewMat._34;
        return &kTempMatrix;
    }
    // Constants
    case SCM_DEF_CONSTS_TAYLOR_SIN:
    case SCM_DEF_CONSTS_TAYLOR_COS:
    // Just set the data
    {
        return ObtainConstantConstantValue(pkEntry, kRCC);
        break;
    }
    // Time
    case SCM_DEF_CONSTS_TIME:
    case SCM_DEF_CONSTS_SINTIME:
    case SCM_DEF_CONSTS_COSTIME:
    case SCM_DEF_CONSTS_TANTIME:
    case SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
    {
        // Grab the attribute from the geometry and set it
        NiFloatExtraData* pkFloatED = 
            (NiFloatExtraData*)kRCC.m_pkMesh->GetExtraData(
            NiShaderConstantMap::GetTimeExtraDataName());
        if (!pkFloatED)
        {
            return NULL;
        }

        float fTime = pkFloatED->GetValue();

        switch (uiInternal)
        {
        case SCM_DEF_CONSTS_TIME:
            kTempMatrix._11 = fTime;
            kTempMatrix._12 = fTime;
            kTempMatrix._13 = fTime;
            kTempMatrix._14 = fTime;
            break;
        case SCM_DEF_CONSTS_SINTIME:
            kTempMatrix._11 = sinf(fTime);
            kTempMatrix._12 = kTempMatrix._11;
            kTempMatrix._13 = kTempMatrix._11;
            kTempMatrix._14 = kTempMatrix._11;
            break;
        case SCM_DEF_CONSTS_COSTIME:
            kTempMatrix._11 = cosf(fTime);
            kTempMatrix._12 = kTempMatrix._11;
            kTempMatrix._13 = kTempMatrix._11;
            kTempMatrix._14 = kTempMatrix._11;
            break;
        case SCM_DEF_CONSTS_TANTIME:
            kTempMatrix._11 = tanf(fTime);
            kTempMatrix._12 = kTempMatrix._11;
            kTempMatrix._13 = kTempMatrix._11;
            kTempMatrix._14 = kTempMatrix._11;
            break;
        case SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
            kTempMatrix._11 = fTime;
            kTempMatrix._12 = sinf(fTime);
            kTempMatrix._13 = cosf(fTime);
            kTempMatrix._14 = tanf(fTime);
            break;
        default:
            NIASSERT(!"Time set --> Invalid case!");
        }
        return &kTempMatrix;
    }
    case SCM_DEF_AMBIENTLIGHT:
    {
        kTempMatrix._11 = kTempMatrix._12 = kTempMatrix._13 = 0.0f;
        kTempMatrix._14 = 1.0f;
        if (kRCC.m_pkEffects)
        {
            NiDynEffectStateIter kIter = kRCC.m_pkEffects->GetLightHeadPos();
            while (kIter)
            {
                NiAmbientLight* pkLight = NiDynamicCast(NiAmbientLight,
                    kRCC.m_pkEffects->GetNextLight(kIter));
                if (pkLight)
                {
                    NiColor kColor = pkLight->GetAmbientColor() *
                        pkLight->GetDimmer();
                    kTempMatrix._11 += kColor.r;
                    kTempMatrix._12 += kColor.g;
                    kTempMatrix._13 += kColor.b;
                }
            }
        }
        return &kTempMatrix;
    }
    case SCM_DEF_FOG_DENSITY:
    {
        NiFogProperty* pkFog = kRCC.m_pkState->GetFog();
        NIASSERT(pkFog);

        float fNear, fFar;
        pkRenderer->GetCameraNearAndFar(fNear, fFar);
        float fDensity = 1.0f / (pkFog->GetDepth() * (fFar - fNear));

        kTempMatrix._11 = fDensity;
        kTempMatrix._12 = fDensity;
        kTempMatrix._13 = fDensity;
        kTempMatrix._14 = fDensity;
        return &kTempMatrix;
    }
    case SCM_DEF_FOG_NEARFAR:
    {
        NiFogProperty* pkFog = kRCC.m_pkState->GetFog();
        NIASSERT(pkFog);

        float fNear, fFar;
        pkRenderer->GetCameraNearAndFar(fNear, fFar);
        float fCameraDepthRange = fFar - fNear;

        float fWorldDepth = fCameraDepthRange * pkFog->GetDepth();
        float fFogNear = fFar - fWorldDepth;

        float fFogFar = fFar + 
            pkRenderer->GetMaxFogFactor() * fWorldDepth;

        kTempMatrix._11 = fFogNear;
        kTempMatrix._12 = fFogFar;
        kTempMatrix._13 = 0.0f;
        kTempMatrix._14 = 0.0f;
        return &kTempMatrix;
    }
    case SCM_DEF_FOG_COLOR:
    {
        NiFogProperty* pkFog = kRCC.m_pkState->GetFog();
        NIASSERT(pkFog);

        const NiColor& kFogColor = pkFog->GetFogColor();

        kTempMatrix._11 = kFogColor.r;
        kTempMatrix._12 = kFogColor.g;
        kTempMatrix._13 = kFogColor.b;
        kTempMatrix._14 = 1.0f;

        return &kTempMatrix;
    }
    case SCM_DEF_PARALLAX_OFFSET:
    {
        NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        float fOffset = 0.0f;
        if (pkTexProp)
        {
            NiTexturingProperty::ParallaxMap* pkParallaxMap = 
                pkTexProp->GetParallaxMap();
            if (pkParallaxMap)
            {
                fOffset = pkParallaxMap->GetOffset();
            }
        }
        kTempMatrix._11 = fOffset;

        return &kTempMatrix;
    }
    case SCM_DEF_BUMP_MATRIX:
    {
        NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        NiTexturingProperty::BumpMap* pkBumpMap = NULL;
        if (pkTexProp)
            pkBumpMap = pkTexProp->GetBumpMap();
        if (pkBumpMap)
        {
            kTempMatrix._11 = pkBumpMap->GetBumpMat00();
            kTempMatrix._12 = pkBumpMap->GetBumpMat01();
            kTempMatrix._13 = pkBumpMap->GetBumpMat10();
            kTempMatrix._14 = pkBumpMap->GetBumpMat11();
        }
        else
        {
            kTempMatrix._11 = 1.0f;
            kTempMatrix._12 = 1.0f;
            kTempMatrix._13 = 1.0f;
            kTempMatrix._14 = 1.0f;
        }

        return &kTempMatrix;
    }
    case SCM_DEF_BUMP_LUMA_OFFSET_AND_SCALE:
    {
        NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        NiTexturingProperty::BumpMap* pkBumpMap = NULL;
        if (pkTexProp)
            pkBumpMap = pkTexProp->GetBumpMap();
        if (pkBumpMap)
        {
            kTempMatrix._11 = pkBumpMap->GetLumaOffset();
            kTempMatrix._12 = pkBumpMap->GetLumaScale();
        }
        else
        {
            kTempMatrix._11 = 0.0f;
            kTempMatrix._12 = 1.0f;
        }

        return &kTempMatrix;
    }
    case SCM_DEF_TEXSIZEBASE:
    case SCM_DEF_TEXSIZEDARK:
    case SCM_DEF_TEXSIZEDETAIL:
    case SCM_DEF_TEXSIZEGLOSS:
    case SCM_DEF_TEXSIZEGLOW:
    case SCM_DEF_TEXSIZEBUMP:
    case SCM_DEF_TEXSIZENORMAL:
    case SCM_DEF_TEXSIZEPARALLAX:
    case SCM_DEF_TEXSIZEDECAL:
    case SCM_DEF_TEXSIZESHADER:
    {
        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
        if (pkTexProp)
        {
            const NiTexturingProperty::Map* pkMap = NULL;

            switch (uiInternal)
            {
            case SCM_DEF_TEXSIZEBASE:
                pkMap = pkTexProp->GetBaseMap();
                break;
            case SCM_DEF_TEXSIZEDARK:
                pkMap = pkTexProp->GetDarkMap();
                break;
            case SCM_DEF_TEXSIZEDETAIL:
                pkMap = pkTexProp->GetDetailMap();
                break;
            case SCM_DEF_TEXSIZEGLOSS:
                pkMap = pkTexProp->GetGlossMap();
                break;
            case SCM_DEF_TEXSIZEGLOW:
                pkMap = pkTexProp->GetGlowMap();
                break;
            case SCM_DEF_TEXSIZEBUMP:
                pkMap = pkTexProp->GetBumpMap();
                break;
            case SCM_DEF_TEXSIZENORMAL:
                pkMap = pkTexProp->GetNormalMap();
                break;
            case SCM_DEF_TEXSIZEPARALLAX:
                pkMap = pkTexProp->GetParallaxMap();
                break;
            case SCM_DEF_TEXSIZEDECAL:
                pkMap = pkTexProp->GetDecalMap(pkEntry->GetExtra());
                break;
            case SCM_DEF_TEXSIZESHADER:
                pkMap = pkTexProp->GetShaderMap(pkEntry->GetExtra());
                break;
            }

            if (pkMap && pkMap->GetTexture())
            {
                NiTexture* pkTex = pkMap->GetTexture();
                kTempMatrix._11 = (float) pkTex->GetWidth();
                kTempMatrix._12 = (float) pkTex->GetHeight();
            }
            else
            {
                kTempMatrix._11 = 0.0f;
                kTempMatrix._12 = 0.0f;
            }
        }        
        return &kTempMatrix;
    }
    case SCM_DEF_ALPHA_TEST_FUNC:
    {
        const NiAlphaProperty* pkAlphaProp = kRCC.m_pkState->GetAlpha();
        // Fill this vector with 0 or 1 for these situations:
        // X: 1 if we should clip when the value is greater than the ref,
        //    0 otherwise.
        //    This value should be set to 1 for TEST_NEVER, TEST_LESS, 
        //    TEST_EQUAL, and TEST_LESS_EQUAL.
        // Y: 1 if we should clip when the value is less than the ref,
        //    0 otherwise.
        //    This value should be set to 1 for TEST_NEVER, TEST_EQUAL, 
        //    TEST_GREATER, and TEST_GREATER_EQUAL.
        // Z: 1 if we should clip when the value is equal to the ref,
        //    0 otherwise.
        //    This value should be set to 1 for TEST_NEVER, TEST_LESS, 
        //    TEST_NOTEQUAL, and TEST_GREATER.
        NiPoint3 kTestConditions = NiPoint3::ZERO;
        if (pkAlphaProp)
        {
            switch(pkAlphaProp->GetTestMode())
            {
            case NiAlphaProperty::TEST_LESS:
                kTestConditions.x = 1.0f;
                kTestConditions.z = 1.0f;
                break;
            case NiAlphaProperty::TEST_EQUAL:
                kTestConditions.x = 1.0f;
                kTestConditions.y = 1.0f;
                break;
            case NiAlphaProperty::TEST_LESSEQUAL:
                kTestConditions.x = 1.0f;
                break;
            case NiAlphaProperty::TEST_GREATER:
                kTestConditions.y = 1.0f;
                kTestConditions.z = 1.0f;
                break;
            case NiAlphaProperty::TEST_NOTEQUAL:
                kTestConditions.z = 1.0f;
                break;
            case NiAlphaProperty::TEST_GREATEREQUAL:
                kTestConditions.y = 1.0f;
                break;
            case NiAlphaProperty::TEST_NEVER:
                kTestConditions.x = 1.0f;
                kTestConditions.y = 1.0f;
                kTestConditions.z = 1.0f;
                break;
            case NiAlphaProperty::TEST_ALWAYS:
            default:
                break;
            }
        }
        
        kTempMatrix._11 = kTestConditions.x;
        kTempMatrix._12 = kTestConditions.y;
        kTempMatrix._13 = kTestConditions.z;
        return &kTempMatrix;
    }
    case SCM_DEF_ALPHA_TEST_REF:
    {
        const NiAlphaProperty* pkAlphaProp = kRCC.m_pkState->GetAlpha();
        float fRef = 0.0f;
        if (pkAlphaProp)
        {
            fRef = (float)pkAlphaProp->GetTestRef() / 255.0f;
        }

        kTempMatrix._11 = fRef;
        return &kTempMatrix;
    }

    case SCM_DEF_SKINWORLDVIEW:
    case SCM_DEF_INVSKINWORLDVIEW:
    case SCM_DEF_SKINWORLDVIEW_T:
    case SCM_DEF_INVSKINWORLDVIEW_T:
    case SCM_DEF_SKINWORLDVIEWPROJ:
    case SCM_DEF_INVSKINWORLDVIEWPROJ:
    case SCM_DEF_SKINWORLDVIEWPROJ_T:
    case SCM_DEF_INVSKINWORLDVIEWPROJ_T:
    case SCM_DEF_SKINWORLD:
    case SCM_DEF_INVSKINWORLD:
    case SCM_DEF_SKINWORLD_T:
    case SCM_DEF_INVSKINWORLD_T:
    case SCM_DEF_BONE_MATRIX_3:
    case SCM_DEF_BONE_MATRIX_4:
    case SCM_DEF_SKINBONE_MATRIX_4:
        {
            NiFixedString kName;
            NIVERIFY(NiShaderConstantMap::LookUpPredefinedMappingName(
                uiInternal, kName));
            NILOG("The predefined mapping %s is deprecated\n",
                (const char*)kName);
        }
        break;

    default:
    {
#ifdef NIDEBUG
            char acString[256];
            NiSprintf(acString, 256, "Constant \"%s\" is not supported.\n",
                (const char*)pkEntry->GetKey());
            NiOutputDebugString(acString);
#endif
        return NULL;
    }
    }

    return NULL;
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::ObtainConstantConstantValue(
    NiShaderConstantMapEntry* pkEntry, const NiRenderCallContext&)
{
    return pkEntry->GetDataSource();
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::ObtainAttributeConstantValue(
    NiShaderConstantMapEntry* pkEntry, const NiRenderCallContext& kRCC, 
    bool bGlobal, NiExtraData* pkExtraData, D3DXMATRIXA16& kTempMatrix)
{
    NIASSERT(kRCC.m_pkMesh != NULL);
    // Attempt to get the extra data for this attribute from the cache
    // rather than using strcmp.
    NiExtraData* pkExtra = 0;
    if (pkExtraData)
    {
        NiSCMExtraData* pkShaderData = 
            (NiSCMExtraData*)pkExtraData;
        pkExtra = pkShaderData->GetNextEntry(pkEntry->GetShaderRegister(),
            kRCC.m_uiPass, GetProgramType(), bGlobal);

        // Check for a match - it's possible for a mismatch to occur if
        // an entry had its shader register changed (such as when storing
        // HLSL shader constant registers after the NiSCMExtraData was 
        // created.)
        if (!pkExtra || (pkExtra->GetName() != pkEntry->GetKey()))
        {
            pkExtra = kRCC.m_pkMesh->GetExtraData(pkEntry->GetKey());

            if (pkExtra)
            {
                // If a new match was found, replace the original
                pkShaderData->AddEntry(pkEntry->GetShaderRegister(), 
                    kRCC.m_uiPass, GetProgramType(), pkExtra, bGlobal);
            }
        }
    }
    else if (!pkExtra)
    {
        pkExtra = kRCC.m_pkMesh->GetExtraData(pkEntry->GetKey());
    }

    // If extra data can't be found, use default value
    if (pkExtra == NULL)
        return ObtainConstantConstantValue(pkEntry, kRCC);

    NiShaderAttributeDesc::AttributeType eType = pkEntry->GetAttributeType();
    switch (eType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
    {
        const NiUInt32 uiSize = pkEntry->GetDataSize();
        NI_UNUSED_ARG(uiSize);

        // Get a pointer to the extra data and verify we have enough data
        // (more data is fine, it's just ignored, but less data could crash)
        if (NiIsExactKindOf(NiFloatsExtraData, pkExtra))
        {
            float* pfValue;
            NiUInt32 uiExtraDataSize;

            NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;
            pkFloatsED->GetArray(uiExtraDataSize, pfValue);

            NIASSERT(uiExtraDataSize*sizeof(float) >= uiSize); 
            return pfValue;
        }
        else if (NiIsExactKindOf(NiIntegersExtraData, pkExtra))
        {
            NiInt32* piValue;
            NiUInt32 uiExtraDataSize;

            NiIntegersExtraData* pkIntsED = (NiIntegersExtraData*)pkExtra;
            pkIntsED->GetArray(uiExtraDataSize, piValue);

            NIASSERT(uiExtraDataSize*sizeof(NiInt32) >= uiSize); 
            return piValue;
        }
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    {
        NiBooleanExtraData* pkBoolED = (NiBooleanExtraData*)pkExtra;

        // Ugly nastiness!
        *((NiInt32*)(&kTempMatrix._11)) = pkBoolED->GetValue();

        return &kTempMatrix._11;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
    {
        NiIntegerExtraData* pkIntED = (NiIntegerExtraData*)pkExtra;

        // Ugly nastiness!
        *((NiInt32*)(&kTempMatrix._11)) = pkIntED->GetValue();

        return &kTempMatrix._11;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
    {
        NiFloatExtraData* pkFloatED = (NiFloatExtraData*)pkExtra;

        kTempMatrix._11 = pkFloatED->GetValue();

        return &kTempMatrix._11;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        NiUInt32 uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);
        NIASSERT(uiSize >= 2);

        return pfValue;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        NiUInt32 uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);
        NIASSERT(uiSize >= 3);

        return pfValue;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    {
        if (NiIsExactKindOf(NiFloatsExtraData, pkExtra))
        {
            NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

            NiUInt32 uiSize;
            float* pfValue;

            pkFloatsED->GetArray(uiSize, pfValue);
            NIASSERT(uiSize >= 4);

            return pfValue;
        }
        else if (NiIsExactKindOf(NiColorExtraData, pkExtra))
        {
            NiColorExtraData* pkColorED = (NiColorExtraData*)pkExtra;

            kTempMatrix._11 = pkColorED->GetRed();
            kTempMatrix._12 = pkColorED->GetGreen();
            kTempMatrix._13 = pkColorED->GetBlue();
            kTempMatrix._14 = pkColorED->GetAlpha();

            return &kTempMatrix;
        }
        else
        {
            NIASSERT(!"Unknown extra data type");
        }
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        NiUInt32 uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);
        NIASSERT(uiSize >= 9);

        NIASSERT(pkEntry->GetDataSize() >= 9 * sizeof(float));

        return pfValue;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        NiUInt32 uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);
        NIASSERT(uiSize >= 16);

        NIASSERT(pkEntry->GetDataSize() >= 16 * sizeof(float));

        return pfValue;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
    {
        NiColorExtraData* pkColorED = (NiColorExtraData*)pkExtra;

        kTempMatrix._11 = pkColorED->GetRed();
        kTempMatrix._12 = pkColorED->GetGreen();
        kTempMatrix._13 = pkColorED->GetBlue();
        kTempMatrix._14 = pkColorED->GetAlpha();

        return &kTempMatrix;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        NiUInt32 uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);
        NIASSERT(uiSize >= 8);

        NIASSERT(pkEntry->GetDataSize() >= 8 * sizeof(float));

        return pfValue;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        NiUInt32 uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);
        NIASSERT(uiSize >= 12);

        NIASSERT(pkEntry->GetDataSize() >= 12 * sizeof(float));

        return pfValue;
    }
    }

    return NULL;
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::ObtainOperatorConstantValue(
    NiShaderConstantMapEntry* pkEntry, const NiRenderCallContext& kRCC, 
    bool bGlobal, NiExtraData* pkExtraData, D3DXMATRIXA16& kTempMatrix)
{
    NiUInt32 uiExtra = pkEntry->GetExtra();

    NiUInt32 uiEntry1 = 
        uiExtra & NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY1_MASK;
    NiUInt32 uiEntry2 = 
        (uiExtra & NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_MASK) >> 
        NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_SHIFT;
    NiUInt32 uiOperator = 
        uiExtra & NiShaderConstantMapEntry::SCME_OPERATOR_MASK;
    bool bTranspose = (uiExtra & 
        NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_TRANSPOSE) ? 
        true : false;
    bool bInverse = (uiExtra & 
        NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_INVERSE) ? 
        true : false;

    // If it's column major, then we must manually transpose.
    if (pkEntry->GetColumnMajor())
        bTranspose = !bTranspose;

    // Grab the two entries
    NiShaderConstantMapEntry* pkEntry1 = GetEntryAtIndex(uiEntry1);
    NiShaderConstantMapEntry* pkEntry2 = GetEntryAtIndex(uiEntry2);

    if (!pkEntry1 || !pkEntry2)
    {
        return NULL;
    }

    // Determine the results data type and set it in the flags
    NiShaderAttributeDesc::AttributeType eType1 = 
        pkEntry1->GetAttributeType();
    NiShaderAttributeDesc::AttributeType eType2 = 
        pkEntry2->GetAttributeType();

    const void* pvOperand1 = NULL;
    const void* pvOperand2 = NULL;

    D3DXMATRIXA16 kOperand1;
    D3DXMATRIXA16 kOperand2;

    // Setup entry 1s value
    if (pkEntry1->IsDefined())
    {
        eType1 = LookUpPredefinedMappingType(pkEntry1->GetKey());
        pvOperand1 = ObtainDefinedConstantValue(pkEntry1, kRCC, kOperand1);
    }
    else if (pkEntry1->IsGlobal())
    {
        pvOperand1 = ObtainGlobalConstantValue(pkEntry1, kRCC);
    }
    else if (pkEntry1->IsAttribute())
    {
        pvOperand1 = ObtainAttributeConstantValue(pkEntry1, kRCC, bGlobal,
            pkExtraData, kOperand1);
    }
    else if (pkEntry1->IsConstant())
    {
        pvOperand1 = ObtainConstantConstantValue(pkEntry1, kRCC);
    }

    if (pvOperand1 == NULL)
    {
        return NULL;
    }

    // Setup entry 2s value
    if (pkEntry2->IsDefined())
    {
        eType2 = LookUpPredefinedMappingType(pkEntry2->GetKey());
        pvOperand2 = ObtainDefinedConstantValue(pkEntry2, kRCC, kOperand2);
    }
    else if (pkEntry2->IsGlobal())
    {
        pvOperand2 = ObtainGlobalConstantValue(pkEntry2, kRCC);
    }
    else if (pkEntry2->IsAttribute())
    {
        pvOperand2 = ObtainAttributeConstantValue(pkEntry2, kRCC, bGlobal,
            pkExtraData, kOperand2);
    }
    else if (pkEntry2->IsConstant())
    {
        pvOperand2 = ObtainConstantConstantValue(pkEntry2, kRCC);
    }

    if (pvOperand2 == NULL)
    {
        return NULL;
    }

    if ((eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED))
    {
        return NULL;
    }

    // Perform the operation
    switch (uiOperator)
    {
    case NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY:
        return PerformOperatorMultiply(pvOperand1, eType1, pvOperand2, eType2, 
            bInverse, bTranspose, kTempMatrix);
    case NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE:
        return PerformOperatorDivide(pvOperand1, eType1, pvOperand2, eType2, 
            bInverse, bTranspose, kTempMatrix);
    case NiShaderConstantMapEntry::SCME_OPERATOR_ADD:
        return PerformOperatorAdd(pvOperand1, eType1, pvOperand2, eType2, 
            bInverse, bTranspose, kTempMatrix);
    case NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT:
        return PerformOperatorSubtract(pvOperand1, eType1, pvOperand2, eType2, 
            bInverse, bTranspose, kTempMatrix);
    }

    return NULL;
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::ObtainObjectConstantValue(
    NiShaderConstantMapEntry* pkEntry, const NiRenderCallContext& kRCC, 
    D3DXMATRIXA16& kResult)
{
    // Get NiDynamicEffect corresponding to this object type.
    NiDynamicEffect* pkDynEffect = GetDynamicEffectForObject(kRCC.m_pkEffects,
        pkEntry->GetObjectType(), pkEntry->GetExtra());

    // Get the register count for the mapping type.
    ObjectMappings eMapping = (ObjectMappings)
        pkEntry->GetInternal();
    NiUInt32 uiRegCount, uiFloatCount;
    NiShaderAttributeDesc::AttributeType eType = LookUpObjectMappingType(
        eMapping, uiRegCount, uiFloatCount);
    if (eType == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
    {
        return NULL;
    }

    // Get data to set.
    if (!ObtainDataFromDynamicEffect(pkEntry, eMapping, pkDynEffect, kRCC, 
        kResult))
    {
    }
    return &kResult;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderConstantMap::ObtainDataFromDynamicEffect(
    NiShaderConstantMapEntry* pkEntry, ObjectMappings eMapping, 
    NiDynamicEffect* pkDynEffect, const NiRenderCallContext& kRCC, 
    D3DXMATRIXA16& kResult)
{
    NIASSERT(kRCC.m_pkWorld != NULL && kRCC.m_pkWorldBound != NULL && 
        kRCC.m_pkMesh != NULL);
#ifdef NIDEBUG
    // Ensure that the data size matches the object type.
    NiUInt32 uiRegCount, uiFloatCount;
    NiShaderAttributeDesc::AttributeType eType = LookUpObjectMappingType(
        eMapping, uiRegCount, uiFloatCount);
    NIASSERT(eType != NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED);
#endif

    switch (eMapping)
    {
    case NiShaderConstantMap::SCM_OBJ_DIMMER:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            kResult[0] = ((NiLight*)pkDynEffect)->GetDimmer();
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_UNDIMMEDAMBIENT:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            const NiColor& kColor = 
                ((NiLight*)pkDynEffect)->GetAmbientColor();
            kResult[0] = kColor.r;
            kResult[1] = kColor.g;
            kResult[2] = kColor.b;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_UNDIMMEDDIFFUSE:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            const NiColor& kColor = 
                ((NiLight*)pkDynEffect)->GetDiffuseColor();
            kResult[0] = kColor.r;
            kResult[1] = kColor.g;
            kResult[2] = kColor.b;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_UNDIMMEDSPECULAR:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            const NiColor& kColor = 
                ((NiLight*)pkDynEffect)->GetSpecularColor();
            kResult[0] = kColor.r;
            kResult[1] = kColor.g;
            kResult[2] = kColor.b;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_AMBIENT:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiColor kColor = ((NiLight*)pkDynEffect)->GetAmbientColor();
            kColor *= ((NiLight*)pkDynEffect)->GetDimmer();
            kResult[0] = kColor.r;
            kResult[1] = kColor.g;
            kResult[2] = kColor.b;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_DIFFUSE:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiColor kColor = ((NiLight*)pkDynEffect)->GetDiffuseColor();
            kColor *= ((NiLight*)pkDynEffect)->GetDimmer();
            kResult[0] = kColor.r;
            kResult[1] = kColor.g;
            kResult[2] = kColor.b;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_SPECULAR:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiColor kColor = ((NiLight*)pkDynEffect)->GetSpecularColor();
            kColor *= ((NiLight*)pkDynEffect)->GetDimmer();
            kResult[0] = kColor.r;
            kResult[1] = kColor.g;
            kResult[2] = kColor.b;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDPOSITION:
        if (pkDynEffect)
        {
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                NiPoint3 kPosition = 
                    ((NiDirectionalLight*)pkDynEffect)->GetWorldDirection() *
                    ms_fDirLightDistance;
                kResult[0] = kPosition.x;
                kResult[1] = kPosition.y;
                kResult[2] = kPosition.z;
                kResult[3] = 1.0f;
            }
            else
            {
                kResult[0] = pkDynEffect->GetWorldTranslate().x;
                kResult[1] = pkDynEffect->GetWorldTranslate().y;
                kResult[2] = pkDynEffect->GetWorldTranslate().z;
                kResult[3] = 1.0f;
            }
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELPOSITION:
        if (pkDynEffect)
        {
            NiTransform kInvWorld;
            kRCC.m_pkWorld->Invert(kInvWorld);

            NiPoint3 kPosition;
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                kPosition = kInvWorld * 
                    (((NiDirectionalLight*)pkDynEffect)->GetWorldDirection() *
                    ms_fDirLightDistance);
            }
            else
            {
                kPosition = kInvWorld * pkDynEffect->GetWorldTranslate();
            }

            kResult[0] = kPosition.x;
            kResult[1] = kPosition.y;
            kResult[2] = kPosition.z;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiPoint3 kDirection;
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::POINT_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWPOINT_LIGHT)
            {
                // Get the normalized vector from the light to the
                // center of the bounding volume of the rendered object
                // in world space.
                kDirection = kRCC.m_pkWorldBound->GetCenter() -
                    pkDynEffect->GetWorldTranslate();
                kDirection.Unitize();
            }
            else if (pkDynEffect->GetEffectType() == 
                NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                kDirection = ((NiDirectionalLight*)
                    pkDynEffect)->GetWorldDirection();
            }
            else
            {
                NIASSERT(NiIsExactKindOf(NiSpotLight, pkDynEffect));
                kDirection = ((NiSpotLight*)pkDynEffect)->GetWorldDirection();
            }

            kResult[0] = kDirection.x;
            kResult[1] = kDirection.y;
            kResult[2] = kDirection.z;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 1.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELDIRECTION:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() != NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiPoint3 kDirection;
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::POINT_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWPOINT_LIGHT)
            {
                // Get the normalized vector from the light to the
                // center of the bounding volume of the rendered object
                // in world space.
                kDirection = kRCC.m_pkWorldBound->GetCenter() -
                    pkDynEffect->GetWorldTranslate();
                kDirection.Unitize();
            }
            else if (pkDynEffect->GetEffectType() == 
                NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                kDirection = 
                    ((NiDirectionalLight*)pkDynEffect)->GetWorldDirection();
            }
            else
            {
                NIASSERT(NiIsKindOf(NiSpotLight, pkDynEffect));
                kDirection = 
                    ((NiSpotLight*)pkDynEffect)->GetWorldDirection();
            }

            // Convert direction vector to rendered object's model space.
            kDirection = kRCC.m_pkWorld->m_Rotate.Transpose() * kDirection;

            kResult[0] = kDirection.x;
            kResult[1] = kDirection.y;
            kResult[2] = kDirection.z;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 1.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDTRANSFORM:
        if (pkDynEffect)
        {
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                NiPoint3 kTranslate = 
                    ((NiDirectionalLight*)pkDynEffect)->GetWorldDirection() * 
                    ms_fDirLightDistance;
                // If it's column major, then we must manually transpose.
                if (pkEntry->GetColumnMajor())
                {
                    NiD3D10Utility::GetD3DTransposeFromNi(kResult,
                        pkDynEffect->GetWorldRotate(), kTranslate, 
                        pkDynEffect->GetWorldScale());
                }
                else
                {
                    NiD3D10Utility::GetD3DFromNi(kResult,
                        pkDynEffect->GetWorldRotate(), kTranslate, 
                        pkDynEffect->GetWorldScale());
                }
            }
            else
            {
                // If it's column major, then we must manually transpose.
                if (pkEntry->GetColumnMajor())
                {
                    NiD3D10Utility::GetD3DTransposeFromNi(kResult,
                        pkDynEffect->GetWorldTransform());
                }
                else
                {
                    NiD3D10Utility::GetD3DFromNi(kResult,
                        pkDynEffect->GetWorldTransform());
                }
            }
            return true;
        }
        else
        {
            D3DXMatrixIdentity(&kResult);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELTRANSFORM:
        if (pkDynEffect)
        {
            NiTransform kInvWorld;
            kRCC.m_pkWorld->Invert(kInvWorld);

            if (pkDynEffect->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                NiTransform kDynEffectWorld =
                    pkDynEffect->GetWorldTransform();
                kDynEffectWorld.m_Translate = ((NiDirectionalLight*)
                    pkDynEffect)->GetWorldDirection() *
                    ms_fDirLightDistance;
                // If it's column major, then we must manually transpose.
                if (pkEntry->GetColumnMajor())
                {
                    NiD3D10Utility::GetD3DTransposeFromNi(kResult, kInvWorld *
                        kDynEffectWorld);
                }
                else
                {
                    NiD3D10Utility::GetD3DFromNi(kResult, kInvWorld *
                        kDynEffectWorld);
                }
            }
            else
            {
                // If it's column major, then we must manually transpose.
                if (pkEntry->GetColumnMajor())
                {
                    NiD3D10Utility::GetD3DTransposeFromNi(kResult, kInvWorld *
                        pkDynEffect->GetWorldTransform());
                }
                else
                {
                    NiD3D10Utility::GetD3DFromNi(kResult, kInvWorld *
                        pkDynEffect->GetWorldTransform());
                }
            }
            return true;
        }
        else
        {
            D3DXMatrixIdentity(&kResult);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_SPOTATTENUATION:
        if (pkDynEffect && 
            (pkDynEffect->GetEffectType() == NiDynamicEffect::SPOT_LIGHT ||
            pkDynEffect->GetEffectType() == NiDynamicEffect::SHADOWSPOT_LIGHT))
        {
            NIASSERT(NiIsKindOf(NiSpotLight, pkDynEffect));

            kResult[0] = 
                NiCos(((NiSpotLight*)pkDynEffect)->GetInnerSpotAngle() * 
                NI_PI / 180.0f);
            kResult[1] = 
                NiCos(((NiSpotLight*)pkDynEffect)->GetSpotAngle() * 
                NI_PI / 180.0f);
            kResult[2] = ((NiSpotLight*)pkDynEffect)->GetSpotExponent();
            kResult[3] = 0.0f;
            return true;
        }
        else
        {
            kResult[0] = -1.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_ATTENUATION:
        if (pkDynEffect && (pkDynEffect->GetEffectType() == 
            NiDynamicEffect::POINT_LIGHT ||
            pkDynEffect->GetEffectType() == 
            NiDynamicEffect::SHADOWPOINT_LIGHT ||
            pkDynEffect->GetEffectType() == 
            NiDynamicEffect::SPOT_LIGHT ||
            pkDynEffect->GetEffectType() == 
            NiDynamicEffect::SHADOWSPOT_LIGHT))
        {
            NIASSERT(NiIsKindOf(NiPointLight, pkDynEffect));

            kResult[0] = 
                ((NiPointLight*)pkDynEffect)->GetConstantAttenuation();
            kResult[1] = 
                ((NiPointLight*)pkDynEffect)->GetLinearAttenuation();
            kResult[2] = 
                ((NiPointLight*)pkDynEffect)->GetQuadraticAttenuation();
            kResult[3] = 0.0f;
            return true;
        }
        else
        {
            kResult[0] = 1.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONMATRIX:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            // If it's column major, then we must manually transpose.
            if (pkEntry->GetColumnMajor())
            {
                NiD3D10Utility::GetD3DTransposeFromNi(kResult, 
                    ((NiTextureEffect*)pkDynEffect)->
                    GetWorldProjectionMatrix(), NiPoint3::ZERO, 1.0f);
            }
            else
            {
                NiD3D10Utility::GetD3DFromNi(kResult, 
                    ((NiTextureEffect*)pkDynEffect)->
                    GetWorldProjectionMatrix(), NiPoint3::ZERO, 1.0f);
            }
            return true;
        }
        else
        {
            D3DXMatrixIdentity(&kResult);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONMATRIX:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            // If it's column major, then we must manually transpose.
            if (pkEntry->GetColumnMajor())
            {
                NiD3D10Utility::GetD3DTransposeFromNi(kResult, 
                    kRCC.m_pkWorld->m_Rotate.Transpose() *
                    ((NiTextureEffect*)pkDynEffect)->
                    GetWorldProjectionMatrix(), NiPoint3::ZERO, 1.0f);
            }
            else
            {
                NiD3D10Utility::GetD3DFromNi(kResult, 
                    kRCC.m_pkWorld->m_Rotate.Transpose() *
                    ((NiTextureEffect*)pkDynEffect)->
                    GetWorldProjectionMatrix(), NiPoint3::ZERO, 1.0f);
            }
            return true;
        }
        else
        {
            D3DXMatrixIdentity(&kResult);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSLATION:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            const NiPoint3& kTranslation = ((NiTextureEffect*)pkDynEffect)->
                GetWorldProjectionTranslation();

            kResult[0] = kTranslation.x;
            kResult[1] = kTranslation.y;
            kResult[2] = kTranslation.z;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONTRANSLATION:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTransform kInvWorld;
            kRCC.m_pkWorld->Invert(kInvWorld);

            NiPoint3 kTranslation = ((NiTextureEffect*)pkDynEffect)->
                GetWorldProjectionTranslation();
            kTranslation = kInvWorld * kTranslation;

            kResult[0] = kTranslation.x;
            kResult[1] = kTranslation.y;
            kResult[2] = kTranslation.z;
            kResult[3] = 1.0f;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDCLIPPINGPLANE:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTextureEffect* pkTexEffect = (NiTextureEffect*) pkDynEffect;
            if (pkTexEffect->GetClippingPlaneEnable())
            {
                const NiPlane& kPlane =
                    pkTexEffect->GetWorldClippingPlane();
                kResult[0] = kPlane.GetNormal().x;
                kResult[1] = kPlane.GetNormal().y;
                kResult[2] = kPlane.GetNormal().z;
                kResult[3] = kPlane.GetConstant();
            }
            else
            {
                kResult[0] = 0.0f;
                kResult[1] = 0.0f;
                kResult[2] = 0.0f;
                kResult[3] = 0.0f;
            }
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELCLIPPINGPLANE:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTextureEffect* pkTexEffect = (NiTextureEffect*) pkDynEffect;
            if (pkTexEffect->GetClippingPlaneEnable())
            {
                NiTransform kInvWorld;
                kRCC.m_pkWorld->Invert(kInvWorld);

                NiPlane kPlane = pkTexEffect->GetWorldClippingPlane();
                NiPoint3 kNormal = kInvWorld.m_Rotate *
                    kPlane.GetNormal();
                NiPoint3 kPoint = kInvWorld * (kPlane.GetNormal() *
                    kPlane.GetConstant());
                float fConstant = kNormal * kPoint;

                kResult[0] = kNormal.x;
                kResult[1] = kNormal.y;
                kResult[2] = kNormal.z;
                kResult[3] = fConstant;
            }
            else
            {
                kResult[0] = 0.0f;
                kResult[1] = 0.0f;
                kResult[2] = 0.0f;
                kResult[3] = 0.0f;
            }
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_TEXCOORDGEN:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            float fTexCoordGen = 0.0f;
            NiTextureEffect::CoordGenType eGen = 
                ((NiTextureEffect*)pkDynEffect)->GetTextureCoordGen();
            switch (eGen)
            {
            case NiTextureEffect::WORLD_PARALLEL:
                // D3DTSS_TCI_CAMERASPACEPOSITION
                fTexCoordGen = 2.0f;
                break;
            case NiTextureEffect::WORLD_PERSPECTIVE:
                // D3DTSS_TCI_CAMERASPACEPOSITION
                fTexCoordGen = 2.0f;
                break;
            case NiTextureEffect::SPHERE_MAP:
                // D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR
                fTexCoordGen = 3.0f;
                break;
            case NiTextureEffect::SPECULAR_CUBE_MAP:
                // D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR
                fTexCoordGen = 3.0f;
                break;
            case NiTextureEffect::DIFFUSE_CUBE_MAP:
                // D3DTSS_TCI_CAMERASPACENORMAL
                fTexCoordGen = 1.0f;
                break;
            default:
                break;
            }

            kResult[0] = fTexCoordGen;
            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSFORM:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {           
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTextureEffect* pkTexEffect = (NiTextureEffect*)pkDynEffect;
            // If it's column major, then we must manually transpose.
            if (pkEntry->GetColumnMajor())
            {
                NiD3D10Utility::GetD3DTransposeFromNi(kResult, 
                    pkTexEffect->GetWorldProjectionMatrix(), 
                    pkTexEffect->GetWorldProjectionTranslation(), 1.0f);
            }
            else
            {
                NiD3D10Utility::GetD3DFromNi(kResult, 
                    pkTexEffect->GetWorldProjectionMatrix(), 
                    pkTexEffect->GetWorldProjectionTranslation(), 1.0f);
            }
            return true;
        }
        else
        {
            D3DXMatrixIdentity(&kResult);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONTRANSFORM:
        if (pkDynEffect && 
            pkDynEffect->GetEffectType() == NiDynamicEffect::TEXTURE_EFFECT)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTextureEffect* pkTexEffect = (NiTextureEffect*)pkDynEffect;
            NiTransform kInvWorld;
            kRCC.m_pkWorld->Invert(kInvWorld);

            NiPoint3 kTranslation = ((NiTextureEffect*)pkDynEffect)->
                GetWorldProjectionTranslation();
            kTranslation = kInvWorld * kTranslation;
            // If it's column major, then we must manually transpose.
            if (pkEntry->GetColumnMajor())
            {
                NiD3D10Utility::GetD3DTransposeFromNi(kResult, 
                    kRCC.m_pkWorld->m_Rotate.Transpose() * 
                    pkTexEffect->GetWorldProjectionMatrix(), 
                    kTranslation, 1.0f);
            }
            else
            {
                NiD3D10Utility::GetD3DFromNi(kResult, 
                    kRCC.m_pkWorld->m_Rotate.Transpose() * 
                    pkTexEffect->GetWorldProjectionMatrix(), 
                    kTranslation, 1.0f);
            }
            return true;
        }
        else
        {
            D3DXMatrixIdentity(&kResult);
            return false;
        }

    case NiShaderConstantMap::SCM_OBJ_WORLDTOSHADOWMAPMATRIX:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiShadowGenerator* pkGenerator = 
                pkDynEffect->GetShadowGenerator();
            NIASSERT(pkGenerator);

            NiShadowMap* pkShadowMap = pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, kRCC.m_pkMesh);
            NIASSERT(pkShadowMap);

            const float* aafWorldToSM = pkShadowMap->GetWorldToShadowMap();
            // If it's column major, then we must manually transpose.
            // The data is already stored transposed, so we only transpose
            // if it's _not_ column major.
            if (pkEntry->GetColumnMajor())
            {
                kResult = *((D3DXMATRIXA16*)aafWorldToSM);
            }
            else
            {
                D3DXMatrixTranspose( &kResult, (D3DXMATRIXA16*)aafWorldToSM);
            }

            return true;
        }
        else
        {
            D3DXMatrixIdentity(&kResult);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_SHADOWMAPTEXSIZE:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiShadowGenerator* pkGenerator = 
                pkDynEffect->GetShadowGenerator();
            NIASSERT(pkGenerator);

            NiShadowMap* pkShadowMap = pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, kRCC.m_pkMesh);
            NIASSERT(pkShadowMap);

            kResult[0] = (float)pkShadowMap->GetTexture()->GetWidth();
            kResult[1] = (float)pkShadowMap->GetTexture()->GetHeight();
            kResult[2] = 0.0f;
            kResult[3] = 0.0f;

            return true;
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 0.0f;
            return false;
        }

    case NiShaderConstantMap::SCM_OBJ_SHADOWBIAS:
    case NiShaderConstantMap::SCM_OBJ_SHADOW_VSM_POWER_EPSILON:
        if (pkDynEffect)
        {
            NiShadowGenerator* pkGenerator = 
                pkDynEffect->GetShadowGenerator();
            NIASSERT(pkGenerator);

            // Only single register shader constants supported by callback.
            return pkGenerator->GetShaderConstantData(kResult, 
                sizeof(float) * 4, kRCC.m_pkMesh, 
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, eMapping, 
                kRCC.m_pkState, kRCC.m_pkEffects, *kRCC.m_pkWorld, 
                *kRCC.m_pkWorldBound, kRCC.m_uiPass);
        }
        else
        {
            kResult[0] = 0.0f;
            kResult[1] = 0.0f;
            kResult[2] = 0.0f;
            kResult[3] = 0.0f;
            return false;
        }
    default:
        break;
    }

    return false;
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::PerformOperatorMultiply(
    const void* pvOperand1, NiShaderAttributeDesc::AttributeType eType1, 
    const void* pvOperand2, NiShaderAttributeDesc::AttributeType eType2, 
    bool bInverse, bool bTranspose, D3DXMATRIXA16& kTempMatrix)
{
    if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4)
    {
        D3DXMATRIX* pkMatrix1 = (D3DXMATRIX*)pvOperand1;
        D3DXMATRIXA16 kOperand1 = *pkMatrix1;
        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4)
        {
            D3DXMATRIX* pkMatrix2 = (D3DXMATRIX*)pvOperand2;
            D3DXMATRIXA16 kOperand2 = *pkMatrix2;

            D3DXMatrixMultiply(&kTempMatrix, &kOperand1, &kOperand2);
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *((NiUInt32*)pvOperand2);
            float fOperand2 = (float)uiOperand2;

            kTempMatrix = kOperand1 * fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand2 = *((float*)pvOperand2);

            kTempMatrix = kOperand1 * fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;
            D3DXVECTOR4 kResult;
            D3DXVec4Transform(&kResult, pkOperand2, &kOperand1);
            kTempMatrix[0] = kResult.x;
            kTempMatrix[1] = kResult.y;
            kTempMatrix[2] = kResult.z;
            kTempMatrix[3] = kResult.w;
        }
        else
        {
            return NULL;
        }

        if (bInverse)
            D3DXMatrixInverse(&kTempMatrix, 0, &kTempMatrix);
        if (bTranspose)
            D3DXMatrixTranspose(&kTempMatrix, &kTempMatrix);
    }
    else if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
        eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
    {
        D3DXVECTOR4* pkOperand1 = (D3DXVECTOR4*)pvOperand1;
        D3DXVECTOR4* pkResult = (D3DXVECTOR4*)(&kTempMatrix);

        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4)
        {
            D3DXMATRIX* pkMatrix2 = (D3DXMATRIX*)pvOperand2;
            D3DXMATRIXA16 kOperand2 = *pkMatrix2;
            D3DXVec4Transform(pkResult, pkOperand1, &kOperand2);
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *((NiUInt32*)pvOperand2);
            float fOperand2 = (float)uiOperand2;

            *pkResult = *pkOperand1 * fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand2 = *((float*)pvOperand2);

            *pkResult = *pkOperand1 * fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;
            for (NiUInt32 i = 0; i < 4; i++)
                (*pkResult)[i] = (*pkOperand1)[i] * (*pkOperand2)[i];
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    return &kTempMatrix;
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::PerformOperatorDivide(
    const void* pvOperand1, NiShaderAttributeDesc::AttributeType eType1, 
    const void* pvOperand2, NiShaderAttributeDesc::AttributeType eType2, 
    bool bInverse, bool bTranspose, D3DXMATRIXA16& kTempMatrix)
{
    if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4)
    {
        D3DXMATRIX* pkMatrix1 = (D3DXMATRIX*)pvOperand1;
        D3DXMATRIXA16 kOperand1 = *pkMatrix1;
        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *(NiUInt32*)pvOperand2;
            float fOperand2 = (float)uiOperand2;

            kTempMatrix = kOperand1 / fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand2 = *(float*)pvOperand2;

            kTempMatrix = kOperand1 / fOperand2;
        }
        else
        {
            return NULL;
        }

        if (bInverse)
            D3DXMatrixInverse(&kTempMatrix, 0, &kTempMatrix);
        if (bTranspose)
            D3DXMatrixTranspose(&kTempMatrix, &kTempMatrix);
    }
    else if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
        eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
    {
        D3DXVECTOR4* pkOperand1 = (D3DXVECTOR4*)pvOperand1;
        D3DXVECTOR4* pkResult = (D3DXVECTOR4*)(&kTempMatrix);

        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *(NiUInt32*)pvOperand2;
            float fOperand2 = (float)uiOperand2;

            *pkResult = *pkOperand1 / fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand2 = *(float*)pvOperand2;

            *pkResult = *pkOperand1 / fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;
            for (NiUInt32 i = 0; i < 4; i++)
                (*pkResult)[i] = (*pkOperand1)[i] / (*pkOperand2)[i];
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    return &kTempMatrix;
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::PerformOperatorAdd(
    const void* pvOperand1, NiShaderAttributeDesc::AttributeType eType1, 
    const void* pvOperand2, NiShaderAttributeDesc::AttributeType eType2, 
    bool, bool, D3DXMATRIXA16& kTempMatrix)
{
    if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
    {
        NiUInt32 uiOperand1 = *(NiUInt32*)pvOperand1;

        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *(NiUInt32*)pvOperand2;

            *((NiUInt32*)(&(kTempMatrix._11))) = uiOperand1 + uiOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand1 = (float)uiOperand1;
            float fOperand2 = *(float*)pvOperand2;

            kTempMatrix._11 = fOperand1 + fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            float fOperand1 = (float)uiOperand1;
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;
            for (NiUInt32 i = 0; i < 4; i++)
                kTempMatrix[i] = fOperand1 + (*pkOperand2)[i];
        }
        else
        {
            return NULL;
        }
    }
    else if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
    {
        float fOperand1 = *(float*)pvOperand1;
        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *(NiUInt32*)pvOperand2;
            float fOperand2 = (float)uiOperand2;

            kTempMatrix._11 = fOperand1 + fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand2 = *(float*)pvOperand2;

            kTempMatrix._11 = fOperand1 + fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;

            for (NiUInt32 i = 0; i < 4; i++)
                kTempMatrix[i] = fOperand1 + (*pkOperand2)[i];
        }
        else
        {
            return NULL;
        }
    }
    else if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
        eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
    {
        D3DXVECTOR4* pkOperand1 = (D3DXVECTOR4*)pvOperand1;

        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *(NiUInt32*)pvOperand2;
            float fOperand2 = (float)uiOperand2;

            for (NiUInt32 i = 0; i < 4; i++)
                kTempMatrix[i] = (*pkOperand1)[i] + fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand2 = *(float*)pvOperand2;

            for (NiUInt32 i = 0; i < 4; i++)
                kTempMatrix[i] = (*pkOperand1)[i] + fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;
            D3DXVECTOR4* pkResult = (D3DXVECTOR4*)(&kTempMatrix);

            *pkResult = *pkOperand1 + *pkOperand2;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    return &kTempMatrix;
}
//---------------------------------------------------------------------------
const void* NiD3D10ShaderConstantMap::PerformOperatorSubtract(
    const void* pvOperand1, NiShaderAttributeDesc::AttributeType eType1, 
    const void* pvOperand2, NiShaderAttributeDesc::AttributeType eType2, 
    bool, bool, D3DXMATRIXA16& kTempMatrix)
{
    if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
    {
        NiUInt32 uiOperand1 = *(NiUInt32*)pvOperand1;

        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *(NiUInt32*)pvOperand2;

            *((NiUInt32*)(&(kTempMatrix._11))) = uiOperand1 - uiOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand1 = (float)uiOperand1;
            float fOperand2 = *(float*)pvOperand2;

            kTempMatrix._11 = fOperand1 - fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            float fOperand1 = (float)uiOperand1;
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;
            for (NiUInt32 i = 0; i < 4; i++)
                kTempMatrix[i] = fOperand1 - (*pkOperand2)[i];
        }
        else
        {
            return NULL;
        }
    }
    else if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
    {
        float fOperand1 = *(float*)pvOperand1;
        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *(NiUInt32*)pvOperand2;
            float fOperand2 = (float)uiOperand2;

            kTempMatrix._11 = fOperand1 - fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand2 = *(float*)pvOperand2;

            kTempMatrix._11 = fOperand1 - fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;

            for (NiUInt32 i = 0; i < 4; i++)
                kTempMatrix[i] = fOperand1 - (*pkOperand2)[i];
        }
        else
        {
            return NULL;
        }
    }
    else if (eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
        eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
    {
        D3DXVECTOR4* pkOperand1 = (D3DXVECTOR4*)pvOperand1;

        if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        {
            NiUInt32 uiOperand2 = *(NiUInt32*)pvOperand2;
            float fOperand2 = (float)uiOperand2;

            for (NiUInt32 i = 0; i < 4; i++)
                kTempMatrix[i] = (*pkOperand1)[i] - fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        {
            float fOperand2 = *(float*)pvOperand2;

            for (NiUInt32 i = 0; i < 4; i++)
                kTempMatrix[i] = (*pkOperand1)[i] - fOperand2;
        }
        else if (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
            eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        {
            D3DXVECTOR4* pkOperand2 = (D3DXVECTOR4*)pvOperand2;
            D3DXVECTOR4* pkResult = (D3DXVECTOR4*)(&kTempMatrix);

            *pkResult = *pkOperand1 - *pkOperand2;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    return &kTempMatrix;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantMap::CalculatePackingEntry(
    NiUInt32& uiCurrentRegister, NiUInt32& uiCurrentElement, 
    NiUInt32 uiRegisterCount, NiUInt32 uiElementCount,
    NiUInt32& uiEncodedStartRegister, NiUInt32& uiEncodedRegisterCount,
    bool bPackRegisters)
{
    NiUInt32 uiStartRegister = uiCurrentRegister;
    NiUInt32 uiStartElement = uiCurrentElement;
    if ((uiRegisterCount > 0 && uiCurrentElement > 0) ||
        (uiCurrentElement + uiElementCount > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS))
    {
        uiStartRegister++;
        uiStartElement = 0;
    }

    NiUInt32 uiFinalRegister = uiStartRegister + uiRegisterCount;
    NiUInt32 uiFinalElement = uiStartElement + uiElementCount;
    while (uiFinalElement > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS)
    {
        uiFinalElement -= 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
        uiFinalRegister++;
    }

    NIASSERT(uiFinalElement >= uiStartElement);
    NIASSERT(uiFinalRegister >= uiStartRegister);
    EncodePackedRegisterAndElement(uiEncodedStartRegister, 
        uiStartRegister, uiStartElement, false);

    EncodePackedRegisterAndElement(uiEncodedRegisterCount, 
        uiFinalRegister - uiStartRegister, uiFinalElement - uiStartElement,
        bPackRegisters);

    if (uiFinalElement == 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS)
    {
        uiFinalElement = 0;
        uiFinalRegister++;
    }

    uiCurrentRegister = uiFinalRegister;
    uiCurrentElement = uiFinalElement;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantMap::CalculatePackingEntryArray(
    NiUInt32 uiNumArrayElements, NiUInt32& uiCurrentRegister, 
    NiUInt32& uiCurrentElement, const NiUInt32 uiRegisterCount, 
    const NiUInt32 uiElementCount, NiUInt32& uiEncodedStartRegister, 
    NiUInt32& uiEncodedRegisterCount, bool bPackRegisters)
{
    NiUInt32 uiTempStartRegister = 0;
    NiUInt32 uiTempRegisterCount = 0;

    for (NiUInt32 i = 0; i < uiNumArrayElements; i++)
    {
        // Each array element starts its own new shader register
        if (uiCurrentElement != 0)
        {
            uiCurrentElement = 0;
            uiCurrentRegister++;
        }
        CalculatePackingEntry(uiCurrentRegister, uiCurrentElement, 
            uiRegisterCount, uiElementCount, uiTempStartRegister, 
            uiTempRegisterCount, bPackRegisters);

        if (i == 0)
        {
            // Store base registers
            uiEncodedStartRegister = uiTempStartRegister;
            uiEncodedRegisterCount = uiTempRegisterCount;
        }
        else
        {
            // Add on new registers
            NIASSERT((uiTempRegisterCount & ~SCM_REGISTER_MASK) 
                == (uiEncodedRegisterCount & ~SCM_REGISTER_MASK));
            uiEncodedRegisterCount = 
                (uiTempRegisterCount & ~SCM_REGISTER_MASK) |
                ((uiTempRegisterCount & SCM_REGISTER_MASK) + 
                (uiEncodedRegisterCount & SCM_REGISTER_MASK));
        }
    }
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderConstantMap::IsRegisterEncoded(NiUInt32 uiRegister)
{
    return ((uiRegister & SCM_REGISTER_ENCODING_MASK) == 
        SCM_REGISTER_ENCODING);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantMap::InsertPadding(NiUInt32 uiPaddingInBytes, 
    NiDataStreamElementSet& kDataStreamElements)
{
    const NiUInt32 uiComponentSize = 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8;

    // Padding had better be a multiple of 4 bytes!
    NIASSERT((uiPaddingInBytes % uiComponentSize) == 0);
    NiUInt32 uiPaddingComponentCount = uiPaddingInBytes / uiComponentSize;

    NiUInt32 uiPaddingRows = uiPaddingComponentCount / 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
    NiUInt32 uiPaddingRemainder = uiPaddingComponentCount % 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;

    // Padding will be of type UINT32.
    while (uiPaddingRows-- > 0)
        kDataStreamElements.AddElement(NiDataStreamElement::F_UINT32_4);
    if (uiPaddingRemainder)
    {
        NiDataStreamElement::Format eFormat = 
            NiDataStreamElement::GetPredefinedFormat(
            NiDataStreamElement::T_UINT32, (NiUInt8)uiPaddingRemainder, 
            false);
        NIASSERT(eFormat != NiDataStreamElement::F_UNKNOWN);
        kDataStreamElements.AddElement(eFormat);
    }
}
//---------------------------------------------------------------------------
