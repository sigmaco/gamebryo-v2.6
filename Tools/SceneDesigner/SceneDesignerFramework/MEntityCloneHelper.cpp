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
#include "SceneDesignerFrameworkPCH.h"

#include "MEntityCloneHelper.h"
#include "MEntityFactory.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MEntityCloneHelper::MEntityCloneHelper() :
    m_pkCloneHelper(NULL)
{
    m_pkCloneHelper = NiNew NiEntityCloneHelper;
}
//---------------------------------------------------------------------------
void MEntityCloneHelper::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        NiDelete m_pkCloneHelper;
        m_pkCloneHelper = NULL;
    }
}
//---------------------------------------------------------------------------
void MEntityCloneHelper::Clone(MEntity* pmEntity, String* strNewName, 
                               bool bInheritProperties)
{
    MVerifyValidInstance;

    const char* pcNewName = MStringToCharPointer(strNewName);
    m_pkCloneHelper->Clone(pmEntity->GetNiEntityInterface(), pcNewName, 
        bInheritProperties);
    MFreeCharPointer(pcNewName);
}
//---------------------------------------------------------------------------
void MEntityCloneHelper::Fixup(bool bClearExternal)
{
    MVerifyValidInstance;

    m_pkCloneHelper->Fixup(bClearExternal);
}
//---------------------------------------------------------------------------
MEntity* MEntityCloneHelper::Lookup(MEntity* pmOriginalEntity)
{
    MVerifyValidInstance;

    return MEntityFactory::Instance->Get(m_pkCloneHelper->Lookup(
        pmOriginalEntity->GetNiEntityInterface()));
}
//---------------------------------------------------------------------------
MEntity* MEntityCloneHelper::GetEntities()[]
{
    MVerifyValidInstance;

    const unsigned int uiEntityCount = m_pkCloneHelper->GetEntityCount();
    MEntity* amEntities[] = new MEntity*[uiEntityCount];
    for (unsigned int ui = 0; ui < uiEntityCount; ++ui)
    {
        amEntities[ui] = MEntityFactory::Instance->Get(
            m_pkCloneHelper->GetEntityAt(ui));
    }

    return amEntities;
}
//---------------------------------------------------------------------------
void MEntityCloneHelper::Reset()
{
    MVerifyValidInstance;

    m_pkCloneHelper->Reset();
}
//---------------------------------------------------------------------------
