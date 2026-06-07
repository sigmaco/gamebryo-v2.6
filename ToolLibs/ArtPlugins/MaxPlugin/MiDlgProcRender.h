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

#ifndef MIDLGPROCRENDER_H
#define MIDLGPROCRENDER_H

#include "MiViewerPlugin.h"
#define WM_HIDEVIEW WM_USER+10

//---------------------------------------------------------------------------
class MiDlgProcRenderer : public ParamMapUserDlgProc 
{
    public:
        MiDlgProcRenderer(MiViewerPlugin *pUtil) {m_pUtil = pUtil;}
        BOOL DlgProc(TimeValue t,IParamMap *map,
            HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
        inline void DeleteThis() { NiDelete this; }
        void SetStripifyState(HWND hWnd);
        void DoOptionsDialog(UINT uiWhichOptions, HWND hWnd);
    private:
        bool IsWindowsNT();

        MiViewerPlugin *m_pUtil;
};
//---------------------------------------------------------------------------

#endif
