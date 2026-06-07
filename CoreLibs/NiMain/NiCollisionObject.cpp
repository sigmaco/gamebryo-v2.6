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

#include "NiCollisionObject.h"
#include "NiAVObject.h"
#include "NiStream.h"

NiImplementRTTI(NiCollisionObject,NiObject);

//---------------------------------------------------------------------------
NiCollisionObject::NiCollisionObject(NiAVObject* pkSceneObject)
{
    SetSceneGraphObject(pkSceneObject);
}
//---------------------------------------------------------------------------
NiCollisionObject::NiCollisionObject()
{
    m_pkSceneObject = 0;
}
//---------------------------------------------------------------------------
NiCollisionObject::~NiCollisionObject()
{
    /* */
}
//---------------------------------------------------------------------------
void NiCollisionObject::SetSceneGraphObject(NiAVObject* pkSceneObject)
{
    m_pkSceneObject = pkSceneObject;

    if (m_pkSceneObject)
    {
        if (m_pkSceneObject->GetCollisionObject() != this)
            m_pkSceneObject->SetCollisionObject(this);
    }
}    
//---------------------------------------------------------------------------
NiAVObject* NiCollisionObject::GetSceneGraphObject() const
{
    return m_pkSceneObject;
}
//---------------------------------------------------------------------------
void NiCollisionObject::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiCollisionObject::ms_RTTI.GetName()));

    AddViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
void NiCollisionObject::AddViewerStrings(NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString(NiCollisionObject::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_pkSceneObject", m_pkSceneObject));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiCollisionObject::CopyMembers(NiCollisionObject* pDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pDest, kCloning);

    // m_pkScreneObject must be filled in...
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiCollisionObject::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkSceneObject
}
//---------------------------------------------------------------------------
void NiCollisionObject::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
    m_pkSceneObject = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiCollisionObject::RegisterStreamables(NiStream& kStream)
{
    if ( !NiObject::RegisterStreamables(kStream) )
        return false;

    if ( m_pkSceneObject )
        m_pkSceneObject->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiCollisionObject::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkSceneObject);
}
//---------------------------------------------------------------------------
bool NiCollisionObject::IsEqual(NiObject* pkObject)
{
    if ( !NiObject::IsEqual(pkObject) )
        return false;

    // Only require that both have a target or both do not.

    if (m_pkSceneObject && !pkObject)
        return false;

    if (!m_pkSceneObject && pkObject)
        return false;
       
    return true;
}
//---------------------------------------------------------------------------
