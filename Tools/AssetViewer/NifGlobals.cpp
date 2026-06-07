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

// NifGlobals.cpp

#include "stdafx.h"

const int g_iParamNotFound = INT_MIN;
const float g_fParamNotFound = -NI_INFINITY;
const CString g_strParamNotFound = "None";
const CString g_strBoundShapePath = "BoundShape.nif";
const CString g_strBoundShapeRootName = "BoundShapeRoot";

bool g_bNeedPromptOnDiscard = false;
unsigned int g_uiMaxFrameRate = 100;
float g_fMinFrameTime = 1.0f / g_uiMaxFrameRate;

bool g_bD3D10 = false;

//---------------------------------------------------------------------------
void MainThreadUpdateAllViews(LPARAM params)
{
    CWinApp* pkApp = AfxGetApp();
    if(!pkApp)
    {
        NiOutputDebugString("No app found in "
            "MainThreadUpdateAllViews(LPARAM params)");
        return;
    }
    CWnd* pkWnd = AfxGetApp()->GetMainWnd();
    if(!pkWnd)
    {
        NiOutputDebugString("No main CWnd* found in "
            "MainThreadUpdateAllViews(LPARAM params)");
        return;
    }
    HWND hWnd = pkWnd->m_hWnd;
    ::PostMessage(hWnd, NIF_BROADCASTUPDATE, 0, params);
}
//---------------------------------------------------------------------------
