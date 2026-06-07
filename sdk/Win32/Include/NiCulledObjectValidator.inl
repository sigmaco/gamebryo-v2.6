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
inline NiCulledObjectValidator::NiCulledObjectValidator(NiAVObject* pkObject,
    NiCamera* pkCamera, NiCullingProcess* pkCullingProcess,
    bool bReturnOnCulled) : m_spObject(pkObject), m_spCamera(pkCamera),
    m_spCullingProcess(pkCullingProcess), m_uiLastFrameID(0),
    m_bLastReturnValue(false), m_bReturnOnCulled(bReturnOnCulled),
    m_bForceUpdate(true)
{
}
//---------------------------------------------------------------------------
inline void NiCulledObjectValidator::SetObject(NiAVObject* pkObject)
{
    m_spObject = pkObject;
    m_bForceUpdate = true;
}
//---------------------------------------------------------------------------
inline NiAVObject* NiCulledObjectValidator::GetObject() const
{
    return m_spObject;
}
//---------------------------------------------------------------------------
inline void NiCulledObjectValidator::SetCamera(NiCamera* pkCamera)
{
    m_spCamera = pkCamera;
    m_bForceUpdate = true;
}
//---------------------------------------------------------------------------
inline NiCamera* NiCulledObjectValidator::GetCamera() const
{
    return m_spCamera;
}
//---------------------------------------------------------------------------
inline void NiCulledObjectValidator::SetCullingProcess(
    NiCullingProcess* pkCullingProcess)
{
    m_spCullingProcess = pkCullingProcess;
    m_bForceUpdate = true;
}
//---------------------------------------------------------------------------
inline NiCullingProcess* NiCulledObjectValidator::GetCullingProcess() const
{
    return m_spCullingProcess;
}
//---------------------------------------------------------------------------
inline void NiCulledObjectValidator::SetReturnOnCulled(bool bReturnOnCulled)
{
    m_bReturnOnCulled = bReturnOnCulled;
    m_bForceUpdate = true;
}
//---------------------------------------------------------------------------
inline bool NiCulledObjectValidator::GetReturnOnCulled() const
{
    return m_bReturnOnCulled;
}
//---------------------------------------------------------------------------
