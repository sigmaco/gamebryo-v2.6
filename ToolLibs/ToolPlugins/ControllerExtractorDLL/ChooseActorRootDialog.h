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

#ifndef CHOOSEACTORROOTDIALOG_H
#define CHOOSEACTORROOTDIALOG_H

class ChooseActorRootDialog : public NiDialog
{
public:
    ChooseActorRootDialog(NiModuleRef hInstance, NiWindowRef hParent);

    NiTPrimitiveSet<NiAVObject*>* m_pkActorRoots;
    NiAVObject* m_pkSelectedActorRoot;

protected:
    virtual void InitDialog();
    virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);
    virtual void OnClose();

    void HandleOK();
};

#endif  // #ifndef CHOOSEACTORROOTDIALOG_H
