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

#include "NiPhysXFluidPCH.h"

#include "NiPhysXProp.h"
#include "NiPhysXScene.h"

#include "NiPhysXFluidSceneDesc.h"
#include "NiPhysXFluidPropDesc.h"

NiImplementRTTI(NiPhysXFluidSceneDesc, NiPhysXSceneDesc);

//---------------------------------------------------------------------------
NiPhysXFluidSceneDesc::NiPhysXFluidSceneDesc() : NiPhysXSceneDesc()
{
}
//---------------------------------------------------------------------------
NiPhysXFluidSceneDesc::~NiPhysXFluidSceneDesc()
{
    m_kFluids.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXFluidSceneDesc::ConvertToProp(NiPhysXScene* pkScene)
{
    // All of the sources, destinations, actors, joints and materials
    // have been created in another NiPhysXProp and attached to this scene.
    // We add just the fluid actors as another prop.
    NiPhysXProp* pkProp = NiNew NiPhysXProp();
    
    pkProp->SetScaleFactor(pkScene->GetScaleFactor());

    pkProp->SetKeepMeshes(false); // No meshes here at all.
    
    NiPhysXFluidPropDesc* pkPropSnapshot = NiNew NiPhysXFluidPropDesc();
    
    NiUInt32 uiSize = m_kFluids.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        pkPropSnapshot->AddFluidDesc(m_kFluids.GetAt(ui));
    }

    pkPropSnapshot->SetNumStates(GetNumStatesBackCompatable());
    
    pkProp->SetSnapshot(pkPropSnapshot);
    
    pkScene->AddProp(pkProp);

    m_kFluids.RemoveAll();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions. Nothing is streamed out - this class only handles
// loading old content.
//---------------------------------------------------------------------------
bool NiPhysXFluidSceneDesc::RegisterStreamables(NiStream&)
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidSceneDesc::SaveBinary(NiStream&)
{
    // This function intentionally left blank
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidSceneDesc);
//---------------------------------------------------------------------------
void NiPhysXFluidSceneDesc::LoadBinary(NiStream& kStream)
{
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 2))
        return;

    NiPhysXSceneDesc::LoadBinary(kStream);

    NiUInt32 uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kFluids.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kFluids.SetAt(i, (NiPhysXFluidDesc*)kStream.ResolveLinkID());
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidSceneDesc::LinkObject(NiStream& kStream)
{
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 2))
        return;

    NiPhysXSceneDesc::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXFluidSceneDesc::IsEqual(NiObject*)
{
    return true;
}
//---------------------------------------------------------------------------
