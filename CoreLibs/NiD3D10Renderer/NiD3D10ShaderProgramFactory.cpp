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

#include "NiD3D10GeometryShader.h"
#include "NiD3D10PixelShader.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10ShaderProgramCreator.h"
#include "NiD3D10ShaderProgramFactory.h"
#include "NiD3D10ShaderFactory.h"
#include "NiD3D10VertexShader.h"

#include <NiFilename.h>

NiD3D10ShaderProgramFactory* NiD3D10ShaderProgramFactory::ms_pkFactory = NULL;

//---------------------------------------------------------------------------
NiD3D10ShaderProgramFactory::NiD3D10ShaderProgramFactory() :
    m_kVertexShaderMap(59),
    m_kGeometryShaderMap(59),
    m_kPixelShaderMap(59),
    m_kShaderCreators(13)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10ShaderProgramFactory::~NiD3D10ShaderProgramFactory()
{
    m_kVertexShaderMap.RemoveAll();
    m_kGeometryShaderMap.RemoveAll();
    m_kPixelShaderMap.RemoveAll();

    m_kShaderCreators.RemoveAll();

    RemoveAllProgramDirectories();
}
//---------------------------------------------------------------------------
NiD3D10ShaderProgramFactory* NiD3D10ShaderProgramFactory::GetInstance()
{
    return ms_pkFactory;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreateVertexShaderFromFile(
    const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
    ID3D10Include* pkInclude, const char* pcEntryPoint, 
    const char* pcShaderTarget, unsigned int uiFlags, 
    const char* pcShaderName, NiD3D10VertexShaderPtr& spVertexShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pcFileName == NULL || pcFileName[0] == '\0')
        return NULL;
    
    // See if it exists already
    NiD3D10VertexShader* pkVertexShader = 
        ms_pkFactory->GetVertexShader(pcShaderName);
    if (pkVertexShader)
    {
        // Already loaded... Return it
        NIASSERT(pkVertexShader->GetVertexShader() != NULL);
        spVertexShader = pkVertexShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    ID3D10Blob* pkShaderCode = ReadShaderFile(pcFileName);
    if (pkShaderCode == NULL)
    {

        // Can't find shader file
        return NULL;
    }

    bool bSuccess = pkCreator->CompileAndCreateVertexShader(pkShaderCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, spVertexShader);

    NIASSERT(bSuccess == false || spVertexShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertVertexShaderIntoMap(spVertexShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreateVertexShaderFromBlob(
    ID3D10Blob* pkShaderCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    const char* pcEntryPoint, const char* pcShaderTarget, 
    unsigned int uiFlags, const char* pcShaderName,
    NiD3D10VertexShaderPtr& spVertexShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pkShaderCode == 0)
        return NULL;

    // See if it exists already
    NiD3D10VertexShader* pkVertexShader = 
        ms_pkFactory->GetVertexShader(pcShaderName);
    if (pkVertexShader)
    {
        // Already loaded... Return it
        NIASSERT(pkVertexShader->GetVertexShader() != NULL);
        spVertexShader = pkVertexShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    bool bSuccess = pkCreator->CompileAndCreateVertexShader(pkShaderCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, spVertexShader);

    NIASSERT(bSuccess == false || spVertexShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertVertexShaderIntoMap(spVertexShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreateVertexShaderFromCompiledBlob(
    ID3D10Blob* pkShaderByteCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    const char* pcEntryPoint, const char* pcShaderTarget, 
    unsigned int uiFlags, const char* pcShaderName,
    NiD3D10VertexShaderPtr& spVertexShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pkShaderByteCode == 0)
        return NULL;

    // See if it exists already
    NiD3D10VertexShader* pkVertexShader = 
        ms_pkFactory->GetVertexShader(pcShaderName);
    if (pkVertexShader)
    {
        // Already loaded... Return it
        NIASSERT(pkVertexShader->GetVertexShader() != NULL);
        spVertexShader = pkVertexShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    bool bSuccess = pkCreator->CreateVertexShader(pkShaderByteCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, spVertexShader);

    NIASSERT(bSuccess == false || spVertexShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertVertexShaderIntoMap(spVertexShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreateGeometryShaderFromFile(
    const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
    ID3D10Include* pkInclude, const char* pcEntryPoint, 
    const char* pcShaderTarget, unsigned int uiFlags, 
    const char* pcShaderName, NiD3D10GeometryShaderPtr& spGeometryShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pcFileName == NULL || pcFileName[0] == '\0')
        return NULL;

    // See if it exists already
    NiD3D10GeometryShader* pkGeometryShader = 
        ms_pkFactory->GetGeometryShader(pcShaderName);
    if (pkGeometryShader)
    {
        // Already loaded... Return it
        NIASSERT(pkGeometryShader->GetGeometryShader() != NULL);
        spGeometryShader = pkGeometryShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    ID3D10Blob* pkShaderCode = ReadShaderFile(pcFileName);
    if (pkShaderCode == NULL)
    {

        // Can't find shader file
        return NULL;
    }

    bool bSuccess = pkCreator->CompileAndCreateGeometryShader(pkShaderCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, NULL, 0, 0, spGeometryShader);

    NIASSERT(bSuccess == false || spGeometryShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertGeometryShaderIntoMap(spGeometryShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreateGeometryShaderFromBlob(
    ID3D10Blob* pkShaderCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    const char* pcEntryPoint, const char* pcShaderTarget, 
    unsigned int uiFlags, const char* pcShaderName,
    NiD3D10GeometryShaderPtr& spGeometryShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pkShaderCode == NULL)
        return NULL;

    // See if it exists already
    NiD3D10GeometryShader* pkGeometryShader = 
        ms_pkFactory->GetGeometryShader(pcShaderName);
    if (pkGeometryShader)
    {
        // Already loaded... Return it
        NIASSERT(pkGeometryShader->GetGeometryShader() != NULL);
        spGeometryShader = pkGeometryShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    bool bSuccess = pkCreator->CompileAndCreateGeometryShader(pkShaderCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, NULL, 0, 0, spGeometryShader);

    NIASSERT(bSuccess == false || spGeometryShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertGeometryShaderIntoMap(spGeometryShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreateGeometryShaderFromCompiledBlob(
    ID3D10Blob* pkShaderByteCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    const char* pcEntryPoint, const char* pcShaderTarget, 
    unsigned int uiFlags, const char* pcShaderName,
    NiD3D10GeometryShaderPtr& spGeometryShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pkShaderByteCode == NULL)
        return NULL;

    // See if it exists already
    NiD3D10GeometryShader* pkGeometryShader = 
        ms_pkFactory->GetGeometryShader(pcShaderName);
    if (pkGeometryShader)
    {
        // Already loaded... Return it
        NIASSERT(pkGeometryShader->GetGeometryShader() != NULL);
        spGeometryShader = pkGeometryShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    bool bSuccess = pkCreator->CreateGeometryShader(pkShaderByteCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, NULL, 0, 0, spGeometryShader);

    NIASSERT(bSuccess == false || spGeometryShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertGeometryShaderIntoMap(spGeometryShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreateGeometryShaderWithStreamOutputFromFile(
    const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
    ID3D10Include* pkInclude, const char* pcEntryPoint, 
    const char* pcShaderTarget, unsigned int uiFlags, 
    const char* pcShaderName, 
    const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
    unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
    NiD3D10GeometryShaderPtr& spGeometryShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pcFileName == NULL || pcFileName[0] == '\0')
        return NULL;

    // See if it exists already
    NiD3D10GeometryShader* pkGeometryShader = 
        ms_pkFactory->GetGeometryShader(pcShaderName);
    if (pkGeometryShader)
    {
        // Already loaded... Return it
        NIASSERT(pkGeometryShader->GetGeometryShader() != NULL);
        spGeometryShader = pkGeometryShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    ID3D10Blob* pkShaderCode = ReadShaderFile(pcFileName);
    if (pkShaderCode == NULL)
    {

        // Can't find shader file
        return NULL;
    }

    bool bSuccess = pkCreator->CompileAndCreateGeometryShader(pkShaderCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, 
        pcShaderTarget, uiFlags, pcShaderName, pkSODeclaration, 
        uiNumEntries, uiOutputStreamStride, spGeometryShader);

    NIASSERT(bSuccess == false || spGeometryShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertGeometryShaderIntoMap(spGeometryShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreateGeometryShaderWithStreamOutputFromBlob(
    ID3D10Blob* pkShaderCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
    const char* pcEntryPoint, const char* pcShaderTarget, unsigned int uiFlags,
    const char* pcShaderName, 
    const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
    unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
    NiD3D10GeometryShaderPtr& spGeometryShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pkShaderCode == NULL)
        return NULL;

    // See if it exists already
    NiD3D10GeometryShader* pkGeometryShader = 
        ms_pkFactory->GetGeometryShader(pcShaderName);
    if (pkGeometryShader)
    {
        // Already loaded... Return it
        NIASSERT(pkGeometryShader->GetGeometryShader() != NULL);
        spGeometryShader = pkGeometryShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    bool bSuccess = pkCreator->CompileAndCreateGeometryShader(pkShaderCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, 
        pcShaderTarget, uiFlags, pcShaderName, pkSODeclaration, 
        uiNumEntries, uiOutputStreamStride, spGeometryShader);

    NIASSERT(bSuccess == false || spGeometryShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertGeometryShaderIntoMap(spGeometryShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::
    CreateGeometryShaderWithStreamOutputFromCompiledBlob(
    ID3D10Blob* pkShaderByteCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
    const char* pcEntryPoint, const char* pcShaderTarget, unsigned int uiFlags,
    const char* pcShaderName, 
    const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
    unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
    NiD3D10GeometryShaderPtr& spGeometryShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pkShaderByteCode == NULL)
        return NULL;

    // See if it exists already
    NiD3D10GeometryShader* pkGeometryShader = 
        ms_pkFactory->GetGeometryShader(pcShaderName);
    if (pkGeometryShader)
    {
        // Already loaded... Return it
        NIASSERT(pkGeometryShader->GetGeometryShader() != NULL);
        spGeometryShader = pkGeometryShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    bool bSuccess = pkCreator->CreateGeometryShader(pkShaderByteCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, 
        pcShaderTarget, uiFlags, pcShaderName, pkSODeclaration, 
        uiNumEntries, uiOutputStreamStride, spGeometryShader);

    NIASSERT(bSuccess == false || spGeometryShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertGeometryShaderIntoMap(spGeometryShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreatePixelShaderFromFile(
    const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
    ID3D10Include* pkInclude, const char* pcEntryPoint, 
    const char* pcShaderTarget, unsigned int uiFlags, 
    const char* pcShaderName, NiD3D10PixelShaderPtr& spPixelShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pcFileName == NULL || pcFileName[0] == '\0')
        return NULL;

    // See if it exists already
    NiD3D10PixelShader* pkPixelShader = 
        ms_pkFactory->GetPixelShader(pcShaderName);
    if (pkPixelShader)
    {
        // Already loaded... Return it
        NIASSERT(pkPixelShader->GetPixelShader() != NULL);
        spPixelShader = pkPixelShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    ID3D10Blob* pkShaderCode = ReadShaderFile(pcFileName);
    if (pkShaderCode == NULL)
    {

        // Can't find shader file
        return NULL;
    }

    bool bSuccess = pkCreator->CompileAndCreatePixelShader(pkShaderCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, spPixelShader);

    NIASSERT(bSuccess == false || spPixelShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertPixelShaderIntoMap(spPixelShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreatePixelShaderFromBlob(
    ID3D10Blob* pkShaderCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    const char* pcEntryPoint, const char* pcShaderTarget, 
    unsigned int uiFlags, const char* pcShaderName,
    NiD3D10PixelShaderPtr& spPixelShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pkShaderCode == 0)
        return NULL;

    // See if it exists already
    NiD3D10PixelShader* pkPixelShader = 
        ms_pkFactory->GetPixelShader(pcShaderName);
    if (pkPixelShader)
    {
        // Already loaded... Return it
        NIASSERT(pkPixelShader->GetPixelShader() != NULL);
        spPixelShader = pkPixelShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    bool bSuccess = pkCreator->CompileAndCreatePixelShader(pkShaderCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, spPixelShader);

    NIASSERT(bSuccess == false || spPixelShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertPixelShaderIntoMap(spPixelShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::CreatePixelShaderFromCompiledBlob(
    ID3D10Blob* pkShaderByteCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    const char* pcEntryPoint, const char* pcShaderTarget, 
    unsigned int uiFlags, const char* pcShaderName,
    NiD3D10PixelShaderPtr& spPixelShader)
{
    if (ms_pkFactory == NULL)
        return NULL;

    if (pkShaderByteCode == 0)
        return NULL;

    // See if it exists already
    NiD3D10PixelShader* pkPixelShader = 
        ms_pkFactory->GetPixelShader(pcShaderName);
    if (pkPixelShader)
    {
        // Already loaded... Return it
        NIASSERT(pkPixelShader->GetPixelShader() != NULL);
        spPixelShader = pkPixelShader;
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    bool bSuccess = pkCreator->CreatePixelShader(pkShaderByteCode, 
        pcFileName, pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, 
        uiFlags, pcShaderName, spPixelShader);

    NIASSERT(bSuccess == false || spPixelShader != NULL);

    // Insert it in the list
    if (bSuccess)
        ms_pkFactory->InsertPixelShaderIntoMap(spPixelShader);

    return bSuccess;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10ShaderProgramFactory::GetUniversalShaderCreationFlags()
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    return pkRenderer->GetGlobalShaderCreationFlags();
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::SetUniversalShaderCreationFlags(
    unsigned int uiFlags)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->SetGlobalShaderCreationFlags(uiFlags);
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::RecreateVertexShader(
    NiD3D10VertexShader* pkVertexShader)
{
    if (!ms_pkFactory || !pkVertexShader)
        return false;

    if (pkVertexShader->GetVertexShader())
    {
        // This should catch cases where a shader is shared between
        // passes!
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = pkVertexShader->GetCreator();

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        // How was it loaded in the first place?
        return false;
    }

    return pkCreator->RecreateVertexShader(pkVertexShader);
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::RecreateGeometryShader(
    NiD3D10GeometryShader* pkGeometryShader)
{
    if (!ms_pkFactory || !pkGeometryShader)
        return false;

    if (pkGeometryShader->GetGeometryShader())
    {
        // This should catch cases where a shader is shared between
        // passes!
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = pkGeometryShader->GetCreator();

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        // How was it loaded in the first place?
        return false;
    }

    return pkCreator->RecreateGeometryShader(pkGeometryShader);
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::RecreatePixelShader(
    NiD3D10PixelShader* pkPixelShader)
{
    if (!ms_pkFactory || !pkPixelShader)
        return false;

    if (pkPixelShader->GetPixelShader())
    {
        // This should catch cases where a shader is shared between
        // passes!
        return true;
    }

    NiD3D10ShaderProgramCreator* pkCreator = pkPixelShader->GetCreator();

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        // How was it loaded in the first place?
        return false;
    }

    return pkCreator->RecreatePixelShader(pkPixelShader);
}
//---------------------------------------------------------------------------
// Shader releasing
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::ReleaseVertexShader(
    NiD3D10VertexShader* pkShader)
{
    if (!ms_pkFactory)
        return;

    if (!pkShader)
        return;

    NiFixedString kTemp = pkShader->GetName();
    ms_pkFactory->RemoveVertexShaderFromMap(kTemp);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::ReleaseGeometryShader(
    NiD3D10GeometryShader* pkShader)
{
    if (!ms_pkFactory)
        return;

    if (!pkShader)
        return;

    NiFixedString kTemp = pkShader->GetName();
    ms_pkFactory->RemoveGeometryShaderFromMap(kTemp);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::ReleasePixelShader(
    NiD3D10PixelShader* pkShader)
{
    if (!ms_pkFactory)
        return;

    if (!pkShader)
        return;

    NiFixedString kTemp = pkShader->GetName();
    ms_pkFactory->RemovePixelShaderFromMap(kTemp);
}
//---------------------------------------------------------------------------
const char* NiD3D10ShaderProgramFactory::GetFirstProgramDirectory(
    NiTListIterator& kIter)
{
    kIter = m_kProgramDirectories.GetHeadPos();
    if (kIter)
    {
        return m_kProgramDirectories.GetNext(kIter);
    }
    return 0;
}
//---------------------------------------------------------------------------
const char* NiD3D10ShaderProgramFactory::GetNextProgramDirectory(
    NiTListIterator& kIter)
{
    if (kIter)
    {
        return m_kProgramDirectories.GetNext(kIter);
    }
    return 0;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::AddProgramDirectory(const char* pcDirectory)
{
    if (pcDirectory == NULL || pcDirectory[0] == '\0')
        return;

    // First, check if it's already in there...
    char* pcCheckName;
    char acSourcePath[NI_MAX_PATH];

    NiStrcpy(acSourcePath, NI_MAX_PATH, pcDirectory);
    for (unsigned int ui = 0; ui < strlen(acSourcePath); ui++)
    {
        if (acSourcePath[ui] == '/')
            acSourcePath[ui] = '\\';
    }

    NiTListIterator kIter = m_kProgramDirectories.GetHeadPos();
    while (kIter)
    {
        pcCheckName = m_kProgramDirectories.GetNext(kIter);
        if (pcCheckName)
        {
            // We know that the stored one is correct as we convert it when
            // we add it...
            if (NiStricmp(pcCheckName, acSourcePath) == 0)
            {
                // Already in there...
                return;
            }
        }
    }

    // Not in there... add it!
    size_t stLen = strlen(acSourcePath) + 1;
    char* pacNewAdd = NiAlloc(char, stLen);
    NIASSERT(pacNewAdd);
    NiStrcpy(pacNewAdd, stLen, acSourcePath);

    m_kProgramDirectories.AddHead(pacNewAdd);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::RemoveProgramDirectory(
    const char* pcDirectory)
{
    if (pcDirectory == NULL || pcDirectory[0] == '\0')
        return;

    // First, check if it's already in there...
    char* pcCheckName;
    char acSourcePath[NI_MAX_PATH];

    NiStrcpy(acSourcePath, NI_MAX_PATH, pcDirectory);
    for (unsigned int ui = 0; ui < strlen(acSourcePath); ui++)
    {
        if (acSourcePath[ui] == '/')
            acSourcePath[ui] = '\\';
    }

    NiTListIterator kIter = m_kProgramDirectories.GetHeadPos();
    while (kIter)
    {
        pcCheckName = m_kProgramDirectories.GetNext(kIter);
        if (pcCheckName)
        {
            // We know that the stored one is correct as we convert it when
            // we add it...
            if (NiStricmp(pcCheckName, acSourcePath) == 0)
            {
                m_kProgramDirectories.Remove(pcCheckName);
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::RemoveAllProgramDirectories()
{
    // First, check if it's already in there...
    char* pcCheckName;

    NiTListIterator kIter = m_kProgramDirectories.GetHeadPos();
    while (kIter)
    {
        pcCheckName = m_kProgramDirectories.GetNext(kIter);
        if (pcCheckName)
        {
            m_kProgramDirectories.Remove(pcCheckName);
            NiFree(pcCheckName);
        }
    }
}
//---------------------------------------------------------------------------
NiD3D10VertexShader* NiD3D10ShaderProgramFactory::GetVertexShader(
    const NiFixedString& kShaderName)
{
    NiD3D10VertexShader* pkVertexShader = NULL;

    if (m_kVertexShaderMap.GetAt(kShaderName, pkVertexShader))
        return pkVertexShader;
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiD3D10GeometryShader* NiD3D10ShaderProgramFactory::GetGeometryShader(
    const NiFixedString& kShaderName)
{
    NiD3D10GeometryShader* pkGeometryShader = NULL;

    if (m_kGeometryShaderMap.GetAt(kShaderName, pkGeometryShader))
        return pkGeometryShader;
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiD3D10PixelShader* NiD3D10ShaderProgramFactory::GetPixelShader(
    const NiFixedString& kShaderName)
{
    NiD3D10PixelShader* pkPixelShader = NULL;

    if (m_kPixelShaderMap.GetAt(kShaderName, pkPixelShader))
        return pkPixelShader;
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::InsertVertexShaderIntoMap(
    NiD3D10VertexShader* pkShader)
{
    if (!pkShader)
        return;

    NiFixedString kTemp = pkShader->GetName();
    m_kVertexShaderMap.SetAt(kTemp, pkShader);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::InsertGeometryShaderIntoMap(
    NiD3D10GeometryShader* pkShader)
{
    if (!pkShader)
        return;

    NiFixedString kTemp = pkShader->GetName();
    m_kGeometryShaderMap.SetAt(kTemp, pkShader);
}

//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::InsertPixelShaderIntoMap(
    NiD3D10PixelShader* pkShader)
{
    if (!pkShader)
        return;

    NiFixedString kTemp = pkShader->GetName();
    m_kPixelShaderMap.SetAt(kTemp, pkShader);
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::ResolveShaderFileName(
    const NiFixedString& kOriginalName, NiFixedString& kTrueName)
{
    // See if the file exists as-is
    if (NiFile::Access(kOriginalName, NiFile::READ_ONLY))
    {
        kTrueName = kOriginalName;
        return true;
    }
    else
    {
        if (!ms_pkFactory)
        {
            // We don't have a valid factory, so we can't grab the directory
            // the app set to check for the file.
            NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "No valid shader program factory\n");
            return false;
        }

        // Path was not valid. 
        NiFilename kFilename(kOriginalName);
        const char* pcFilename = kFilename.GetFilename();
        const char* pcFileExt = kFilename.GetExt();

        char pcTrueName[_MAX_PATH];

        NiTListIterator kIter;
        const char* pcProgDir = ms_pkFactory->GetFirstProgramDirectory(kIter);
        while (pcProgDir)
        {
            bool bGood = true;

            if (pcProgDir == NULL || pcProgDir[0] == '\0')
            {
                NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                    "Invalid or no shader program directory\n");
                bGood = false;
            }
            else if ((pcProgDir[strlen(pcProgDir) - 1] != '/') &&
                (pcProgDir[strlen(pcProgDir) - 1] != '\\'))
            {
                NiSprintf(pcTrueName, _MAX_PATH, "%s\\%s%s", pcProgDir, 
                    pcFilename, pcFileExt);
            }
            else
            {
                NiSprintf(pcTrueName, _MAX_PATH, "%s%s%s", pcProgDir, 
                    pcFilename, pcFileExt);
            }

            if (bGood)
            {
                if (!NiFile::Access(pcTrueName, NiFile::READ_ONLY))
                {
                    // Not found!
                    NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "Shader program file not found %s in "
                        "directory %s\n", kOriginalName, pcProgDir);
                    bGood = false;
                }
            }

            if (!bGood)
            {
                pcProgDir = ms_pkFactory->GetNextProgramDirectory(kIter);
            }
            else
            {
                // Found it...
                kTrueName = pcTrueName;
                return true;
            }
        }
    }

    // It can be assumed the file was NOT found at this point!
    kTrueName = 0;
    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramFactory::RegisterShaderCreator(const char* pcExt, 
    NiD3D10ShaderProgramCreator* pkCreator)
{
    // Make sure pcExt refers to the extension only!
    if (pcExt == NULL || strchr(pcExt, '.') != NULL)
        return false;

    // Only store lowercase extension
    char acFileExt[_MAX_EXT];
    unsigned int i = 0;
    for (; i < _MAX_EXT; i++)
    {
        acFileExt[i] = static_cast<char>(tolower(pcExt[i]));
        if (pcExt[i] == '\0')
            break;
    }
    NIASSERT(i < _MAX_EXT);

    NIASSERT(ms_pkFactory);

    NiFixedString kExt = acFileExt;
    ms_pkFactory->m_kShaderCreators.SetAt(kExt, pkCreator);
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::UnregisterShaderCreator(
    NiD3D10ShaderProgramCreator* pkCreator)
{
    NIASSERT(ms_pkFactory);

    NiTMapIterator kIter = ms_pkFactory->m_kShaderCreators.GetFirstPos();
    while (kIter)
    {
        NiFixedString kExt = NULL;
        NiD3D10ShaderProgramCreator* pkMapCreator = NULL;
        ms_pkFactory->m_kShaderCreators.GetNext(kIter, kExt, pkMapCreator);
        if (pkCreator == pkMapCreator)
            ms_pkFactory->m_kShaderCreators.RemoveAt(kExt);
    }
}
//---------------------------------------------------------------------------
NiD3D10ShaderProgramCreator* NiD3D10ShaderProgramFactory::GetShaderCreator(
    const char* pcFilename)
{
    NIASSERT(ms_pkFactory);

    if (pcFilename == NULL || pcFilename[0] == '\0')
        return NULL;

    NiFilename kFilename(pcFilename);
    char acExt[_MAX_EXT];
    NiStrcpy(acExt, _MAX_EXT, kFilename.GetExt());

    // Check for NULL string.
    if (acExt[0] == '\0')
        return NULL;

    // Only look for lowercase extension
    const char* pcSrc = acExt + 1; // skip the '.'
    if (pcSrc == NULL || pcSrc[0] == '\0')
        pcSrc = pcFilename;

    unsigned int i = 0;
    for (; i < _MAX_EXT; i++)
    {
        acExt[i] = static_cast<char>(tolower(pcSrc[i]));
        if (acExt[i] == '\0')
            break;
    }
    NIASSERT(i < _MAX_EXT);

    NiD3D10ShaderProgramCreator* pkCreator = NULL;
    NiFixedString kExt = acExt;
    if (ms_pkFactory->m_kShaderCreators.GetAt(kExt, pkCreator))
        return pkCreator;
    else
        return NULL;
}
//---------------------------------------------------------------------------
ID3D10Blob* NiD3D10ShaderProgramFactory::ReadShaderFile(
    const NiFixedString& kFileName)
{
    // Resolve shader program file
    NiFixedString kShaderPath;
    if (!ResolveShaderFileName(kFileName, kShaderPath))
    {
        // Can't resolve the shader!
        NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, true, 
            "Failed to find shader program file %s\n", kFileName);
        return NULL;
    }

    NiFile* pkIstr = NiFile::GetFile(kShaderPath, NiFile::READ_ONLY);
    if ((!pkIstr) || (!(*pkIstr)))
    {
        NiDelete pkIstr;
        return NULL;
    }

    unsigned int uiBufferSize = pkIstr->GetFileSize();
    if (uiBufferSize == 0)
    {
        NiDelete pkIstr;
        return NULL;
    }

    ID3D10Blob* pkBlob = NULL;
    HRESULT hr = NiD3D10Renderer::D3D10CreateBlob(uiBufferSize, &pkBlob);
    if (FAILED(hr) || pkBlob == NULL)
    {
        if (pkBlob)
            pkBlob->Release();
        NiDelete pkIstr;
        return NULL;
    }

    // Read file into memory
    pkIstr->Read(pkBlob->GetBufferPointer(), 
        (unsigned int)pkBlob->GetBufferSize());
    NiDelete pkIstr;

    return pkBlob;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::_SDMInit()
{
    if (ms_pkFactory == NULL)
        ms_pkFactory = NiNew NiD3D10ShaderProgramFactory();
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramFactory::_SDMShutdown()
{
    NiDelete ms_pkFactory;
    ms_pkFactory = NULL;
}
//---------------------------------------------------------------------------
