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
#include "NiMainPCH.h"

#include "NiVSMBlurPixelProgramDescriptor.h"

//---------------------------------------------------------------------------
NiVSMBlurPixelProgramDescriptor::NiVSMBlurPixelProgramDescriptor()
{
}
//---------------------------------------------------------------------------
unsigned int NiVSMBlurPixelProgramDescriptor::GetUVSetForMap(
    unsigned int uiWhichMap)
{
    switch (uiWhichMap)
    {
    case 0:
        return GetUVSETFORMAP00();
    case 1:
        return GetUVSETFORMAP01();   
    case 2:
        return GetUVSETFORMAP02();    
    case 3:
        return GetUVSETFORMAP03();   
    case 4:
        return GetUVSETFORMAP04();   
    case 5:
        return GetUVSETFORMAP05();   
    case 6:
        return GetUVSETFORMAP06();   
    case 7:
        return GetUVSETFORMAP07();  
    case 8:
        return GetUVSETFORMAP08();  
    case 9:
        return GetUVSETFORMAP09(); 
    case 10:
        return GetUVSETFORMAP10();  
    case 11:
        return GetUVSETFORMAP11();  
              
    default:
        NIASSERT(!"Should never reach here!");
        return UINT_MAX;
        break;
    }

}
//---------------------------------------------------------------------------
void NiVSMBlurPixelProgramDescriptor::SetUVSetForMap(unsigned int uiWhichMap,
    unsigned int uiUVSet)
{
    switch (uiWhichMap)
    {
    case 0:
        SetUVSETFORMAP00(uiUVSet); 
        break;
    case 1:
        SetUVSETFORMAP01(uiUVSet); 
        break;
    case 2:
        SetUVSETFORMAP02(uiUVSet); 
        break;
    case 3:
        SetUVSETFORMAP03(uiUVSet); 
        break; 
    case 4:
        SetUVSETFORMAP04(uiUVSet); 
        break; 
    case 5:
        SetUVSETFORMAP05(uiUVSet); 
        break; 
    case 6:
        SetUVSETFORMAP06(uiUVSet); 
        break; 
    case 7:
        SetUVSETFORMAP07(uiUVSet); 
        break;  
    case 8:
        SetUVSETFORMAP08(uiUVSet); 
        break;  
    case 9:
        SetUVSETFORMAP09(uiUVSet); 
        break;  
    case 10:
        SetUVSETFORMAP10(uiUVSet); 
        break;  
    case 11:
        SetUVSETFORMAP11(uiUVSet); 
        break;  
    default:
        NIASSERT(!"Should never reach here!");
        break;
    }

}
//---------------------------------------------------------------------------
