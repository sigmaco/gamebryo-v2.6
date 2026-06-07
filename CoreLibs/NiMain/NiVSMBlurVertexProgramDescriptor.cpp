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

#include "NiVSMBlurVertexProgramDescriptor.h"

//---------------------------------------------------------------------------
NiVSMBlurVertexProgramDescriptor::NiVSMBlurVertexProgramDescriptor()
{
}
//---------------------------------------------------------------------------
void NiVSMBlurVertexProgramDescriptor::SetOutputUVUsage(
    unsigned int uiWhichOutputUV, unsigned int uiInputUV,
    NiVSMBlurMaterial::TexGenOutput eTextureOutput)
{
    switch (uiWhichOutputUV)
    {
    default:
        NIASSERT(!"Should not get here!");
        break;
    case 0:
        SetUVSET00(uiInputUV);
        SetUVSET00TEXOUTPUT(eTextureOutput);
        break;
    case 1:
        SetUVSET01(uiInputUV);
        SetUVSET01TEXOUTPUT(eTextureOutput);
        break;
    case 2:
        SetUVSET02(uiInputUV);
        SetUVSET02TEXOUTPUT(eTextureOutput);
        break;
    case 3:
        SetUVSET03(uiInputUV);
        SetUVSET03TEXOUTPUT(eTextureOutput);
        break;
    case 4:
        SetUVSET04(uiInputUV);
        SetUVSET04TEXOUTPUT(eTextureOutput);
        break;
    case 5:
        SetUVSET05(uiInputUV);
        SetUVSET05TEXOUTPUT(eTextureOutput);
        break;
    case 6:
        SetUVSET06(uiInputUV);
        SetUVSET06TEXOUTPUT(eTextureOutput);
        break;
    case 7:
        SetUVSET07(uiInputUV);
        SetUVSET07TEXOUTPUT(eTextureOutput);
        break;
    case 8:
        SetUVSET08(uiInputUV);
        SetUVSET08TEXOUTPUT(eTextureOutput);
        break;
    case 9:
        SetUVSET09(uiInputUV);
        SetUVSET09TEXOUTPUT(eTextureOutput);
        break;
    case 10:
        SetUVSET10(uiInputUV);
        SetUVSET10TEXOUTPUT(eTextureOutput);
        break;
    case 11:
        SetUVSET11(uiInputUV);
        SetUVSET11TEXOUTPUT(eTextureOutput);
        break;
    }
}
//---------------------------------------------------------------------------
void NiVSMBlurVertexProgramDescriptor::GetOutputUVUsage(
    unsigned int uiWhichOutputUV, unsigned int& uiInputUV,
    NiVSMBlurMaterial::TexGenOutput& eTextureOutput)
{
    switch (uiWhichOutputUV)
    {
    default:
        NIASSERT(!"Should not get here!");
        break;
    case 0:
        uiInputUV = GetUVSET00();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET00TEXOUTPUT();
        break;
    case 1:
        uiInputUV = GetUVSET01();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET01TEXOUTPUT();
        break;
    case 2:
        uiInputUV = GetUVSET02();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET02TEXOUTPUT();
        break;
    case 3:
        uiInputUV = GetUVSET03();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET03TEXOUTPUT();
        break;
    case 4:
        uiInputUV = GetUVSET04();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET04TEXOUTPUT();
        break;
    case 5:
        uiInputUV = GetUVSET05();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET05TEXOUTPUT();
        break;
    case 6:
        uiInputUV = GetUVSET06();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET06TEXOUTPUT();
        break;
    case 7:
        uiInputUV = GetUVSET07();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET07TEXOUTPUT();
        break;
    case 8:
        uiInputUV = GetUVSET08();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET08TEXOUTPUT();
        break;
    case 9:
        uiInputUV = GetUVSET09();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET09TEXOUTPUT();
        break;
    case 10:
        uiInputUV = GetUVSET10();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET10TEXOUTPUT();
        break;
    case 11:
        uiInputUV = GetUVSET11();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput) 
            GetUVSET11TEXOUTPUT();
        break;
    }
}
//---------------------------------------------------------------------------
