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

#include "NiObject.h"
#include "NiCloningProcess.h"
#include "NiStream.h"
#include <NiSystem.h>
#include <NiVersion.h>

// Including this to allow for optimizations relating to the static data
// manager initialization for NiMain
#include "NiMainSDM.h"
static NiMainSDM NiMainSDMObject;

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED =
    GAMEBRYO_MODULE_VERSION_STRING(NiMain);
//---------------------------------------------------------------------------

NiImplementRootRTTI(NiObject);

//---------------------------------------------------------------------------
NiObject::NiObject()
{
}
//---------------------------------------------------------------------------
NiObject::~NiObject ()
{
}
//---------------------------------------------------------------------------
unsigned int NiObject::GetBlockAllocationSize() const
{
    return 0;
}
//---------------------------------------------------------------------------
NiObjectGroup* NiObject::GetGroup() const
{
    return NULL;
}
//---------------------------------------------------------------------------
void NiObject::SetGroup(NiObjectGroup*)
{
    // The object group is no longer stored in NiObject, so this function does
    // nothing.
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiObject* NiObject::Clone()
{
    (void)NiMemMarker(NI_MEM_MARKER_BEGIN, __FUNCTION__, this);

    NiCloningProcess kCloning;
    NiObject* pClone = CreateClone(kCloning);
    ProcessClone(kCloning);

    (void)NiMemMarker(NI_MEM_MARKER_END, __FUNCTION__, this);

    return pClone;
}
//---------------------------------------------------------------------------
NiObject* NiObject::Clone(NiCloningProcess& kCloning)
{
    (void)NiMemMarker(NI_MEM_MARKER_BEGIN, __FUNCTION__, this);

    NiObject* pClone = CreateClone(kCloning);
    ProcessClone(kCloning);
    
    (void)NiMemMarker(NI_MEM_MARKER_END, __FUNCTION__, this);

    return pClone;
}
//---------------------------------------------------------------------------
NiObject* NiObject::CreateClone(
    NiCloningProcess&) 
{ 
    // Default behavior of cloning is to share.
    return this; 
}
//---------------------------------------------------------------------------
NiObject* NiObject::CreateSharedClone(NiCloningProcess& kCloning)
{
    NiObject* pkClone;
    if (kCloning.m_pkCloneMap->GetAt(this, pkClone))
        return pkClone;
    else
        return CreateClone(kCloning);
}
//---------------------------------------------------------------------------
void NiObject::ProcessClone(NiCloningProcess& kCloning)
{
    // Add this item into a hash table which allows any code with a pointer
    // to "this" to determine if ProcessClone has already been called for
    // this object.
    kCloning.m_pkProcessMap->SetAt(this, true);
}
//---------------------------------------------------------------------------
void NiObject::CopyMembers(NiObject* pDest,
    NiCloningProcess& kCloning)
{
    // Add an item into hash table which allows any code with a pointer to
    // "this" to access the cloned copy of "this" (pDest).
    kCloning.m_pkCloneMap->SetAt(this, pDest);
}
#ifndef __SPU__
//---------------------------------------------------------------------------
NiObjectPtr NiObject::CreateDeepCopy()
{
    (void)NiMemMarker(NI_MEM_MARKER_END, __FUNCTION__, this);

    NiStream stream;
    stream.InsertObject(this);
    
    /* stream object to memory block */
    char* pBuffer = 0;
    int iBufferSize = 0;
    NIVERIFY(stream.Save(pBuffer, iBufferSize));
    
    /* stream memory block back to object */
    stream.Load(pBuffer, iBufferSize);
    NiObjectPtr spCopy = stream.GetObjectAt(0);
    NiFree(pBuffer);
   
#if defined(NI_MEMORY_DEBUGGER)
    (void)NiMemMarker(NI_MEM_MARKER_END, __FUNCTION__, this);
#endif

    return spCopy;
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiObject::LoadBinary(NiStream& stream)
{
    if (stream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 114))
    {
        unsigned int uiID;
        NiStreamLoadBinary(stream, uiID);
        SetGroup(stream.GetGroupFromID(uiID));
    }
}
//---------------------------------------------------------------------------
void NiObject::LinkObject(NiStream&)
{
}
//---------------------------------------------------------------------------
void NiObject::PostLinkObject(NiStream&) 
{
}
//---------------------------------------------------------------------------
bool NiObject::RegisterStreamables(NiStream& stream)
{
    // If NiStream::RegisterSaveObject returns false, the object is already 
    // registered, and the caller does not need to register its members.

    return stream.RegisterSaveObject(this);
}
//---------------------------------------------------------------------------
void NiObject::SaveBinary(NiStream&)
{
}
//---------------------------------------------------------------------------
bool NiObject::StreamCanSkip()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiObject::GetStreamableRTTIName(char* acName, 
    unsigned int uiMaxSize) const
{
    return GetRTTI()->CopyName(acName, uiMaxSize);
}
//---------------------------------------------------------------------------
bool NiObject::IsEqual(NiObject* pObject)
{
    if ( !pObject )
        return false;

    if ( strcmp(GetRTTI()->GetName(),pObject->GetRTTI()->GetName()) != 0 )
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiObject::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    pStrings->Add(NiGetViewerString(NiObject::ms_RTTI.GetName()));
    pStrings->Add(NiGetViewerString("this",this));
    pStrings->Add(NiGetViewerString("m_uiRefCount",GetRefCount()));
}
//---------------------------------------------------------------------------
#endif
