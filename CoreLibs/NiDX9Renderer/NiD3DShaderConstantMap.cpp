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
#include "NiD3DRendererPCH.h"

#include "NiD3DRendererHeaders.h"
#include "NiD3DShaderConstantMap.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DUtility.h"
#include <NiDebug.h>
#include <NiMain.h>
#include <NiMesh.h>
#include <NiSkinningMeshModifier.h>

// -2 ^ 20. This number is used to create the light position for directional
// lights.
const float NiD3DShaderConstantMap::ms_fDirLightDistance = -1048576.0f;

float NiD3DShaderConstantMap::ms_afObjectData[16];

//---------------------------------------------------------------------------
float NiD3DShaderConstantMap::ms_vTaylorSin[4] = 
    { 1.0f, -0.16161616f, 0.0083333f, -0.00019841f };
float NiD3DShaderConstantMap::ms_vTaylorCos[4] = 
    { -0.5f, -0.041666666f, -0.0013888889f, 0.000024801587f };

BOOL NiD3DShaderConstantMap::ms_bMappingValue = false;
int NiD3DShaderConstantMap::ms_aiMappingValue[4] = {0, 0, 0, 0};

float NiD3DShaderConstantMap::ms_vMappingValue[4];
D3DXMATRIX NiD3DShaderConstantMap::ms_mMappingValue;

//---------------------------------------------------------------------------
D3DXALIGNEDMATRIX NiD3DShaderConstantMap::ms_kMatrices[2];
float NiD3DShaderConstantMap::ms_afVector4[2][4];

D3DXALIGNEDMATRIX NiD3DShaderConstantMap::ms_kMatrixResult;
float NiD3DShaderConstantMap::ms_fVector4Result[4];

//---------------------------------------------------------------------------
NiD3DShaderConstantMap::NiD3DShaderConstantMap(
    NiGPUProgram::ProgramType eType) : 
    NiShaderConstantMap(eType),
    m_pkD3DDevice(NULL),
    m_pkD3DRenderer(NULL),
    m_pkD3DRenderState(NULL),
    m_bModified(true),
    m_eLastError(NISHADERERR_OK),
    m_pkLastShaderProgram(NULL)
{
    SetD3DRenderer(NiVerifyStaticCast(NiD3DRenderer,
        NiRenderer::GetRenderer()));
    m_aspEntries.RemoveAll();
}
//---------------------------------------------------------------------------
// Device and renderer access functions
//---------------------------------------------------------------------------
void NiD3DShaderConstantMap::SetD3DDevice(D3DDevicePtr pkD3DDevice)
{
    // Safety check the constant map entry value
    // This is done here so it is only performed once per run of the app.
    NIASSERT(NiShaderAttributeDesc::ATTRIB_TYPE_COUNT < 
        NiShaderConstantMapEntry::GetAttributeMask());

    if (m_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
    m_pkD3DDevice = pkD3DDevice;
    if (m_pkD3DDevice)
        D3D_POINTER_REFERENCE(m_pkD3DDevice);
}
//---------------------------------------------------------------------------
void NiD3DShaderConstantMap::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    m_pkD3DRenderer = pkD3DRenderer;
    if (m_pkD3DRenderer)
    {
        SetD3DDevice(m_pkD3DRenderer->GetD3DDevice());
        SetD3DRenderState(m_pkD3DRenderer->GetRenderState());
    }
    else
    {
        SetD3DDevice(0);
        SetD3DRenderState(0);
    }
}
//---------------------------------------------------------------------------
void NiD3DShaderConstantMap::SetD3DRenderState(NiD3DRenderState* pkRS)
{
    m_pkD3DRenderState = pkRS;
}
//---------------------------------------------------------------------------
NiD3DShaderConstantMap::~NiD3DShaderConstantMap()
{
    NIASSERT(m_pkD3DRenderer);

    // We need to release any entries which are global
    for (unsigned int ui = 0; ui < m_aspEntries.GetAllocatedSize(); ui++)
    {
        NiShaderConstantMapEntryPtr spEntry = m_aspEntries.GetAt(ui);
        if (spEntry && spEntry->IsGlobal())
        {
            NiFixedString kKey= spEntry->GetKey();
            NiD3DShaderFactory::ReleaseGlobalShaderConstant(kKey);
        }
    }

    m_aspEntries.RemoveAll();
    SetD3DRenderer(0);
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::AddEntry(const char* pszKey, 
    unsigned int uiFlags, unsigned int uiExtra, 
    unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
    const char* pszVariableName, unsigned int uiDataSize, 
    unsigned int uiDataStride, const void* pvDataSource, bool bCopyData)
{
    m_eLastError = NISHADERERR_OK;

    // See if the entry is in the list...
    NiShaderConstantMapEntry* pkEntry = GetEntry(pszKey);
    if (pkEntry && pkEntry->GetExtra() == uiExtra &&
        pkEntry->GetFlags() == uiFlags)
    {
        // Was already in the list. Return an error
        m_eLastError = NISHADERERR_DUPLICATEENTRYKEY;
        return m_eLastError;
    }

    // Check what the entry is
    if (NiShaderConstantMapEntry::IsAttribute(uiFlags))
    {
        m_eLastError = AddAttributeEntry(pszKey, uiFlags, uiExtra, 
            uiShaderRegister, uiRegisterCount, pszVariableName, uiDataSize, 
            uiDataStride, pvDataSource, bCopyData);
    }
    else if (NiShaderConstantMapEntry::IsConstant(uiFlags))
    {
        m_eLastError = AddConstantEntry(pszKey, uiFlags, uiExtra, 
            uiShaderRegister, uiRegisterCount, pszVariableName, uiDataSize, 
            uiDataStride, pvDataSource, bCopyData);
    }
    else if (NiShaderConstantMapEntry::IsDefined(uiFlags))
    {
        m_eLastError = AddPredefinedEntry(pszKey, uiExtra, 
            uiShaderRegister, pszVariableName);
    }
    else if (NiShaderConstantMapEntry::IsGlobal(uiFlags))
    {
        m_eLastError = AddGlobalEntry(pszKey, uiFlags, uiExtra, 
            uiShaderRegister, uiRegisterCount, pszVariableName, uiDataSize, 
            uiDataStride, pvDataSource, bCopyData);
    }
    else if (NiShaderConstantMapEntry::IsOperator(uiFlags))
    {
        m_eLastError = AddOperatorEntry(pszKey, uiFlags, uiExtra, 
            uiShaderRegister, uiRegisterCount, pszVariableName);
    }
    else if (NiShaderConstantMapEntry::IsObject(uiFlags))
    {
        m_eLastError = AddObjectEntry(pszKey, uiShaderRegister,
            pszVariableName, uiExtra,
            NiShaderConstantMapEntry::GetObjectType(uiFlags));
    }
    else
    {
        NIASSERT(!"ConstantMap AddEntry - Invalid mapping type!");
        m_eLastError = NISHADERERR_INVALIDMAPPING;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::AddPredefinedEntry(const char* pszKey, 
    unsigned int uiExtra, unsigned int uiShaderRegister, 
    const char* pszVariableName)
{
    // 
    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    pkEntry->SetKey(pszKey);
    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetVariableName(pszVariableName);

    // Set the flags to just the DEFINED type. The setup predefined call
    // will fill in the position masks
    pkEntry->SetFlags(NiShaderConstantMapEntry::SCME_MAP_DEFINED);

    // Look-up and set the data.
    m_eLastError = SetupPredefinedEntry(pkEntry);

    // Insert it!
    if (m_eLastError == NISHADERERR_OK)
    {
        m_eLastError = InsertEntry(pkEntry);
    }
    else
    {
        NiDelete pkEntry;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::AddAttributeEntry(const char* pszKey, 
    unsigned int uiFlags, unsigned int uiExtra, 
    unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
    const char* pszVariableName, unsigned int uiDataSize, 
    unsigned int uiDataStride, const void* pvDataSource, bool bCopyData)
{
    // 
    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    // Attributes will have to be retrieved from the geoemtry each time they
    // are set...
    pkEntry->SetKey(pszKey);

    // Make sure the flags are set correctly
    // Clear the old map
    uiFlags &= ~NiShaderConstantMapEntry::SCME_MAP_MASK;
    // Set the confirmed one
    uiFlags |= NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;

    pkEntry->SetFlags(uiFlags);

    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetRegisterCount(uiRegisterCount);
    pkEntry->SetVariableName(pszVariableName);

    pkEntry->SetData(uiDataSize, uiDataStride, (void*)pvDataSource, 
        bCopyData);

    // Insert it!
    m_eLastError = InsertEntry(pkEntry);
    if (m_eLastError != NISHADERERR_OK)
    {
        NiDelete pkEntry;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::AddConstantEntry(const char* pszKey, 
    unsigned int uiFlags, unsigned int uiExtra, 
    unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
    const char* pszVariableName, unsigned int uiDataSize, 
    unsigned int uiDataStride, const void* pvDataSource, bool bCopyData)
{
    // 
    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    // 
    pkEntry->SetKey(pszKey);

    // Make sure the flags are set correctly
    // Clear the old map
    uiFlags &= ~NiShaderConstantMapEntry::SCME_MAP_MASK;
    // Set the confirmed one
    uiFlags |= NiShaderConstantMapEntry::SCME_MAP_CONSTANT;

    if (uiDataSize == 2 * sizeof(float))
        uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
    else if (uiDataSize == 3 * sizeof(float))
        uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
    else if (uiDataSize == 4 * sizeof(float))
        uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
    else if (uiDataSize == 8 * sizeof(float))
        uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
    else if (uiDataSize == 9 * sizeof(float))
        uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
    else if (uiDataSize == 12 * sizeof(float))
        uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
    else if (uiDataSize == 16 * sizeof(float))
        uiFlags |= NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;

    pkEntry->SetFlags(uiFlags);

    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetRegisterCount(uiRegisterCount);
    pkEntry->SetVariableName(pszVariableName);

    pkEntry->SetData(uiDataSize, uiDataStride, (void*)pvDataSource, 
        bCopyData);

    // Insert it!
    m_eLastError = InsertEntry(pkEntry);
    if (m_eLastError != NISHADERERR_OK)
    {
        NiDelete pkEntry;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::AddGlobalEntry(const char* pszKey, 
    unsigned int uiFlags, unsigned int uiExtra, 
    unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
    const char* pszVariableName, unsigned int uiDataSize, 
    unsigned int uiDataStride, const void* pvDataSource, bool bCopyData)
{
    // 
    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    // 
    pkEntry->SetKey(pszKey);

    // Make sure the flags are set correctly
    // Clear the old map
    uiFlags &= ~NiShaderConstantMapEntry::SCME_MAP_MASK;
    // Set the confirmed one
    uiFlags |= NiShaderConstantMapEntry::SCME_MAP_GLOBAL;
    pkEntry->SetFlags(uiFlags);

    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetRegisterCount(uiRegisterCount);
    pkEntry->SetVariableName(pszVariableName);

    pkEntry->SetData(uiDataSize, uiDataStride, (void*)pvDataSource, 
        bCopyData);

    // Insert it!
    m_eLastError = InsertEntry(pkEntry);
    if (m_eLastError != NISHADERERR_OK)
    {
        NiDelete pkEntry;
    }
    else
    {
        NiShaderAttributeDesc::AttributeType eType = 
            NiShaderConstantMapEntry::GetAttributeType(
            uiFlags);
        // Register the shader constant map entry
        if (!NiD3DShaderFactory::RegisterGlobalShaderConstant(
            pszKey, eType, uiDataSize, pvDataSource))
        {
            NIASSERT(!"Failed to add global constant!");
        }
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::AddOperatorEntry(const char* pszKey, 
    unsigned int uiFlags, unsigned int uiExtra, 
    unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
    const char* pszVariableName)
{
    // 
    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    // 
    pkEntry->SetKey(pszKey);

    // Make sure the flags are set correctly
    // Clear the old map
    uiFlags &= ~NiShaderConstantMapEntry::SCME_MAP_MASK;
    // Set the confirmed one
    uiFlags |= NiShaderConstantMapEntry::SCME_MAP_OPERATOR;
    pkEntry->SetFlags(uiFlags);

    pkEntry->SetExtra(uiExtra);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetRegisterCount(uiRegisterCount);
    pkEntry->SetVariableName(pszVariableName);

    // Insert it!
    m_eLastError = InsertEntry(pkEntry);
    if (m_eLastError != NISHADERERR_OK)
    {
        NiDelete pkEntry;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::AddObjectEntry(const char* pszKey,
    unsigned int uiShaderRegister, const char* pszVariableName,
    unsigned int uiObjectIndex, NiShaderAttributeDesc::ObjectType eObjectType)
{
    NiShaderConstantMapEntry* pkEntry = NiNew NiShaderConstantMapEntry();
    NIASSERT(pkEntry);

    pkEntry->SetKey(pszKey);
    pkEntry->SetShaderRegister(uiShaderRegister);
    pkEntry->SetVariableName(pszVariableName);
    pkEntry->SetExtra(uiObjectIndex);

    // Set the flags to the object type.
    pkEntry->SetFlags(NiShaderConstantMapEntry::SCME_MAP_OBJECT |
        NiShaderConstantMapEntry::GetObjectFlags(eObjectType));

    // Look-up and set the data.
    m_eLastError = SetupObjectEntry(pkEntry);

    // Insert it!
    if (m_eLastError == NISHADERERR_OK)
    {
        m_eLastError = InsertEntry(pkEntry);
    }
    else
    {
        NiDelete pkEntry;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::RemoveEntry(const char* pszKey)
{
    unsigned int uiIndex = GetEntryIndex(pszKey);

    if (uiIndex != 0xffffffff)
    {
        NiShaderConstantMapEntry* pkEntry = m_aspEntries.GetAt(uiIndex);
        if (pkEntry && pkEntry->IsGlobal())
        {
            NiFixedString kKey= pkEntry->GetKey();
            NiD3DShaderFactory::ReleaseGlobalShaderConstant(kKey);
        }

        m_aspEntries.SetAt(uiIndex, 0);
    }
    else
    {
        m_eLastError = NISHADERERR_ENTRYNOTFOUND;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiShaderConstantMapEntry* NiD3DShaderConstantMap::GetEntry(
    const char* pszKey)
{
    NiShaderConstantMapEntry* pkEntry = 0;
    bool bFound = false;

    unsigned int uiSize = m_aspEntries.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        pkEntry = m_aspEntries.GetAt(ui);
        if (pkEntry)
        {
            if (NiStricmp(pszKey, pkEntry->GetKey()) == 0)
            {
                bFound = true;
                break;
            }
        }
    }

    if (!bFound)
        return 0;

    return pkEntry;
}
//---------------------------------------------------------------------------
NiShaderConstantMapEntry* 
    NiD3DShaderConstantMap::GetEntryAtIndex(unsigned int uiIndex)
{
    return m_aspEntries.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetShaderConstants(
    const NiRenderCallContext& kRCC,
    NiD3DShaderProgram* pkShaderProgram, 
    bool bGlobal)
{
    m_pkLastShaderProgram = pkShaderProgram;

    // Functions called from here will assume that pkMesh is not NULL.
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);
    NIASSERT(pkMesh != NULL);

    // The implementation just cycles through the list, updating the 
    // constants.
    NiShaderConstantMapEntry* pkEntry;

    NiShaderError eReturnResult = NISHADERERR_OK;

    // Grab the NiSCMExtraData object which basically holds cached
    // values for extra data pointers so we don't have to call strcmp
    // too much.
    NiSCMExtraData* pkShaderData = 
        (NiSCMExtraData*)
        pkMesh->GetExtraData(NiD3DShader::ms_kNDLShaderMapName);

    const unsigned int uiNumEntries = m_aspEntries.GetSize();
    unsigned int uiExtra = 0;
    NiShaderAttributeDesc::ObjectType eObjectType =
        NiShaderAttributeDesc::OT_UNDEFINED;
    NiDynamicEffect* pkDynEffect = NULL;
    for (unsigned int ui = 0; ui < uiNumEntries; )
    {
        pkEntry = m_aspEntries.GetAt(ui++);
        if (pkEntry)
        {

            // Allow for skipping of entries if the have a -1 in the 
            // shader register. These constants can be used with 
            // the Operator attribute, see:
            // Programmer's Guide/NiShader/NSF NSB Shaders/NSF Reference/
            //   Shader Constant Maps
            // "NOTE: If the Register entry for any of the mapping types is -1,
            // the data will NOT get set in the shader constant registers. 
            // This technique is a handy way to have data available to your 
            // NiD3DShader-derived class without setting it on the hardware." 
            if ((int)(pkEntry->GetShaderRegister()) == -1 &&
                pkEntry->GetVariableName() == 0)
            {
                continue;
            }
            // If the shader register is > max constants, skip that too
    //        if (pkEntry->GetShaderRegister() >= ...)
    //            continue;

            // Skip entries that don't need to be updated on each submesh
            if ((kRCC.m_uiSubmesh > 0) && (pkEntry->GetInternal() !=
                NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_3))
            {
                continue;
            }

            NiShaderError eResult = NISHADERERR_OK;

            switch (pkEntry->GetFlags() &
                NiShaderConstantMapEntry::SCME_MAP_MASK)
            {
            case NiShaderConstantMapEntry::SCME_MAP_DEFINED:
                {
                    eResult = SetDefinedConstant(
                        kRCC, pkShaderProgram, pkEntry);
                    break;
                }

            case NiShaderConstantMapEntry::SCME_MAP_CONSTANT:
                {
                    eResult = SetConstantConstant(
                        kRCC, pkShaderProgram, pkEntry);
                    break;
                }

            case NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE:
                {
                    eResult = SetAttributeConstant(
                        kRCC, pkShaderProgram, pkEntry, bGlobal, pkShaderData);
                    break;
                }

            case NiShaderConstantMapEntry::SCME_MAP_GLOBAL:
                {
                    eResult = SetGlobalConstant(
                        kRCC, pkShaderProgram, pkEntry);
                    break;
                }

            case NiShaderConstantMapEntry::SCME_MAP_OPERATOR:
                {
                    eResult = SetOperatorConstant(
                        kRCC, pkShaderProgram, pkEntry);
                    break;
                }

            case NiShaderConstantMapEntry::SCME_MAP_OBJECT:
                {
                    if (pkDynEffect == NULL || 
                        uiExtra != pkEntry->GetExtra() || 
                        eObjectType != pkEntry->GetObjectType())
                    {
                        uiExtra = pkEntry->GetExtra();
                        eObjectType = pkEntry->GetObjectType();
                        pkDynEffect = GetDynamicEffectForObject(
                            kRCC.m_pkEffects, eObjectType, uiExtra);
                    }

                    eResult = SetObjectConstant(
                        kRCC, pkShaderProgram, pkEntry, pkDynEffect);
                    break;
                }

            default:
                {
                    eResult = NISHADERERR_INVALIDMAPPING;
                }
            }

            if (eResult == NISHADERERR_OK)
            {
                continue;
            }
            else
            {
                switch (eResult)
                {
                case NISHADERERR_INVALIDMAPPING:
                    NiD3DRenderer::Error(
                        "NiD3DShaderConstantMap::SetShaderConstants - "
                        "Constant %s has invalid mapping 0x%8x\n",
                        (const char*)pkEntry->GetKey(), (pkEntry->GetFlags() &
                        NiShaderConstantMapEntry::SCME_MAP_MASK));
                    break;
                case NISHADERERR_SETCONSTANTFAILED:
                    NiD3DRenderer::Error(
                        "NiD3DShaderConstantMap::SetShaderConstants - "
                        "Constant %s failed to be set\n",
                        (const char*)pkEntry->GetKey());
                    break;
                case NISHADERERR_ENTRYNOTFOUND:
                    NiD3DRenderer::Error(
                        "NiD3DShaderConstantMap::SetShaderConstants - "
                        "Constant %s has unknown entry 0x%8x\n",
                        (const char*)pkEntry->GetKey(),
                        pkEntry->GetInternal());
                    break;
                case NISHADERERR_DYNEFFECTNOTFOUND:
                    if (NiShaderErrorSettings::GetAllowDynEffectNotFound())
                    {
                        NiD3DRenderer::Warning(
                            "NiD3DShaderConstantMap::SetShaderConstants - "
                            "Constant %s references a nonexistent "
                            "NiDynamicEffect object. Default values used.\n",
                            (const char*)pkEntry->GetKey());
                    }
                    break;
                default:
                    NiD3DRenderer::Error(
                        "NiD3DShaderConstantMap::SetShaderConstants - "
                        "Constant %s failed to be set with unknown error\n",
                        (const char*)pkEntry->GetKey());
                    break;
                }

                eReturnResult = NISHADERERR_UNKNOWN;
            }
        }
    }

    return eReturnResult;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetShaderConstantArray(
    NiD3DShaderProgram* pkShaderProgram, NiShaderConstantMapEntry* pkEntry,
    const float * pfData)
{
    // Compute the array length
    const unsigned int uiSize = pkEntry->GetDataSize();
    const unsigned int uiStride = pkEntry->GetDataStride();
    const unsigned int uiArrayLength = uiSize / uiStride;

    // Compute the actual stride parameters for uploading
    unsigned int uiRegistersPerElement;
    unsigned int uiItemsPerRegister;
    ComputeRestridingValues(pkEntry, uiRegistersPerElement, 
        uiItemsPerRegister);

    // set registers
    if (pfData == NULL)
    {
        return NISHADERERR_UNKNOWN;
    }
    else
    {
        if (pkShaderProgram->SetShaderConstantArray(pkEntry, pfData,
            uiArrayLength, uiRegistersPerElement, NULL))
            return NISHADERERR_OK;
        else
            return NISHADERERR_SETCONSTANTFAILED;
    }
}
//---------------------------------------------------------------------------
// uiRegistersPerElement is the number of registers needed for one array
// element (i.e., one stride worth of data), while uiItemsPerRegister is the
// number of 4 byte items (e.g., floats) that will need to be mapped from the
// input to a single register, only supports strides from 1..4,8,9,12,16 floats
bool NiD3DShaderConstantMap::ComputeRestridingValues(
    NiShaderConstantMapEntry* pkEntry, unsigned int & uiRegistersPerElement, 
    unsigned int & uiItemsPerRegister)
{
    unsigned int uiStride = pkEntry->GetDataStride();

    switch (uiStride) {
        case 1*sizeof(float):
            uiItemsPerRegister = 1;
            uiRegistersPerElement = 1;
            break;
        case 2*sizeof(float):
            uiItemsPerRegister = 2;
            uiRegistersPerElement = 1;
            break;
        case 3*sizeof(float):
            uiItemsPerRegister = 3;
            uiRegistersPerElement = 1;
            break;
        case 4*sizeof(float):
            uiItemsPerRegister = 4;
            uiRegistersPerElement = 1;
            break;
        case 8*sizeof(float):
            uiItemsPerRegister = 4;
            uiRegistersPerElement = 2;
            break;
        case 9*sizeof(float):
            uiItemsPerRegister = 3;
            uiRegistersPerElement = 3;
            break;
        case 12*sizeof(float):
            uiItemsPerRegister = 4;
            uiRegistersPerElement = 3;
            break;
        case 16*sizeof(float):
            uiItemsPerRegister = 4;
            uiRegistersPerElement = 4;
            break;
        default:
            uiItemsPerRegister = 1;
            uiRegistersPerElement = 1;
            NIASSERT(!"Unsupported stride for shader array");
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
const void* NiD3DShaderConstantMap::MapAttributeValue(
    NiShaderConstantMapEntry* pkEntry,
    NiExtraData* pkExtra)
{
    NIASSERT(pkEntry);
    NIASSERT(pkExtra);

    if (pkEntry->IsBool())
    {
        NiBooleanExtraData* pkBoolED = (NiBooleanExtraData*)pkExtra;

        ms_bMappingValue = pkBoolED->GetValue();
        return &ms_bMappingValue;
    }
    else if (pkEntry->IsString())
    {
        // How do we actually set this???
        // This should never happen!
        // A string attribute should NOT be mapped
        return 0;
    }
    else if (pkEntry->IsUnsignedInt())
    {
        NiIntegerExtraData* pkIntED = (NiIntegerExtraData*)pkExtra;
        
        ms_aiMappingValue[0] = pkIntED->GetValue();
        ms_aiMappingValue[1] = ms_aiMappingValue[0];
        ms_aiMappingValue[2] = ms_aiMappingValue[0];
        ms_aiMappingValue[3] = ms_aiMappingValue[0];

        return ms_aiMappingValue;
    }
    else if (pkEntry->IsFloat())
    {
        NiFloatExtraData* pkFloatED = (NiFloatExtraData*)pkExtra;
        
        ms_vMappingValue[0] = pkFloatED->GetValue();
        ms_vMappingValue[1] = ms_vMappingValue[0];
        ms_vMappingValue[2] = ms_vMappingValue[0];
        ms_vMappingValue[3] = ms_vMappingValue[0];

        return ms_vMappingValue;
    }
    else if (pkEntry->IsPoint2())
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        unsigned int uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);

        // For now, we will set XYZW to XYXY
        ms_vMappingValue[0] = pfValue[0];
        ms_vMappingValue[1] = pfValue[1];
        ms_vMappingValue[2] = pfValue[0];
        ms_vMappingValue[3] = pfValue[1];

        return ms_vMappingValue;
    }
    else if (pkEntry->IsPoint3())
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        unsigned int uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);

        ms_vMappingValue[0] = pfValue[0];
        ms_vMappingValue[1] = pfValue[1];
        ms_vMappingValue[2] = pfValue[2];
        // We assume that W = 1.0 not supplied
        ms_vMappingValue[3] = 1.0f;

        return ms_vMappingValue;
    }
    else if (pkEntry->IsPoint4())
    {
        if (NiIsExactKindOf(NiFloatsExtraData, pkExtra))
        {
            NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

            unsigned int uiSize;
            float* pfValue;

            pkFloatsED->GetArray(uiSize, pfValue);

            ms_vMappingValue[0] = pfValue[0];
            ms_vMappingValue[1] = pfValue[1];
            ms_vMappingValue[2] = pfValue[2];
            ms_vMappingValue[3] = pfValue[3];
        }
        else if (NiIsExactKindOf(NiColorExtraData, pkExtra))
        {
            NiColorExtraData* pkColorED = (NiColorExtraData*)pkExtra;
            ms_vMappingValue[0] = pkColorED->GetRed();
            ms_vMappingValue[1] = pkColorED->GetGreen();
            ms_vMappingValue[2] = pkColorED->GetBlue();
            // We are assuming the alpha will be set, 
            // even when it's an 'NiColor'
            ms_vMappingValue[3] = pkColorED->GetAlpha();
        }
        else
        {
            NIASSERT(!"Unknown extra data type");
        }
        return ms_vMappingValue;
    }
    else if (pkEntry->IsMatrix3())
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        unsigned int uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);

        if (uiSize < 9)
        {
            // This is a problem... There should be a 3x3 matrix here!
            NIASSERT(!"Matrix Mapping Entry does not have 9 entries!");
        }

        NIASSERT(pkEntry->GetRegisterCount() == 3);

        ms_mMappingValue._11 = pfValue[0];
        ms_mMappingValue._12 = pfValue[1];
        ms_mMappingValue._13 = pfValue[2];
        ms_mMappingValue._14 = 0.0f;
        ms_mMappingValue._21 = pfValue[3];
        ms_mMappingValue._22 = pfValue[4];
        ms_mMappingValue._23 = pfValue[5];
        ms_mMappingValue._24 = 0.0f;
        ms_mMappingValue._31 = pfValue[6];
        ms_mMappingValue._32 = pfValue[7];
        ms_mMappingValue._33 = pfValue[8];
        ms_mMappingValue._34 = 0.0f;

        return ms_mMappingValue;
    }
    else if (pkEntry->IsMatrix4())
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;

        unsigned int uiSize;
        float* pfValue;

        pkFloatsED->GetArray(uiSize, pfValue);

        if (uiSize < 16)
        {
            // This is a problem... There should be a 4x4 matrix here!
            NIASSERT(!"Transform Mapping Entry does not have 16 entries!");
        }

        NIASSERT(pkEntry->GetRegisterCount() == 4);

        return pfValue;
    }
    else if (pkEntry->IsColor())
    {
        NiColorExtraData* pkColorED = (NiColorExtraData*)pkExtra;
        
        ms_vMappingValue[0] = pkColorED->GetRed();
        ms_vMappingValue[1] = pkColorED->GetGreen();
        ms_vMappingValue[2] = pkColorED->GetBlue();
        // We are assuming the alpha will be set, even when it's an 'NiColor'
        ms_vMappingValue[3] = pkColorED->GetAlpha();

        return ms_vMappingValue;
    }
    else if (pkEntry->IsTexture())
    {
        return 0;
    }

    return 0;
}
//---------------------------------------------------------------------------
const void* NiD3DShaderConstantMap::MapAttributeArrayValue(
    NiShaderConstantMapEntry* pkEntry, NiExtraData* pkExtra)
{
    NIASSERT(pkEntry != NULL);
    NIASSERT(pkEntry->IsArray());

    if (pkExtra == NULL)
    {
        // Attribute not found - use default value
        return MapConstantArray(pkEntry, 
            (const float*)pkEntry->GetDataSource());
    }
    else
    {
#if defined(NIDEBUG)
        unsigned int uiSize = pkEntry->GetDataSize();
#endif

        // Get a pointer to the extra data and verify we have enough data
        // (more data is fine, it's just ignored, but less data could crash)
        if (NiIsExactKindOf(NiFloatsExtraData, pkExtra))
        {
            float* pfValue;
            unsigned int uiExtraDataSize;

            NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;
            pkFloatsED->GetArray(uiExtraDataSize, pfValue);

            NIASSERT(uiExtraDataSize*sizeof(float) >= uiSize); 
            return MapConstantArray(pkEntry, (const float*)pfValue);
        }
        else if (NiIsExactKindOf(NiIntegersExtraData, pkExtra))
        {
            int* piValue;
            unsigned int uiExtraDataSize;

            NiIntegersExtraData* pkIntsED = (NiIntegersExtraData*)pkExtra;
            pkIntsED->GetArray(uiExtraDataSize, piValue);

            NIASSERT(uiExtraDataSize*sizeof(int) >= uiSize); 
            NIASSERT(sizeof(float) == sizeof(int));
            return MapConstantArray(pkEntry, (const float*)piValue);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
const void* NiD3DShaderConstantMap::MapConstantValue(
    NiShaderConstantMapEntry* pkEntry)
{
    NIASSERT(pkEntry);
    unsigned int uiCount = pkEntry->GetDataSize() / pkEntry->GetDataStride();
    switch (uiCount)
    {
    case 1:
        {
            if (pkEntry->IsBool())
            {
                ms_bMappingValue = *(bool*)(pkEntry->GetDataSource());
                return &ms_bMappingValue;
            }
            else if (pkEntry->IsUnsignedInt())
            {
                ms_aiMappingValue[0] = *(int*)(pkEntry->GetDataSource());
                ms_aiMappingValue[1] = ms_aiMappingValue[0];
                ms_aiMappingValue[2] = ms_aiMappingValue[0];
                ms_aiMappingValue[3] = ms_aiMappingValue[0];
                return ms_aiMappingValue;
            }

            float* pfValue = (float*)(pkEntry->GetDataSource());
        
            ms_vMappingValue[0] = pfValue[0];
            ms_vMappingValue[1] = pfValue[0];
            ms_vMappingValue[2] = pfValue[0];
            ms_vMappingValue[3] = pfValue[0];

            return ms_vMappingValue;
        }
    case 2:
        {
            if (pkEntry->IsUnsignedInt())
            {
                int* piValue = (int*)(pkEntry->GetDataSource());
                ms_aiMappingValue[0] = piValue[0];
                ms_aiMappingValue[1] = piValue[1];
                ms_aiMappingValue[2] = ms_aiMappingValue[0];
                ms_aiMappingValue[3] = ms_aiMappingValue[1];
                return ms_aiMappingValue;
            }

            float* pfValue = (float*)(pkEntry->GetDataSource());
        
            ms_vMappingValue[0] = pfValue[0];
            ms_vMappingValue[1] = pfValue[1];
            ms_vMappingValue[2] = pfValue[0];
            ms_vMappingValue[3] = pfValue[1];

            return ms_vMappingValue;
        }
    case 3:
        {
            if (pkEntry->IsUnsignedInt())
            {
                int* piValue = (int*)(pkEntry->GetDataSource());
                ms_aiMappingValue[0] = piValue[0];
                ms_aiMappingValue[1] = piValue[1];
                ms_aiMappingValue[2] = piValue[2];
                ms_aiMappingValue[3] = 1;
                return ms_aiMappingValue;
            }

            float* pfValue = (float*)(pkEntry->GetDataSource());
        
            ms_vMappingValue[0] = pfValue[0];
            ms_vMappingValue[1] = pfValue[1];
            ms_vMappingValue[2] = pfValue[2];
            // We assume that W = 1.0 when not present
            ms_vMappingValue[3] = 1.0f;

            return ms_vMappingValue;
        }
    case 4:
        {
#ifndef _XENON
            if (pkEntry->IsUnsignedInt())
            {
                int* piValue = (int*)(pkEntry->GetDataSource());
                ms_aiMappingValue[0] = piValue[0];
                ms_aiMappingValue[1] = piValue[1];
                ms_aiMappingValue[2] = piValue[2];
                ms_aiMappingValue[3] = piValue[3];
                return ms_aiMappingValue;
            }

            float* pfValue = (float*)(pkEntry->GetDataSource());
        
            ms_vMappingValue[0] = pfValue[0];
            ms_vMappingValue[1] = pfValue[1];
            ms_vMappingValue[2] = pfValue[2];
            ms_vMappingValue[3] = pfValue[3];

            return ms_vMappingValue;
#else
            return pkEntry->GetDataSource();
#endif
        }
    case 8:
        {
            float* pfValue = (float*)(pkEntry->GetDataSource());

            ms_mMappingValue._11 = pfValue[0];
            ms_mMappingValue._12 = pfValue[1];
            ms_mMappingValue._13 = pfValue[2];
            ms_mMappingValue._14 = pfValue[3];
            ms_mMappingValue._21 = pfValue[4];
            ms_mMappingValue._22 = pfValue[5];
            ms_mMappingValue._23 = pfValue[6];
            ms_mMappingValue._24 = pfValue[7];
            ms_mMappingValue._31 = 0.0f;
            ms_mMappingValue._32 = 0.0f;
            ms_mMappingValue._33 = 0.0f;
            ms_mMappingValue._34 = 0.0f;
            ms_mMappingValue._41 = 0.0f;
            ms_mMappingValue._42 = 0.0f;
            ms_mMappingValue._43 = 0.0f;
            ms_mMappingValue._44 = 0.0f;

            return ms_mMappingValue;
        }
    case 9:
        {
            float* pfValue = (float*)(pkEntry->GetDataSource());

            ms_mMappingValue._11 = pfValue[0];
            ms_mMappingValue._12 = pfValue[1];
            ms_mMappingValue._13 = pfValue[2];
            ms_mMappingValue._14 = 0.0f;
            ms_mMappingValue._21 = pfValue[3];
            ms_mMappingValue._22 = pfValue[4];
            ms_mMappingValue._23 = pfValue[5];
            ms_mMappingValue._24 = 0.0f;
            ms_mMappingValue._31 = pfValue[6];
            ms_mMappingValue._32 = pfValue[7];
            ms_mMappingValue._33 = pfValue[8];
            ms_mMappingValue._34 = 0.0f;
            ms_mMappingValue._41 = 0.0f;
            ms_mMappingValue._42 = 0.0f;
            ms_mMappingValue._43 = 0.0f;
            ms_mMappingValue._44 = 0.0f;

            return ms_mMappingValue;
        }
    case 12:
        {
            float* pfValue = (float*)(pkEntry->GetDataSource());

            ms_mMappingValue._11 = pfValue[ 0];
            ms_mMappingValue._12 = pfValue[ 1];
            ms_mMappingValue._13 = pfValue[ 2];
            ms_mMappingValue._14 = pfValue[ 3];
            ms_mMappingValue._21 = pfValue[ 4];
            ms_mMappingValue._22 = pfValue[ 5];
            ms_mMappingValue._23 = pfValue[ 6];
            ms_mMappingValue._24 = pfValue[ 7];
            ms_mMappingValue._31 = pfValue[ 8];
            ms_mMappingValue._32 = pfValue[ 9];
            ms_mMappingValue._33 = pfValue[10];
            ms_mMappingValue._34 = pfValue[11];
            ms_mMappingValue._41 = 0.0f;
            ms_mMappingValue._42 = 0.0f;
            ms_mMappingValue._43 = 0.0f;
            ms_mMappingValue._44 = 0.0f;

            return ms_mMappingValue;
        }
    case 16:
        {
#ifndef _XENON
            float* pfValue = (float*)(pkEntry->GetDataSource());

            ms_mMappingValue._11 = pfValue[ 0];
            ms_mMappingValue._12 = pfValue[ 1];
            ms_mMappingValue._13 = pfValue[ 2];
            ms_mMappingValue._14 = pfValue[ 3];
            ms_mMappingValue._21 = pfValue[ 4];
            ms_mMappingValue._22 = pfValue[ 5];
            ms_mMappingValue._23 = pfValue[ 6];
            ms_mMappingValue._24 = pfValue[ 7];
            ms_mMappingValue._31 = pfValue[ 8];
            ms_mMappingValue._32 = pfValue[ 9];
            ms_mMappingValue._33 = pfValue[10];
            ms_mMappingValue._34 = pfValue[11];
            ms_mMappingValue._41 = pfValue[12];
            ms_mMappingValue._42 = pfValue[13];
            ms_mMappingValue._43 = pfValue[14];
            ms_mMappingValue._44 = pfValue[15];

            return ms_mMappingValue;
#else
            return pkEntry->GetDataSource();
#endif
        }
    }

#ifdef _XENON
    if (pkEntry->GetDataStride() == 4 // source data must be tightly packed
        && ((uiCount & 3) == 0)) // data must be a multiple of 4 floats
    {
        return pkEntry->GetDataSource();
    }
#endif

    return 0;
}
//---------------------------------------------------------------------------
const void* NiD3DShaderConstantMap::MapConstantArray(
    NiShaderConstantMapEntry* pkEntry, const float * pfSource)
{
    NIASSERT(pkEntry->IsArray());

    // Figure out how the data needs to repacked (or if it needs to be)
    unsigned int uiRegistersPerElement;
    unsigned int uiItemsPerRegister;
    ComputeRestridingValues(pkEntry, uiRegistersPerElement, 
        uiItemsPerRegister);

    if (uiItemsPerRegister == 4)
    {
        // The data stream is already densely packed
        return pfSource;
    }
    else
    {
        // mapping is not contiguous
        const unsigned int uiSize = pkEntry->GetDataSize();
        const unsigned int uiStride = pkEntry->GetDataStride();
        const unsigned int uiArrayLength = uiSize / uiStride;

        // Each element (or row in the case of a 3x3 matrix) is less than
        // four floats and each one goes into a float4 register.
        m_afArrayMapping.SetSize(0);
        m_afArrayMapping.SetSize(4*uiRegistersPerElement*uiArrayLength);

        // copy the data a register at a time, restriding as we go
        unsigned int uiRegistersRemaining = 
            uiArrayLength*uiRegistersPerElement;
        float* pfDst = m_afArrayMapping.GetBase();
        while (uiRegistersRemaining--)
        {
            NiMemcpy(pfDst, pfSource, uiItemsPerRegister*sizeof(float));
            pfSource += uiItemsPerRegister;
            pfDst += 4;
        }

        return m_afArrayMapping.GetBase();
    }
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderConstantMap::GetEntryIndex(const char* pszKey)
{
    NiShaderConstantMapEntry* pkEntry;
    unsigned int uiSize = m_aspEntries.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        pkEntry = m_aspEntries.GetAt(ui);
        if (pkEntry)
        {
            if (NiStricmp(pszKey, pkEntry->GetKey()) == 0)
            {
                return ui;
            }
        }
    }

    return 0xffffffff;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::InsertEntry(
    NiShaderConstantMapEntry* pkEntry)
{
    if (m_aspEntries.AddFirstEmpty(pkEntry) == 0xffffffff)
    {
        // FAILED
        m_eLastError = NISHADERERR_ENTRYNOTADDED;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// This is a helper function that is only used by this file.
//---------------------------------------------------------------------------
static void FillDataArray(float* pfData, const D3DMATRIX& kMatrix)
{
    pfData[0] = kMatrix._11;
    pfData[1] = kMatrix._12;
    pfData[2] = kMatrix._13;
    pfData[3] = kMatrix._14;
    pfData[4] = kMatrix._21;
    pfData[5] = kMatrix._22;
    pfData[6] = kMatrix._23;
    pfData[7] = kMatrix._24;
    pfData[8] = kMatrix._31;
    pfData[9] = kMatrix._32;
    pfData[10] = kMatrix._33;
    pfData[11] = kMatrix._34;
    pfData[12] = kMatrix._41;
    pfData[13] = kMatrix._42;
    pfData[14] = kMatrix._43;
    pfData[15] = kMatrix._44;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetupPredefinedEntry(
    NiShaderConstantMapEntry* pkEntry)
{
    // Look up the key.
    bool bFound = false;

    unsigned int uiRegCount = 4;
    unsigned int uiMappingID;

    if (!LookUpPredefinedMapping(pkEntry->GetKey(), uiMappingID, uiRegCount))
        return NISHADERERR_INVALIDMAPPING;

    NiShaderAttributeDesc::AttributeType eAttribType = 
        LookUpPredefinedMappingType(uiMappingID, uiRegCount);
    unsigned int uiFlags = pkEntry->GetFlags();
    uiFlags &= ~NiShaderConstantMapEntry::GetAttributeMask();
    uiFlags |= NiShaderConstantMapEntry::GetAttributeFlags(eAttribType);
    pkEntry->SetFlags(uiFlags);

    if (uiMappingID != 0)
    {
        if (uiMappingID < SCM_DEF_MATRIXTYPE_COUNT)
        {
            if (uiMappingID == SCM_DEF_SKINBONE_MATRIX_3)
            {
                // Determine the register count from the bone count
                unsigned int uiBoneCount = (pkEntry->GetExtra() &
                    0xFFFF0000) >> 16;
                pkEntry->SetRegisterCount(uiBoneCount * 3);
            }
            else
            {
                pkEntry->SetRegisterCount(uiRegCount);
            }
        }

        // FOUND IT!
        pkEntry->SetInternal(uiMappingID);

        switch (uiMappingID)
        {
            // Constants
            // We will just set these.
        case SCM_DEF_CONSTS_TAYLOR_SIN:
            pkEntry->SetData(sizeof(float)*4, sizeof(float)*4, 
                (void*)ms_vTaylorSin, false);
            break;
        case SCM_DEF_CONSTS_TAYLOR_COS:
            pkEntry->SetData(sizeof(float)*4, sizeof(float)*4,
                (void*)ms_vTaylorCos, false);
            break;
        }
        bFound = true;
    }

    if (!bFound)
        return NISHADERERR_INVALIDMAPPING;

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetDefinedConstant(
    const NiRenderCallContext& kRCC,
    NiD3DShaderProgram* pkShaderProgram, 
    NiShaderConstantMapEntry* pkEntry)
{
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);
    const NiPropertyState* pkState = kRCC.m_pkState;
    const NiDynamicEffectState* pkEffects = kRCC.m_pkEffects;

    NiShaderError eResult = NISHADERERR_OK;
    unsigned int uiInternal = pkEntry->GetInternal();
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

            D3DXALIGNEDMATRIX kTempMat = m_pkD3DRenderer->GetD3DProj();

            if (bInv)
                D3DXMatrixInverse(&kTempMat, 0, &kTempMat);
            if (bTrans)
                D3DXMatrixTranspose(&kTempMat, &kTempMat);

            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
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

            D3DXALIGNEDMATRIX kTempMat = m_pkD3DRenderer->GetD3DView();

            if (bInv)
                D3DXMatrixInverse(&kTempMat, 0, &kTempMat);
            if (bTrans)
                D3DXMatrixTranspose(&kTempMat, &kTempMat);

            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
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

            D3DXALIGNEDMATRIX kTempMat = *(m_pkD3DRenderer->GetD3DWorld());
            if (bInv)
                D3DXMatrixInverse(&kTempMat, 0, &kTempMat);
            if (bTrans)
                D3DXMatrixTranspose(&kTempMat, &kTempMat);

            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
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

            D3DXALIGNEDMATRIX kD3DWorld = *(m_pkD3DRenderer->GetD3DWorld());
            D3DXALIGNEDMATRIX kD3DView = m_pkD3DRenderer->GetD3DView();
            D3DXALIGNEDMATRIX kTempMat = kD3DWorld * kD3DView;

            if (bInv)
                D3DXMatrixInverse(&kTempMat, 0, &kTempMat);
            if (bTrans)
                D3DXMatrixTranspose(&kTempMat, &kTempMat);

            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
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

            D3DXALIGNEDMATRIX kD3DView = m_pkD3DRenderer->GetD3DView();
            D3DXALIGNEDMATRIX kD3DProj = m_pkD3DRenderer->GetD3DProj();
            D3DXALIGNEDMATRIX kTempMat = kD3DView * kD3DProj;

            if (bInv)
                D3DXMatrixInverse(&kTempMat, 0, &kTempMat);
            if (bTrans)
                D3DXMatrixTranspose(&kTempMat, &kTempMat);

            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
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

            D3DXALIGNEDMATRIX kD3DWorld = *(m_pkD3DRenderer->GetD3DWorld());
            D3DXALIGNEDMATRIX kD3DView = m_pkD3DRenderer->GetD3DView();
            D3DXALIGNEDMATRIX kD3DProj = m_pkD3DRenderer->GetD3DProj();
            D3DXALIGNEDMATRIX kD3DWorldView = kD3DWorld * kD3DView;
            D3DXALIGNEDMATRIX kTempMat = kD3DWorldView * kD3DProj;

            if (bInv)
                D3DXMatrixInverse(&kTempMat, 0, &kTempMat);
            if (bTrans)
                D3DXMatrixTranspose(&kTempMat, &kTempMat);

            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_SKINBONE_MATRIX_3:
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;

            NiSkinningMeshModifier* pkSkin = 
                NiGetModifier(NiSkinningMeshModifier, pkMesh);

            if (!pkSkin)
                break;

            // Get the per-partition bone matrix palette
            NiDataStreamRef* pkBonePaletteStreamRef =
                pkMesh->FindStreamRef(NiCommonSemantics::BONE_PALETTE());
            NIASSERT(pkBonePaletteStreamRef != NULL);
            if (!pkBonePaletteStreamRef)
                break;

            const NiDataStream::Region& kRegion =
                pkBonePaletteStreamRef->GetRegionForSubmesh(kRCC.m_uiSubmesh);
            NiUInt32 uiRegionIdx = 
                pkBonePaletteStreamRef->GetRegionIndexForSubmesh(
                kRCC.m_uiSubmesh);

            NiDataStream* pkBonePaletteStream = 
                pkBonePaletteStreamRef->GetDataStream();

#if defined(NIDEBUG)
            // The palette indicies are assumed to be 16 bit unsigned ints,
            // packed in their own stream
            const NiDataStreamElement& kElem =
                pkBonePaletteStream->GetElementDescAt(0);
            NIASSERT(kElem.GetFormat() == NiDataStreamElement::F_UINT16_1);
            NIASSERT(pkBonePaletteStream->GetStride() == sizeof(NiUInt16));
            NIASSERT(kElem.GetOffset() == 0);
#endif
            const NiUInt32 uiBoneCount = kRegion.GetRange();

            // Get a pointer to the current region's bone palette
            const NiUInt16* puiBonePalette =
                (NiUInt16*)pkBonePaletteStream->LockRegion(uiRegionIdx,
                NiDataStream::LOCK_READ);
            NIASSERT(puiBonePalette);

            // Set the shader constants
            if (pkShaderProgram->SetShaderConstantArray(pkEntry,
                pkSkin->GetBoneMatrices(), uiBoneCount, 3, puiBonePalette))
            {
                eResult = NISHADERERR_OK;
            }

            pkBonePaletteStream->Unlock(
                NiDataStream::LOCK_READ);
        }
        break;
        // Texture transforms
    case SCM_DEF_TEXTRANSFORMBASE:
    case SCM_DEF_INVTEXTRANSFORMBASE:
    case SCM_DEF_TEXTRANSFORMBASE_T:
    case SCM_DEF_INVTEXTRANSFORMBASE_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMBASE_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMBASE_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMDARK:
    case SCM_DEF_INVTEXTRANSFORMDARK:
    case SCM_DEF_TEXTRANSFORMDARK_T:
    case SCM_DEF_INVTEXTRANSFORMDARK_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMDARK_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMDARK_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMDETAIL:
    case SCM_DEF_INVTEXTRANSFORMDETAIL:
    case SCM_DEF_TEXTRANSFORMDETAIL_T:
    case SCM_DEF_INVTEXTRANSFORMDETAIL_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMDETAIL_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMDETAIL_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMGLOSS:
    case SCM_DEF_INVTEXTRANSFORMGLOSS:
    case SCM_DEF_TEXTRANSFORMGLOSS_T:
    case SCM_DEF_INVTEXTRANSFORMGLOSS_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMGLOSS_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMGLOSS_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMGLOW:
    case SCM_DEF_INVTEXTRANSFORMGLOW:
    case SCM_DEF_TEXTRANSFORMGLOW_T:
    case SCM_DEF_INVTEXTRANSFORMGLOW_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMGLOW_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMGLOW_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMBUMP:
    case SCM_DEF_INVTEXTRANSFORMBUMP:
    case SCM_DEF_TEXTRANSFORMBUMP_T:
    case SCM_DEF_INVTEXTRANSFORMBUMP_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMBUMP_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMBUMP_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMNORMAL:
    case SCM_DEF_INVTEXTRANSFORMNORMAL:
    case SCM_DEF_TEXTRANSFORMNORMAL_T:
    case SCM_DEF_INVTEXTRANSFORMNORMAL_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMNORMAL_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMNORMAL_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMPARALLAX:
    case SCM_DEF_INVTEXTRANSFORMPARALLAX:
    case SCM_DEF_TEXTRANSFORMPARALLAX_T:
    case SCM_DEF_INVTEXTRANSFORMPARALLAX_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMPARALLAX_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMPARALLAX_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMDECAL:
    case SCM_DEF_INVTEXTRANSFORMDECAL:
    case SCM_DEF_TEXTRANSFORMDECAL_T:
    case SCM_DEF_INVTEXTRANSFORMDECAL_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMDECAL_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMDECAL_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_TEXTRANSFORMSHADER:
    case SCM_DEF_INVTEXTRANSFORMSHADER:
    case SCM_DEF_TEXTRANSFORMSHADER_T:
    case SCM_DEF_INVTEXTRANSFORMSHADER_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMSHADER_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMSHADER_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kTempMat;
            SetupTextureTransformMatrix(kTempMat, pkMatrix, bInv, bTrans);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTempMat))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;


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
        }
        break;
        // Materials
    case SCM_DEF_MATERIAL_DIFFUSE:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                float kColor[4];

                kColor[0] = pkMaterial->GetDiffuseColor().r;
                kColor[1] = pkMaterial->GetDiffuseColor().g;
                kColor[2] = pkMaterial->GetDiffuseColor().b;
                kColor[3] = pkMaterial->GetAlpha();

                if (!pkShaderProgram->SetShaderConstant(pkEntry, kColor))
                {
                    eResult = NISHADERERR_SETCONSTANTFAILED;
                }
            }
        }
        break;
    case SCM_DEF_MATERIAL_AMBIENT:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                float kColor[4];

                kColor[0] = pkMaterial->GetAmbientColor().r;
                kColor[1] = pkMaterial->GetAmbientColor().g;
                kColor[2] = pkMaterial->GetAmbientColor().b;
                kColor[3] = pkMaterial->GetAlpha();

                if (!pkShaderProgram->SetShaderConstant(pkEntry, kColor))
                {
                    eResult = NISHADERERR_SETCONSTANTFAILED;
                }
            }
        }
        break;
    case SCM_DEF_MATERIAL_SPECULAR:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                float kColor[4];

                kColor[0] = pkMaterial->GetSpecularColor().r;
                kColor[1] = pkMaterial->GetSpecularColor().g;
                kColor[2] = pkMaterial->GetSpecularColor().b;
                kColor[3] = pkMaterial->GetAlpha();

                if (!pkShaderProgram->SetShaderConstant(pkEntry, kColor))
                {
                    eResult = NISHADERERR_SETCONSTANTFAILED;
                }
            }
        }
        break;
    case SCM_DEF_MATERIAL_EMISSIVE:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                float kColor[4];

                kColor[0] = pkMaterial->GetEmittance().r;
                kColor[1] = pkMaterial->GetEmittance().g;
                kColor[2] = pkMaterial->GetEmittance().b;
                kColor[3] = pkMaterial->GetAlpha();

                if (!pkShaderProgram->SetShaderConstant(pkEntry, kColor))
                {
                    eResult = NISHADERERR_SETCONSTANTFAILED;
                }
            }
        }
        break;
    case SCM_DEF_MATERIAL_POWER:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                float kColor[4];

                kColor[0] = pkMaterial->GetShineness();
                kColor[1] = pkMaterial->GetShineness();
                kColor[2] = pkMaterial->GetShineness();
                kColor[3] = pkMaterial->GetShineness();

                if (!pkShaderProgram->SetShaderConstant(pkEntry, kColor))
                {
                    eResult = NISHADERERR_SETCONSTANTFAILED;
                }
            }
        }
        break;
        // Eye
    case SCM_DEF_EYE_POS:
        {
            D3DMATRIX kViewMat = m_pkD3DRenderer->GetInvView();
            float kPos[4];
            kPos[0] = kViewMat._41;
            kPos[1] = kViewMat._42;
            kPos[2] = kViewMat._43;
            kPos[3] = kViewMat._44;

            if (!pkShaderProgram->SetShaderConstant(pkEntry, kPos))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
        }
        break;
    case SCM_DEF_EYE_DIR:
        {
            D3DMATRIX kViewMat = m_pkD3DRenderer->GetInvView();
            float kDir[4];
            kDir[0] = kViewMat._31;
            kDir[1] = kViewMat._32;
            kDir[2] = kViewMat._33;
            kDir[3] = kViewMat._34;

            if (!pkShaderProgram->SetShaderConstant(pkEntry, kDir))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
        }
        break;
        // Constants
    case SCM_DEF_CONSTS_TAYLOR_SIN:
    case SCM_DEF_CONSTS_TAYLOR_COS:
        // Just set the data
        if (!pkShaderProgram->SetShaderConstant(pkEntry))
            eResult = NISHADERERR_SETCONSTANTFAILED;
        break;
        // Time
    case SCM_DEF_CONSTS_TIME:
    case SCM_DEF_CONSTS_SINTIME:
    case SCM_DEF_CONSTS_COSTIME:
    case SCM_DEF_CONSTS_TANTIME:
    case SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
        {
            // Grab the attribue from the geometry and set it
            NiFloatExtraData* pkFloatED = 
                (NiFloatExtraData*)pkMesh->GetExtraData(
                NiShaderConstantMap::GetTimeExtraDataName());
            if (!pkFloatED)
            {
                // Flag this error
                eResult = NISHADERERR_ENTRYNOTFOUND;
            }

            float fTime = pkFloatED->GetValue();
            float afData[4];

            switch (uiInternal)
            {
            case SCM_DEF_CONSTS_TIME:
                {
                    afData[0] = fTime;
                    afData[1] = fTime;
                    afData[2] = fTime;
                    afData[3] = fTime;
                }
                break;
            case SCM_DEF_CONSTS_SINTIME:
                {
                    float fSin = sinf(fTime);
                    afData[0] = fSin;
                    afData[1] = fSin;
                    afData[2] = fSin;
                    afData[3] = fSin;
                }
                break;
            case SCM_DEF_CONSTS_COSTIME:
                {
                    float fCos = cosf(fTime);
                    afData[0] = fCos;
                    afData[1] = fCos;
                    afData[2] = fCos;
                    afData[3] = fCos;
                }
                break;
            case SCM_DEF_CONSTS_TANTIME:
                {
                    float fTan = tanf(fTime);
                    afData[0] = fTan;
                    afData[1] = fTan;
                    afData[2] = fTan;
                    afData[3] = fTan;
                }
                break;
            case SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
                {
                    afData[0] = fTime;
                    afData[1] = sinf(fTime);
                    afData[2] = cosf(fTime);
                    afData[3] = tanf(fTime);
                }
                break;
            default:
                NIASSERT(!"Time set --> Invalid case!");
                return NISHADERERR_ENTRYNOTFOUND;
            }

            if (!pkShaderProgram->SetShaderConstant(pkEntry, afData))
                eResult = NISHADERERR_SETCONSTANTFAILED;
        }
        break;
    case SCM_DEF_AMBIENTLIGHT:
        {
            float kAmbient[4];
            kAmbient[0] = kAmbient[1] = kAmbient[2] = 0.0f;
            kAmbient[3] = 1.0f;
            if (pkEffects)
            {
                NiDynEffectStateIter kIter = pkEffects->GetLightHeadPos();
                while (kIter)
                {
                    NiAmbientLight* pkLight = NiDynamicCast(NiAmbientLight,
                        pkEffects->GetNextLight(kIter));
                    if (pkLight && pkLight->GetSwitch())
                    {
                        NiColor kColor = pkLight->GetAmbientColor() *
                            pkLight->GetDimmer();
                        kAmbient[0] += kColor.r;
                        kAmbient[1] += kColor.g;
                        kAmbient[2] += kColor.b;
                    }
                }
            }
            if (!pkShaderProgram->SetShaderConstant(pkEntry, kAmbient))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
        }
        break;
    case SCM_DEF_FOG_DENSITY:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NIASSERT(pkFog);

            float fNear, fFar;
            m_pkD3DRenderState->GetCameraNearAndFar(fNear, fFar);
            float fDensity = 1.0f / (pkFog->GetDepth() * (fFar - fNear));

            float kDensity[4];
            kDensity[0] = kDensity[1] = kDensity[2] = kDensity[3] = fDensity;
            if (!pkShaderProgram->SetShaderConstant(pkEntry, kDensity))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
        }
        break;
    case SCM_DEF_FOG_NEARFAR:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NIASSERT(pkFog);

            float fNear, fFar;
            m_pkD3DRenderState->GetCameraNearAndFar(fNear, fFar);
            float fCameraDepthRange = fFar - fNear;

            float fWorldDepth = fCameraDepthRange * pkFog->GetDepth();
            float fFogNear = fFar - fWorldDepth;

            float fFogFar = fFar + 
                m_pkD3DRenderState->GetMaxFogFactor() * fWorldDepth;

            float kNearFar[4];
            kNearFar[0] = fFogNear;
            kNearFar[1] = fFogFar;
            kNearFar[2] = 0.0f;
            kNearFar[3] = 0.0f;
            if (!pkShaderProgram->SetShaderConstant(pkEntry, kNearFar))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
        }
        break;
    case SCM_DEF_FOG_COLOR:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NIASSERT(pkFog);
            if (!pkShaderProgram->SetShaderConstant(pkEntry, 
                &pkFog->GetFogColor()))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
            break;
        }
     case SCM_DEF_PARALLAX_OFFSET:
         {
            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &fOffset))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
            break;
        }
    case SCM_DEF_BUMP_MATRIX:
         {
            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            float afMatrix[4];
            afMatrix[0] = 1.0f;
            afMatrix[1] = 1.0f;
            afMatrix[2] = 1.0f;
            afMatrix[3] = 1.0f;

            if (pkTexProp)
            {
                NiTexturingProperty::BumpMap* pkBumpMap = 
                    pkTexProp->GetBumpMap();
                if (pkBumpMap)
                {
                    afMatrix[0] = pkBumpMap->GetBumpMat00();
                    afMatrix[1] = pkBumpMap->GetBumpMat01();
                    afMatrix[2] = pkBumpMap->GetBumpMat10();
                    afMatrix[3] = pkBumpMap->GetBumpMat11();
                }
            }
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &afMatrix))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
            break;
            break;
        }
    case SCM_DEF_BUMP_LUMA_OFFSET_AND_SCALE:
         {
            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            NiPoint2 kOffsetAndScale(0.0f, 1.0f);
            if (pkTexProp)
            {
                NiTexturingProperty::BumpMap* pkBumpMap = 
                    pkTexProp->GetBumpMap();
                if (pkBumpMap)
                {
                    kOffsetAndScale.x = pkBumpMap->GetLumaOffset();
                    kOffsetAndScale.y = pkBumpMap->GetLumaScale();
                }
            }
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kOffsetAndScale))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
            break;
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
            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                    
                NiPoint2 kSize(0.0f, 0.0f);
                if (pkMap && pkMap->GetTexture())
                {
                    NiTexture* pkTex = pkMap->GetTexture();
                    kSize.x = (float) pkTex->GetWidth();
                    kSize.y = (float) pkTex->GetHeight();
                }

                if (!pkShaderProgram->SetShaderConstant(pkEntry, &kSize))
                {
                    eResult = NISHADERERR_SETCONSTANTFAILED;
                }
            }        
            break;
        }
    case SCM_DEF_ALPHA_TEST_FUNC:
        {
            NIASSERT(pkState);
            const NiAlphaProperty* pkAlphaProp = pkState->GetAlpha();
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
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &kTestConditions))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
            break;
        }
    case SCM_DEF_ALPHA_TEST_REF:
        {
            NIASSERT(pkState);
            const NiAlphaProperty* pkAlphaProp = pkState->GetAlpha();
            float fRef = 0.0f;
            if (pkAlphaProp)
            {
                fRef = (float)pkAlphaProp->GetTestRef() / 255.0f;
            }
            if (!pkShaderProgram->SetShaderConstant(pkEntry, &fRef))
            {
                eResult = NISHADERERR_SETCONSTANTFAILED;
            }
            break;
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
#ifdef NIDEBUG
        {
            char acString[256];
            NiSprintf(acString, 256, "Constant \"%s\" is not supported.\n",
                (const char*)pkEntry->GetKey());
            NiOutputDebugString(acString);
        }
#endif
        break;
    }

    return eResult;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetConstantConstant(
    const NiRenderCallContext&,
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry)
{
    if (pkEntry->IsArray())
    {
        // if necessary, unpack values for float4 registers, then upload
        const float * pfData = (const float*)MapConstantArray(pkEntry, 
            (const float*)pkEntry->GetDataSource());
        return SetShaderConstantArray(pkShaderProgram, pkEntry, pfData);
    }
    else
    {
        // Just set the data
        const void* pvData = MapConstantValue(pkEntry);
        if (!pvData)
            return NISHADERERR_UNKNOWN;

        if (pkShaderProgram->SetShaderConstant(pkEntry, pvData))
            return NISHADERERR_OK;
        else
            return NISHADERERR_SETCONSTANTFAILED;
    }
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetAttributeConstant(
    const NiRenderCallContext& kRCC,
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry,
    bool bGlobal,
    NiSCMExtraData* pkSCMExtraData)
{
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

    // Grab the attribute from the geometry and set it
    const void* pvValue = NULL;

    // Attempt to get the extra data for this attribute from the cache
    // rather than using strcmp.
    NiExtraData* pkExtra = 0;
    if (pkSCMExtraData)
    {
        pkExtra = pkSCMExtraData->GetNextEntry(pkEntry->GetShaderRegister(),
            kRCC.m_uiPass, GetProgramType(), bGlobal);

        // Check for a match - it's possible for a mismatch to occur if
        // an entry had its shader register changed (such as when storing
        // HLSL shader constant registers after the NiSCMExtraData was 
        // created.)
        if (!pkExtra || (pkExtra->GetName() != pkEntry->GetKey()))
        {
            pkExtra = pkMesh->GetExtraData(pkEntry->GetKey());
        
            if (pkExtra)
            {
                // If a new match was found, replace the original
                pkSCMExtraData->AddEntry(pkEntry->GetShaderRegister(), 
                    kRCC.m_uiPass, GetProgramType(), pkExtra, bGlobal);
            }
        }
    }
    else if (!pkExtra)
    {
        pkExtra = pkMesh->GetExtraData(pkEntry->GetKey());
    }

    if (pkEntry->IsArray())
    {
        // if necessary, unpack values for float4 registers, then upload
        const float * pfData = (const float*)
            MapAttributeArrayValue(pkEntry, pkExtra);
        return SetShaderConstantArray(pkShaderProgram, pkEntry, pfData);
    }
    else
    {
        if (pkExtra == NULL)
        {
            // Attribute not found - use default value
            pvValue = MapConstantValue(pkEntry);
            if (pvValue == NULL)
                return NISHADERERR_ENTRYNOTFOUND;
        }
        else
        {
            pvValue = MapAttributeValue(pkEntry, pkExtra);
            if (!pvValue)
                return NISHADERERR_INVALIDMAPPING;
        }
        NIASSERT(pvValue);

        if (pkEntry->IsMatrix4())
        {
            // We can 'cheat' here, since we know that it will fill all 4
            // components of the constant register!
            if (!pkShaderProgram->SetShaderConstant(pkEntry, pvValue, 4))
                return NISHADERERR_SETCONSTANTFAILED;
        }
        else if ((pkEntry->IsPoint3()) || 
            (pkEntry->IsPoint4()) || 
            (pkEntry->IsPoint2()) || 
            (pkEntry->IsFloat()) || 
            (pkEntry->IsColor()) ||
            (pkEntry->IsUnsignedInt()) ||
            (pkEntry->IsBool()))
        {
            // Bools and ints have already been mapped to floats if necessary
            if (!pkShaderProgram->SetShaderConstant(pkEntry, pvValue))
                return NISHADERERR_SETCONSTANTFAILED;
        }
        else if (pkEntry->IsMatrix3())
        {
            if (!pkShaderProgram->SetShaderConstant(pkEntry, pvValue, 3))
                return NISHADERERR_SETCONSTANTFAILED;
        }
        else if (pkEntry->IsTexture())
        {
            // Textures are hooked up when setting the sampler states
        }
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetGlobalConstant(
    const NiRenderCallContext&,
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry)
{
    NiShaderError eResult = NISHADERERR_OK;
    if (pkEntry->IsBool())
    {
        // The data is only a single bool...
        ms_bMappingValue = *((bool*)pkEntry->GetDataSource());
        if (!pkShaderProgram->SetShaderConstant(pkEntry, 
            &ms_bMappingValue))
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }
    else if (pkEntry->IsUnsignedInt())
    {
        // The data is only a single unsigned int...
        ms_aiMappingValue[0] = *((const int*)pkEntry->GetDataSource());
        ms_aiMappingValue[1] = ms_aiMappingValue[0];
        ms_aiMappingValue[2] = ms_aiMappingValue[0];
        ms_aiMappingValue[3] = ms_aiMappingValue[0];

        if (!pkShaderProgram->SetShaderConstant(pkEntry, 
            ms_aiMappingValue))
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }
    else if (pkEntry->IsFloat())
    {
        // Data is only a single float
        float* pfData = (float*)pkEntry->GetDataSource();
        ms_vMappingValue[0] = pfData[0];
        ms_vMappingValue[1] = ms_vMappingValue[0];
        ms_vMappingValue[2] = ms_vMappingValue[0];
        ms_vMappingValue[3] = ms_vMappingValue[0];

        if (!pkShaderProgram->SetShaderConstant(pkEntry, 
            ms_vMappingValue))
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }
    else if (pkEntry->IsPoint2())
    {
        // Data is only 2 floats
        float* pfData = (float*)pkEntry->GetDataSource();
        ms_vMappingValue[0] = pfData[0];
        ms_vMappingValue[1] = pfData[1];
        ms_vMappingValue[2] = ms_vMappingValue[0];
        ms_vMappingValue[3] = ms_vMappingValue[1];

        if (!pkShaderProgram->SetShaderConstant(pkEntry, 
            ms_vMappingValue))
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }
    else if (pkEntry->IsPoint3())
    {
        // Data is only 3 floats
        float* pfData = (float*)pkEntry->GetDataSource();
        ms_vMappingValue[0] = pfData[0];
        ms_vMappingValue[1] = pfData[1];
        ms_vMappingValue[2] = pfData[2];
        ms_vMappingValue[3] = 1.0f;

        if (!pkShaderProgram->SetShaderConstant(pkEntry, 
            ms_vMappingValue))
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }
    else if (pkEntry->IsPoint4() || pkEntry->IsColor())
    {
        // Data is 4 floats
        float* pfData = (float*)pkEntry->GetDataSource();
        ms_vMappingValue[0] = pfData[0];
        ms_vMappingValue[1] = pfData[1];
        ms_vMappingValue[2] = pfData[2];
        ms_vMappingValue[3] = pfData[3];

        if (!pkShaderProgram->SetShaderConstant(pkEntry, 
            ms_vMappingValue))
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }
    else if (pkEntry->IsMatrix3())
    {
        // Data is a 3x3 matrix
        float* pfData = (float*)pkEntry->GetDataSource();
        ms_mMappingValue._11 = pfData[0];
        ms_mMappingValue._12 = pfData[1];
        ms_mMappingValue._13 = pfData[2];
        ms_mMappingValue._14 = 0.0f;
        ms_mMappingValue._21 = pfData[3];
        ms_mMappingValue._22 = pfData[4];
        ms_mMappingValue._23 = pfData[5];
        ms_mMappingValue._24 = 0.0f;
        ms_mMappingValue._31 = pfData[6];
        ms_mMappingValue._32 = pfData[7];
        ms_mMappingValue._33 = pfData[8];
        ms_mMappingValue._34 = 0.0f;

        if (!pkShaderProgram->SetShaderConstant(pkEntry, 
            ms_mMappingValue))
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }
    else if (pkEntry->IsMatrix4())
    {
        // Data is a 4x4 matrix
        if (!pkShaderProgram->SetShaderConstant(pkEntry, 
            pkEntry->GetDataSource()))
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }
    else if (pkEntry->IsArray())
    {
        // if necessary, unpack values for float4 registers, then upload
        const float * pfData = (const float*)MapConstantArray(pkEntry, 
            (const float*)pkEntry->GetDataSource());
        eResult = SetShaderConstantArray(pkShaderProgram, pkEntry, pfData);
    }

    return eResult;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetOperatorConstant(
    const NiRenderCallContext& kRCC,
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry)
{
    unsigned int uiExtra = pkEntry->GetExtra();

    unsigned int uiEntry1 = 
        uiExtra & NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY1_MASK;
    unsigned int uiEntry2 = 
        (uiExtra & NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_MASK) >> 
        NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_SHIFT;
    unsigned int uiOperator = 
        uiExtra & NiShaderConstantMapEntry::SCME_OPERATOR_MASK;
    bool bTranspose = (uiExtra & 
        NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_TRANSPOSE) ? 
        true : false;
    bool bInverse = (uiExtra & 
        NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_INVERSE) ? 
        true : false;

    // Grab the two entries
    NiShaderConstantMapEntry* pkEntry1 = GetEntryAtIndex(uiEntry1);
    NiShaderConstantMapEntry* pkEntry2 = GetEntryAtIndex(uiEntry2);

    if (!pkEntry1 || !pkEntry2)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Invalid entries in OperatorConstant\n");
        return NISHADERERR_UNKNOWN;
    }

    // Determine the results data type and set it in the flags
    unsigned int uiCheckType1;
    unsigned int uiCheckType2;
    NiShaderAttributeDesc::AttributeType eType1 = 
        pkEntry1->GetAttributeType();
    NiShaderAttributeDesc::AttributeType eType2 = 
        pkEntry2->GetAttributeType();

    // Setup entry 1s value
    if (pkEntry1->IsDefined())
    {
        eType1 = LookUpPredefinedMappingType(pkEntry1->GetKey());
        uiCheckType1 = SetupDefinedConstantValue(kRCC, 0, pkEntry1);
    }
    else if (pkEntry1->IsGlobal())
    {
        uiCheckType1 = SetupConstantConstantValue(kRCC, 0, pkEntry1);
    }
    else if (pkEntry1->IsAttribute())
    {
        uiCheckType1 = SetupAttributeConstantValue(kRCC, 0, pkEntry1);
    }
    else if (pkEntry1->IsConstant())
    {
        uiCheckType1 = SetupAttributeConstantValue(kRCC, 0, pkEntry1);
    }

    // Setup entry 2s value
    if (pkEntry2->IsDefined())
    {
        eType2 = LookUpPredefinedMappingType(pkEntry2->GetKey());
        uiCheckType2 = SetupDefinedConstantValue(kRCC, 1, pkEntry2);
    }
    else if (pkEntry2->IsAttribute())
    {
        uiCheckType2 = SetupAttributeConstantValue(kRCC, 1, pkEntry2);
    }
    else if (pkEntry2->IsGlobal())
    {
        uiCheckType2 = SetupGlobalConstantValue(kRCC, 1, pkEntry2);
    }
    else if (pkEntry2->IsConstant())
    {
        uiCheckType2 = SetupConstantConstantValue(kRCC, 1, pkEntry2);
    }

    if ((eType1 == 
        NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        (eType2 == 
        NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED))
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Invalid operands in OperatorConstant\n");
        return NISHADERERR_UNKNOWN;
    }

    // Perform the operation
    switch (uiOperator)
    {
    case NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY:
        return PerformOperatorMultiply(pkShaderProgram, pkEntry, eType1, 
            eType2, bInverse, bTranspose);
    case NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE:
        return PerformOperatorDivide(pkShaderProgram, pkEntry, eType1, 
            eType2, bInverse, bTranspose);
    case NiShaderConstantMapEntry::SCME_OPERATOR_ADD:
        return PerformOperatorAdd(pkShaderProgram, pkEntry, eType1, eType2, 
            bInverse, bTranspose);
    case NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT:
        return PerformOperatorSubtract(pkShaderProgram, pkEntry, eType1, 
            eType2, bInverse, bTranspose);
    default:
        return NISHADERERR_UNKNOWN;
    }
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetupObjectEntry(
    NiShaderConstantMapEntry* pkEntry)
{
    unsigned int uiRegCount = 0;
    unsigned int uiMappingID;
    if (!LookUpObjectMapping(pkEntry->GetKey(), uiMappingID))
    {
        return NISHADERERR_INVALIDMAPPING;
    }

    unsigned int uiFloatCount;
    NiShaderAttributeDesc::AttributeType eAttribType =
        LookUpObjectMappingType(uiMappingID, uiRegCount, uiFloatCount);
    unsigned int uiFlags = pkEntry->GetFlags();
    uiFlags &= ~NiShaderConstantMapEntry::GetAttributeMask();
    uiFlags |= NiShaderConstantMapEntry::GetAttributeFlags(eAttribType);
    pkEntry->SetFlags(uiFlags);

    if (uiMappingID != NiShaderConstantMap::SCM_OBJ_INVALID)
    {
        pkEntry->SetInternal(uiMappingID);
        pkEntry->SetRegisterCount(uiRegCount);
    }
    else
    {
        return NISHADERERR_INVALIDMAPPING;
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
bool NiD3DShaderConstantMap::GetDynamicEffectData(
    void* pvData,
    unsigned int uiDataSize,
    ObjectMappings eMapping,
    NiDynamicEffect* pkDynEffect,
    const NiRenderCallContext& kRCC)
{
#if defined(NIDEBUG)
    // Ensure that the data size matches the object type.
    unsigned int uiRegCount, uiFloatCount;
    NiShaderAttributeDesc::AttributeType eType = LookUpObjectMappingType(
        eMapping, uiRegCount, uiFloatCount);
    NIASSERT(eType != NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED);
    NIASSERT(uiDataSize >= 4 * uiRegCount * sizeof(float));
#endif

    float* pfData = (float*) pvData;

    const NiTransform& kWorld = *kRCC.m_pkWorld;
    NIASSERT(kRCC.m_pkWorld);
    const NiBound& kWorldBound = *kRCC.m_pkWorldBound;
    NIASSERT(kRCC.m_pkWorldBound);
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);
    const NiDynamicEffectState* pkEffects = kRCC.m_pkEffects;
    const NiPropertyState* pkState = kRCC.m_pkState;

    D3DMATRIX kIdentMatrix;
    memset(&kIdentMatrix, 0, sizeof(kIdentMatrix));
    kIdentMatrix._11 = kIdentMatrix._22 = kIdentMatrix._33 = kIdentMatrix._44
        = 1.0f;

    switch (eMapping)
    {
    case NiShaderConstantMap::SCM_OBJ_DIMMER:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            pfData[0] = ((NiLight*) pkDynEffect)->GetDimmer();
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_UNDIMMEDAMBIENT:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            const NiColor& kColor = ((NiLight*)
                pkDynEffect)->GetAmbientColor();
            pfData[0] = kColor.r;
            pfData[1] = kColor.g;
            pfData[2] = kColor.b;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_UNDIMMEDDIFFUSE:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            const NiColor& kColor = ((NiLight*)
                pkDynEffect)->GetDiffuseColor();
            pfData[0] = kColor.r;
            pfData[1] = kColor.g;
            pfData[2] = kColor.b;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_UNDIMMEDSPECULAR:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            const NiColor& kColor = ((NiLight*)
                pkDynEffect)->GetSpecularColor();
            pfData[0] = kColor.r;
            pfData[1] = kColor.g;
            pfData[2] = kColor.b;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_AMBIENT:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiColor kColor = ((NiLight*) pkDynEffect)->GetAmbientColor();
            kColor *= ((NiLight*) pkDynEffect)->GetDimmer();
            pfData[0] = kColor.r;
            pfData[1] = kColor.g;
            pfData[2] = kColor.b;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_DIFFUSE:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiColor kColor = ((NiLight*) pkDynEffect)->GetDiffuseColor();
            kColor *= ((NiLight*) pkDynEffect)->GetDimmer();
            pfData[0] = kColor.r;
            pfData[1] = kColor.g;
            pfData[2] = kColor.b;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_SPECULAR:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiColor kColor = ((NiLight*) pkDynEffect)->GetSpecularColor();
            kColor *= ((NiLight*) pkDynEffect)->GetDimmer();
            pfData[0] = kColor.r;
            pfData[1] = kColor.g;
            pfData[2] = kColor.b;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDPOSITION:
        if (pkDynEffect)
        {
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                NiPoint3 kPosition = ((NiDirectionalLight*)
                    pkDynEffect)->GetWorldDirection() *
                    ms_fDirLightDistance;
                pfData[0] = kPosition.x;
                pfData[1] = kPosition.y;
                pfData[2] = kPosition.z;
                pfData[3] = 1.0f;
            }
            else
            {
                pfData[0] = pkDynEffect->GetWorldTranslate().x;
                pfData[1] = pkDynEffect->GetWorldTranslate().y;
                pfData[2] = pkDynEffect->GetWorldTranslate().z;
                pfData[3] = 1.0f;
            }
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELPOSITION:
        if (pkDynEffect)
        {
            NiTransform kInvWorld;
            kWorld.Invert(kInvWorld);

            NiPoint3 kPosition;
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                kPosition = kInvWorld * (((NiDirectionalLight*)
                    pkDynEffect)->GetWorldDirection() *
                    ms_fDirLightDistance);
            }
            else
            {
                kPosition = kInvWorld * pkDynEffect->GetWorldTranslate();
            }

            pfData[0] = kPosition.x;
            pfData[1] = kPosition.y;
            pfData[2] = kPosition.z;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION:
        if (pkDynEffect)
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
                kDirection = kWorldBound.GetCenter() -
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
                kDirection = ((NiSpotLight*)
                    pkDynEffect)->GetWorldDirection();
            }

            pfData[0] = kDirection.x;
            pfData[1] = kDirection.y;
            pfData[2] = kDirection.z;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 1.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELDIRECTION:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            NiPoint3 kDirection;
            if (pkDynEffect->GetEffectType() == 
                NiDynamicEffect::POINT_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWPOINT_LIGHT)
            {
                // Get the normalized vector from the light to the
                // center of the bounding volume of the rendered object
                // in world space.
                kDirection = kWorldBound.GetCenter() -
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
                kDirection = ((NiSpotLight*)
                    pkDynEffect)->GetWorldDirection();
            }

            // Convert direction vector to rendered object's model space.
            kDirection = kWorld.m_Rotate.Transpose() * kDirection;

            pfData[0] = kDirection.x;
            pfData[1] = kDirection.y;
            pfData[2] = kDirection.z;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 1.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDTRANSFORM:
        if (pkDynEffect)
        {
            D3DMATRIX kMatrix;
            kMatrix._14 = kMatrix._24 = kMatrix._34 = 0.0f;
            kMatrix._44 = 1.0f;
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                NiD3DUtility::GetD3DFromNi(kMatrix,
                    pkDynEffect->GetWorldRotate(), ((NiDirectionalLight*)
                    pkDynEffect)->GetWorldDirection() *
                    ms_fDirLightDistance, pkDynEffect->GetWorldScale());
            }
            else
            {
                NiD3DUtility::GetD3DFromNi(kMatrix,
                    pkDynEffect->GetWorldTransform());
            }
            FillDataArray(pfData, kMatrix);
            return true;
        }
        else
        {
            FillDataArray(pfData, kIdentMatrix);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELTRANSFORM:
        if (pkDynEffect)
        {
            NiTransform kInvWorld;
            kWorld.Invert(kInvWorld);

            D3DMATRIX kMatrix;
            kMatrix._14 = kMatrix._24 = kMatrix._34 = 0.0f;
            kMatrix._44 = 1.0f;
            if (pkDynEffect->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT)
            {
                NiTransform kDynEffectWorld =
                    pkDynEffect->GetWorldTransform();
                kDynEffectWorld.m_Translate = ((NiDirectionalLight*)
                    pkDynEffect)->GetWorldDirection() *
                    ms_fDirLightDistance;
                NiD3DUtility::GetD3DFromNi(kMatrix, kInvWorld *
                    kDynEffectWorld);
            }
            else
            {
                NiD3DUtility::GetD3DFromNi(kMatrix, kInvWorld *
                    pkDynEffect->GetWorldTransform());
            }
            FillDataArray(pfData, kMatrix);
            return true;
        }
        else
        {
            FillDataArray(pfData, kIdentMatrix);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_SPOTATTENUATION:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            if (pkDynEffect->GetEffectType() == NiDynamicEffect::SPOT_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWSPOT_LIGHT)
            {
                pfData[0] = 
                    ((NiSpotLight*)pkDynEffect)->GetInnerSpotAngleCos();
                pfData[1] = ((NiSpotLight*)pkDynEffect)->GetSpotAngleCos();
                pfData[2] = ((NiSpotLight*)pkDynEffect)->GetSpotExponent();
                pfData[3] = 0.0f;
            }
            else
            {
                pfData[0] = -1.0f;
                pfData[1] = 0.0f;
                pfData[2] = 0.0f;
                pfData[3] = 0.0f;
            }
            return true;
        }
        else
        {
            pfData[0] = -1.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_ATTENUATION:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiLight, pkDynEffect));

            if (pkDynEffect->GetEffectType() == 
                NiDynamicEffect::POINT_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWPOINT_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SPOT_LIGHT ||
                pkDynEffect->GetEffectType() == 
                NiDynamicEffect::SHADOWSPOT_LIGHT)
            {
                pfData[0] = ((NiPointLight*)
                    pkDynEffect)->GetConstantAttenuation();
                pfData[1] = ((NiPointLight*)
                    pkDynEffect)->GetLinearAttenuation();
                pfData[2] = ((NiPointLight*)
                    pkDynEffect)->GetQuadraticAttenuation();
                pfData[3] = 0.0f;
            }
            else
            {
                pfData[0] = 1.0f;
                pfData[1] = 0.0f;
                pfData[2] = 0.0f;
                pfData[3] = 0.0f;
            }
            return true;
        }
        else
        {
            pfData[0] = 1.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONMATRIX:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            D3DMATRIX kMatrix;
            kMatrix._14 = kMatrix._24 = kMatrix._34 = 0.0f;
            kMatrix._44 = 1.0f;
            NiD3DUtility::GetD3DFromNi(kMatrix, ((NiTextureEffect*)
                pkDynEffect)->GetWorldProjectionMatrix(), NiPoint3::ZERO,
                1.0f);
            FillDataArray(pfData, kMatrix);
            return true;
        }
        else
        {
            FillDataArray(pfData, kIdentMatrix);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONMATRIX:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            D3DMATRIX kMatrix;
            kMatrix._14 = kMatrix._24 = kMatrix._34 = 0.0f;
            kMatrix._44 = 1.0f;
            NiD3DUtility::GetD3DFromNi(kMatrix,
                kWorld.m_Rotate.Transpose() * ((NiTextureEffect*)
                pkDynEffect)->GetWorldProjectionMatrix(), NiPoint3::ZERO,
                1.0f);
            FillDataArray(pfData, kMatrix);
            return true;
        }
        else
        {
            FillDataArray(pfData, kIdentMatrix);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSLATION:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            const NiPoint3& kTranslation = ((NiTextureEffect*)
                pkDynEffect)->GetWorldProjectionTranslation();

            pfData[0] = kTranslation.x;
            pfData[1] = kTranslation.y;
            pfData[2] = kTranslation.z;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONTRANSLATION:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTransform kInvWorld;
            kWorld.Invert(kInvWorld);

            NiPoint3 kTranslation = ((NiTextureEffect*)
                pkDynEffect)->GetWorldProjectionTranslation();
            kTranslation = kInvWorld * kTranslation;

            pfData[0] = kTranslation.x;
            pfData[1] = kTranslation.y;
            pfData[2] = kTranslation.z;
            pfData[3] = 1.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 1.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDCLIPPINGPLANE:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTextureEffect* pkTexEffect = (NiTextureEffect*) pkDynEffect;
            if (pkTexEffect->GetClippingPlaneEnable())
            {
                const NiPlane& kPlane =
                    pkTexEffect->GetWorldClippingPlane();
                pfData[0] = kPlane.GetNormal().x;
                pfData[1] = kPlane.GetNormal().y;
                pfData[2] = kPlane.GetNormal().z;
                pfData[3] = kPlane.GetConstant();
            }
            else
            {
                pfData[0] = 0.0f;
                pfData[1] = 0.0f;
                pfData[2] = 0.0f;
                pfData[3] = 0.0f;
            }
            return true;
        }
        else
        {
            pfData[0] = 1.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELCLIPPINGPLANE:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTextureEffect* pkTexEffect = (NiTextureEffect*) pkDynEffect;
            if (pkTexEffect->GetClippingPlaneEnable())
            {
                NiTransform kInvWorld;
                kWorld.Invert(kInvWorld);

                NiPlane kPlane = pkTexEffect->GetWorldClippingPlane();
                NiPoint3 kNormal = kInvWorld.m_Rotate *
                    kPlane.GetNormal();
                NiPoint3 kPoint = kInvWorld * (kPlane.GetNormal() *
                    kPlane.GetConstant());
                float fConstant = kNormal * kPoint;

                pfData[0] = kNormal.x;
                pfData[1] = kNormal.y;
                pfData[2] = kNormal.z;
                pfData[3] = fConstant;
            }
            else
            {
                pfData[0] = 0.0f;
                pfData[1] = 0.0f;
                pfData[2] = 0.0f;
                pfData[3] = 0.0f;
            }
            return true;
        }
        else
        {
            pfData[0] = 1.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_TEXCOORDGEN:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            float fTexCoordGen = 0.0f;
            switch (((NiTextureEffect*)
                pkDynEffect)->GetTextureCoordGen())
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

            pfData[0] = fTexCoordGen;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSFORM:
        if (pkDynEffect)
        {           
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTextureEffect* pkTexEffect = (NiTextureEffect*)pkDynEffect;
            D3DMATRIX kMatrix;
            kMatrix._14 = kMatrix._24 = kMatrix._34 = 0.0f;
            kMatrix._44 = 1.0f;
            NiD3DUtility::GetD3DFromNi(kMatrix, 
                pkTexEffect->GetWorldProjectionMatrix(), 
                pkTexEffect->GetWorldProjectionTranslation(),
                1.0f);
            FillDataArray(pfData, kMatrix);
            return true;
        }
        else
        {
            FillDataArray(pfData, kIdentMatrix);
            return false;
        }
    case NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONTRANSFORM:
        if (pkDynEffect)
        {
            NIASSERT(NiIsKindOf(NiTextureEffect, pkDynEffect));

            NiTextureEffect* pkTexEffect = (NiTextureEffect*)pkDynEffect;
            D3DMATRIX kMatrix;
            kMatrix._14 = kMatrix._24 = kMatrix._34 = 0.0f;
            kMatrix._44 = 1.0f;

            NiTransform kInvWorld;
            kWorld.Invert(kInvWorld);

            NiPoint3 kTranslation = ((NiTextureEffect*)
                pkDynEffect)->GetWorldProjectionTranslation();
            kTranslation = kInvWorld * kTranslation;
            NiD3DUtility::GetD3DFromNi(kMatrix, 
                kWorld.m_Rotate.Transpose() * 
                pkTexEffect->GetWorldProjectionMatrix(), 
                kTranslation,
                1.0f);
            FillDataArray(pfData, kMatrix);
            return true;
        }
        else
        {
            FillDataArray(pfData, kIdentMatrix);
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
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkMesh);
            NIASSERT(pkShadowMap);

            D3DXMATRIX kMatrix, kMatrixT;
            const float* aafWorldToSM = pkShadowMap->GetWorldToShadowMap();
            NiMemcpy(&kMatrix, aafWorldToSM, sizeof(float) * 16);

            D3DXMatrixTranspose(&kMatrixT, &kMatrix);
            FillDataArray(pfData, kMatrixT);

            return true;
        }
        else
        {
            FillDataArray(pfData, kIdentMatrix);
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
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkMesh);
            NIASSERT(pkShadowMap);

            pfData[0] = (float)pkShadowMap->GetTexture()->GetWidth();
            pfData[1] = (float)pkShadowMap->GetTexture()->GetHeight();
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;

            return true;
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
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
            return pkGenerator->GetShaderConstantData(pvData, 
                sizeof(float) * 4, pkMesh, 
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, eMapping, 
                pkState, pkEffects, kWorld, kWorldBound, kRCC.m_uiPass);
        }
        else
        {
            pfData[0] = 0.0f;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return false;
        }

    default:
        break;
    }

    memset(pvData, 0, uiDataSize);
    return false;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::SetObjectConstant(
    const NiRenderCallContext& kRCC,
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry,
    NiDynamicEffect* pkDynEffect)
{
    NiShaderError eResult = NISHADERERR_OK;
    NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

    // Get the register count for the mapping type.
    ObjectMappings eMapping = (ObjectMappings)
        pkEntry->GetInternal();
    unsigned int uiRegCount, uiFloatCount;
    NiShaderAttributeDesc::AttributeType eType = LookUpObjectMappingType(
        eMapping, uiRegCount, uiFloatCount);
    if (eType == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
    {
        return NISHADERERR_ENTRYNOTFOUND;
    }

    // uiRegCount now contains the register count. Each register is 4 floats.
    unsigned int uiArraySize = uiRegCount * 4;
    NIASSERT(uiArraySize <= 16);

    // Get data to set.
    if (!GetDynamicEffectData(
        (void*) &ms_afObjectData,
        uiArraySize * sizeof(float), 
        eMapping, 
        pkDynEffect, 
        kRCC))
    {
        eResult = NISHADERERR_DYNEFFECTNOTFOUND;
    }

    // Set constant data.
    if (!pkShaderProgram->SetShaderConstant(pkEntry, &ms_afObjectData))
    {
        eResult = NISHADERERR_SETCONSTANTFAILED;
    }

    return eResult;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderConstantMap::SetupDefinedConstantValue(
    const NiRenderCallContext& kRCC,
    unsigned int uiOperatorNum,
    NiShaderConstantMapEntry* pkEntry)
{
    const NiDynamicEffectState* pkEffects = kRCC.m_pkEffects;

    const NiPropertyState* pkState = kRCC.m_pkState;

    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

    unsigned int uiInternal = pkEntry->GetInternal();
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

            ms_kMatrices[uiOperatorNum] = m_pkD3DRenderer->GetD3DProj();

            if (bInv)
            {
                D3DXMatrixInverse(&ms_kMatrices[uiOperatorNum], 0, 
                    &ms_kMatrices[uiOperatorNum]);
            }
            if (bTrans)
            {
                D3DXMatrixTranspose(&ms_kMatrices[uiOperatorNum], 
                    &ms_kMatrices[uiOperatorNum]);
            }

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
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

            ms_kMatrices[uiOperatorNum] = m_pkD3DRenderer->GetD3DView();

            if (bInv)
            {
                D3DXMatrixInverse(&ms_kMatrices[uiOperatorNum], 0, 
                    &ms_kMatrices[uiOperatorNum]);
            }
            if (bTrans)
            {
                D3DXMatrixTranspose(&ms_kMatrices[uiOperatorNum], 
                    &ms_kMatrices[uiOperatorNum]);
            }

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
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

            ms_kMatrices[uiOperatorNum] = *(m_pkD3DRenderer->GetD3DWorld());
            if (bInv)
            {
                D3DXMatrixInverse(&ms_kMatrices[uiOperatorNum], 0, 
                    &ms_kMatrices[uiOperatorNum]);
            }
            if (bTrans)
            {
                D3DXMatrixTranspose(&ms_kMatrices[uiOperatorNum], 
                    &ms_kMatrices[uiOperatorNum]);
            }

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
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

            D3DXALIGNEDMATRIX kD3DWorld = *(m_pkD3DRenderer->GetD3DWorld());
            D3DXALIGNEDMATRIX kD3DView = m_pkD3DRenderer->GetD3DView();
            ms_kMatrices[uiOperatorNum] = kD3DWorld * kD3DView;

            if (bInv)
            {
                D3DXMatrixInverse(&ms_kMatrices[uiOperatorNum], 0, 
                    &ms_kMatrices[uiOperatorNum]);
            }
            if (bTrans)
            {
                D3DXMatrixTranspose(&ms_kMatrices[uiOperatorNum], 
                    &ms_kMatrices[uiOperatorNum]);
            }

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
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

            D3DXALIGNEDMATRIX kD3DView = m_pkD3DRenderer->GetD3DView();
            D3DXALIGNEDMATRIX kD3DProj = m_pkD3DRenderer->GetD3DProj();
            ms_kMatrices[uiOperatorNum] = kD3DView * kD3DProj;

            if (bInv)
            {
                D3DXMatrixInverse(&ms_kMatrices[uiOperatorNum], 0, 
                    &ms_kMatrices[uiOperatorNum]);
            }
            if (bTrans)
            {
                D3DXMatrixTranspose(&ms_kMatrices[uiOperatorNum], 
                    &ms_kMatrices[uiOperatorNum]);
            }

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
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

            D3DXALIGNEDMATRIX kD3DWorld = *(m_pkD3DRenderer->GetD3DWorld());
            D3DXALIGNEDMATRIX kD3DView = m_pkD3DRenderer->GetD3DView();
            D3DXALIGNEDMATRIX kD3DProj = m_pkD3DRenderer->GetD3DProj();
            D3DXALIGNEDMATRIX kD3DWorldView = kD3DWorld * kD3DView;
            ms_kMatrices[uiOperatorNum] = kD3DWorldView * kD3DProj;

            if (bInv)
            {
                D3DXMatrixInverse(&ms_kMatrices[uiOperatorNum], 0, 
                    &ms_kMatrices[uiOperatorNum]);
            }
            if (bTrans)
            {
                D3DXMatrixTranspose(&ms_kMatrices[uiOperatorNum], 
                    &ms_kMatrices[uiOperatorNum]);
            }

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;

    case SCM_DEF_SKINBONE_MATRIX_3:
        // Not yet supported
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

        // Texture transforms
    case SCM_DEF_TEXTRANSFORMBASE:
    case SCM_DEF_INVTEXTRANSFORMBASE:
    case SCM_DEF_TEXTRANSFORMBASE_T:
    case SCM_DEF_INVTEXTRANSFORMBASE_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMBASE_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMBASE_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            SetupTextureTransformMatrix(ms_kMatrices[uiOperatorNum], pkMatrix, 
                bInv, bTrans);

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
    case SCM_DEF_TEXTRANSFORMDARK:
    case SCM_DEF_INVTEXTRANSFORMDARK:
    case SCM_DEF_TEXTRANSFORMDARK_T:
    case SCM_DEF_INVTEXTRANSFORMDARK_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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

                        if ((uiInternal == SCM_DEF_INVTEXTRANSFORMDARK) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMDARK_T))
                        {
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMDARK_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMDARK_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            SetupTextureTransformMatrix(ms_kMatrices[uiOperatorNum], pkMatrix, 
                bInv, bTrans);

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
    case SCM_DEF_TEXTRANSFORMDETAIL:
    case SCM_DEF_INVTEXTRANSFORMDETAIL:
    case SCM_DEF_TEXTRANSFORMDETAIL_T:
    case SCM_DEF_INVTEXTRANSFORMDETAIL_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMDETAIL_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMDETAIL_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            SetupTextureTransformMatrix(ms_kMatrices[uiOperatorNum], pkMatrix, 
                bInv, bTrans);

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
    case SCM_DEF_TEXTRANSFORMGLOSS:
    case SCM_DEF_INVTEXTRANSFORMGLOSS:
    case SCM_DEF_TEXTRANSFORMGLOSS_T:
    case SCM_DEF_INVTEXTRANSFORMGLOSS_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMGLOSS_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMGLOSS_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            SetupTextureTransformMatrix(ms_kMatrices[uiOperatorNum], pkMatrix, 
                bInv, bTrans);

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
    case SCM_DEF_TEXTRANSFORMGLOW:
    case SCM_DEF_INVTEXTRANSFORMGLOW:
    case SCM_DEF_TEXTRANSFORMGLOW_T:
    case SCM_DEF_INVTEXTRANSFORMGLOW_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMGLOW_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMGLOW_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            SetupTextureTransformMatrix(ms_kMatrices[uiOperatorNum], pkMatrix, 
                bInv, bTrans);

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
    case SCM_DEF_TEXTRANSFORMBUMP:
    case SCM_DEF_INVTEXTRANSFORMBUMP:
    case SCM_DEF_TEXTRANSFORMBUMP_T:
    case SCM_DEF_INVTEXTRANSFORMBUMP_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMBUMP_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMBUMP_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            SetupTextureTransformMatrix(ms_kMatrices[uiOperatorNum], pkMatrix, 
                bInv, bTrans);

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
    case SCM_DEF_TEXTRANSFORMDECAL:
    case SCM_DEF_INVTEXTRANSFORMDECAL:
    case SCM_DEF_TEXTRANSFORMDECAL_T:
    case SCM_DEF_INVTEXTRANSFORMDECAL_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMDECAL_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMDECAL_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            SetupTextureTransformMatrix(ms_kMatrices[uiOperatorNum], pkMatrix, 
                bInv, bTrans);

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }
        break;
    case SCM_DEF_TEXTRANSFORMSHADER:
    case SCM_DEF_INVTEXTRANSFORMSHADER:
    case SCM_DEF_TEXTRANSFORMSHADER_T:
    case SCM_DEF_INVTEXTRANSFORMSHADER_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
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
                            bInv = true;
                        }
                        if ((uiInternal == SCM_DEF_TEXTRANSFORMSHADER_T) ||
                            (uiInternal == SCM_DEF_INVTEXTRANSFORMSHADER_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            SetupTextureTransformMatrix(ms_kMatrices[uiOperatorNum], pkMatrix, 
                bInv, bTrans);

            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
        }

        // Lighting
    case SCM_DEF_LIGHT_POS_WS:
    case SCM_DEF_LIGHT_DIR_WS:
    case SCM_DEF_LIGHT_POS_OS:
    case SCM_DEF_LIGHT_DIR_OS:
        // Not yet supported
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
        // Materials
    case SCM_DEF_MATERIAL_DIFFUSE:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                ms_afVector4[uiOperatorNum][0] =
                    pkMaterial->GetDiffuseColor().r;
                ms_afVector4[uiOperatorNum][1] =
                    pkMaterial->GetDiffuseColor().g;
                ms_afVector4[uiOperatorNum][2] =
                    pkMaterial->GetDiffuseColor().b;
                ms_afVector4[uiOperatorNum][3] = pkMaterial->GetAlpha();

                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case SCM_DEF_MATERIAL_AMBIENT:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                ms_afVector4[uiOperatorNum][0] =
                    pkMaterial->GetAmbientColor().r;
                ms_afVector4[uiOperatorNum][1] =
                    pkMaterial->GetAmbientColor().g;
                ms_afVector4[uiOperatorNum][2] =
                    pkMaterial->GetAmbientColor().b;
                ms_afVector4[uiOperatorNum][3] = pkMaterial->GetAlpha();

                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case SCM_DEF_MATERIAL_SPECULAR:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                ms_afVector4[uiOperatorNum][0] =
                    pkMaterial->GetSpecularColor().r;
                ms_afVector4[uiOperatorNum][1] =
                    pkMaterial->GetSpecularColor().g;
                ms_afVector4[uiOperatorNum][2] =
                    pkMaterial->GetSpecularColor().b;
                ms_afVector4[uiOperatorNum][3] = pkMaterial->GetAlpha();

                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case SCM_DEF_MATERIAL_EMISSIVE:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                ms_afVector4[uiOperatorNum][0] = pkMaterial->GetEmittance().r;
                ms_afVector4[uiOperatorNum][1] = pkMaterial->GetEmittance().g;
                ms_afVector4[uiOperatorNum][2] = pkMaterial->GetEmittance().b;
                ms_afVector4[uiOperatorNum][3] = pkMaterial->GetAlpha();

                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case SCM_DEF_MATERIAL_POWER:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                ms_afVector4[uiOperatorNum][0] = pkMaterial->GetShineness();
                ms_afVector4[uiOperatorNum][1] = pkMaterial->GetShineness();
                ms_afVector4[uiOperatorNum][2] = pkMaterial->GetShineness();
                ms_afVector4[uiOperatorNum][3] = pkMaterial->GetShineness();

                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    // Eye
    case SCM_DEF_EYE_POS:
        {
            D3DMATRIX kViewMat = m_pkD3DRenderer->GetInvView();
            ms_afVector4[uiOperatorNum][0] = kViewMat._41;
            ms_afVector4[uiOperatorNum][1] = kViewMat._42;
            ms_afVector4[uiOperatorNum][2] = kViewMat._43;
            ms_afVector4[uiOperatorNum][3] = kViewMat._44;
        }
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        break;
    case SCM_DEF_EYE_DIR:
        {
            D3DMATRIX kViewMat = m_pkD3DRenderer->GetInvView();
            ms_afVector4[uiOperatorNum][0] = kViewMat._31;
            ms_afVector4[uiOperatorNum][1] = kViewMat._32;
            ms_afVector4[uiOperatorNum][2] = kViewMat._33;
            ms_afVector4[uiOperatorNum][3] = kViewMat._34;
        }
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;

    // Constants
    case SCM_DEF_CONSTS_TAYLOR_SIN:
    case SCM_DEF_CONSTS_TAYLOR_COS:
        // Not yet supported
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
        // Time
    case SCM_DEF_CONSTS_TIME:
    case SCM_DEF_CONSTS_SINTIME:
    case SCM_DEF_CONSTS_COSTIME:
    case SCM_DEF_CONSTS_TANTIME:
    case SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
        {
            // Grab the attribute from the geometry and set it
            NiFloatExtraData* pkFloatED = 
                (NiFloatExtraData*)pkMesh->GetExtraData(
                NiShaderConstantMap::GetTimeExtraDataName());
            if (!pkFloatED)
            {
                // Flag this error
                return NISHADERERR_ENTRYNOTFOUND;
            }

            float fTime = pkFloatED->GetValue();

            switch (uiInternal)
            {
            case SCM_DEF_CONSTS_TIME:
                {
                    ms_afVector4[uiOperatorNum][0] = fTime;
                    ms_afVector4[uiOperatorNum][1] = fTime;
                    ms_afVector4[uiOperatorNum][2] = fTime;
                    ms_afVector4[uiOperatorNum][3] = fTime;
                }
                break;
            case SCM_DEF_CONSTS_SINTIME:
                {
                    float fSin = sinf(fTime);
                    ms_afVector4[uiOperatorNum][0] = fSin;
                    ms_afVector4[uiOperatorNum][1] = fSin;
                    ms_afVector4[uiOperatorNum][2] = fSin;
                    ms_afVector4[uiOperatorNum][3] = fSin;
                }
                break;
            case SCM_DEF_CONSTS_COSTIME:
                {
                    float fCos = cosf(fTime);
                    ms_afVector4[uiOperatorNum][0] = fCos;
                    ms_afVector4[uiOperatorNum][1] = fCos;
                    ms_afVector4[uiOperatorNum][2] = fCos;
                    ms_afVector4[uiOperatorNum][3] = fCos;
                }
                break;
            case SCM_DEF_CONSTS_TANTIME:
                {
                    float fTan = tanf(fTime);
                    ms_afVector4[uiOperatorNum][0] = fTan;
                    ms_afVector4[uiOperatorNum][1] = fTan;
                    ms_afVector4[uiOperatorNum][2] = fTan;
                    ms_afVector4[uiOperatorNum][3] = fTan;
                }
                break;
            case SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
                {
                    ms_afVector4[uiOperatorNum][0] = fTime;
                    ms_afVector4[uiOperatorNum][1] = sinf(fTime);
                    ms_afVector4[uiOperatorNum][2] = cosf(fTime);
                    ms_afVector4[uiOperatorNum][3] = tanf(fTime);
                }
                break;
            default:
                NIASSERT(!"Time set --> Invalid case!");
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            }

            return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        break;
    case SCM_DEF_AMBIENTLIGHT:
        ms_afVector4[uiOperatorNum][0] = 0.0f;
        ms_afVector4[uiOperatorNum][1] = 0.0f;
        ms_afVector4[uiOperatorNum][2] = 0.0f;
        ms_afVector4[uiOperatorNum][3] = 1.0f;
        if (pkEffects)
        {
            NiDynEffectStateIter kIter = pkEffects->GetLightHeadPos();
            while (kIter)
            {
                NiAmbientLight* pkLight = NiDynamicCast(NiAmbientLight,
                    pkEffects->GetNextLight(kIter));
                if (pkLight && pkLight->GetSwitch())
                {
                    NiColor kColor = pkLight->GetAmbientColor() *
                        pkLight->GetDimmer();
                    ms_afVector4[uiOperatorNum][0] += kColor.r;
                    ms_afVector4[uiOperatorNum][1] += kColor.g;
                    ms_afVector4[uiOperatorNum][2] += kColor.b;
                }
            }
        }
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
    case SCM_DEF_FOG_DENSITY:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NIASSERT(pkFog);

            float fNear, fFar;
            m_pkD3DRenderState->GetCameraNearAndFar(fNear, fFar);
            float fDensity = 1.0f / (pkFog->GetDepth() * (fFar - fNear));

            ms_afVector4[uiOperatorNum][0] = fDensity;
            ms_afVector4[uiOperatorNum][1] = fDensity;
            ms_afVector4[uiOperatorNum][2] = fDensity;
            ms_afVector4[uiOperatorNum][3] = fDensity;

            return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        break;
    case SCM_DEF_FOG_NEARFAR:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NI_UNUSED_ARG(pkFog);
            NIASSERT(pkFog);

            float fNear, fFar;
            m_pkD3DRenderState->GetCameraNearAndFar(fNear, fFar);
            
            ms_afVector4[uiOperatorNum][0] = fNear;
            ms_afVector4[uiOperatorNum][1] = fFar;
            ms_afVector4[uiOperatorNum][2] = 0.0f;
            ms_afVector4[uiOperatorNum][3] = 0.0f;

            return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
    case SCM_DEF_FOG_COLOR:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NIASSERT(pkFog);

            ms_afVector4[uiOperatorNum][0] = pkFog->GetFogColor().r;
            ms_afVector4[uiOperatorNum][1] = pkFog->GetFogColor().g;
            ms_afVector4[uiOperatorNum][2] = pkFog->GetFogColor().b;
            ms_afVector4[uiOperatorNum][3] = 1.0f;

            return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
        }
    case SCM_DEF_ALPHA_TEST_FUNC:
        {
            NIASSERT(pkState);
            const NiAlphaProperty* pkAlphaProp = pkState->GetAlpha();
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
            ms_afVector4[uiOperatorNum][0] = kTestConditions.x;
            ms_afVector4[uiOperatorNum][1] = kTestConditions.y;
            ms_afVector4[uiOperatorNum][2] = kTestConditions.z;
            ms_afVector4[uiOperatorNum][3] = 1.0f;

            return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
    case SCM_DEF_ALPHA_TEST_REF:
        {
            NIASSERT(pkState);
            const NiAlphaProperty* pkAlphaProp = pkState->GetAlpha();
            float fRef = 0.0f;
            if (pkAlphaProp)
            {
                fRef = (float)pkAlphaProp->GetTestRef() / 255.0f;
            }
            ms_afVector4[uiOperatorNum][0] = fRef;
            ms_afVector4[uiOperatorNum][1] = fRef;
            ms_afVector4[uiOperatorNum][2] = fRef;
            ms_afVector4[uiOperatorNum][3] = fRef;

            return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
        }

    // Deprecated predefined mappings
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
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    }

    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderConstantMap::SetupConstantConstantValue(
    const NiRenderCallContext&,
    unsigned int uiOperatorNum, 
    NiShaderConstantMapEntry* pkEntry)
{
    float* pfData = (float*)pkEntry->GetDataSource();
    if (pkEntry->IsUnsignedInt())
    {
        ms_afVector4[uiOperatorNum][0] = *pfData;
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
    }
    else if (pkEntry->IsFloat())
    {
        ms_afVector4[uiOperatorNum][0] = pfData[0];
        return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
    }
    else if (pkEntry->IsPoint2())
    {
        ms_afVector4[uiOperatorNum][0] = pfData[0];
        ms_afVector4[uiOperatorNum][1] = pfData[1];
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
    }
    else if (pkEntry->IsPoint3())
    {
        ms_afVector4[uiOperatorNum][0] = pfData[0];
        ms_afVector4[uiOperatorNum][1] = pfData[1];
        ms_afVector4[uiOperatorNum][2] = pfData[2];
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
    }
    else if (pkEntry->IsPoint4())
    {
        ms_afVector4[uiOperatorNum][0] = pfData[0];
        ms_afVector4[uiOperatorNum][1] = pfData[1];
        ms_afVector4[uiOperatorNum][2] = pfData[2];
        ms_afVector4[uiOperatorNum][3] = pfData[3];
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
    }
    else if (pkEntry->IsMatrix3())
    {
        ms_kMatrices[uiOperatorNum]._11 = pfData[0];
        ms_kMatrices[uiOperatorNum]._12 = pfData[1];
        ms_kMatrices[uiOperatorNum]._13 = pfData[2];
        ms_kMatrices[uiOperatorNum]._14 = 0.0f;
        ms_kMatrices[uiOperatorNum]._21 = pfData[3];
        ms_kMatrices[uiOperatorNum]._22 = pfData[4];
        ms_kMatrices[uiOperatorNum]._23 = pfData[5];
        ms_kMatrices[uiOperatorNum]._24 = 0.0f;
        ms_kMatrices[uiOperatorNum]._31 = pfData[6];
        ms_kMatrices[uiOperatorNum]._32 = pfData[7];
        ms_kMatrices[uiOperatorNum]._33 = pfData[8];
        ms_kMatrices[uiOperatorNum]._34 = 0.0f;
        ms_kMatrices[uiOperatorNum]._41 = 0.0f;
        ms_kMatrices[uiOperatorNum]._42 = 0.0f;
        ms_kMatrices[uiOperatorNum]._43 = 0.0f;
        ms_kMatrices[uiOperatorNum]._44 = 1.0f;
        return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
    }
    else if (pkEntry->IsMatrix4())
    {
        ms_kMatrices[uiOperatorNum]._11 = pfData[ 0];
        ms_kMatrices[uiOperatorNum]._12 = pfData[ 1];
        ms_kMatrices[uiOperatorNum]._13 = pfData[ 2];
        ms_kMatrices[uiOperatorNum]._14 = pfData[ 3];
        ms_kMatrices[uiOperatorNum]._21 = pfData[ 4];
        ms_kMatrices[uiOperatorNum]._22 = pfData[ 5];
        ms_kMatrices[uiOperatorNum]._23 = pfData[ 6];
        ms_kMatrices[uiOperatorNum]._24 = pfData[ 7];
        ms_kMatrices[uiOperatorNum]._31 = pfData[ 8];
        ms_kMatrices[uiOperatorNum]._32 = pfData[ 9];
        ms_kMatrices[uiOperatorNum]._33 = pfData[10];
        ms_kMatrices[uiOperatorNum]._34 = pfData[11];
        ms_kMatrices[uiOperatorNum]._41 = pfData[12];
        ms_kMatrices[uiOperatorNum]._42 = pfData[13];
        ms_kMatrices[uiOperatorNum]._43 = pfData[14];
        ms_kMatrices[uiOperatorNum]._44 = pfData[15];
        return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
    }
    else if (pkEntry->IsColor())
    {
        ms_afVector4[uiOperatorNum][0] = pfData[0];
        ms_afVector4[uiOperatorNum][1] = pfData[1];
        ms_afVector4[uiOperatorNum][2] = pfData[2];
        ms_afVector4[uiOperatorNum][3] = pfData[3];
        return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
    }
    else if (pkEntry->IsArray())
    {
        NIASSERT(!"array of constants is not supported yet");
    }
    else
    {
        NIASSERT(!"unsupported constant type");
    }

    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderConstantMap::SetupAttributeConstantValue(
    const NiRenderCallContext& kRCC,
    unsigned int uiOperatorNum, 
    NiShaderConstantMapEntry* pkEntry)
{
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

    // Grab the attribue from the geometry and set it
    NiExtraData* pkExtra = 
        pkMesh->GetExtraData(pkEntry->GetKey());
    if (!pkExtra)
        return NISHADERERR_ENTRYNOTFOUND;

    pkEntry->GetShaderRegister();
    const void* pvData = MapAttributeValue(pkEntry, pkExtra);

    if (pkEntry->IsUnsignedInt())
    {
        ms_afVector4[uiOperatorNum][0] = (float)*((unsigned int*)pvData);
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
    }
    else if (pkEntry->IsFloat())
    {
        ms_afVector4[uiOperatorNum][0] = ((float*)pvData)[0];
        return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
    }
    else if (pkEntry->IsPoint2())
    {
        ms_afVector4[uiOperatorNum][0] = ((float*)pvData)[0];
        ms_afVector4[uiOperatorNum][1] = ((float*)pvData)[1];
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
    }
    else if (pkEntry->IsPoint3())
    {
        ms_afVector4[uiOperatorNum][0] = ((float*)pvData)[0];
        ms_afVector4[uiOperatorNum][1] = ((float*)pvData)[1];
        ms_afVector4[uiOperatorNum][2] = ((float*)pvData)[2];
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
    }
    else if (pkEntry->IsPoint4())
    {
        ms_afVector4[uiOperatorNum][0] = ((float*)pvData)[0];
        ms_afVector4[uiOperatorNum][1] = ((float*)pvData)[1];
        ms_afVector4[uiOperatorNum][2] = ((float*)pvData)[2];
        ms_afVector4[uiOperatorNum][3] = ((float*)pvData)[3];
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
    }
    else if (pkEntry->IsMatrix3())
    {
        ms_kMatrices[uiOperatorNum]._11 = ((float*)pvData)[0];
        ms_kMatrices[uiOperatorNum]._12 = ((float*)pvData)[1];
        ms_kMatrices[uiOperatorNum]._13 = ((float*)pvData)[2];
        ms_kMatrices[uiOperatorNum]._14 = 0.0f;
        ms_kMatrices[uiOperatorNum]._21 = ((float*)pvData)[3];
        ms_kMatrices[uiOperatorNum]._22 = ((float*)pvData)[4];
        ms_kMatrices[uiOperatorNum]._23 = ((float*)pvData)[5];
        ms_kMatrices[uiOperatorNum]._24 = 0.0f;
        ms_kMatrices[uiOperatorNum]._31 = ((float*)pvData)[6];
        ms_kMatrices[uiOperatorNum]._32 = ((float*)pvData)[7];
        ms_kMatrices[uiOperatorNum]._33 = ((float*)pvData)[8];
        ms_kMatrices[uiOperatorNum]._34 = 0.0f;
        ms_kMatrices[uiOperatorNum]._41 = 0.0f;
        ms_kMatrices[uiOperatorNum]._42 = 0.0f;
        ms_kMatrices[uiOperatorNum]._43 = 0.0f;
        ms_kMatrices[uiOperatorNum]._44 = 1.0f;
        return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
    }
    else if (pkEntry->IsMatrix4())
    {
        ms_kMatrices[uiOperatorNum]._11 = ((float*)pvData)[ 0];
        ms_kMatrices[uiOperatorNum]._12 = ((float*)pvData)[ 1];
        ms_kMatrices[uiOperatorNum]._13 = ((float*)pvData)[ 2];
        ms_kMatrices[uiOperatorNum]._14 = ((float*)pvData)[ 3];
        ms_kMatrices[uiOperatorNum]._21 = ((float*)pvData)[ 4];
        ms_kMatrices[uiOperatorNum]._22 = ((float*)pvData)[ 5];
        ms_kMatrices[uiOperatorNum]._23 = ((float*)pvData)[ 6];
        ms_kMatrices[uiOperatorNum]._24 = ((float*)pvData)[ 7];
        ms_kMatrices[uiOperatorNum]._31 = ((float*)pvData)[ 8];
        ms_kMatrices[uiOperatorNum]._32 = ((float*)pvData)[ 9];
        ms_kMatrices[uiOperatorNum]._33 = ((float*)pvData)[10];
        ms_kMatrices[uiOperatorNum]._34 = ((float*)pvData)[11];
        ms_kMatrices[uiOperatorNum]._41 = ((float*)pvData)[12];
        ms_kMatrices[uiOperatorNum]._42 = ((float*)pvData)[13];
        ms_kMatrices[uiOperatorNum]._43 = ((float*)pvData)[14];
        ms_kMatrices[uiOperatorNum]._44 = ((float*)pvData)[15];
        return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
    }
    else if (pkEntry->IsColor())
    {
        ms_afVector4[uiOperatorNum][0] = ((float*)pvData)[0];
        ms_afVector4[uiOperatorNum][1] = ((float*)pvData)[1];
        ms_afVector4[uiOperatorNum][2] = ((float*)pvData)[2];
        ms_afVector4[uiOperatorNum][3] = ((float*)pvData)[3];
        return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
    }
    else if (pkEntry->IsArray())
    {
        NIASSERT(!"array of constant attributes is not supported yet");
    }
    else
    {
        NIASSERT(!"unsupported constant attribute type");
    }

    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderConstantMap::SetupGlobalConstantValue(
    const NiRenderCallContext& kRCC,
    unsigned int uiOperatorNum, 
    NiShaderConstantMapEntry* pkEntry)
{
    return SetupConstantConstantValue(kRCC, uiOperatorNum, pkEntry);
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::PerformOperatorMultiply(
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry, 
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2,
    bool bInverse, 
    bool bTranspose)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        switch (eType2)
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            D3DXMatrixMultiply(&ms_kMatrixResult, &ms_kMatrices[0], 
                &ms_kMatrices[1]);
            if (bInverse)
            {
                D3DXMatrixInverse(&ms_kMatrixResult, 0, &ms_kMatrixResult);
            }
            if (bTranspose)
            {
                D3DXMatrixTranspose(&ms_kMatrixResult, &ms_kMatrixResult);
            }
            if (pkShaderProgram->SetShaderConstant(pkEntry, 
                &ms_kMatrixResult))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            ms_kMatrixResult = ms_kMatrices[0] * ms_afVector4[1][0];
            if (bInverse)
            {
                D3DXMatrixInverse(&ms_kMatrixResult, 0,  &ms_kMatrixResult);
            }
            if (bTranspose)
            {
                D3DXMatrixTranspose(&ms_kMatrixResult, &ms_kMatrixResult);
            }
            if (pkShaderProgram->SetShaderConstant(pkEntry, 
                &ms_kMatrixResult))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            {
                D3DXVECTOR4 kSource(ms_afVector4[1][0], ms_afVector4[1][1], 
                    ms_afVector4[1][2], ms_afVector4[1][3]);
                D3DXVECTOR4 kResult;
                D3DXVec4Transform(&kResult, &kSource, &ms_kMatrices[0]);

                ms_fVector4Result[0] = kResult.x;
                ms_fVector4Result[1] = kResult.y;
                ms_fVector4Result[2] = kResult.z;
                ms_fVector4Result[3] = kResult.w;
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        default:
            break;
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        switch (eType2)
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            {
                D3DXVECTOR4 kSource(ms_afVector4[0][0], ms_afVector4[0][1], 
                    ms_afVector4[0][2], ms_afVector4[0][3]);
                D3DXVECTOR4 kResult;
                D3DXVec4Transform(&kResult, &kSource, &ms_kMatrices[1]);

                ms_fVector4Result[0] = kResult.x;
                ms_fVector4Result[1] = kResult.y;
                ms_fVector4Result[2] = kResult.z;
                ms_fVector4Result[3] = kResult.w;
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] =
                    ms_afVector4[0][i] * ms_afVector4[1][0];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] =
                    ms_afVector4[0][i] * ms_afVector4[1][i];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        default:
            break;
        }
        break;
    default:
        break;
    }
    return NISHADERERR_UNKNOWN;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::PerformOperatorDivide(
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry, 
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2,
    bool bInverse,
    bool bTranspose)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        switch (eType2)
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            ms_kMatrixResult = ms_kMatrices[0] / ms_afVector4[1][0];
            if (bInverse)
            {
                D3DXMatrixInverse(&ms_kMatrixResult, 0, &ms_kMatrixResult);
            }
            if (bTranspose)
            {
                D3DXMatrixTranspose(&ms_kMatrixResult, &ms_kMatrixResult);
            }
            if (pkShaderProgram->SetShaderConstant(pkEntry,
                &ms_kMatrixResult))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        default:
            break;
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        switch (eType2)
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] = ms_afVector4[0][i] * 
                    1.0f / (ms_afVector4[1][0]);
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry,
                ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] =
                    ms_afVector4[0][i] / (ms_afVector4[1][i]);
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        default:
            break;
        }
        break;
    default:
        break;
    }
    return NISHADERERR_UNKNOWN;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::PerformOperatorAdd(
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry,
    NiShaderAttributeDesc::AttributeType eType1,
    NiShaderAttributeDesc::AttributeType eType2,
    bool,
    bool)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        switch (eType2)
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] = ms_afVector4[0][i] + 
                    ms_afVector4[1][i];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] = ms_afVector4[1][i] +
                    ms_afVector4[0][0];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        default:
            break;
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        switch (eType2)
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] = ms_afVector4[0][i] +
                    ms_afVector4[1][0];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] =
                    ms_afVector4[0][i] + ms_afVector4[1][i];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        default:
            break;
        }
        break;
    default:
        break;
    }
    return NISHADERERR_UNKNOWN;
}
//---------------------------------------------------------------------------
NiShaderError NiD3DShaderConstantMap::PerformOperatorSubtract(
    NiD3DShaderProgram* pkShaderProgram,
    NiShaderConstantMapEntry* pkEntry, 
    NiShaderAttributeDesc::AttributeType eType1,
    NiShaderAttributeDesc::AttributeType eType2,
    bool,
    bool)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        switch (eType2)
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] = ms_afVector4[0][i] -
                    ms_afVector4[1][i];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] = ms_afVector4[0][0] -
                    ms_afVector4[1][i];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        default:
            break;
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        switch (eType2)
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] = ms_afVector4[0][i] -
                    ms_afVector4[1][0];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            for (unsigned int i = 0; i < 4; ++i)
            {
                ms_fVector4Result[i] = ms_afVector4[0][i] -
                    ms_afVector4[1][i];
            }

            if (pkShaderProgram->SetShaderConstant(pkEntry, ms_fVector4Result))
            {
                return NISHADERERR_OK;
            }
            else
            {
                return NISHADERERR_SETCONSTANTFAILED;
            }
        default:
            break;
        }
        break;
    default:
        break;
    }
    return NISHADERERR_UNKNOWN;
}
//---------------------------------------------------------------------------
void NiD3DShaderConstantMap::SetupTextureTransformMatrix(
    D3DXMATRIX& kResult, 
    const NiMatrix3* pkTexMatrix,
    bool bInv,
    bool bTrans, 
    unsigned int uiTexCoordSize)
{
    if (pkTexMatrix)
    {
        kResult._11 = pkTexMatrix->GetEntry(0, 0);
        kResult._21 = pkTexMatrix->GetEntry(0, 1);
        kResult._12 = pkTexMatrix->GetEntry(1, 0);
        kResult._22 = pkTexMatrix->GetEntry(1, 1);

        if (uiTexCoordSize == 4 || uiTexCoordSize == 3)
        {
            kResult._31 = 0.0f; 
            kResult._41 = pkTexMatrix->GetEntry(0, 2); 
            kResult._32 = 0.0f; 
            kResult._42 = pkTexMatrix->GetEntry(1, 2); 
        }
        else if (uiTexCoordSize == 2)
        {
            kResult._31 = pkTexMatrix->GetEntry(0, 2); 
            kResult._41 = 0.0f; 
            kResult._32 = pkTexMatrix->GetEntry(1, 2); 
            kResult._42 = 0.0f; 
        }
        else
        {
            // If you're transforming a 1D texture coordinate, 
            // you've got bigger problems.
            NIASSERT(false);
        }

        kResult._13 = kResult._23 = 
            kResult._33 = kResult._43 = 0.0f; 
        kResult._14 = kResult._24 = 
            kResult._34 = kResult._44 = 0.0f;
        
        if (bInv)
            D3DXMatrixInverse(&kResult, 0, &kResult);
        if (bTrans)
            D3DXMatrixTranspose(&kResult, &kResult);
    }
    else
    {
        D3DXMatrixIdentity(&kResult);
    }
}
