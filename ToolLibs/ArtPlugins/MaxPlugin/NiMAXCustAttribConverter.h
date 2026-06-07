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

#ifndef NIMAXCUSTATTRIBCONVERTER_H
#define NIMAXCUSTATTRIBCONVERTER_H

#include "NiMAX.h"
#include "NiMAXConverter.h"
#include "NiMAXUV.h"
class CustAttrib;

//---------------------------------------------------------------------------
class NiMAXCustAttribConverter
{
public:
    NiMAXCustAttribConverter(TimeValue animStart, TimeValue animEnd);
    ~NiMAXCustAttribConverter();

    static void Init();
    static void Shutdown();
    
    void Convert(Animatable *obj, NiObjectNET* pkNiObj, 
        bool bIgnoreStandardCAs, NiTexturingProperty* pkTexProp = NULL,
        NiMAXUV* pkNiMAXUV =NULL);

    bool ConvertParamBlock(IParamBlock2* pkParamBlock, NiObjectNET* pkNiObj, 
        NiTexturingProperty* pkTexProp = NULL, NiMAXUV* pkNiMAXUV =NULL);

    static void GetUVInfo(Mtl* pkMaterial, TimeValue kAnimStart,
        NiMAXUV*& pkNiMAXUV, int iAttrId, bool bFirstMulti = true);

    bool ConvertTexmap(
        Texmap* pkTm, 
        NiTexture::FormatPrefs& kPrefs, 
        unsigned int& uiSizeDivideBy, 
        NiTObjectArray<NiString>*& pkFilenames, 
        bool& bExternalImages, 
        bool& bOrignalImages, 
        bool& bOrignalImagesVRAM, 
        NiTexturingProperty::FilterMode& eFilterMode, 
        NiPixelFormat& ePixelFormat, 
        bool bImageHasAlpha,
        unsigned short& usMaxAnisotropy);

protected:
    NiExtraData* ConvertFloatAttrib(IParamBlock2* pkParamBlock, int iIndex, 
        NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertNodeAttrib(IParamBlock2* pkParamBlock, int iIndex, 
        NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertStringAttrib(IParamBlock2* pkParamBlock, int iIndex, 
        NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertColorAttrib(IParamBlock2* pkParamBlock, int iIndex, 
        NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertPoint3Attrib(IParamBlock2* pkParamBlock, int iIndex, 
        NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertPoint4Attrib(IParamBlock2* pkParamBlock, int iIndex, 
        NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertFRGBAAttrib(IParamBlock2* pkParamBlock, int iIndex, 
        NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertMatrix3Attrib(IParamBlock2* pkParamBlock, 
        int iIndex, NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertIntAttrib(IParamBlock2* pkParamBlock, 
        int iIndex, NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertBoolAttrib(IParamBlock2* pkParamBlock, 
        int iIndex, NiObjectNET* pkObj, int& iIncrement); 
    NiExtraData* ConvertFloatTabAttrib(IParamBlock2* pkParamBlock, 
        int iIndex, NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertColorTabAttrib(IParamBlock2* pkParamBlock,
        int iIndex, NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertIntTabAttrib(IParamBlock2* pkParamBlock, 
        int iIndex, NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertStringTabAttrib(IParamBlock2* pkParamBlock, 
        int iIndex, NiObjectNET* pkObj, int& iIncrement);
    NiExtraData* ConvertTexMapAttrib(IParamBlock2* pkParamBlock, int iIndex,
        NiObjectNET* pkObj, NiTexturingProperty*& pkTexProp,
        NiMAXUV* pkNiMAXUV, int& iIncrement);


    bool ConvertTextureExportCA(
        CustAttrib * pkAttribute, 
        NiTexture::FormatPrefs& kPrefs, 
        unsigned int& uiSizeDivideBy, 
        bool& bExternalImages, 
        bool& bOrignalImages, 
        bool& bOrignalImagesVRAM, 
        NiTexturingProperty::FilterMode& eFilterMode, 
        NiPixelFormat& ePixelFormat, 
        bool bImageHasAlpha,
        unsigned short& uiMaxAnisotropy);

    bool ConvertMipMapPyramidCA(CustAttrib * pkAttribute,  
        NiTexture::FormatPrefs& kPrefs, 
        NiTObjectArray<NiString>*& pkFilenames);

    TimeValue m_kAnimStart;
    TimeValue m_kAnimEnd;

protected:
    void CreateColorCustAttribAnimator(Control *pColorCont, 
        Control *pAlphaCont, NiObjectNET* pkObj, 
        NiColorExtraData* pkColorED);
    
    void CreateColorArrayCustAttribAnimator(
        Control *pColorCont, Control *pAlphaCont, NiObjectNET* pkObj,
        NiFloatsExtraData* pkColorED, unsigned int uiStartIdx);

    static bool IsStandardCA(const char* pcString);
    static bool IsMaxCA(const char* pcString);
};

//---------------------------------------------------------------------------

#endif
