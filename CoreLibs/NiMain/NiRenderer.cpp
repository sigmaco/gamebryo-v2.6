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
#include "NiMainPCH.h"

#include "NiRenderer.h"
#include "NiAccumulator.h"
#include "NiNode.h"
#include "NiCamera.h"
#include "NiRenderObject.h"
#include "NiMaterialNodeLibrary.h"
#include "NiMaterialProperty.h"
#include "NiRenderTargetGroup.h"
#include "NiStandardMaterial.h"
#include "StandardMaterialNodeLibrary/NiStandardMaterialNodeLibrary.h"
#include "NiShadowTechnique.h"

NiImplementRTTI(NiRenderer, NiObject);

char NiRenderer::ms_acErrorString[256] = "";
NiRenderer* NiRenderer::ms_pkRenderer = NULL;

//---------------------------------------------------------------------------
NiRenderer::NiRenderer() :
    m_bRenderTargetGroupActive(false),
    m_eFrameState(FRAMESTATE_OUTSIDE_FRAME),
    m_uiFrameID(0),
    m_usMaxAnisotropy(1)
{
    NIASSERT(ms_pkRenderer == NULL);
    ms_pkRenderer = this;

#if defined(_WII)
    // NiStandardMaterial isn't used on the Wii 
#else
    NiMaterialNodeLibraryPtr spMaterialNodeLibrary = 
        NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary();
    NiStandardMaterial* pkStandardMaterial =
        NiNew NiStandardMaterial(spMaterialNodeLibrary, false);
    pkStandardMaterial->AddDefaultFallbacks();
    m_spInitialDefaultMaterial = pkStandardMaterial;
#endif
}
//---------------------------------------------------------------------------
NiRenderer::~NiRenderer()
{
    ms_pkRenderer = NULL;

    DeleteAllMacros();
}
//---------------------------------------------------------------------------
void NiRenderer::RemoveEffect(NiDynamicEffect* pkEffect)
{
    if (ms_pkRenderer)
        ms_pkRenderer->PurgeEffect(pkEffect);
}
//---------------------------------------------------------------------------
void NiRenderer::RemoveMaterial(NiMaterialProperty* pkMaterial)
{
    if (ms_pkRenderer)
        ms_pkRenderer->PurgeMaterial(pkMaterial);
}
//---------------------------------------------------------------------------
void NiRenderer::PurgeAllRendererData(NiAVObject* pkScene)
{
    PurgeAllTextures(true);
    if (pkScene)
        pkScene->PurgeRendererData(this);
};
//---------------------------------------------------------------------------
bool NiRenderer::MapWindowPointToBufferPoint(unsigned int uiX,
    unsigned int uiY, float& fX, float& fY, 
    const NiRenderTargetGroup* pkTarget)
{
    //const NiRenderTargetGroup* pkCurrentRTGroup = pkTarget;

    if (pkTarget == NULL)
        pkTarget = GetDefaultRenderTargetGroup();

    if (!pkTarget)
        return false;

    unsigned int uiXSize = pkTarget->GetWidth(0);
    unsigned int uiYSize = pkTarget->GetHeight(0);

    if (uiXSize && uiYSize)
    {
        fX = (float)(uiX) / (float)uiXSize;
        fY = 1.0f - (float)(uiY) / (float)uiYSize;

        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiRenderer::FindClosestDepthStencilFormat( 
    const NiPixelFormat* pkFrontBufferFormat) const
{
    return FindClosestDepthStencilFormat(pkFrontBufferFormat, 24, 8);
}
//---------------------------------------------------------------------------
void NiRenderer::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiRenderer::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
void NiRenderer::SetDefaultProgramCache(NiFragmentMaterial* pkMaterial)
{
    SetDefaultProgramCache(pkMaterial, 
        NiFragmentMaterial::GetDefaultAutoSaveProgramCache(),
        NiFragmentMaterial::GetDefaultWriteDebugProgramData(),
        NiFragmentMaterial::GetDefaultLoadProgramCacheOnCreation(),
        NiFragmentMaterial::GetDefaultLockProgramCache(),
        NiMaterial::GetDefaultWorkingDirectory());
}
//---------------------------------------------------------------------------
void NiRenderer::Message(const char* pcFormat, ...)
{
    NI_UNUSED_ARG(pcFormat);
#ifdef NIDEBUG
    char acOutput[1024] = "NiRenderer MESSAGE: ";
    size_t stIndex = strlen(acOutput);

    va_list kArgs;
    va_start(kArgs, pcFormat);
    stIndex += NiVsprintf(&acOutput[stIndex], 1024 - stIndex, pcFormat, kArgs);
    va_end(kArgs);

    // Newline and null terminate
    stIndex = NiMin((int)stIndex, sizeof(acOutput) - 2);
    if (acOutput[stIndex-1] != '\n')
    {
        acOutput[stIndex  ] = '\n';
        acOutput[stIndex+1] = NULL;
    }
    else
    {
        acOutput[stIndex] = NULL;
    }

    NiOutputDebugString(acOutput);
#endif  //#ifdef NIDEBUG
}
//---------------------------------------------------------------------------
void NiRenderer::Warning(const char* pcFormat, ...)
{
    NI_UNUSED_ARG(pcFormat);
#ifdef NIDEBUG
    char acOutput[1024] = "NiRenderer WARNING: ";
    size_t stIndex = strlen(acOutput);

    va_list kArgs;
    va_start(kArgs, pcFormat);
    stIndex += NiVsprintf(&acOutput[stIndex], 1024 - stIndex, pcFormat, kArgs);
    va_end(kArgs);

    // Newline and null terminate
    stIndex = NiMin((int)stIndex, sizeof(acOutput) - 2);
    if (acOutput[stIndex-1] != '\n')
    {
        acOutput[stIndex  ] = '\n';
        acOutput[stIndex+1] = NULL;
    }
    else
    {
        acOutput[stIndex] = NULL;
    }

    NiOutputDebugString(acOutput);
#endif  //#ifdef NIDEBUG
}
//---------------------------------------------------------------------------
void NiRenderer::Error(const char* pcFormat, ...)
{
    NI_UNUSED_ARG(pcFormat);
#ifdef NIDEBUG
    char acOutput[1024] = "NiRenderer ERROR: ";
    size_t stIndex = strlen(acOutput);

    va_list kArgs;
    va_start(kArgs, pcFormat);
    stIndex += NiVsprintf(&acOutput[stIndex], 1024 - stIndex, pcFormat, kArgs);
    va_end(kArgs);

    // Newline and null terminate
    stIndex = NiMin((int)stIndex, sizeof(acOutput) - 2);
    if (acOutput[stIndex-1] != '\n')
    {
        acOutput[stIndex  ] = '\n';
        acOutput[stIndex+1] = NULL;
    }
    else
    {
        acOutput[stIndex] = NULL;
    }

    NiOutputDebugString(acOutput);
#endif  //#ifdef NIDEBUG
}
//---------------------------------------------------------------------------
void NiRenderer::_SDMShutdown()
{
    ms_pkRenderer = NULL;
}
//---------------------------------------------------------------------------
void NiRenderer::SetCameraData(const NiCamera* pkCamera)
{
    SetCameraData(pkCamera->GetWorldLocation(),
        pkCamera->GetWorldDirection(), pkCamera->GetWorldUpVector(),
        pkCamera->GetWorldRightVector(), pkCamera->GetViewFrustum(),
        pkCamera->GetViewPort());
}
//---------------------------------------------------------------------------
void NiRenderer::GetCameraData(NiCamera& kCamera)
{
    NiPoint3 kWorldLoc, kWorldDir, kWorldUp, kWorldRight;
    NiFrustum kFrustum;
    NiRect<float> kPort;
    GetCameraData(kWorldLoc, kWorldDir, kWorldUp, kWorldRight, kFrustum,
        kPort);

    // Build world-space camera transform.
    NiTransform kWorldTransform;
    kWorldTransform.m_Translate = kWorldLoc;
    kWorldTransform.m_Rotate = NiMatrix3(kWorldDir, kWorldUp, kWorldRight);
    kWorldTransform.m_fScale = 1.0f;

    // Compute local camera transform.
    NiTransform kLocalTransform;
    NiNode* pkParent = kCamera.GetParent();
    if (pkParent)
    {
        // If camera has a parent, un-apply parent transform.
        NiTransform kInverseParent;
        pkParent->GetWorldTransform().Invert(kInverseParent);
        kLocalTransform = kInverseParent * kWorldTransform;
    }
    else
    {
        kLocalTransform = kWorldTransform;
    }

    // Set both local and world transform on camera so that it does not need
    // to be updated here.
    kCamera.SetTranslate(kLocalTransform.m_Translate);
    kCamera.SetRotate(kLocalTransform.m_Rotate);
    kCamera.SetScale(kLocalTransform.m_fScale);
    kCamera.SetWorldTranslate(kWorldTransform.m_Translate);
    kCamera.SetWorldRotate(kWorldTransform.m_Rotate);
    kCamera.SetWorldScale(kWorldTransform.m_fScale);

    // Set frustum and viewport on camera.
    kCamera.SetViewFrustum(kFrustum);
    kCamera.SetViewPort(kPort);
}
//---------------------------------------------------------------------------
void NiRenderer::SetDefaultMaterial(NiMaterial* pkMaterial)
{
    if (pkMaterial == NULL)
        pkMaterial = m_spInitialDefaultMaterial;
    m_spCurrentDefaultMaterial = pkMaterial;
    NIASSERT(m_spCurrentDefaultMaterial);
}
//---------------------------------------------------------------------------
void NiRenderer::ApplyDefaultMaterial(NiRenderObject* pkGeometry)
{
    pkGeometry->ApplyAndSetActiveMaterial(GetDefaultMaterial());
}
//---------------------------------------------------------------------------
bool NiRenderer::PrecacheMesh(NiRenderObject*, bool)
{
    // Default behavior: do nothing.
    return true;
}
//---------------------------------------------------------------------------
bool NiRenderer::PerformPrecache()
{
    // Default behavior: do nothing.
    return true;
}
//---------------------------------------------------------------------------
void NiRenderer::GetOnScreenCoord(
    const float fXOffset, const float fYOffset,
    const unsigned int uiWidth, const unsigned int uiHeight,
    unsigned int& uiOutX, unsigned int& uiOutY,
    const DisplayCorner eCorner, 
    const bool bForceSafeZone)
{
    unsigned int uiX = 0;
    unsigned int uiY = 0;
    unsigned int uiXOffset, uiYOffset;

    unsigned int uiScreenWidth = GetDefaultBackBuffer()->GetWidth();
    unsigned int uiScreenHeight = GetDefaultBackBuffer()->GetHeight();

    ConvertFromNDCToPixels(fXOffset, fYOffset, uiXOffset, uiYOffset);

    // Use given ScreenCorner to determine which direction the
    // given offset will be moved.
    switch (eCorner)
    {
    case CORNER_TOP_LEFT:
        uiX = uiXOffset;
        uiY = uiYOffset;
        break;
    case CORNER_TOP_RIGHT:
        uiX = uiScreenWidth - uiXOffset;
        uiY = uiYOffset;
        break;
    case CORNER_BOTTOM_LEFT:
        uiX = uiXOffset;
        uiY = uiScreenHeight - uiYOffset;
        break;
    case CORNER_BOTTOM_RIGHT:
        uiX = uiScreenWidth - uiXOffset;
        uiY = uiScreenHeight - uiYOffset;
        break;
    }

    unsigned int uiRight = uiScreenWidth;
    unsigned int uiBottom = uiScreenHeight;

    if (bForceSafeZone)
    {
        ForceInSafeZone(uiRight, uiBottom);
        ForceInSafeZone(uiX, uiY);
    }

    // Ensure an object of uiWidth and uiHeight will be able to fully 
    // fit onto the display.
    int iXDiff = uiRight - (uiX + uiWidth); 
    int iYDiff = uiBottom - (uiY + uiHeight);
    iXDiff = NiMin(0, iXDiff);
    iYDiff = NiMin(0, iYDiff);

    uiOutX = NiMax(uiX + iXDiff, 0);
    uiOutY = NiMax(uiY + iYDiff, 0);
}
//---------------------------------------------------------------------------
bool NiRenderer::EndUsingRenderTargetGroup()
{
    if (!Check_RenderTargetGroupActive("EndUsingRenderTargetGroup", true))
        return false;

    if (!Do_EndUsingRenderTargetGroup())
        return false;

    UnlockRenderer(); // Finish lock for duration of render target

    m_pkCurrentRenderTargetGroup = NULL;
    m_bRenderTargetGroupActive = false;
    return true;
}
//---------------------------------------------------------------------------
bool NiRenderer::BeginInternalFrame()
{
    if (m_eFrameState == FRAMESTATE_INTERNAL_FRAME)
        return false;

    m_eSavedFrameState = m_eFrameState;
    m_eFrameState = FRAMESTATE_INTERNAL_FRAME;
    return true;
}
//---------------------------------------------------------------------------
bool NiRenderer::EndInternalFrame()
{
    if (!Check_FrameState("EndInternalFrame", FRAMESTATE_INTERNAL_FRAME))
        return false;
    m_eFrameState = m_eSavedFrameState;
    return true;
}
//---------------------------------------------------------------------------
bool NiRenderer::EndFrame()
{
    if (!Check_FrameState("EndFrame", FRAMESTATE_INSIDE_FRAME))
        return false;

    if (!Do_EndFrame())
    {
        m_eFrameState = FRAMESTATE_OUTSIDE_FRAME;
#ifdef NIDEBUG
        Warning("NiRenderer::EndFrame> Failed - " 
            "Returning to frame to 'not set' state.\n");
#endif
        return false;
    }

    m_eFrameState = FRAMESTATE_WAITING_FOR_DISPLAY;
    return true;
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiRenderer::CreateCompatibleDepthStencilBuffer(
    Ni2DBuffer* pkBuffer) const
{
    if (!pkBuffer)
        return NULL;

    const NiPixelFormat* pkFormat = 
        FindClosestDepthStencilFormat(pkBuffer->GetPixelFormat());

    if (pkFormat)
    {
        NiDepthStencilBuffer* pkDSBuffer = NiDepthStencilBuffer::Create(
            pkBuffer->GetWidth(), 
            pkBuffer->GetHeight(),
            NiRenderer::GetRenderer(), //"this" not used b/c of const
            *pkFormat,
            pkBuffer->GetMSAAPref());
        return pkDSBuffer;
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiRenderer::SaveScreenShot(const char*, EScreenshotFormat)
{
    return false;
}
//---------------------------------------------------------------------------
void NiRenderer::SetRenderShadowTechnique(
    NiShadowTechnique*)
{
    // Base class version does nothing.
    return;
}
// Shader macros cleanup methods
//---------------------------------------------------------------------------
void NiRenderer::ShaderData::DeleteAllMacros()
{
    m_kStringMap.RemoveAll();
}
//---------------------------------------------------------------------------
void NiRenderer::DeleteAllMacros()
{
    // Remove all global macros
    m_kGlobalShaderData.DeleteAllMacros();

    // Delete allocated tables
    for (NiUInt32 i = 0; i < m_kShaderData.GetSize(); i++)
    {
        ShaderData* pkData = m_kShaderData.GetAt(i);
        if (pkData)
            NiDelete pkData;
    }

    // Clear arrays of tables and file type maps
    m_kShaderData.RemoveAll();
    m_kFiletypeMap.RemoveAll();
}
//---------------------------------------------------------------------------
// Returns pointer to ShaderData structure for specified file type and creates
// new structure if requested, associating it with file type pcFileType
//---------------------------------------------------------------------------
NiRenderer::ShaderData* NiRenderer::GetShaderData(
    const char* pcFileType,
    bool bCreate)
{
    if (!pcFileType)
        return NULL;

    NiMacrosIterator kIter = m_kFiletypeMap.GetFirstPos();
    while (kIter)
    {
        NiFixedString kStr;
        ShaderData* pkShaderData;
        m_kFiletypeMap.GetNext(kIter, kStr, pkShaderData);
        if (kStr.EqualsNoCase(pcFileType))
            return pkShaderData;
    }

    if (!bCreate)
        return NULL;

    ShaderData* pkData = NiNew ShaderData();
    m_kShaderData.Add(pkData);
    m_kFiletypeMap.SetAt(NiFixedString(pcFileType), pkData);
    return pkData;
}
//---------------------------------------------------------------------------
void NiRenderer::BuildMacroList(
    const char* pcFileType,
    ShaderData* pkUserMacros,
    NiTFixedStringMap<NiFixedString>& kFinalList)
{
    ShaderData* pkGData = GetGlobalShaderData();
    NIASSERT(pkGData);
    ShaderData* pkTData = GetShaderData(pcFileType);
    bool bPerTypeMacroExist = pkTData && pkTData->GetMacroCount() > 0;
    bool bUserMacroExist = pkUserMacros && pkUserMacros->GetMacroCount() > 0;
    NiFixedString kName;
    NiFixedString kValue;

    // Add global macros. Global macro/flags structure always exists.
    NiMacrosIterator kIter = pkGData->GetFirstMacro();
    while(kIter)
    {
        pkGData->GetNextMacro(kIter, kName, kValue);

        // Skip if same named macro exists in per-file or user definitions
        if (bPerTypeMacroExist && pkTData->IsMacro(kName))
            continue;

        if (bUserMacroExist && pkUserMacros->IsMacro(kName))
            continue;

        kFinalList.SetAt(kName, kValue);
    }

    if (bPerTypeMacroExist)
    {
        // Add file type-specific macros
        kIter = pkTData->GetFirstMacro();
        while(kIter)
        {
            pkTData->GetNextMacro(kIter, kName, kValue);

            // Skip if same named macro exists in user specified definitions
            if (bUserMacroExist && pkUserMacros->IsMacro(kName))
                continue;

            kFinalList.SetAt(kName, kValue);
        }
    }

    // And, finally, append user-specified macros
    if (!bUserMacroExist)
        return;

    kIter = pkUserMacros->GetFirstMacro();
    while(kIter)
    {
        pkUserMacros->GetNextMacro(kIter, kName, kValue);
        kFinalList.SetAt(kName, kValue);
    }
}
//---------------------------------------------------------------------------
