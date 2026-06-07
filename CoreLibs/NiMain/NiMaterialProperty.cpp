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

#include "NiMaterialProperty.h"
#include "NiRenderer.h"

NiImplementRTTI(NiMaterialProperty,NiProperty);

NiMaterialPropertyPtr NiMaterialProperty::ms_spDefault;
int NiMaterialProperty::ms_iNextIndex = 1;

//---------------------------------------------------------------------------
#if !defined(WIN32) && !defined(_XENON)
NiCriticalSection NiMaterialProperty::ms_kCritSec;
#endif
//---------------------------------------------------------------------------
NiMaterialProperty::NiMaterialProperty()
{
    m_amb.r = m_amb.g = m_amb.b = 0.5f;
    m_diff.r = m_diff.g = m_diff.b = 0.5f;
    m_spec.r = m_spec.g = m_spec.b = 0.0f;
    m_emit.r = m_emit.g = m_emit.b = 0.0f;
    m_fShine = 4.0f;

   m_fAlpha = 1.0f;
    
#if !defined(WIN32) && !defined(_XENON)
    ms_kCritSec.Lock();
    m_iIndex = ms_iNextIndex++;    
    ms_kCritSec.Unlock();
#else
    m_iIndex = InterlockedIncrement((LONG*)&ms_iNextIndex);
#endif

    m_uiRevID = 1;

#if defined(WIN32) || defined(_XENON)
    m_pvRendererData = 0;
#endif // defined(WIN32) || defined(_XENON)
}
//---------------------------------------------------------------------------
NiMaterialProperty::~NiMaterialProperty()
{
    NiRenderer::RemoveMaterial(this);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMaterialProperty);
//---------------------------------------------------------------------------
void NiMaterialProperty::CopyMembers(NiMaterialProperty* pDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pDest, kCloning);

    pDest->m_amb = m_amb;
    pDest->m_diff = m_diff;
    pDest->m_spec = m_spec;
    pDest->m_emit = m_emit;
    pDest->m_fShine = m_fShine;
    pDest->m_fAlpha = m_fAlpha;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMaterialProperty);
//---------------------------------------------------------------------------
void NiMaterialProperty::LoadBinary(NiStream& stream)
{
    NiProperty::LoadBinary(stream);
    m_amb.LoadBinary(stream);
    m_diff.LoadBinary(stream);
    m_spec.LoadBinary(stream);
    m_emit.LoadBinary(stream);
    NiStreamLoadBinary(stream,m_fShine);
    NiStreamLoadBinary(stream,m_fAlpha);
}
//---------------------------------------------------------------------------
void NiMaterialProperty::LinkObject(NiStream& stream)
{
    NiProperty::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiMaterialProperty::RegisterStreamables(NiStream& stream)
{
    return NiProperty::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiMaterialProperty::SaveBinary(NiStream& stream)
{
    NiProperty::SaveBinary(stream);
    m_amb.SaveBinary(stream);
    m_diff.SaveBinary(stream);
    m_spec.SaveBinary(stream);
    m_emit.SaveBinary(stream);
    NiStreamSaveBinary(stream,m_fShine);
    NiStreamSaveBinary(stream,m_fAlpha);
}
//---------------------------------------------------------------------------
bool NiMaterialProperty::IsEqual(NiObject* pObject)
{
    if ( !NiProperty::IsEqual(pObject) )
        return false;

    NiMaterialProperty* pMat = (NiMaterialProperty*) pObject;

    if ( m_amb != pMat->m_amb
    ||   m_diff != pMat->m_diff
    ||   m_spec != pMat->m_spec
    ||   m_emit != pMat->m_emit
    ||   m_fShine != pMat->m_fShine
    ||   m_fAlpha != pMat->m_fAlpha )
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMaterialProperty::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiProperty::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiMaterialProperty::ms_RTTI.GetName()));

    pStrings->Add(m_amb.GetViewerString("m_amb"));
    pStrings->Add(m_diff.GetViewerString("m_diff"));
    pStrings->Add(m_spec.GetViewerString("m_spec"));
    pStrings->Add(m_emit.GetViewerString("m_emit"));
    pStrings->Add(NiGetViewerString("m_fShine",m_fShine));
    pStrings->Add(NiGetViewerString("m_fAlpha",m_fAlpha));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiMaterialProperty::_SDMInit()
{
    ms_spDefault = NiNew NiMaterialProperty;
}
//---------------------------------------------------------------------------
    
void NiMaterialProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
