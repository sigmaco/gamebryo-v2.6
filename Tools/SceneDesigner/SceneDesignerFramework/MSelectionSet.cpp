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

#include "MSelectionSet.h"
#include "MSelectionSetFactory.h"
#include "MEntityFactory.h"
#include "ServiceProvider.h"
#include "MRenameSelectionSetCommand.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MSelectionSet* MSelectionSet::Create(String* strName)
{
    const char* pcName = MStringToCharPointer(strName);
    NiEntitySelectionSet* pkSelectionSet = NiNew NiEntitySelectionSet(pcName);
    MFreeCharPointer(pcName);

    return MSelectionSetFactory::Instance->Get(pkSelectionSet);
}
//---------------------------------------------------------------------------
MSelectionSet* MSelectionSet::Create(String* strName,
    unsigned int uiInitialSize)
{
    const char* pcName = MStringToCharPointer(strName);
    NiEntitySelectionSet* pkSelectionSet = NiNew NiEntitySelectionSet(pcName,
        uiInitialSize);
    MFreeCharPointer(pcName);

    return MSelectionSetFactory::Instance->Get(pkSelectionSet);
}
//---------------------------------------------------------------------------
MSelectionSet::MSelectionSet(NiEntitySelectionSet* pkSelectionSet) :
    m_pkSelectionSet(pkSelectionSet)
{
    MInitRefObject(m_pkSelectionSet);
}
//---------------------------------------------------------------------------
void MSelectionSet::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkSelectionSet);
}
//---------------------------------------------------------------------------
String* MSelectionSet::ToString()
{
    MVerifyValidInstance;

    return this->Name;
}
//---------------------------------------------------------------------------
NiEntitySelectionSet* MSelectionSet::GetNiEntitySelectionSet()
{
    MVerifyValidInstance;

    return m_pkSelectionSet;
}
//---------------------------------------------------------------------------
String* MSelectionSet::get_Name()
{
    MVerifyValidInstance;

    return m_pkSelectionSet->GetName();
}
//---------------------------------------------------------------------------
void MSelectionSet::set_Name(String* strName)
{
    MVerifyValidInstance;

    const char* pcName = MStringToCharPointer(strName);
    CommandService->ExecuteCommand(new MRenameSelectionSetCommand(
        NiNew NiRenameSelectionSetCommand(m_pkSelectionSet, pcName)),
        true);
    MFreeCharPointer(pcName);
}
//---------------------------------------------------------------------------
unsigned int MSelectionSet::get_EntityCount()
{
    MVerifyValidInstance;

    return m_pkSelectionSet->GetEntityCount();
}
//---------------------------------------------------------------------------
void MSelectionSet::AddEntity(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    m_pkSelectionSet->AddEntity(pmEntity->GetNiEntityInterface());
}
//---------------------------------------------------------------------------
void MSelectionSet::RemoveEntity(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    m_pkSelectionSet->RemoveEntity(pmEntity->GetNiEntityInterface());
}
//---------------------------------------------------------------------------
void MSelectionSet::RemoveAllEntities()
{
    MVerifyValidInstance;

    m_pkSelectionSet->RemoveAllEntities();
}
//---------------------------------------------------------------------------
MEntity* MSelectionSet::GetEntities()[]
{
    MVerifyValidInstance;

    unsigned int uiEntityCount = m_pkSelectionSet->GetEntityCount();

    MEntity* amEntities[] = new MEntity*[uiEntityCount];
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        amEntities[ui] = MEntityFactory::Instance->Get(
            m_pkSelectionSet->GetEntityAt(ui));
    }

    return amEntities;
}
//---------------------------------------------------------------------------
MEntity* MSelectionSet::GetEntityByName(String* strName)
{
    MVerifyValidInstance;

    const char* pcName = MStringToCharPointer(strName);
    NiFixedString kName = pcName;
    MFreeCharPointer(pcName);

    for (unsigned int ui = 0; ui < m_pkSelectionSet->GetEntityCount(); ui++)
    {
        NiEntityInterface* pkEntity = m_pkSelectionSet->GetEntityAt(ui);
        if (pkEntity->GetName() == kName)
        {
            return MEntityFactory::Instance->Get(pkEntity);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
ICommandService* MSelectionSet::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
