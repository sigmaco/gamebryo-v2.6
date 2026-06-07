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
#include "NiCgShaderLibPCH.h"

#include <NiShaderConstantMapEntry.h>
#include <NiDX9RenderState.h>

#include "NiD3DCgVertexShader.h"

//---------------------------------------------------------------------------
NiD3DCgVertexShader::~NiD3DCgVertexShader()
{
    NiFree(m_pcEntryPoint);
    NiFree(m_pcShaderTarget);

    if (m_kCgProgram)
    {
        cgDestroyProgram(m_kCgProgram);
        m_kCgProgram = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3DCgVertexShader::DestroyRendererData()
{
    NiD3DVertexShader::DestroyRendererData();
}
//---------------------------------------------------------------------------
void NiD3DCgVertexShader::RecreateRendererData()
{
    NiD3DVertexShader::RecreateRendererData();
}
//---------------------------------------------------------------------------
bool NiD3DCgVertexShader::SetShaderConstant(
    NiShaderConstantMapEntry* pkEntry, const void* pvDataSource, 
    unsigned int uiRegisterCount)
{
    if (m_kCgProgram == NULL || m_pkD3DRenderState == NULL)
        return false;

    if (pvDataSource == NULL)
        pvDataSource = pkEntry->GetDataSource();

    const char* pcVariableName = pkEntry->GetVariableName();
    if (pcVariableName == NULL)
    {
        // If setting constants here, use register instead of key name?
        pcVariableName = pkEntry->GetKey();
    }

    CGparameter kVariable = cgGetNamedParameter(m_kCgProgram, pcVariableName);
    if (kVariable == NULL)
        return false;

#if CG_VERSION_NUM >= 1400
    if (!cgIsParameterUsed(kVariable, m_kCgProgram))
#else //#if CG_VERSION_NUM >= 1400
    if (!cgIsParameterReferenced(kVariable))
#endif //#if CG_VERSION_NUM >= 1400
    {
        // Parameter is not used, but this is not an error condition.
        return true;
    }

    if (uiRegisterCount == 0)
    {
        uiRegisterCount = pkEntry->GetRegisterCount();
        if (uiRegisterCount == 0)
            uiRegisterCount = 1;
    }

    unsigned int uiExpectedRegCount = (pkEntry->IsMatrix3() ? 3 : 
        (pkEntry->IsMatrix4() ? 4 : 1));

    unsigned int uiRegister = cgGetParameterResourceIndex(kVariable);
    if (uiRegisterCount == uiExpectedRegCount)
    {
#if defined(WIN32)
        // Cg does not use Bool or Int shader constants!
        float afTemp[4];
        if (pkEntry->IsBool())
        {
            afTemp[0] = (*(BOOL*)pvDataSource ? 1.0f : 0.0f);
            afTemp[1] = afTemp[0];
            afTemp[2] = afTemp[0];
            afTemp[3] = afTemp[0];

            pvDataSource = afTemp;
        }
        else if (pkEntry->IsUnsignedInt())
        {
            afTemp[0] = (float)((int*)pvDataSource)[0];
            afTemp[1] = (float)((int*)pvDataSource)[1];
            afTemp[2] = (float)((int*)pvDataSource)[1];
            afTemp[3] = (float)((int*)pvDataSource)[1];

            pvDataSource = afTemp;
        }
#endif  // #if defined(WIN32)

        return m_pkD3DRenderState->SetVertexShaderConstantF(
            uiRegister, (const float*)pvDataSource, 
            uiRegisterCount);
    }
    else
    {
#if defined(WIN32)
        // Cg does not use Bool or Int shader constants!
        float afTemp[4];
        if (pkEntry->IsBool())
        {
            NIASSERT(uiExpectedRegCount == 1);

            afTemp[0] = (*(BOOL*)pvDataSource ? 1.0f : 0.0f);
            afTemp[1] = afTemp[0];
            afTemp[2] = afTemp[0];
            afTemp[3] = afTemp[0];

            pvDataSource = afTemp;
        }
        else if (pkEntry->IsUnsignedInt())
        {
            NIASSERT(uiExpectedRegCount == 1);

            afTemp[0] = (float)((int*)pvDataSource)[0];
            afTemp[1] = (float)((int*)pvDataSource)[1];
            afTemp[2] = (float)((int*)pvDataSource)[1];
            afTemp[3] = (float)((int*)pvDataSource)[1];

            pvDataSource = afTemp;
        }
#endif //#if defined(WIN32)

        for (unsigned int i = uiRegister; 
            i < uiRegisterCount; i += uiExpectedRegCount)
        {
            bool bSuccess = m_pkD3DRenderState->SetVertexShaderConstantF(i, 
                (const float*)pvDataSource, uiExpectedRegCount);

            if (bSuccess == false)
                return false;
        }
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiD3DCgVertexShader::SetShaderConstantArray(
    NiShaderConstantMapEntry* pkEntry, const void* pvDataSource, 
    unsigned int uiNumEntries, unsigned int uiRegistersPerEntry,
    unsigned short* pusReorderArray)
{
    if (m_kCgProgram == NULL || m_pkD3DRenderState == NULL)
        return false;

    if (pvDataSource == NULL)
        pvDataSource = pkEntry->GetDataSource();

    const char* pcVariableName = pkEntry->GetVariableName();
    if (pcVariableName == NULL)
    {
        // If setting constants here, use register instead of key name?
        pcVariableName = pkEntry->GetKey();
    }

    CGparameter kVariable = cgGetNamedParameter(m_kCgProgram, pcVariableName);
    if (kVariable == NULL)
        return false;

#if CG_VERSION_NUM >= 1400
    if (!cgIsParameterUsed(kVariable, m_kCgProgram))
#else //#if CG_VERSION_NUM >= 1400
    if (!cgIsParameterReferenced(kVariable))
#endif //#if CG_VERSION_NUM >= 1400
    {
        // Parameter is not used, but this is not an error condition.
        return true;
    }

    unsigned int uiRegister = cgGetParameterResourceIndex(kVariable);

    for (unsigned int i = 0; i < uiNumEntries; i++)
    {
        unsigned short usNewIndex = (pusReorderArray == NULL) ?
            (unsigned short)(i) : pusReorderArray[i];
        bool bSuccess = m_pkD3DRenderState->SetVertexShaderConstantF(
            uiRegister, ((const float*)pvDataSource) + 
            (usNewIndex * uiRegistersPerEntry * 4), uiRegistersPerEntry);
        if (bSuccess == false)
            return false;
        uiRegister += uiRegistersPerEntry;
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3DCgVertexShader::GetVariableCount()
{
    if (!m_kCgProgram)
        return 0;

    unsigned int uiCurIndex = 0;
    CGparameter kParam = cgGetFirstParameter( m_kCgProgram, CG_GLOBAL  );
#ifdef NIDEBUG
        CGerror eError = cgGetError();
        NIASSERT(eError == CG_NO_ERROR);
#endif
    while (kParam)
    {
        CGenum eVariability = cgGetParameterVariability(kParam);
        if (eVariability == CG_UNIFORM)
        {
            uiCurIndex++;
        }

        kParam = cgGetNextParameter(kParam);

#ifdef NIDEBUG
        CGerror eError = cgGetError();
        NIASSERT(eError == CG_NO_ERROR);
#endif
    }
    return uiCurIndex;
}
//---------------------------------------------------------------------------
const char* NiD3DCgVertexShader::GetVariableName(unsigned int uiIndex)
{
    NIASSERT(uiIndex < GetVariableCount());
    int iCurIndex = -1;
#ifdef NIDEBUG
        CGerror eError = cgGetError();
        NIASSERT(eError == CG_NO_ERROR);
#endif
    CGparameter kParam = cgGetFirstParameter( m_kCgProgram, CG_GLOBAL  );
    while (kParam)
    {
        CGenum eVariability = cgGetParameterVariability(kParam);

        if (eVariability == CG_UNIFORM)
        {
            iCurIndex++;
        }

        if (iCurIndex == (int)uiIndex)
            return cgGetParameterName(kParam);

        kParam = cgGetNextParameter(kParam);
#ifdef NIDEBUG
        CGerror eError = cgGetError();
        NIASSERT(eError == CG_NO_ERROR);
#endif
    }

    return NULL;
}
//---------------------------------------------------------------------------
