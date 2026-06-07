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

#include "NiD3DShaderProgramFactory.h"
#include "NiD3DShaderProgramCreator.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DVertexShader.h"
#include "NiD3DPixelShader.h"
#include "NiD3DMacros.h"
#include "NiD3DDefines.h"
#include "NiD3DRendererHeaders.h"
#include "NiD3DUtility.h"

//---------------------------------------------------------------------------
NiD3DShaderProgramFactory* NiD3DShaderProgramFactory::ms_pkFactory = NULL;

NiTStringPointerMap<NiD3DShaderProgramCreator*>* 
    NiD3DShaderProgramFactory::ms_pkShaderCreators = NULL;

D3DDevicePtr NiD3DShaderProgramFactory::ms_pkD3DDevice = NULL;
NiD3DRenderer* NiD3DShaderProgramFactory::ms_pkD3DRenderer = NULL;
NiD3DRenderState* NiD3DShaderProgramFactory::ms_pkD3DRenderState = NULL;

//---------------------------------------------------------------------------
// Device and renderer access functions
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::SetD3DDevice(D3DDevicePtr pkD3DDevice)
{
    if (ms_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(ms_pkD3DDevice);
    ms_pkD3DDevice = pkD3DDevice;
    if (ms_pkD3DDevice)
        D3D_POINTER_REFERENCE(ms_pkD3DDevice);
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    ms_pkD3DRenderer = pkD3DRenderer;
    if (ms_pkD3DRenderer)
    {
        SetD3DDevice(ms_pkD3DRenderer->GetD3DDevice());
        SetD3DRenderState(ms_pkD3DRenderer->GetRenderState());
    }
    else
    {
        SetD3DRenderState(0);
        SetD3DDevice(0);
    }
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::SetD3DRenderState(NiD3DRenderState* pkRS)
{
    ms_pkD3DRenderState = pkRS;
}
//---------------------------------------------------------------------------
NiD3DShaderProgramFactory::NiD3DShaderProgramFactory()
{
    // Maps for shader program names to shaders.
    m_pkVertexShaderMap = NiNew NiTStringPointerMap<NiD3DVertexShader*>(59,
        false);
    NIASSERT(m_pkVertexShaderMap);
    m_pkPixelShaderMap = NiNew NiTStringPointerMap<NiD3DPixelShader*>(59, 
        false);
    NIASSERT(m_pkPixelShaderMap);
}
//---------------------------------------------------------------------------
NiD3DShaderProgramFactory::~NiD3DShaderProgramFactory()
{
    if (m_pkVertexShaderMap)
        m_pkVertexShaderMap->RemoveAll();
    NiDelete m_pkVertexShaderMap;

    if (m_pkPixelShaderMap)
        m_pkPixelShaderMap->RemoveAll();
    NiDelete m_pkPixelShaderMap;

    RemoveAllProgramDirectories();
}
//---------------------------------------------------------------------------
NiD3DShaderProgramFactory* NiD3DShaderProgramFactory::GetInstance()
{
    return ms_pkFactory;
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::Shutdown()
{
    NiDelete ms_pkFactory;
    ms_pkFactory = 0;
}
//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramFactory::CreateVertexShaderFromFile(
    const char* pcFileName, const char* pcShaderName, 
    const char* pcEntryPoint, const char* pcShaderTarget, 
    NiD3DVertexDeclaration hDecl, unsigned int uiUsage, bool bRecoverable)
{
    if (!ms_pkFactory)
        return NULL;

    if (!pcFileName || pcFileName[0] == '\0')
        return NULL;
    
    // See if it exists already
    NiD3DVertexShader* pkVertexShader = 
        ms_pkFactory->GetVertexShader(pcShaderName);
    if (pkVertexShader)
    {
        // Already loaded... Return it
        NIASSERT(pkVertexShader->GetShaderHandle() != NULL);
        return pkVertexShader;
    }

    NiD3DShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    pkVertexShader = pkCreator->CreateVertexShaderFromFile(pcFileName, 
        pcShaderName, pcEntryPoint, pcShaderTarget, hDecl, uiUsage, 
        bRecoverable);

    // Insert it in the list
    ms_pkFactory->InsertVertexShaderIntoMap(pkVertexShader);

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramFactory::CreateVertexShaderFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcExt, 
    const char* pcShaderName, const char* pcEntryPoint, 
    const char* pcShaderTarget, NiD3DVertexDeclaration hDecl, 
    unsigned int uiUsage, bool bRecoverable)
{
    if (!ms_pkFactory)
        return NULL;

    if (!pvBuffer || uiBufferSize == 0)
        return NULL;
    
    // See if it exists already
    NiD3DVertexShader* pkVertexShader = 
        ms_pkFactory->GetVertexShader(pcShaderName);
    if (pkVertexShader)
    {
        // Already loaded... Return it
        NIASSERT(pkVertexShader->GetShaderHandle() != NULL);
        return pkVertexShader;
    }

    NiD3DShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcExt);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    pkVertexShader = pkCreator->CreateVertexShaderFromBuffer(pvBuffer, 
        uiBufferSize, pcShaderName, pcEntryPoint, pcShaderTarget, 
        hDecl, uiUsage, bRecoverable);

    // Insert it in the list
    ms_pkFactory->InsertVertexShaderIntoMap(pkVertexShader);

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramFactory::CreatePixelShaderFromFile(
    const char* pcFileName, const char* pcShaderName, 
    const char* pcEntryPoint, const char* pcShaderTarget, bool bRecoverable)
{
    if (!ms_pkFactory)
        return NULL;

    if (!pcFileName || pcFileName[0] == '\0')
        return NULL;
    
    // See if it exists already
    NiD3DPixelShader* pkPixelShader = 
        ms_pkFactory->GetPixelShader(pcShaderName);
    if (pkPixelShader)
    {
        // Already loaded... Return it
        NIASSERT(pkPixelShader->GetShaderHandle() != NULL);
        return pkPixelShader;
    }

    NiD3DShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcFileName);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    pkPixelShader = pkCreator->CreatePixelShaderFromFile(pcFileName, 
        pcShaderName, pcEntryPoint, pcShaderTarget, bRecoverable);

    // Insert it in the list
    ms_pkFactory->InsertPixelShaderIntoMap(pkPixelShader);

    return pkPixelShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramFactory::CreatePixelShaderFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcExt, 
    const char* pcShaderName, const char* pcEntryPoint, 
    const char* pcShaderTarget, bool bRecoverable)
{
    if (!ms_pkFactory)
        return NULL;

    if (!pvBuffer || uiBufferSize == 0)
        return NULL;
    
    // See if it exists already
    NiD3DPixelShader* pkPixelShader = 
        ms_pkFactory->GetPixelShader(pcShaderName);
    if (pkPixelShader)
    {
        // Already loaded... Return it
        NIASSERT(pkPixelShader->GetShaderHandle() != NULL);
        return pkPixelShader;
    }

    NiD3DShaderProgramCreator* pkCreator = 
        ms_pkFactory->GetShaderCreator(pcExt);

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        return NULL;
    }

    pkPixelShader = pkCreator->CreatePixelShaderFromBuffer(pvBuffer, 
        uiBufferSize, pcShaderName, pcEntryPoint, pcShaderTarget, 
        bRecoverable);

    // Insert it in the list
    ms_pkFactory->InsertPixelShaderIntoMap(pkPixelShader);

    return pkPixelShader;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramFactory::RecreateVertexShader(
    NiD3DVertexShader* pkVertexShader)
{
    if (!ms_pkFactory || !pkVertexShader)
        return false;

    if (pkVertexShader->GetShaderHandle())
    {
        // This should catch cases where a shader is shared between
        // passes!
        return true;
    }

    NiD3DShaderProgramCreator* pkCreator = pkVertexShader->GetCreator();

    if (pkCreator == NULL)
    {
        // No knowledge of this shader format - can't load it
        // How was it loaded in the first place?
        return false;
    }

    return pkCreator->RecreateVertexShader(pkVertexShader);
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramFactory::RecreatePixelShader(
    NiD3DPixelShader* pkPixelShader)
{
    if (!ms_pkFactory || !pkPixelShader)
        return false;

    if (pkPixelShader->GetShaderHandle())
    {
        // This should catch cases where a shader is shared between
        // passes!
        return true;
    }

    NiD3DShaderProgramCreator* pkCreator = pkPixelShader->GetCreator();

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
void NiD3DShaderProgramFactory::ReleaseVertexShader(
    NiD3DVertexShader* pkShader)
{
    if (!ms_pkFactory)
        return;

    if (!pkShader)
        return;

    ms_pkFactory->RemoveVertexShaderFromMap(pkShader->GetName());
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::ReleasePixelShader(
    NiD3DPixelShader* pkShader)
{
    if (!ms_pkFactory)
        return;

    if (!pkShader)
        return;

    ms_pkFactory->RemovePixelShaderFromMap(pkShader->GetName());
}
//---------------------------------------------------------------------------
const char* NiD3DShaderProgramFactory::GetFirstProgramDirectory(
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
const char* NiD3DShaderProgramFactory::GetNextProgramDirectory(
    NiTListIterator& kIter)
{
    if (kIter)
    {
        return m_kProgramDirectories.GetNext(kIter);
    }
    return 0;
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::AddProgramDirectory(const char* pacDirectory)
{
    if (!pacDirectory || !strcmp(pacDirectory, ""))
        return;

    // First, check if it's already in there...
    char* pacCheckName;
    char acSourcePath[NI_MAX_PATH];

    NiStrcpy(acSourcePath, NI_MAX_PATH, pacDirectory);
    for (unsigned int ui = 0; ui < strlen(acSourcePath); ui++)
    {
        if (acSourcePath[ui] == '/')
            acSourcePath[ui] = '\\';
    }

    NiTListIterator kIter = m_kProgramDirectories.GetHeadPos();
    while (kIter)
    {
        pacCheckName = m_kProgramDirectories.GetNext(kIter);
        if (pacCheckName)
        {
            // We know that the stored one is correct as we convert it when
            // we add it...
            if (NiStricmp(pacCheckName, acSourcePath) == 0)
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
void NiD3DShaderProgramFactory::RemoveProgramDirectory(
    const char* pcDirectory)
{
    if (!pcDirectory || !strcmp(pcDirectory, ""))
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
void NiD3DShaderProgramFactory::RemoveAllProgramDirectories()
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
NiD3DVertexShader* NiD3DShaderProgramFactory::GetVertexShader(
    const char* pcShaderName)
{
    if (!m_pkVertexShaderMap)
    {
        NIASSERT(!"GetVertexShader> Invalid Map!");
        return NULL;
    }

    NiD3DVertexShader* pkVertexShader = NULL;

    if (m_pkVertexShaderMap->GetAt(pcShaderName, pkVertexShader))
        return pkVertexShader;
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramFactory::GetPixelShader(
    const char* pcShaderName)
{
    if (!m_pkPixelShaderMap)
    {
        NIASSERT(!"GetPixelShader> Invalid Map!");
        return NULL;
    }

    NiD3DPixelShader* pkPixelShader = NULL;

    if (m_pkPixelShaderMap->GetAt(pcShaderName, pkPixelShader))
        return pkPixelShader;
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::InsertVertexShaderIntoMap(
    NiD3DVertexShader* pkShader)
{
    if (!pkShader)
        return;

    if (!m_pkVertexShaderMap)
    {
        NIASSERT(!"InsertVertexShaderIntoMap> Invalid Map!");
        return;
    }

    m_pkVertexShaderMap->SetAt(pkShader->GetName(), pkShader);
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::RemoveVertexShaderFromMap(
    const char* pcShaderName)
{
    if (!pcShaderName)
        return;

    if (!m_pkVertexShaderMap)
    {
        NIASSERT(!"RemoveVertexShaderFromMap> Invalid Map!");
        return;
    }

    m_pkVertexShaderMap->RemoveAt(pcShaderName);
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::InsertPixelShaderIntoMap(
    NiD3DPixelShader* pkShader)
{
    if (!pkShader)
        return;

    if (!m_pkPixelShaderMap)
    {
        NIASSERT(!"InsertPixelShaderIntoMap> Invalid Map!");
        return;
    }

    m_pkPixelShaderMap->SetAt(pkShader->GetName(), pkShader);
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::RemovePixelShaderFromMap(
    const char* pcShaderName)
{
    if (!pcShaderName)
        return;

    if (!m_pkPixelShaderMap)
    {
        NIASSERT(!"RemovePixelShaderFromMap> Invalid Map!");
        return;
    }

    m_pkPixelShaderMap->RemoveAt(pcShaderName);
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramFactory::ResolveShaderFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiStringLen)
{
    // See if the file exists as-is
    if (NiFile::Access(pcOriginalName, NiFile::READ_ONLY))
    {
        NiStrcpy(pcTrueName, uiStringLen, pcOriginalName);
        return true;
    }
    else
    {
        if (!ms_pkFactory)
        {
            // We don't have a valid factory, so we can't grab the directory
            // the app set to check for the file.
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "No valid shader program factory\n");
            return false;
        }

        // Path was not valid. 
        char acDrive[_MAX_DRIVE];
        char acDir[_MAX_PATH];
        char acFileName[_MAX_PATH];
        char acFileExt[_MAX_EXT];

        NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, 
            acFileName, acFileExt);

        NiTListIterator kIter;
        const char* pacProgDir;

        pacProgDir = ms_pkFactory->GetFirstProgramDirectory(kIter);
        while (pacProgDir)
        {
            bool bGood = true;

            if (!pacProgDir || pacProgDir[0] == '\0')
            {
                NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                    "Invalid or no shader program directory\n");
                bGood = false;
            }
            else
            if ((pacProgDir[strlen(pacProgDir) - 1] != '/') &&
                (pacProgDir[strlen(pacProgDir) - 1] != '\\'))
            {
                NiSprintf(pcTrueName, uiStringLen, "%s\\%s%s", pacProgDir, 
                    acFileName, acFileExt);
            }
            else
            {
                NiSprintf(pcTrueName, uiStringLen, "%s%s%s", pacProgDir, 
                    acFileName, acFileExt);
            }

            if (bGood)
            {
                if (!NiFile::Access(pcTrueName, NiFile::READ_ONLY))
                {
                    // Not found!
                    NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "Shader program file not found %s in "
                        "directory %s\n", 
                        pcOriginalName, pacProgDir);
                    bGood = false;
                }
            }

            if (!bGood)
            {
                pacProgDir = ms_pkFactory->GetNextProgramDirectory(kIter);
            }
            else
            {
                // Found it...
                return true;
            }
        }
    }

    // It can be assumed the file was NOT found at this point!
    return false;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramFactory::RegisterShaderCreator(const char* pcExt, 
    NiD3DShaderProgramCreator* pkCreator)
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

    if (ms_pkShaderCreators == NULL)
        CreateShaderCreatorMap();
    ms_pkShaderCreators->SetAt(acFileExt, pkCreator);
    return true;
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::UnregisterShaderCreator(
    NiD3DShaderProgramCreator* pkCreator)
{
    if (ms_pkShaderCreators == NULL)
        return;

    NiTMapIterator kIter = ms_pkShaderCreators->GetFirstPos();
    while (kIter)
    {
        const char* pcExt = NULL;
        NiD3DShaderProgramCreator* pkMapCreator = NULL;
        ms_pkShaderCreators->GetNext(kIter, pcExt, pkMapCreator);
        if (pkCreator == pkMapCreator)
            ms_pkShaderCreators->RemoveAt(pcExt);
    }
}
//---------------------------------------------------------------------------
NiD3DShaderProgramCreator* NiD3DShaderProgramFactory::GetShaderCreator(
    const char* pcFilename)
{
    if (ms_pkShaderCreators == NULL ||
        pcFilename == NULL || pcFilename[0] == '\0')
    {
        return NULL;
    }

    char acFileExt[_MAX_EXT];

    NiD3DUtility::GetFileExtension(pcFilename, acFileExt, _MAX_EXT);

    // Only look for lowercase extension
    const char* pcSrc = acFileExt + 1; // skip the '.'
    if (pcSrc == NULL || pcSrc[0] == '\0')
        pcSrc = pcFilename;

    unsigned int i = 0;
    for (; i < _MAX_EXT; i++)
    {
        acFileExt[i] = static_cast<char>(tolower(pcSrc[i]));
        if (acFileExt[i] == '\0')
            break;
    }
    NIASSERT(i < _MAX_EXT);

    NiD3DShaderProgramCreator* pkCreator = NULL;
    if (ms_pkShaderCreators->GetAt(acFileExt, pkCreator))
        return pkCreator;
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::CreateShaderCreatorMap()
{
    NIASSERT(ms_pkShaderCreators == NULL);
    ms_pkShaderCreators = 
        NiNew NiTStringPointerMap<NiD3DShaderProgramCreator*>;
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::_SDMInit()
{
    if (ms_pkShaderCreators == NULL)
        CreateShaderCreatorMap();
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramFactory::_SDMShutdown()
{
    ms_pkShaderCreators->RemoveAll();
    NiDelete ms_pkShaderCreators;
    ms_pkShaderCreators = NULL;
}
//---------------------------------------------------------------------------
