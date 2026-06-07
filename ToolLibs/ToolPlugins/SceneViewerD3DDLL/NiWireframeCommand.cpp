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
#include "stdafx.h"  
#include "NiWireframeCommand.h"
#include "NiMain.h"

#if defined (WIN32)
#include "windows.h"
#endif

NiWindowRef NiWireframeCommand::ms_kWnd = 0;
int NiWireframeCommand::ms_iMsgID = 0;
NiCommandID NiWireframeCommand::ms_kID = 0;
//---------------------------------------------------------------------------
NiWireframeCommand::NiWireframeCommand()
{
}
//---------------------------------------------------------------------------
bool NiWireframeCommand::Apply(NiSceneViewer* pkViewer)
{
    NiNode* pkRoot = pkViewer->GetScene();
    NiWireframeProperty* pkWireframe = GetWireframeProperty(pkRoot);
    if (pkWireframe == NULL)
    {
        pkWireframe = NiNew NiWireframeProperty();
        pkRoot->AttachProperty(pkWireframe);
    }

    pkWireframe->SetWireframe(!pkWireframe->GetWireframe());

#if defined (WIN32)
    if (ms_kWnd != 0)
    {
        SendMessage(ms_kWnd, (UINT) ms_iMsgID, pkWireframe->GetWireframe(), 0);
    }
#endif
    
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiWireframeCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiWireframeCommand::~NiWireframeCommand()
{

}
//---------------------------------------------------------------------------
NiWireframeProperty* NiWireframeCommand::GetWireframeProperty(NiNode* scene)
{
    // properties
    NiProperty* pkProp;
    NiPropertyList* pkPropList;

    pkPropList = &scene->GetPropertyList();
    NiTListIterator kIter = pkPropList->GetHeadPos();
    while (kIter)
    {
        pkProp = pkPropList->GetNext(kIter);
        if (NiIsKindOf(NiWireframeProperty, pkProp))
            return (NiWireframeProperty*)pkProp;
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiSceneCommand* NiWireframeCommand::Create(NiSceneCommandInfo*)
{
    return NiNew NiWireframeCommand();
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiWireframeCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Toggle Wireframe Mode");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiWireframeCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("WIREFRAME",
        NiWireframeCommand::Create);
}
//---------------------------------------------------------------------------
void NiWireframeCommand::RegisterListener(NiWindowRef kWnd, int iMsgID)
{
    ms_kWnd = kWnd;
    ms_iMsgID = iMsgID;
}
//---------------------------------------------------------------------------
