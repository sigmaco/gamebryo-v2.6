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
inline void NiRenderer::LockRenderer()
{
    m_kRendererLock.Lock();

    if (LockCount() == 1)
        Do_LockRenderer();
}
//---------------------------------------------------------------------------
inline void NiRenderer::UnlockRenderer()
{
    if (LockCount() == 1)
        Do_UnlockRenderer();

    m_kRendererLock.Unlock();
}
//---------------------------------------------------------------------------
inline unsigned int NiRenderer::LockCount() const
{
    return m_kRendererLock.GetCurrentLockCount();
}
//---------------------------------------------------------------------------
inline bool NiRenderer::BeginFrame()
{
    if (!Check_FrameState("BeginFrame", FRAMESTATE_OUTSIDE_FRAME))
        return false;

    if (!Do_BeginFrame())
        return false;

    m_eFrameState = FRAMESTATE_INSIDE_FRAME;
    return true;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::DisplayFrame()
{
    if (!Check_FrameState("DisplayFrame", FRAMESTATE_WAITING_FOR_DISPLAY))
        return false;

    if (!Do_DisplayFrame())
    {
#ifdef NIDEBUG
        Warning("NiRenderer::DisplayFrame> Failed - " 
            "Returning to frame to 'not set' state.\n");
#endif
        m_eFrameState = FRAMESTATE_OUTSIDE_FRAME;
        return false;
    }

    m_eFrameState = FRAMESTATE_OUTSIDE_FRAME;
    m_uiFrameID++;
    return true;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::BeginOffScreenFrame()
{
    if (!Check_FrameState("BeginOffScreenFrame", FRAMESTATE_OUTSIDE_FRAME))
        return false;

    if (!Do_BeginFrame())
        return false;

    m_eFrameState = FRAMESTATE_INSIDE_OFFSCREEN_FRAME;
    return true;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::EndOffScreenFrame()
{
    if (!Check_FrameState("EndOffScreenFrame", 
        FRAMESTATE_INSIDE_OFFSCREEN_FRAME))
        return false;

    if (!Do_EndFrame())
    {
#ifdef NIDEBUG
        Warning("NiRenderer::EndOffScreenFrame> Failed - " 
            "Returning to frame to 'not set' state.\n");
#endif
        m_eFrameState = FRAMESTATE_OUTSIDE_FRAME;
        return false;
    }

    m_eFrameState = FRAMESTATE_OUTSIDE_FRAME;
    return true;
}
//---------------------------------------------------------------------------
inline void NiRenderer::ClearBuffer(
    const NiRect<float>* pkR, unsigned int uiMode)
{
    if (!Check_RenderTargetGroupActive("ClearBuffer", true))
        return;

    Do_ClearBuffer(pkR, uiMode);
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetCameraData(const NiPoint3& kWorldLoc,
        const NiPoint3& kWorldDir, const NiPoint3& kWorldUp, 
        const NiPoint3& kWorldRight, const NiFrustum& kFrustum, 
        const NiRect<float>& kPort)
{
    if (!Check_RenderTargetGroupActive("SetCameraData", true))
        return;

    Do_SetCameraData(
        kWorldLoc, kWorldDir, kWorldUp, kWorldRight, kFrustum, kPort);
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetScreenSpaceCameraData(
    const NiRect<float>* pkPort)
{
    if (!Check_RenderTargetGroupActive("SetScreenSpaceCameraData", true))
        return;

    Do_SetScreenSpaceCameraData(pkPort);
}
//---------------------------------------------------------------------------
inline void NiRenderer::GetCameraData(NiPoint3& kWorldLoc, NiPoint3& kWorldDir,
    NiPoint3& kWorldUp, NiPoint3& kWorldRight, NiFrustum& kFrustum,
    NiRect<float>& kPort)
{
    if (!Check_RenderTargetGroupActive("GetCameraData", true))
        return;

    Do_GetCameraData(kWorldLoc, kWorldDir, kWorldUp, kWorldRight, kFrustum,
        kPort);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::BeginUsingRenderTargetGroup(
    NiRenderTargetGroup* pkTarget, 
    unsigned int uiClearMode)
{
    if (!Check_RenderTargetGroupActive("BeginUsingRenderTargetGroup", false))
        return false;

    if (!pkTarget)
    {
        Warning("NiRenderer::BeginUsingRenderTargetGroup> "
            "Failed - NULL pointer for render target. "
            "Call BeginUsingDefaultRenderTargetGroup if you intended to use "
            "the backbuffer instead.");
        return false;
    }

    LockRenderer(); // Start lock for duration of render target

    m_pkCurrentRenderTargetGroup = pkTarget;

    bool bSuccess = Do_BeginUsingRenderTargetGroup(pkTarget, uiClearMode);
    
    if (bSuccess)
        m_bRenderTargetGroupActive = true;
    else
        UnlockRenderer();

    return bSuccess;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::BeginUsingDefaultRenderTargetGroup(
    unsigned int uiClearMode)
{
    if (!Check_RenderTargetGroupActive(
        "BeginUsingDefaultRenderTargetGroup", false))
        return false;

    return BeginUsingRenderTargetGroup(
        GetDefaultRenderTargetGroup(), uiClearMode);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::IsRenderTargetGroupActive() const
{
    return m_bRenderTargetGroupActive;
}
//---------------------------------------------------------------------------
inline NiMaterial* NiRenderer::GetDefaultMaterial() const
{
    return m_spCurrentDefaultMaterial;
}
//---------------------------------------------------------------------------
inline NiMaterial* NiRenderer::GetInitialDefaultMaterial() const
{
    return m_spInitialDefaultMaterial;
}
//---------------------------------------------------------------------------
inline void NiRenderer::RenderMesh(NiRenderObject* pkMesh)
{
    NIASSERT(Check_RenderTargetGroupActive("RenderMesh", true));

    Do_RenderMesh((NiMesh*)pkMesh);
}
//---------------------------------------------------------------------------
inline float NiRenderer::GetMaxFogValue() const 
{ 
    return 1.0f; 
}
//---------------------------------------------------------------------------
inline unsigned short NiRenderer::GetMaxAnisotropy() const
{
    return m_usMaxAnisotropy;
}
//---------------------------------------------------------------------------
inline const char* NiRenderer::GetLastErrorString() 
{ 
    return ms_acErrorString; 
}
//---------------------------------------------------------------------------
inline void NiRenderer::ClearLastErrorString() 
{ 
    SetLastErrorString(0); 
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetLastErrorString(const char* pcStr)
{
    if (pcStr)
        NiStrncpy(ms_acErrorString, 256, pcStr, 255);
    else
        ms_acErrorString[0] = '\0';
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetSorter(NiAccumulator* pkAccum)
{
    m_spAccum = pkAccum;
}
//---------------------------------------------------------------------------
inline NiAccumulator* NiRenderer::GetSorter() const
{
    return m_spAccum;
}
//---------------------------------------------------------------------------
inline NiDynamicEffectState* NiRenderer::GetEffectState() const
{
    return m_pkCurrEffects;
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetEffectState(NiDynamicEffectState* pkState)
{
    m_pkCurrEffects = pkState;
}
//---------------------------------------------------------------------------
inline NiPropertyState* NiRenderer::GetPropertyState() const
{
    return m_pkCurrProp;
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetPropertyState(NiPropertyState* pkState)
{
    m_pkCurrProp = pkState;
}
//---------------------------------------------------------------------------
inline NiRenderer* NiRenderer::GetRenderer()
{
    return ms_pkRenderer;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::PreloadTexture(NiTexture*) 
{ 
    return true; 
}
//---------------------------------------------------------------------------
inline void NiRenderer::LockSourceDataCriticalSection()
{
    m_kSourceDataCriticalSection.Lock();
}
//---------------------------------------------------------------------------
inline void NiRenderer::UnlockSourceDataCriticalSection()
{
    m_kSourceDataCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
inline NiRenderer::EFrameState NiRenderer::GetFrameState() const
{
    return m_eFrameState;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::GetInsideFrameState() const
{
    return (m_eFrameState == FRAMESTATE_INSIDE_FRAME) || 
        (m_eFrameState == FRAMESTATE_INSIDE_OFFSCREEN_FRAME) ||
        (m_eFrameState == FRAMESTATE_INTERNAL_FRAME);
}
//---------------------------------------------------------------------------
inline unsigned int NiRenderer::GetFrameID() const
{
    return m_uiFrameID;
}
//---------------------------------------------------------------------------
inline NiShader* NiRenderer::GetErrorShader() const
{
    return m_spErrorShader;
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetErrorShader(NiShader* pkErrorShader)
{
    if (pkErrorShader)
        m_spErrorShader = pkErrorShader;
}
//---------------------------------------------------------------------------
inline void NiRenderer::LockPrecacheCriticalSection()
{
    LockRenderer();
    m_kPrecacheCriticalSection.Lock();
}
//---------------------------------------------------------------------------
inline void NiRenderer::UnlockPrecacheCriticalSection()
{
    m_kPrecacheCriticalSection.Unlock();
    UnlockRenderer();
}
//---------------------------------------------------------------------------
inline bool NiRenderer::Check_FrameState(
#ifdef NIDEBUG
    const char* pcCallingFunction,
#else
    const char*,
#endif
    EFrameState eExpectedValue) const
{
    if (m_eFrameState == eExpectedValue)
        return true;

#ifdef NIDEBUG
    const char* apcCurrentValueStrings[5] = {
        "'not set'",
        "'set'",
        "'set for offscreen'",
        "'waiting for display'",
        "'set for internal'"
    };
    const char* apcNextStepStrings[5] = {
        "'BeginFrame()'",
        "'EndFrame()'",
        "'EndOffscreenFrame()'",
        "'DisplayFrame()'",
        "'EndInternalFrame()"
    };
    NIASSERT((unsigned int)m_eFrameState < 5);

    Warning("NiRenderer::%s> Failed - " 
        "called while frame is %s, Must call %s first.\n",
        pcCallingFunction,
        apcCurrentValueStrings[m_eFrameState],
        apcNextStepStrings[m_eFrameState]);
#endif
    return false;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::Check_RenderTargetGroupActive(
#ifdef NIDEBUG
    const char* pcCallingFunction, 
#else
    const char*, 
#endif
    bool bExpectedValue) const
{
    // Check that we are in a frame of rendering
    if (!GetInsideFrameState())
    {
#ifdef NIDEBUG
        Warning("NiRenderer::%s> Failed - " 
            "called while frame is not set for rendering, Must call "
            "'BeginFrame() or BeginOffScreenFrame()' first.\n", 
            pcCallingFunction);
#endif
        return false;
    }

    // Check that render target group is active (or we're in bypass mode)
    if ((m_bRenderTargetGroupActive == bExpectedValue) ||
        (m_eFrameState == FRAMESTATE_INTERNAL_FRAME))
        return true;

#ifdef NIDEBUG
    if (m_bRenderTargetGroupActive)
    {
        Warning("NiRenderer::%s> Failed - " 
            "called while render target is 'set', Must call "
            "'EndUsingRenderTargetGroup()' first.\n",
            pcCallingFunction);
    }
    else
    {
        Warning("NiRenderer::%s> Failed - " 
            "called while render target is 'not set', Must call "
            "'BeginUsingRenderTargetGroup()' first.\n",
            pcCallingFunction);
    }
#endif
    return false;
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetSafeZone(const NiRect<float>& kRect)
{
    m_kDisplaySafeZone = kRect;
}
//---------------------------------------------------------------------------
inline NiRect<float> NiRenderer::GetSafeZone()
{
    return m_kDisplaySafeZone;
}
//---------------------------------------------------------------------------
inline void NiRenderer::ConvertFromPixelsToNDC(const unsigned int uiInX, 
    const unsigned int uiInY, float& fOutX, float& fOutY)
{
    fOutX = (float)uiInX / (float)GetDefaultBackBuffer()->GetWidth();
    fOutY = (float)uiInY / (float)GetDefaultBackBuffer()->GetHeight();
}
//---------------------------------------------------------------------------
inline void NiRenderer::ConvertFromNDCToPixels(const float fInX, 
    const float fInY, unsigned int& uiOutX, unsigned int& uiOutY)
{
    uiOutX = (unsigned int)
        (fInX * (float)GetDefaultBackBuffer()->GetWidth());
    uiOutY = (unsigned int)
        (fInY * (float)GetDefaultBackBuffer()->GetHeight());
}
//---------------------------------------------------------------------------
inline void NiRenderer::ForceInSafeZone(
    unsigned int& uiX, unsigned int& uiY)
{
    NiRect<int> kDisplaySafeZonePixels;
    float fWidth = (float)GetDefaultBackBuffer()->GetWidth();
    float fHeight = (float)GetDefaultBackBuffer()->GetHeight();

    // Create a pixel relative representation of the safe zone bounds
    kDisplaySafeZonePixels.m_left = 
        (unsigned int)(m_kDisplaySafeZone.m_left * fWidth);
    kDisplaySafeZonePixels.m_right = 
        (unsigned int)(m_kDisplaySafeZone.m_right * fWidth);
    kDisplaySafeZonePixels.m_top = 
        (unsigned int)(m_kDisplaySafeZone.m_top * fHeight);
    kDisplaySafeZonePixels.m_bottom = 
        (unsigned int)(m_kDisplaySafeZone.m_bottom * fHeight);

    // Clamp uiX inside of the safe zone
    uiX = NiMax(uiX, kDisplaySafeZonePixels.m_left);
    uiX = NiMin(uiX, kDisplaySafeZonePixels.m_right);

    // Clamp fY inside of the safe zone
    uiY = NiMax(uiY, kDisplaySafeZonePixels.m_top);
    uiY = NiMin(uiY, kDisplaySafeZonePixels.m_bottom);
}
//---------------------------------------------------------------------------
inline void NiRenderer::ForceInSafeZoneNDC(float& fX, float& fY)
{
    // Clamp fX inside of the safe zone
    fX = NiMax(fX, m_kDisplaySafeZone.m_left);
    fX = NiMin(fX, m_kDisplaySafeZone.m_right);

    // Clamp fY inside of the safe zone
    fY = NiMax(fY, m_kDisplaySafeZone.m_top);
    fY = NiMin(fY, m_kDisplaySafeZone.m_bottom);
}
//---------------------------------------------------------------------------
inline NiRenderer::ShaderData::ShaderData()
{
    m_uiShaderFlags = 0;
    m_bIsChanged = true;
}
//---------------------------------------------------------------------------
// Returns pointer to global ShaderData structure
//---------------------------------------------------------------------------
inline NiRenderer::ShaderData* NiRenderer::GetGlobalShaderData()
{
    return &m_kGlobalShaderData;
}
//---------------------------------------------------------------------------
// Shader macros adding methods
//---------------------------------------------------------------------------
inline bool NiRenderer::ShaderData::AddMacro(
    const NiFixedString& kMacroName,
    const NiFixedString& kMacroValue)
{
    // Do not allow empty macro names
    if (!kMacroName.GetLength())
        return false;

    m_kStringMap.SetAt(kMacroName, kMacroValue);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::AddGlobalMacro(
    const NiFixedString& kMacroName,
    const NiFixedString& kMacroValue)
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    return pkData->AddMacro(kMacroName, kMacroValue);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::AddMacro(
    const char* pcFileType,
    const NiFixedString& kMacroName,
    const NiFixedString& kMacroValue)
{
    ShaderData* pkData = GetShaderData(pcFileType, true);
    NIASSERT(pkData);

    return pkData->AddMacro(kMacroName, kMacroValue);
}
//---------------------------------------------------------------------------
// Shader macros removal methods
//---------------------------------------------------------------------------
inline bool NiRenderer::ShaderData::RemoveMacro(
    const NiFixedString& kMacroName)
{
    return m_kStringMap.RemoveAt(kMacroName);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::RemoveGlobalMacro(const NiFixedString& kMacroName)
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    return pkData->RemoveMacro(kMacroName);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::RemoveMacro(
    const char* pcFileType,
    const NiFixedString& kMacroName)
{
    ShaderData* pkData = GetShaderData(pcFileType);

    if (!pkData)
        return false;

    return pkData->RemoveMacro(kMacroName);
}
//---------------------------------------------------------------------------
// Shader macros count
//---------------------------------------------------------------------------
inline NiUInt32 NiRenderer::ShaderData::GetMacroCount() const
{
    return m_kStringMap.GetCount();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiRenderer::GetGlobalMacroCount()
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    return pkData->GetMacroCount();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiRenderer::GetMacroCount(const char* pcFileType)
{
    ShaderData* pkData = GetShaderData(pcFileType);

    if (!pkData)
        return 0;

    return pkData->GetMacroCount();
}
//---------------------------------------------------------------------------
// Shader macros presence checking methods
//---------------------------------------------------------------------------
inline bool NiRenderer::ShaderData::IsMacro(
    const NiFixedString& kMacroName) const
{
    NiFixedString kVal;
    return m_kStringMap.GetAt(kMacroName, kVal);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::IsGlobalMacro(const NiFixedString& kMacroName)
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    return pkData->IsMacro(kMacroName);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::IsMacro(
    const char* pcFileType,
    const NiFixedString& kMacroName)
{
    ShaderData* pkData = GetShaderData(pcFileType);

    if (!pkData)
        return false;

    return pkData->IsMacro(kMacroName);
}
//---------------------------------------------------------------------------
// Shader macros value querying methods
//---------------------------------------------------------------------------
inline bool NiRenderer::ShaderData::GetMacroValue(
    const NiFixedString& kMacroName,
    NiFixedString& kMacroValue) const
{
    return m_kStringMap.GetAt(kMacroName, kMacroValue);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::GetGlobalMacroValue(
    const NiFixedString& kMacroName,
    NiFixedString& kMacroValue)
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    return pkData->GetMacroValue(kMacroName, kMacroValue);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::GetMacroValue(
    const char* pcFileType,
    const NiFixedString& kMacroName,
    NiFixedString& kMacroValue)
{
    ShaderData* pkData = GetShaderData(pcFileType);

    if (!pkData)
        return false;

    return pkData->GetMacroValue(kMacroName, kMacroValue);
}
//---------------------------------------------------------------------------
// Methods to iterate through shader macro definitions
//---------------------------------------------------------------------------
inline NiMacrosIterator NiRenderer::ShaderData::GetFirstMacro() const
{
    return m_kStringMap.GetFirstPos();
}
//---------------------------------------------------------------------------
inline NiMacrosIterator NiRenderer::GetFirstGlobalMacro()
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    return pkData->GetFirstMacro();
}
//---------------------------------------------------------------------------
inline NiMacrosIterator NiRenderer::GetFirstMacro(
    const char* pcFileType)
{
    ShaderData* pkData = GetShaderData(pcFileType);

    if (!pkData)
        return NULL;

    return pkData->GetFirstMacro();
}
//---------------------------------------------------------------------------
inline bool NiRenderer::ShaderData::GetNextMacro(
    NiMacrosIterator& kPos,
    NiFixedString& kMacroName,
    NiFixedString& kMacroValue) const
{
    if (!kPos)
        return false;

    m_kStringMap.GetNext(kPos, kMacroName, kMacroValue);
    return true;
}
//---------------------------------------------------------------------------
inline bool NiRenderer::GetNextGlobalMacro(
        NiMacrosIterator& kPos,
        NiFixedString& kMacroName,
        NiFixedString& kMacroValue)
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    return pkData->GetNextMacro(kPos, kMacroName, kMacroValue);
}
//---------------------------------------------------------------------------
inline bool NiRenderer::GetNextMacro(
        const char* pcFileType,
        NiMacrosIterator& kPos,
        NiFixedString& kMacroName,
        NiFixedString& kMacroValue)
{
    ShaderData* pkData = GetShaderData(pcFileType);

    if (!pkData)
        return false;

    return pkData->GetNextMacro(kPos, kMacroName, kMacroValue);
}
//---------------------------------------------------------------------------
// Shader creation flags setting and getting methods
//---------------------------------------------------------------------------
inline NiUInt32 NiRenderer::ShaderData::GetShaderCreationFlags() const
{
    return m_uiShaderFlags;
}
//---------------------------------------------------------------------------
inline void NiRenderer::ShaderData::SetShaderCreationFlags(NiUInt32 uiFlags)
{
    m_uiShaderFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiRenderer::GetGlobalShaderCreationFlags()
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    return pkData->GetShaderCreationFlags();
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetGlobalShaderCreationFlags(NiUInt32 uiFlags)
{
    ShaderData* pkData = GetGlobalShaderData();
    NIASSERT(pkData);

    pkData->SetShaderCreationFlags(uiFlags);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiRenderer::GetShaderCreationFlags(
    const char* pcFileType)
{
    ShaderData* pkData = GetShaderData(pcFileType);

    if (!pkData)
        return 0;

    return pkData->GetShaderCreationFlags();
}
//---------------------------------------------------------------------------
inline void NiRenderer::SetShaderCreationFlags(
    const char* pcFileType,
    NiUInt32 uiFlags)
{
    ShaderData* pkData = GetShaderData(pcFileType, true);
    NIASSERT(pkData);
    pkData->SetShaderCreationFlags(uiFlags);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiRenderer::GetAllShaderCreationFlags(
    const char* pcFileType,
    NiUInt32 uiUserFlags)
{
    return GetGlobalShaderCreationFlags() |
        GetShaderCreationFlags(pcFileType) | uiUserFlags;
}
//---------------------------------------------------------------------------
