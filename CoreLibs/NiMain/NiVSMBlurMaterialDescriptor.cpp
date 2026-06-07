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

#include "NiVSMBlurMaterialDescriptor.h"
#include "NiShadowManager.h"
#include "NiShadowGenerator.h"
#include "NiShadowTechnique.h"
#include "NiAmbientLight.h"
#include "NiDirectionalLight.h"
#include "NiPointLight.h"
#include "NiSpotLight.h"

//---------------------------------------------------------------------------
NiVSMBlurMaterialDescriptor::NiVSMBlurMaterialDescriptor()
{
}
//---------------------------------------------------------------------------
void NiVSMBlurMaterialDescriptor::SetTextureUsage(
    unsigned int uiWhichTexture, unsigned int uiInputUV,
    NiVSMBlurMaterial::TexGenOutput eTextureOutput)
{
    switch (uiWhichTexture)
    {
    default:
        NIASSERT(!"Should not get here!");
        break;
    case 0:
        SetMAP00(uiInputUV); 
        SetMAP00TEXOUTPUT(eTextureOutput);
        break;
    case 1:
        SetMAP01(uiInputUV);
        SetMAP01TEXOUTPUT(eTextureOutput);
        break;
    case 2:
        SetMAP02(uiInputUV); 
        SetMAP02TEXOUTPUT(eTextureOutput);
        break;
    case 3:
        SetMAP03(uiInputUV); 
        SetMAP03TEXOUTPUT(eTextureOutput);
        break;
    case 4:
        SetMAP04(uiInputUV); 
        SetMAP04TEXOUTPUT(eTextureOutput);
        break;
    case 5:
        SetMAP05(uiInputUV); 
        SetMAP05TEXOUTPUT(eTextureOutput);
        break;
    case 6:
        SetMAP06(uiInputUV); 
        SetMAP06TEXOUTPUT(eTextureOutput);
        break;
    case 7:
        SetMAP07(uiInputUV); 
        SetMAP07TEXOUTPUT(eTextureOutput);
        break;
    case 8:
        SetMAP08(uiInputUV); 
        SetMAP08TEXOUTPUT(eTextureOutput);
        break;
    case 9:
        SetMAP09(uiInputUV); 
        SetMAP09TEXOUTPUT(eTextureOutput);
        break;
    case 10:
        SetMAP10(uiInputUV); 
        SetMAP10TEXOUTPUT(eTextureOutput);
        break;
    case 11:
        SetMAP11(uiInputUV); 
        SetMAP11TEXOUTPUT(eTextureOutput);
        break;
    }
}
//---------------------------------------------------------------------------
void NiVSMBlurMaterialDescriptor::GetTextureUsage(
    unsigned int uiWhichTexture, unsigned int& uiInputUV,
    NiVSMBlurMaterial::TexGenOutput& eTextureOutput)
{
    switch (uiWhichTexture)
    {
    default:
        NIASSERT(!"Should not get here!");
        break;
    case 0:
        uiInputUV = GetMAP00();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP00TEXOUTPUT();
        break;
    case 1:
        uiInputUV = GetMAP01();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP01TEXOUTPUT();
        break;
    case 2:
        uiInputUV = GetMAP02();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP02TEXOUTPUT();
        break;
    case 3:
        uiInputUV = GetMAP03();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP03TEXOUTPUT();
        break;
    case 4:
        uiInputUV = GetMAP04();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP04TEXOUTPUT();
        break;
    case 5:
        uiInputUV = GetMAP05();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP05TEXOUTPUT();
        break;
    case 6:
        uiInputUV = GetMAP06();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP06TEXOUTPUT();
        break;
    case 7:
        uiInputUV = GetMAP07();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP07TEXOUTPUT();
        break;
    case 8:
        uiInputUV = GetMAP08();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP08TEXOUTPUT();
        break;
    case 9:
        uiInputUV = GetMAP09();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP09TEXOUTPUT();
        break;
    case 10:
        uiInputUV = GetMAP10();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP10TEXOUTPUT();
        break;
    case 11:
        uiInputUV = GetMAP11();
        eTextureOutput = (NiVSMBlurMaterial::TexGenOutput)
            GetMAP11TEXOUTPUT();
        break;
    }
}
//---------------------------------------------------------------------------
