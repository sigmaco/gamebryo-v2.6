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

#include "TerrainPluginPCH.h"

#include "MBrushCommand.h"


using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

//--------------------------------------------------------------------------
MBrushCommand::MBrushCommand(MEntity* pmEntity) :
    m_pmEntity(pmEntity)
{
    m_pmPreviousCommand = 0;
    m_pmNextCommand = 0;
}
//--------------------------------------------------------------------------
System::String __gc * MBrushCommand::get_Name()
{
    return m_pkCommandInterface->GetName();
}
//--------------------------------------------------------------------------
NiEntityCommandInterface * MBrushCommand::GetNiEntityCommandInterface()
{
    return m_pkCommandInterface;
}
//--------------------------------------------------------------------------
void MBrushCommand::DoCommand(bool, bool bUndoable)
{
    if (!m_pkCommandInterface)
        return;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommandInterface->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    m_pmEntity->set_Dirty(true);
}
//--------------------------------------------------------------------------
void MBrushCommand::UndoCommand(bool)
{
    if (!m_pkCommandInterface)
        return;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommandInterface->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    m_pmEntity->set_Dirty(true);
}
//---------------------------------------------------------------------------
void MBrushCommand::Do_Dispose(bool)
{
    NiDelete(m_pkCommandInterface);
    
    m_pmPreviousCommand = 0;
    m_pmNextCommand = 0;
}
//---------------------------------------------------------------------------
void MBrushCommand::InitializeCommandInterface(
    NiTerrainInteractor* pkTerrain, 
    NiDeformablePointSetPtr spPointSet,
    MBrushType* pmBrush,
    MBrushOperation* pmBrushOperation,
    MFalloff* pmActiveFalloff,
    NiBrushPassPointInfo* pkPointInfo,
    NiPoint3 kIntersection,
    float fBrushSize,
    float fTerrainScale,
    NiPoint3* pkListOfIntersections,
    int iNumberOfIntersections,
    int iCurrentIndex)
{
    m_pkCommandInterface = NiNew NiBrushCommandInterface(pkTerrain, spPointSet,
        pmBrush, pmBrushOperation, pmActiveFalloff, 
        pkPointInfo, kIntersection, fBrushSize, fTerrainScale, 
        pkListOfIntersections, iNumberOfIntersections, iCurrentIndex);
}
//---------------------------------------------------------------------------
void MBrushCommand::SetNextCommand(MBrushCommand* pmCommand)
{
    m_pmNextCommand = pmCommand;
    m_pkCommandInterface->SetNextCommand
        (dynamic_cast<NiBrushCommandInterface*>
        (pmCommand->GetNiEntityCommandInterface()));
}
//---------------------------------------------------------------------------
void MBrushCommand::SetPreviousCommand(MBrushCommand* pmCommand)
{
    m_pmPreviousCommand = pmCommand;
    m_pkCommandInterface->SetPreviousCommand
        (dynamic_cast<NiBrushCommandInterface*>
        (pmCommand->GetNiEntityCommandInterface()));
}
//---------------------------------------------------------------------------
MBrushCommand* MBrushCommand::GetNextCommand()
{
    if (m_pmNextCommand)
        return m_pmNextCommand;

    return NULL;
}
//---------------------------------------------------------------------------
MBrushCommand* MBrushCommand::GetPreviousCommand()
{
    if (m_pmPreviousCommand)
        return m_pmPreviousCommand;

    return NULL;
}
//---------------------------------------------------------------------------
NiUInt32 MBrushCommand::GetSize()
{
    return m_pkCommandInterface->GetSize();
}
//---------------------------------------------------------------------------
void MBrushCommand::ReinitialiseCommand()
{
    if (m_pkCommandInterface)
    {
        NiDelete(m_pkCommandInterface);
        m_pkCommandInterface = 0;
    }
}
