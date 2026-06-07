#ifdef QUICKSAVE
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

#ifndef MIDLGPROCQUICKSAVE_H
#define MIDLGPROCQUICKSAVE_H

#include "MiViewerPlugin.h"

//---------------------------------------------------------------------------
class MiDlgProcQuickSave : public ParamMapUserDlgProc 
{
    public:
        MiDlgProcQuickSave(Interface *pIntf) { m_pIntf = pIntf;}
        BOOL DlgProc(TimeValue t,IParamMap *map,
            HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
        void DeleteThis() { NiDelete this; }

    private:
        Interface *m_pIntf;
};
//---------------------------------------------------------------------------

#endif
#endif