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

#include "NiD3DHLSLPixelShader.h"
#include "NiShaderConstantMapEntry.h"

NiImplementRTTI(NiD3DHLSLPixelShader, NiD3DPixelShader);

//---------------------------------------------------------------------------
NiD3DHLSLPixelShader::~NiD3DHLSLPixelShader()
{
    NiFree(m_pcEntryPoint);
    NiFree(m_pcShaderTarget);

    if (m_pkConstantTable)
    {
        m_pkConstantTable->Release();
        m_pkConstantTable = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3DHLSLPixelShader::DestroyRendererData()
{
    if (m_pkConstantTable)
    {
        m_pkConstantTable->Release();
        m_pkConstantTable = NULL;
    }

    NiD3DPixelShader::DestroyRendererData();
}
//---------------------------------------------------------------------------
void NiD3DHLSLPixelShader::RecreateRendererData()
{
    // Constant table will be replaced by NiD3DShaderProgramCreatorHLSL.
    NiD3DPixelShader::RecreateRendererData();
}
//---------------------------------------------------------------------------
bool NiD3DHLSLPixelShader::SetShaderConstant(
    NiShaderConstantMapEntry* pkEntry, const void* pvDataSource, 
    unsigned int)
{
    if (m_pkConstantTable == NULL || m_pkD3DDevice == NULL)
        return false;

    if (pvDataSource == NULL)
        pvDataSource = pkEntry->GetDataSource();

    if (false == pkEntry->GetVariableHookupValid())
    {
        NiFixedString kVariableName = pkEntry->GetVariableName();
        if (!kVariableName.Exists())
        {
            // If setting constants here, use register instead of key name
            kVariableName = pkEntry->GetKey();
            pkEntry->SetVariableName(kVariableName);
        }

        D3DXHANDLE pkVariable = m_pkConstantTable->GetConstantByName(NULL,
            kVariableName);

        if (pkVariable)
        {
            D3DXCONSTANT_DESC kDesc;
            unsigned int uiCount = 1;
            HRESULT hr = 
                m_pkConstantTable->GetConstantDesc(
                pkVariable, &kDesc, &uiCount);
            NI_UNUSED_ARG(hr);
            NIASSERT(SUCCEEDED(hr) && uiCount == 1);

            pkEntry->SetShaderRegister(kDesc.RegisterIndex);
            pkEntry->SetRegisterCount(kDesc.RegisterCount);
            pkEntry->SetRegisterSet(kDesc.RegisterSet);
            pkEntry->SetVariableHookupValid(true);
        }
        else
        {
            NiD3DRenderer::Warning(
                "NiD3DHLSLPixelShader::SetShaderConstant"
                " - %s - variable %s not found\n", m_pszName, kVariableName);
            return false;
        }
    }

    bool bReturn = false;

    if (pkEntry->IsMatrix3() || pkEntry->IsMatrix4())
    {
#if defined(NIDEBUG)
        D3DXHANDLE pkVariable = m_pkConstantTable->GetConstantByName(NULL,
            pkEntry->GetVariableName());
        D3DXCONSTANT_DESC kDesc;
        unsigned int uiCount = 1;
        m_pkConstantTable->GetConstantDesc(
            pkVariable, &kDesc, &uiCount);
        NIASSERT(
            kDesc.RegisterSet == D3DXRS_FLOAT4 &&
            kDesc.Class == D3DXPC_MATRIX_COLUMNS &&
            kDesc.Type == D3DXPT_FLOAT);
#endif //#if defined(NIDEBUG)

        D3DXALIGNEDMATRIX kMat;
        D3DXMatrixTranspose(&kMat, (CONST D3DXMATRIX*)pvDataSource);
        if (m_pkD3DRenderState->SetPixelShaderConstantF(
            pkEntry->GetShaderRegister(), (const float*)&kMat, 
            pkEntry->GetRegisterCount()))
        {
            bReturn = true;
        }
    }
    else if (pkEntry->IsPoint2() ||
        pkEntry->IsPoint3() ||
        pkEntry->IsPoint4() ||
        pkEntry->IsColor())
    {
#if defined(NIDEBUG)
        D3DXHANDLE pkVariable = m_pkConstantTable->GetConstantByName(NULL,
            pkEntry->GetVariableName());
        D3DXCONSTANT_DESC kDesc;
        unsigned int uiCount = 1;
        m_pkConstantTable->GetConstantDesc(
            pkVariable, &kDesc, &uiCount);
        // Vector or scalar can set a float, though only 1 value will apply
        NIASSERT(kDesc.RegisterSet == D3DXRS_FLOAT4 &&
            (kDesc.Class == D3DXPC_VECTOR ||
            kDesc.Class == D3DXPC_SCALAR) &&
            kDesc.Type == D3DXPT_FLOAT);
#endif //#if defined(NIDEBUG)

        if (m_pkD3DRenderState->SetPixelShaderConstantF(
            pkEntry->GetShaderRegister(), (const float*)pvDataSource, 
            pkEntry->GetRegisterCount()))
        {
            bReturn = true;
        }
    }
    else if (pkEntry->IsFloat())
    {
#if defined(NIDEBUG)
        D3DXHANDLE pkVariable = m_pkConstantTable->GetConstantByName(NULL,
            pkEntry->GetVariableName());
        D3DXCONSTANT_DESC kDesc;
        unsigned int uiCount = 1;
        m_pkConstantTable->GetConstantDesc(
            pkVariable, &kDesc, &uiCount);
        NIASSERT(kDesc.RegisterSet == D3DXRS_FLOAT4 &&
            kDesc.Class == D3DXPC_SCALAR &&
            kDesc.Type == D3DXPT_FLOAT);
#endif //#if defined(NIDEBUG)

        if (m_pkD3DRenderState->SetPixelShaderConstantF(
            pkEntry->GetShaderRegister(), (const float*)pvDataSource, 
            pkEntry->GetRegisterCount()))
        {
            bReturn = true;
        }
    }
    else if (pkEntry->IsUnsignedInt())
    {
#if defined(NIDEBUG)
        D3DXHANDLE pkVariable = m_pkConstantTable->GetConstantByName(NULL,
            pkEntry->GetVariableName());
        D3DXCONSTANT_DESC kDesc;
        unsigned int uiCount = 1;
        m_pkConstantTable->GetConstantDesc(
            pkVariable, &kDesc, &uiCount);

        NIASSERT(kDesc.RegisterSet == (int)pkEntry->GetRegisterSet() &&
            kDesc.Class == D3DXPC_SCALAR &&
            kDesc.Type == D3DXPT_INT &&
            uiCount == 1);
#endif //#if defined(NIDEBUG)

        if (pkEntry->GetRegisterSet() == D3DXRS_FLOAT4)
        {
            float fValue = (float)*((const int*)pvDataSource);
            return m_pkD3DRenderState->SetPixelShaderConstantF(
                pkEntry->GetShaderRegister(), &fValue, 
                pkEntry->GetRegisterCount());
        }
        if (pkEntry->GetRegisterSet() == D3DXRS_INT4)
        {
            return m_pkD3DRenderState->SetPixelShaderConstantI(
                pkEntry->GetShaderRegister(), (const int*)pvDataSource, 
                pkEntry->GetRegisterCount());
        }
    }
    else if (pkEntry->IsBool())
    {
#if defined(NIDEBUG)
        D3DXHANDLE pkVariable = m_pkConstantTable->GetConstantByName(NULL,
            pkEntry->GetVariableName());
        D3DXCONSTANT_DESC kDesc;
        unsigned int uiCount = 1;
        m_pkConstantTable->GetConstantDesc(
            pkVariable, &kDesc, &uiCount);

        NIASSERT(kDesc.RegisterSet == (int)pkEntry->GetRegisterSet() &&
            kDesc.Class == D3DXPC_SCALAR &&
            kDesc.Type == D3DXPT_BOOL &&
            uiCount == 1);
#endif //#if defined(NIDEBUG)

        if (pkEntry->GetRegisterSet() == D3DXRS_FLOAT4)
        {
            float fValue = (float)*((const BOOL*)pvDataSource);
            return m_pkD3DRenderState->SetPixelShaderConstantF(
                pkEntry->GetShaderRegister(), &fValue, 
                pkEntry->GetRegisterCount());
        }
        else if (pkEntry->GetRegisterSet() == D3DXRS_BOOL)
        {
            return m_pkD3DRenderState->SetPixelShaderConstantB(
                pkEntry->GetShaderRegister(), (const BOOL*)pvDataSource, 
                pkEntry->GetRegisterCount());
        }
    }
    else
    {
        NIASSERT(pkEntry->IsString() || pkEntry->IsTexture());
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiD3DHLSLPixelShader::SetShaderConstantArray(
    NiShaderConstantMapEntry* pkEntry,
    const void* pvDataSource,
    unsigned int uiNumEntries,
    unsigned int uiRegistersPerEntry,
    const unsigned short* pusReorderArray)
{
    if (m_pkConstantTable == NULL || m_pkD3DDevice == NULL)
        return false;

    if (pvDataSource == NULL)
        pvDataSource = pkEntry->GetDataSource();

    D3DXHANDLE pkVariable;
    D3DXCONSTANT_DESC kDesc;
    if (false == pkEntry->GetVariableHookupValid())
    {
        NiFixedString kVariableName = pkEntry->GetVariableName();
        if (!kVariableName.Exists())
        {
            // If setting constants here, use register instead of key name
            kVariableName = pkEntry->GetKey();
            pkEntry->SetVariableName(kVariableName);
        }

        pkVariable = m_pkConstantTable->GetConstantByName(NULL, kVariableName);
        if (pkVariable)
        {
            unsigned int uiCount = 1;
            HRESULT hr = 
                m_pkConstantTable->GetConstantDesc(pkVariable, 
                &kDesc, &uiCount);
            NI_UNUSED_ARG(hr);
            NIASSERT(SUCCEEDED(hr) && uiCount == 1);

            NIASSERT(uiNumEntries <= kDesc.Elements);
            NIASSERT(kDesc.RegisterSet == D3DXRS_FLOAT4 &&
                kDesc.Type == D3DXPT_FLOAT);

            pkEntry->SetShaderRegister(kDesc.RegisterIndex);
            pkEntry->SetRegisterCount(kDesc.RegisterCount);
            pkEntry->SetRegisterSet(kDesc.RegisterSet);
            pkEntry->SetVariableHookupValid(true);

            NIASSERT(kDesc.Elements * uiRegistersPerEntry ==
                kDesc.RegisterCount);
        }
        else
        {
            NiD3DRenderer::Warning(
                "NiD3DHLSLPixelShader::SetShaderConstantArray"
                " - %s - variable %s not found\n", m_pszName, kVariableName);
            return false;
        }
    }
    else
    {
        pkVariable = m_pkConstantTable->GetConstantByName(NULL,
            pkEntry->GetVariableName());
        NIASSERT(pkVariable);
        
        unsigned int uiCount = 1;
        HRESULT hr = 
            m_pkConstantTable->GetConstantDesc(
            pkVariable, &kDesc, &uiCount);
        NI_UNUSED_ARG(hr);
        NIASSERT(SUCCEEDED(hr) && uiCount == 1);
        NIASSERT(kDesc.RegisterSet == D3DXRS_FLOAT4 &&
            kDesc.Type == D3DXPT_FLOAT);
    }

    // adjust number of entries based on register count
    //  This number may differ because the shader program may not use all
    //  array entries and thus not allocate registers for the entire array.
    uiNumEntries = pkEntry->GetRegisterCount()/uiRegistersPerEntry;

    bool bNeedsTranspose = (kDesc.Class == D3DXPC_MATRIX_COLUMNS);

    if (pusReorderArray)
    {
        unsigned int uiBaseRegister = pkEntry->GetShaderRegister();
        NIASSERT(uiBaseRegister + uiRegistersPerEntry <= 
            pkEntry->GetShaderRegister() +  pkEntry->GetRegisterCount());

        for (unsigned int i = 0; i < uiNumEntries; i++)
        {
            NIASSERT(uiBaseRegister + uiRegistersPerEntry <= 
                pkEntry->GetShaderRegister() + pkEntry->GetRegisterCount());
            unsigned short usNewIndex = pusReorderArray[i];

#if defined(NIDEBUG)
            D3DXHANDLE pkEntryHandle = m_pkConstantTable->GetConstantElement(
                pkVariable, i);
            NIASSERT(pkEntryHandle);

            unsigned int uiCount = 1;
            HRESULT hr = m_pkConstantTable->GetConstantDesc(
                pkEntryHandle, &kDesc, &uiCount);
            NIASSERT(SUCCEEDED(hr) && uiCount == 1);

            NIASSERT(
                kDesc.RegisterIndex == uiBaseRegister &&
                kDesc.RegisterCount == uiRegistersPerEntry);
#endif //#if defined(NIDEBUG)

            D3DXALIGNEDMATRIX kMat;
            const float* pfEntryData = ((const float*)pvDataSource) + 
                (usNewIndex * uiRegistersPerEntry * 4);
            if (bNeedsTranspose)
            {
                D3DXMatrixTranspose(&kMat, (CONST D3DXMATRIX*)(pfEntryData));
                pfEntryData = (const float*)&kMat;
            }

            if (!m_pkD3DRenderState->SetPixelShaderConstantF(uiBaseRegister, 
                pfEntryData, uiRegistersPerEntry))
            {
                return false;
            }

            uiBaseRegister += uiRegistersPerEntry;
        }
    }
    else
    {
        // set registers
        unsigned int uiBaseRegister = pkEntry->GetShaderRegister();
        NIASSERT(uiBaseRegister + uiRegistersPerEntry <= 
            pkEntry->GetShaderRegister() +  pkEntry->GetRegisterCount());

        const float* pfDataSourceIterator = (const float*)pvDataSource;
        for (unsigned int i = 0; i < uiNumEntries; i++)
        {
#if defined(NIDEBUG)
            D3DXHANDLE pkEntryHandle = 
                m_pkConstantTable->GetConstantElement(pkVariable, i);
            NIASSERT(pkEntryHandle);

            unsigned int uiCount = 1;
            HRESULT hr = m_pkConstantTable->GetConstantDesc(
                pkEntryHandle, &kDesc, &uiCount);
            NIASSERT(SUCCEEDED(hr) && uiCount == 1);

            NIASSERT(kDesc.RegisterIndex == uiBaseRegister &&
                kDesc.RegisterCount == uiRegistersPerEntry);
#endif //#if defined(NIDEBUG)

            D3DXALIGNEDMATRIX kMat;
            const float* pfEntryData = pfDataSourceIterator;
            if (bNeedsTranspose)
            {
                D3DXMatrixTranspose(&kMat, (CONST D3DXMATRIX*)(pfEntryData));
                pfEntryData = (const float*)&kMat;
            }

            if (!m_pkD3DRenderState->SetPixelShaderConstantF(
                uiBaseRegister, pfEntryData, uiRegistersPerEntry))
            {
                return false;
            }

            uiBaseRegister += uiRegistersPerEntry;
            pfDataSourceIterator += uiRegistersPerEntry * 4;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3DHLSLPixelShader::GetVariableCount()
{
    if (!m_pkConstantTable)
        return 0;

    D3DXCONSTANTTABLE_DESC kDesc;
    if (SUCCEEDED(m_pkConstantTable->GetDesc(&kDesc)))
        return kDesc.Constants;
    else
        return 0;
}
//---------------------------------------------------------------------------
const char* NiD3DHLSLPixelShader::GetVariableName(unsigned int uiIndex)
{
    NIASSERT(uiIndex < GetVariableCount());
    D3DXHANDLE hConstant = m_pkConstantTable->GetConstant(NULL, uiIndex);

    if (hConstant == NULL)
        return NULL;

    D3DXCONSTANT_DESC kDesc;
    unsigned int uiCount = 1;
    if (FAILED(m_pkConstantTable->GetConstantDesc(hConstant, &kDesc,
        &uiCount)) || uiCount == 0)
    {
        return NULL;
    }

    return kDesc.Name;
}
//---------------------------------------------------------------------------
