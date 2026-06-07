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
#ifdef QUICKSAVE

#include "MiDlgProcRegister.h"
#include "MiExportPlugin.h"

void GamebryoQuickSave()
{
    CHECK_MEMORY();
    GetCOREInterface()->FileSave();

    TSTR pFullName, pPath, pName, pExt, pCurName;
    pFullName = GetCOREInterface()->GetCurFilePath();

    SplitFilename(pFullName, &pPath, &pName, &pExt);
    
    pCurName = pPath + _T("\\") + pName + _T(".nif");
    
    MiExportPlugin kExport;
    kExport.DoExport(pCurName, NULL, GetCOREInterface(), FALSE);
    GetCOREInterface()->FileSave();
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
BOOL MiDlgProcQuickSave::DlgProc(TimeValue, IParamMap*,
    HWND, UINT msg, WPARAM wParam, LPARAM ) 
{
    switch (msg) 
    {
    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case ID_QUICKSAVE:
            {
                GamebryoQuickSave();
            }

            break;              
        }   
        break;
    default:
        return FALSE;
    }
    return FALSE; 
}
//---------------------------------------------------------------------------
Value* DoGamebryoQuickSave_cf(Value**, int count) 
{
    // check we have 1 arg and that it's a string
    check_arg_count(openFile, 0, count);
    GamebryoQuickSave();
    return &ok;
}

def_visible_primitive( DoGamebryoQuickSave,
                      "GamebryoQuickSave");
//---------------------------------------------------------------------------


#endif