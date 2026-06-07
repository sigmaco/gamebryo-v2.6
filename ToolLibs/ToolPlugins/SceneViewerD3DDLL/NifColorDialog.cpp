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
#include "SceneViewerDll.h"
#include "NifColorDialog.h"

COLORREF NifColorDialog::ms_akCustColors[NUM_CUST_COLORS];

//---------------------------------------------------------------------------
bool NifColorDialog::GetColor(NiWindowRef hWnd, NiColor& kColor)
{
    CHOOSECOLOR kCol;
    static bool bFirst = true;

    if (bFirst)
    {
        for (int i = 0; i < NUM_CUST_COLORS; i++)
        {
            ms_akCustColors[i] = RGB(0,0,0);
        }
        bFirst = false;
    }

    kCol.lStructSize = sizeof(CHOOSECOLOR);
    kCol.hwndOwner = hWnd;
    kCol.rgbResult = 
        RGB(kColor.r * 255.0f, kColor.g * 255.0f, kColor.b * 255.0f);
    kCol.lpCustColors = ms_akCustColors;
    kCol.Flags = CC_RGBINIT | CC_FULLOPEN;

    BOOL b = ChooseColor(&kCol);
    if (b)
    {
        kColor.r = GetRValue(kCol.rgbResult) / 255.0f;
        kColor.g = GetGValue(kCol.rgbResult) / 255.0f;
        kColor.b = GetBValue(kCol.rgbResult) / 255.0f;
    }

    return b == TRUE ? true : false;
}
//---------------------------------------------------------------------------
