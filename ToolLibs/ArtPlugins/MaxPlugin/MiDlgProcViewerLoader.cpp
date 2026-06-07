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

#include "MaxImmerse.h"
#include "MiDlgProcViewerLoader.h"
//---------------------------------------------------------------------------
BOOL MiDlgProcViewerLoader::DlgProc(TimeValue t, IParamMap* pMap, HWND hWnd,
    UINT msg, WPARAM wParam, LPARAM lParam) 
{
    switch (msg) 
    {
    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDC_VIEWER_LOAD_MESSAGE:
        case IDC_VIEWER_LOAD_PROGRESS:
            m_pUtil->m_pViewerLoaderPMap->Invalidate();
            break;
        }
        break;
    default:
        return FALSE;
    }
    return FALSE; 
}

//---------------------------------------------------------------------------
