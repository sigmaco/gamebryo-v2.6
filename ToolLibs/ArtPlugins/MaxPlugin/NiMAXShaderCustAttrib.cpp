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
#include "NiMAXShader.h"
#include "NiStringTokenizer.h"
#include <NiPoint2.h>
#include <NiPoint3.h>
#include <NiMatrix3.h>

#pragma warning(push)
// unreferenced formal parameter
#pragma warning(disable: 4100) 
// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201) 
// nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable: 4238) 
// nonstandard extension used : 'default argument' : conversion from '' to ' &'
#pragma warning(disable: 4239) 
// 'argument' : conversion from '' to '', signed/unsigned mismatch
#pragma warning(disable: 4245) 
// '' : assignment operator could not be generated
#pragma warning(disable: 4512) 

//#define DEBUG_MAXSCRIPT
#undef STRICT // Avoid a warning b/c Maxscrpt.h defines STRICT too
#include "Maxscrpt.h" 
#include "scripted.h" 
#include "Name.h"
#include "Numbers.h"    
#include "Arrays.h"
#include "Strings.h"
#include "definsfn.h"   // generate static instances from def_x macros for 
                        // MaxScript

#include "IMtlEdit.h"

#pragma warning(pop)

bool NiMAXShader::ms_bAddedRedefinitionCallback = false;

//---------------------------------------------------------------------------
void DebugMaxScript(const char* command)
{
    CHECK_MEMORY();
    FILE* stream = NULL;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    fopen_s(&stream, "NiShaderCustomAttribTemp.ms", "w");
#else //
    stream = fopen("NiShaderCustomAttribTemp.ms", "w");
#endif //
    fprintf( stream, "%s", command);
    fclose( stream );
    open_script("NiShaderCustomAttribTemp.ms");
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
NiString CreateFloatUI(NiString strVarName, NiString strCaption, float fVal, 
    float fMin, float fMax, bool bAcross = false, 
    unsigned int uiAcrossCnt = 1, unsigned int uiFieldWidth = 70, 
    bool bOffset = true)
{
    CHECK_MEMORY();
    NiString strDef = "spinner ";
    strDef += strVarName + " \"" + strCaption + "\" ";
    
    strDef += "type:#float ";
    strDef += "range:[";
    strDef += NiString::FromFloat(fMin) + " , " +
        NiString::FromFloat(fMax) + " , " + 
        NiString::FromFloat(fVal) + "]";
    if (bAcross)
        strDef += " across:" + NiString::FromInt(uiAcrossCnt);
    strDef += " align:#left fieldwidth:" + NiString::FromInt(uiFieldWidth);
    if (bOffset)
        strDef += " offset:[0,6]";


    CHECK_MEMORY();
    return strDef;
}
//---------------------------------------------------------------------------
void AddFloatParam(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString&)
{
    CHECK_MEMORY();
    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";
    float fVal;
    bool bIsHidden = pkAttrDesc->IsHidden();
    pkAttrDesc->GetValue_Float(fVal);
    strParameterDef += strAttrName + " type:#float default:";
    strParameterDef += NiString::FromFloat(fVal);
    if (!bIsHidden)
    {
        strParameterDef += " ui:";
        strParameterDef += strAttrUIName;
    }

    float fMax = 3.40E38f;
    float fMin = -3.40E38f;
    if (pkAttrDesc->IsRanged())
    {
        pkAttrDesc->GetRange_Float(fMin, fMax);
    }

    if (!bIsHidden)
    {
        strRolloutDef += CreateFloatUI(strAttrUIName, strAttrName + ": ",
            fVal, fMin, fMax);
    }
}
//---------------------------------------------------------------------------
void AddPoint2Param(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString& strSetupBodyDef)
{
    CHECK_MEMORY();
    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";
    NiPoint2 kPt;
    NiPoint2 kPtMin(-3.40E38f, -3.40E38f);
    NiPoint2 kPtMax(3.40E38f, 3.40E38f);

    bool bIsHidden = pkAttrDesc->IsHidden();

    if (pkAttrDesc->IsRanged())
    {
        pkAttrDesc->GetRange_Point2(kPtMin, kPtMax);
    }

    pkAttrDesc->GetValue_Point2(kPt);
    strParameterDef += strAttrName + " type:#floatTab tabSize:2 "
        "default:-339999995214436420000000000000000000000.000000 "
        "tabSizeVariable:true";
    if (!bIsHidden)
        strParameterDef += " ui:(";

    if (!bIsHidden)
    {
        strRolloutDef += "label " + strAttrUIName + "Label \"" + strAttrName + 
            "\" align:#left offset:[0,6]\n\t\t";
    }

    unsigned int uiSize = 2;
    for (unsigned int uj = 0; uj < uiSize; uj++)
    {
        if (!bIsHidden)
        {
            strAttrUIName = strAttrName + "UI_";
            strAttrUIName += NiString::FromInt(uj);

            strParameterDef += strAttrUIName;

            if (uj != uiSize -1)
                strParameterDef += " , ";
        }
        float fVal;
        float fMin;
        float fMax;

        if (uj == 0)
        {
            fVal = kPt.x;
            fMin = kPtMin.x;
            fMax = kPtMax.x;
            if (!bIsHidden)
            {
                strRolloutDef += CreateFloatUI(strAttrUIName, "x: ",
                    fVal, fMin, fMax, true, 2, 70, false);
            }
        }
        else
        {
            fVal = kPt.y;
            fMin = kPtMin.y;
            fMax = kPtMax.y;
            if (!bIsHidden)
            {
                strRolloutDef += CreateFloatUI(strAttrUIName, "y: ",
                    fVal, fMin, fMax, false, 1, 70, false);
            }
        }

    
        NiString strSetupName = strAttrName + "[" + 
            NiString::FromInt(uj + 1) + "]";
        strSetupBodyDef += "if " + strSetupName + 
            " == -339999995214436420000000000000000000000.000000 then ";
        strSetupBodyDef += strSetupName + " = " + 
            NiString::FromFloat(fVal) + "\n\t\t";

        if (!bIsHidden)
            strRolloutDef += "\n\t\t";
    }

    if (!bIsHidden)
        strParameterDef += ")";
}
//---------------------------------------------------------------------------
void AddPoint3Param(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString& strSetupBodyDef)
{
    CHECK_MEMORY();
    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";

    NiPoint3 kPt;
    NiPoint3 kPtMin(-3.40E38f, -3.40E38f, -3.40E38f);
    NiPoint3 kPtMax(3.40E38f, 3.40E38f, 3.40E38f);

    bool bIsHidden = pkAttrDesc->IsHidden();

    if (pkAttrDesc->IsRanged())
    {
        pkAttrDesc->GetRange_Point3(kPtMin, kPtMax);
    }

    pkAttrDesc->GetValue_Point3(kPt);

    strParameterDef += strAttrName + " type:#floatTab tabSize:3 "
        "default:-339999995214436420000000000000000000000.000000 "
        "tabSizeVariable:true";
    if (!bIsHidden)
        strParameterDef += " ui:(";
    unsigned int uiSize = 3;

    if (!bIsHidden)
    {
        strRolloutDef += "label " + strAttrUIName + "Label \"" + strAttrName + 
            "\" align:#left offset:[0,6]\n\t\t";
    }

    for (unsigned int uj = 0; uj < uiSize; uj++)
    {
        if (!bIsHidden)
        {
            strAttrUIName = strAttrName + "UI_";
            strAttrUIName += NiString::FromInt(uj);

            strParameterDef += strAttrUIName;

            if (uj != uiSize -1)
                strParameterDef += " , ";
        }
        float fVal;
        float fMin;
        float fMax;

        if (uj == 0)
        {
            fVal = kPt.x;
            fMin = kPtMin.x;
            fMax = kPtMax.x;
            if (!bIsHidden)
            {
                strRolloutDef += CreateFloatUI(strAttrUIName, "x: ",
                    fVal, fMin, fMax, true, 3, 70, false);
            }
        }
        else if (uj == 1)
        {
            fVal = kPt.y;
            fMin = kPtMin.y;
            fMax = kPtMax.y;
            if (!bIsHidden)
            {
                strRolloutDef += CreateFloatUI(strAttrUIName, "y: ",
                        fVal, fMin, fMax, false, 1, 70, false);
            }
        }
        else
        {
            fVal = kPt.z;
            fMin = kPtMin.z;
            fMax = kPtMax.z;
            if (!bIsHidden)
            {
                strRolloutDef += CreateFloatUI(strAttrUIName, "z: ",
                    fVal, fMin, fMax, false, 1, 70, false);
            }
        }
        
        
        NiString strSetupName = strAttrName + "[" + 
            NiString::FromInt(uj + 1) + "]";
        strSetupBodyDef += "if " + strSetupName + 
            " == -339999995214436420000000000000000000000.000000 then ";
        strSetupBodyDef += strSetupName + " = " + 
            NiString::FromFloat(fVal) + "\n\t\t";
        
        if (!bIsHidden)
            strRolloutDef += "\n\t\t";
    }
    
    if (!bIsHidden)
        strParameterDef += ")";

}

//---------------------------------------------------------------------------
void AddPoint4Param(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString& strSetupBodyDef)
{
    CHECK_MEMORY();
    float* pfVal = NiAlloc(float, 4);
    pfVal[0] =  pfVal[1] =  pfVal[2] =  pfVal[3] =  0.0f;
    float* pfMin =  NiAlloc(float, 4);
    pfMin[0] =  pfMin[1] =  pfMin[2] =  pfMin[3] =  -3.40E38f;
    float* pfMax =  NiAlloc(float, 4);
    pfMax[0] =  pfMax[1] =  pfMax[2] =  pfMax[3] =  3.40E38f;
    
    bool bIsHidden = pkAttrDesc->IsHidden();
    if (pkAttrDesc->IsRanged())
    {
        pkAttrDesc->GetRange_Floats(4, pfMin, pfMax);
    }

    pkAttrDesc->GetValue_Point4(pfVal);

    NiString strAttrName = pkAttrDesc->GetName();
    NiString strName = "";
    NiString strAttrUIName = strAttrName + "UI";
    strParameterDef += strAttrName + " type:#floatTab tabSize:4 "
        "default:-339999995214436420000000000000000000000.000000 "
        "tabSizeVariable:true";
    
    if (!bIsHidden)
        strParameterDef += " ui:(";

    if (!bIsHidden)
    {
        strRolloutDef += "label " + strAttrUIName + "Label \"" + strAttrName + 
            "\" align:#left offset:[0,6]\n\t\t";
    }
    unsigned int uiSize = 4;
    for (unsigned int uj = 0; uj < uiSize; uj++)
    {
        if (!bIsHidden)
        {
            strAttrUIName = strAttrName + "UI_";
            strAttrUIName += NiString::FromInt(uj);

            strParameterDef += strAttrUIName;
            
            if (uj != uiSize -1)
                strParameterDef += " , ";
        }

        if (uj == 0)
        {
            strName = "x: ";
            if (!bIsHidden)
            {
                strRolloutDef += CreateFloatUI(strAttrUIName, strName,
                    pfVal[uj], pfMin[uj], pfMax[uj], true, 4, 52, false);
            }
        }
        else
        {
            if (uj == 1)
                strName = "y: ";
            if (uj == 2)
                strName = "z: ";
            if (uj == 3)
                strName = "w: ";

            if (!bIsHidden)
            {
                strRolloutDef += CreateFloatUI(strAttrUIName, strName,
                    pfVal[uj], pfMin[uj], pfMax[uj], false, 1, 52, false);
            }
        }

        NiString strSetupName = strAttrName + "[" + 
            NiString::FromInt(uj + 1) + "]";
        strSetupBodyDef += "if " + strSetupName + 
            " == -339999995214436420000000000000000000000.000000 then ";
        strSetupBodyDef += strSetupName + " = " + 
            NiString::FromFloat(pfVal[uj]) + "\n\t\t";

        if (!bIsHidden)
            strRolloutDef += "\n\t\t";
    }

    if (!bIsHidden)
        strParameterDef += ")";
    NiFree(pfVal);
    NiFree(pfMin);
    NiFree(pfMax);
}
//---------------------------------------------------------------------------
void AddMatrix3Param(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString& strSetupBodyDef)
{
    CHECK_MEMORY();
    NiPoint3 kPtMin(-3.40E38f, -3.40E38f, -3.40E38f);
    NiPoint3 kPtMax(3.40E38f, 3.40E38f, 3.40E38f);
    NiMatrix3 kVal;
    NiMatrix3 kMin(kPtMin, kPtMin, kPtMin);
    NiMatrix3 kMax(kPtMax, kPtMax, kPtMax);

    bool bIsHidden = pkAttrDesc->IsHidden();
    pkAttrDesc->GetValue_Matrix3(kVal);

    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";
    strParameterDef += strAttrName + " type:#floatTab tabSize:9 "
        "default:-339999995214436420000000000000000000000.000000"
        " tabSizeVariable:true";
    if (!bIsHidden)
        strParameterDef += " ui:(";
    
    unsigned int uiSize = 3;
    NiString strName = "";
    
    if (!bIsHidden)
    {           
        strRolloutDef += "label " + strAttrUIName + "Label \"" + strAttrName + 
            "\" align:#left offset:[0,6]\n\t\t";
    }

    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        for (unsigned int uj = 0; uj < uiSize; uj++)
        {
            if (!bIsHidden)
            {
                strAttrUIName = strAttrName + "UI_";
                strAttrUIName += NiString::FromInt(ui) + "_";
                strAttrUIName += NiString::FromInt(uj);

                strParameterDef += strAttrUIName;

                if (ui != uiSize - 1 || uj != uiSize -1)
                    strParameterDef += " , ";
            }

            float fVal = kVal.GetEntry(ui, uj);
            float fMin = kMin.GetEntry(ui, uj);
            float fMax = kMax.GetEntry(ui, uj);

            strName = "(" + NiString::FromInt(ui) + ", " +
                NiString::FromInt(uj) + "): ";

            if (!bIsHidden)
            {       
                if (uj == 0)
                {
                    strRolloutDef += CreateFloatUI(strAttrUIName, strName,
                        fVal, fMin, fMax, true, 3, 52, false);
                }
                else 
                {
                    strRolloutDef += CreateFloatUI(strAttrUIName, strName,
                        fVal, fMin, fMax, false, 1, 52, false);
                }
            }
            NiString strSetupName = strAttrName + "[" + 
                NiString::FromInt(ui*uiSize + uj + 1) + "]";
            strSetupBodyDef += "if " + strSetupName + 
                " == -339999995214436420000000000000000000000.000000 then ";
            strSetupBodyDef += strSetupName + " = " + 
                NiString::FromFloat(fVal) + "\n\t\t";

            if (!bIsHidden)
                strRolloutDef += "\n\t\t";
        }
    }
    if (!bIsHidden)
        strParameterDef += ")";
}

//---------------------------------------------------------------------------
void AddMatrix4Param(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString& strSetupBodyDef)
{
    CHECK_MEMORY();
    NiPoint3 kPtMin(-3.40E38f, -3.40E38f, -3.40E38f);
    NiPoint3 kPtMax(3.40E38f, 3.40E38f, 3.40E38f);
    NiMatrix3 kMatMin(kPtMin, kPtMin, kPtMin);
    NiMatrix3 kMatMax(kPtMax, kPtMax, kPtMax);

    bool bIsHidden = pkAttrDesc->IsHidden();
    float* pfVal = NiAlloc(float, 16);
    float fMin = -3.40E38f;
    float fMax = 3.40E38f;
    
    pkAttrDesc->GetValue_Matrix4(pfVal, 16 * sizeof(float));

    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";
    strParameterDef += strAttrName + " type:#floatTab tabSize:16 "
        "default:-339999995214436420000000000000000000000.000000 "
        "tabSizeVariable:true";
    
    if (!bIsHidden)
        strParameterDef += " ui:(";
    unsigned int uiSize = 4;
    NiString strName = "";
    
    if (!bIsHidden)
    {
        strRolloutDef += "label " + strAttrUIName + "Label \"" + strAttrName + 
            " (Matrix 4x4): \" align:#left offset:[0,6]\n\t\t";
    }
    // Rotation Matrix
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        for (unsigned int uj = 0; uj < uiSize; uj++)
        {
            if (!bIsHidden)
            {
                strAttrUIName = strAttrName + "UI_";
                strAttrUIName += NiString::FromInt(ui) + "_";
                strAttrUIName += NiString::FromInt(uj);

                if (ui == uiSize - 1 && uj == uiSize - 1)
                    strParameterDef += strAttrUIName;
                else
                    strParameterDef += strAttrUIName + ", ";
            }

            float fVal = pfVal[ui*uiSize + uj];
            
            strName = "(" + NiString::FromInt(ui) + ", " +
                NiString::FromInt(uj) + "): ";

            if (!bIsHidden)
            {           
                if (uj == 0)
                {
                    strRolloutDef += CreateFloatUI(strAttrUIName, "",
                        fVal, fMin, fMax, true, 4, 52, false);
                }
                else 
                {
                    strRolloutDef += CreateFloatUI(strAttrUIName, "",
                        fVal, fMin, fMax, false, 1, 52, false);
                }
            }
            
            NiString strSetupName = strAttrName + "[" + 
                NiString::FromInt(ui*uiSize + uj + 1) + "]";
            strSetupBodyDef += "if " + strSetupName + 
                " == -339999995214436420000000000000000000000.000000 then ";
            strSetupBodyDef += strSetupName + " = " + 
                NiString::FromFloat(fVal) + "\n\t\t";

            if (!bIsHidden)
                strRolloutDef += "\n\t\t";
        }
    }

    NiFree(pfVal);
    if (!bIsHidden)
        strParameterDef += ")";
}
//---------------------------------------------------------------------------
void AddBoolParam(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString&)
{
    CHECK_MEMORY();
    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";

    bool bIsHidden = pkAttrDesc->IsHidden();
    bool bValue;
    pkAttrDesc->GetValue_Bool(bValue);
    strParameterDef += strAttrName + " type:#boolean default:";
    if(bValue)
        strParameterDef += "true";
    else
        strParameterDef += "false";
    if (!bIsHidden)
    {
        strParameterDef += " ui:";
        strParameterDef += strAttrUIName;
    }

    if (!bIsHidden)
    {           
        strRolloutDef += "checkbox ";
        strRolloutDef += strAttrUIName + " \"" + 
            strAttrName + "\" ";
        strRolloutDef += "checked: ";
        if (bValue)
            strRolloutDef += "true ";
        else
            strRolloutDef += "false ";

        strRolloutDef += " align:#left offset:[0,6]";
    }
}
//---------------------------------------------------------------------------
void AddColorParam(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString&)
{
    CHECK_MEMORY();
    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";
    NiColorA kValue;
    pkAttrDesc->GetValue_ColorA(kValue);
    
    bool bIsHidden = pkAttrDesc->IsHidden();
    NiString strRGBVal, strAVal;

    strRGBVal += NiString::FromFloat(kValue.r * 255.0f) + " ";
    strRGBVal += NiString::FromFloat(kValue.g * 255.0f) + " ";
    strRGBVal += NiString::FromFloat(kValue.b * 255.0f);
    strAVal += NiString::FromFloat((float) kValue.a);

    strParameterDef += strAttrName + " type:#color default:";
    strParameterDef += "(color "+ strRGBVal + ") ";
    if (!bIsHidden)
        strParameterDef += " ui:" + strAttrUIName;
    strParameterDef += "\n\t\t";
    strParameterDef += strAttrName + "Alpha type:#float default:";
    strParameterDef += strAVal;
    if (!bIsHidden)
    {
        strParameterDef += " ui:";
        strParameterDef += strAttrUIName + "Alpha";
    }

    if (!bIsHidden)
    {               
        strRolloutDef += "colorpicker ";
        strRolloutDef += strAttrUIName + " \"" + strAttrName + 
            ": \" align:#left across:2 offset:[0,6]\n\t\t";
        strRolloutDef += "spinner ";
        strRolloutDef += strAttrUIName + "Alpha \"Alpha: \" ";
        strRolloutDef += "type:#float range:[0, 1, 1] scale:0.05 align:#left"
            " fieldwidth:70 height:16 offset:[0, 9]";
    }
}
//---------------------------------------------------------------------------
void AddStringParam(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString&)
{
    CHECK_MEMORY();
    bool bIsHidden = pkAttrDesc->IsHidden();
    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";
    const char* pcValue;
    pkAttrDesc->GetValue_String(pcValue);

    strParameterDef += strAttrName + " type:#string default:\"";
    strParameterDef += (char*)pcValue;
    strParameterDef += "\" ";
    if (!bIsHidden)
        strParameterDef += "ui:" + strAttrUIName;

    if (!bIsHidden)
    {           
        strRolloutDef += "edittext ";
        strRolloutDef += strAttrUIName + " \"" + 
            strAttrName + ": \" ";
        strRolloutDef += "text:\"";
        strRolloutDef += (char*)pcValue;
        strRolloutDef += "\" align:#left width:275 offset:[0,6]";
    }
}
//---------------------------------------------------------------------------
void AddTextureParam(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString& strSetupBodyDef)
{
    CHECK_MEMORY();
    bool bIsHidden = pkAttrDesc->IsHidden();
    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";
    const char* pcValue;
    unsigned int uiIndex;
    pkAttrDesc->GetValue_Texture(uiIndex, pcValue);

    NIASSERT(uiIndex >= 0);
    NiString strValue = pcValue;
    if (!strValue.IsEmpty() && strValue.FindReverse('\\') ==
        NIPT_INVALID_INDEX)
    {
        NiString strDefaultTexturePath = "Data\\Textures\\";
#if defined(_MSC_VER) && _MSC_VER >= 1400
        char acTemp[NI_MAX_PATH];
        size_t stBufferLength = 0;
        if (getenv_s(&stBufferLength, NULL, 0, "EGB_SHADER_LIBRARY_PATH") == 0
            && stBufferLength > 0)
        {
            getenv_s(&stBufferLength, acTemp, NI_MAX_PATH, 
                "EGB_SHADER_LIBRARY_PATH");
        }
        else
        {
            acTemp[0] = '\0';
        }
        NiString strShaderPath(acTemp);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        NiString strShaderPath = getenv("EGB_SHADER_LIBRARY_PATH");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
        if(strShaderPath.FindReverse('\\') != strShaderPath.Length() - 1)
            strShaderPath += "\\";
        strValue = strShaderPath + strDefaultTexturePath + strValue;
    }

    strValue.TrimLeft(' ');
    strValue.Replace("\\", "\\\\");

    strParameterDef += strAttrName + " type:#textureMap ";
    if (!bIsHidden)
        strParameterDef += "ui:" + strAttrUIName + "\n\t\t";
    else 
        strParameterDef += "\n\t\t";

    strParameterDef += strAttrName + "Index type:#integer animatable:false ";
    strParameterDef += "default: " + NiString::FromInt(uiIndex);
    
    if (!bIsHidden)
    {           
        strRolloutDef += "label " + strAttrUIName + "Label \"" + strAttrName + 
            ":\" across:2 align:#left offset:[0,6]\n\t\t";

        strRolloutDef += "mapButton ";
        strRolloutDef += strAttrUIName + 
            " align:#left width:200 offset:[-50,0]";
    }

    if (!strValue.IsEmpty())
    {
        strSetupBodyDef += "if " + strAttrName + " == undefined do \n\t\t(\n";
        strSetupBodyDef += "\t\t\t" + strAttrName + 
            " = bitmapTexture filename:\"";
        strSetupBodyDef += strValue;
        strSetupBodyDef += "\"\n\t\t\tassignNewName " + strAttrName + 
            "\n\t\t)\n";
    }

    // The texture index must ALWAYS match the one in the definition! 
    // Otherwise the shader system will use the wrong texture and KABOOM!!
    strSetupBodyDef += "\t\t"+ strAttrName + "Index = " + 
        NiString::FromInt(uiIndex) + "\n";

}
//---------------------------------------------------------------------------
void AddArrayParam(const NiShaderAttributeDesc* pkAttrDesc,
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString& strSetupBodyDef)
{
    CHECK_MEMORY();
    NiShaderAttributeDesc::AttributeType eSubType;
    unsigned int uiElementSize;
    unsigned int uiNumElements;
    unsigned int uiBufferSize;
    float* pfValues = NULL;
    float* pfMin = NULL;
    float* pfMax = NULL;
    
    pkAttrDesc->GetArrayParams(eSubType, uiElementSize, uiNumElements);
    uiBufferSize = uiElementSize * uiNumElements;
    unsigned int uiFloatsPerRow = 1;

    switch(eSubType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        uiFloatsPerRow = 1;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        uiFloatsPerRow = 2;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        uiFloatsPerRow = 3;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        uiFloatsPerRow = 4;
        break;
    default:
        return;
        break;
    }

    bool bIsHidden = pkAttrDesc->IsHidden();
    bool bIsRanged = pkAttrDesc->IsRanged();

    pfValues = NiAlloc(float, uiNumElements * uiFloatsPerRow);
    void* pvValuePointer = (void*)pfValues;
    pkAttrDesc->GetValue_Array(pvValuePointer, uiBufferSize);
    
    if (eSubType != NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
    {
        if (bIsRanged)
        {
            pfMin = NiAlloc(float, uiNumElements * uiFloatsPerRow);
            pfMax = NiAlloc(float, uiNumElements * uiFloatsPerRow);
            void* pvMinPointer = (void*)pfMin;
            void* pvMaxPointer = (void*)pfMax;
            if(!pkAttrDesc->GetRange_Array(pvMinPointer, pvMaxPointer, 
                uiBufferSize))
            {
                bIsRanged = false;
            }
        }

        NiString strAttrName = pkAttrDesc->GetName();
        NiString strAttrUIName = strAttrName + "UI";
        strParameterDef += strAttrName + " type:#floatTab tabsize:" + 
        NiString::FromInt(uiNumElements * uiFloatsPerRow) + 
        " default:-339999995214436420000000000000000000000.000000"
            " tabSizeVariable:true";
        if (!bIsHidden)
        {
            strParameterDef += " ui:(";
            strRolloutDef += "label " + strAttrUIName + "Label \"" + 
                strAttrName + "\" align:#left offset:[0,6]\n\t\t";
        }

        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            for (unsigned int uj = 0; uj < uiFloatsPerRow; uj++)
            {
                NiString strName = "";
                unsigned int uiCurrent;
                uiCurrent = ui * uiFloatsPerRow + uj;
                if (!bIsHidden)
                {
                    strAttrUIName = strAttrName + "ArrayUI_";
                    strAttrUIName += NiString::FromInt(uiCurrent);

                    strParameterDef += strAttrUIName;
                    if (uiCurrent != uiNumElements * 
                        uiFloatsPerRow - 1)
                    {
                        strParameterDef += " , ";
                    }
                }
                float fVal = pfValues[uiCurrent];
                float fMin = 0.0f;
                float fMax = 0.0f;
                if (bIsRanged)
                {
                    fMin = pfMin[uiCurrent];
                    fMax = pfMax[uiCurrent];
                }

                if (uiCurrent % uiFloatsPerRow == 0)
                {
                    strName = "(" + NiString::FromInt(ui) + "): ";
                }

                if (!bIsHidden)
                {
                    if (bIsRanged)
                    {
                        strRolloutDef += CreateFloatUI(strAttrUIName, strName,
                            fVal, fMin, fMax, true, uiFloatsPerRow, 52, false);
                    }
                    else
                    {
                        strRolloutDef += CreateFloatUI(strAttrUIName, strName,
                            fVal, -3.40E38f, 3.40E38f, true, uiFloatsPerRow, 
                            52, false);
                    }
                }
                NiString strSetupName = strAttrName + "[" + 
                    NiString::FromInt(uiCurrent + 1) + "]";
                strSetupBodyDef += "if " + strSetupName + 
                    " == -339999995214436420000000000000000000000.000000"
                    " then ";
                strSetupBodyDef += strSetupName + " = " + 
                    NiString::FromFloat(fVal) + "\n\t\t";

                if (!bIsHidden)
                    strRolloutDef += "\n\t\t";
            }
        }
    }
    else    // if type = colorpicker
    {
        NiString strAttrName = pkAttrDesc->GetName();
        NiString strParamDef2 = strAttrName + "Alpha type:#floatTab tabSize:"
            + NiString::FromInt(uiNumElements) + 
            " default:-339999995214436420000000000000000000000.000000 ";
        strParameterDef += strAttrName + " type:#colorTab tabsize:" +
            NiString::FromInt(uiNumElements) + " default:(color 255 255 255) ";
        NiString strAttrUIName = strAttrName + "UI";

        if (!bIsHidden)
        {
            strParameterDef += "ui:(";
            strParamDef2 += "ui:(";
            strRolloutDef += "label " + strAttrUIName + "Label \"" + 
                strAttrName + "\" align:#left offset:[0,6]\n\t\t";
        }
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            NiColorA kValue;
            kValue.r = pfValues[ui * 4];
            kValue.g = pfValues[ui * 4 + 1];
            kValue.b = pfValues[ui * 4 + 2];
            kValue.a = pfValues[ui * 4 + 3];
            
            NiString strRGBVal, strAVal;

            strRGBVal += NiString::FromInt((int)(kValue.r * 255.0f)) + ", ";
            strRGBVal += NiString::FromInt((int)(kValue.g * 255.0f)) + ", ";
            strRGBVal += NiString::FromInt((int)(kValue.b * 255.0f));
            strAVal += NiString::FromFloat((float) kValue.a);

            NiString strSetupName = strAttrName + "[" + 
                NiString::FromInt(ui + 1) + "]";
            NiString strSetupAlphaName = strAttrName + "Alpha[" + 
                NiString::FromInt(ui + 1) + "]";
            strSetupBodyDef += "if " + strSetupAlphaName + 
                    " == -339999995214436420000000000000000000000.000000"
                    " then ";
            strSetupBodyDef += strSetupName + " = " + 
                "[" + strRGBVal + "]\n\t\t";
            strSetupBodyDef += "if " + strSetupAlphaName + 
                    " == -339999995214436420000000000000000000000.000000"
                    " then ";
            strSetupBodyDef += strSetupAlphaName + " = " + 
                strAVal + "\n\t\t";

            if (!bIsHidden)
            {
                strAttrUIName = strAttrName + "ArrayUI_";
                strAttrUIName += NiString::FromInt(ui + 1);
                strParameterDef += strAttrUIName;
                strParamDef2 += strAttrUIName + "Alpha";
                if (ui != uiNumElements - 1)
                {
                    strParameterDef += " , ";
                    strParamDef2 += " , ";
                }
                strRolloutDef += "\n\t\t";
                strRolloutDef += "colorpicker ";
                strRolloutDef += strAttrUIName + " \"(" + NiString::FromInt(ui)
                    + "): \" color:[" + strRGBVal + "] align:#left across:2 "
                    "offset:[0,6]\n\t\t";
                strRolloutDef += "spinner ";
                strRolloutDef += strAttrUIName + "Alpha \"Alpha: \" ";
                strRolloutDef += "type:#float range:[0, 1, 1] scale:0.05 "
                    "align:#left fieldwidth:70 height:16 offset:[0, 9]";
            }
        }
        strParameterDef += ")\n\t\t" + strParamDef2;
        if (!bIsHidden)
            strRolloutDef += "\n\t\t";
    }
    if (!bIsHidden)
        strParameterDef += ")";

    NiFree(pfValues);
    NiFree(pfMin);
    NiFree(pfMax);
}
//---------------------------------------------------------------------------
void AddUnsignedIntParam(const NiShaderAttributeDesc* pkAttrDesc, 
    NiString& strParameterDef, NiString& strRolloutDef, 
    NiString&)
{
    CHECK_MEMORY();
    bool bIsHidden = pkAttrDesc->IsHidden();
    NiString strAttrName = pkAttrDesc->GetName();
    NiString strAttrUIName = strAttrName + "UI";

    unsigned int uiMax = 2147483281;
    unsigned int uiMin = 0;
    if (pkAttrDesc->IsRanged())
    {
        pkAttrDesc->GetRange_UnsignedInt(uiMin, uiMax);

        if (uiMin > 2147483281)
            uiMin = 0;
        if (uiMax <= uiMin || uiMax > 2147483281)
            uiMax = 2147483281;
    }

    unsigned int uiVal;
    pkAttrDesc->GetValue_UnsignedInt(uiVal);

    if (uiVal > 2147483281 || uiVal < uiMin)
        uiVal = uiMin;
    else if (uiVal > uiMax)
        uiVal = uiMax;

    strParameterDef += strAttrName + " type:#integer default:";
    strParameterDef += NiString::FromInt(uiVal);
    if (!bIsHidden)
    {
        strParameterDef += " ui:";
        strParameterDef += strAttrUIName;

        strRolloutDef += "spinner ";
        strRolloutDef += strAttrUIName + " \"" + 
            strAttrName + ": \" ";
    }


    if (!bIsHidden)
    {               
        strRolloutDef += "type:#integer ";
        strRolloutDef += "range:[";
        strRolloutDef += NiString::FromInt(uiMin) + " , " +
            NiString::FromInt(uiMax) + " , " + 
            NiString::FromInt(uiVal) + "] ";
        strRolloutDef += " align:#left fieldwidth:70 offset:[0,6]";
    }
}
//---------------------------------------------------------------------------

void NiMAXShader::RemoveShaderCustAttrib()
{
    CHECK_MEMORY();
    NiString strMaxScript = 
        "include \"Gamebryo\\NiShaderHelpers.ms\"\n"
        "mtlTemp = GetInternalMatchingMaterial meditMaterials[activeMeditSlot]"
        " \"" + GetMtlName() + "\"\n"
        "\n"
        "old_attr_def = undefined\n"
        "for i = 1 to custAttributes.count mtlTemp do\n"
        "(\n"
        "   old_attr_def = custAttributes.getdef mtlTemp i\n"
        "   if old_attr_def.name == \"NiShaderCustAttribs\" do (\n"
        "       custAttributes.makeUnique mtlTemp old_attr_def\n"
        "       custAttributes.delete mtlTemp i\n"
        "   )\n"
        ")\n";

    MAXScriptEvaluate(strMaxScript);
}

//---------------------------------------------------------------------------
NiString NiMAXShader::BuildAttributeDefinition(NiShaderDesc* pkDesc, 
    const char* pcShaderName)
{
    CHECK_MEMORY();
    // attributes "NiShaderCustAttribs"
    // (
    NiString strAttributeDef = "attributes \"";
    strAttributeDef += NI_SHADER_CUST_ATTRIB_NAME;
    strAttributeDef += "\"\n(\n";

    NiString strParameterDef = "\tparameters main rollout:params\n\t(\n";
    NiString strOnCreateHeader = "\ton create do \n\t";
    NiString strOnUpdateHeader = "\ton update do \n\t";
    NiString strSetupBody = "(\n\t\t";
    NiString strRolloutDef   = "\trollout params \"";
    strRolloutDef += (char*)pcShaderName;
    strRolloutDef += " Custom Attributes\"\n\t(\n";

    for (unsigned int ui = 0; ui < pkDesc->GetNumberOfAttributes(); ui++)
    {
        const NiShaderAttributeDesc* pkAttrDesc = NULL;

        if (ui == 0)
            pkAttrDesc = pkDesc->GetFirstAttribute();
        else
            pkAttrDesc = pkDesc->GetNextAttribute();

    
        unsigned int uiSetupBodyLength = strSetupBody.Length();
        if (pkAttrDesc)
        {
            strParameterDef += "\t\t";
            strRolloutDef += "\t\t";
            
            NiString strAttrName = pkAttrDesc->GetName();
            NiString strAttrUIName = strAttrName + "UI";

            NiShaderAttributeDesc::AttributeType eAttrType = 
                pkAttrDesc->GetType();
            CHECK_MEMORY();

            switch(eAttrType)
            {

                case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
                    AddBoolParam(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
                    AddStringParam(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                    AddUnsignedIntParam(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                    AddFloatParam(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;  
                case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                    AddPoint2Param(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                    AddPoint3Param(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                    AddPoint4Param(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                    AddMatrix3Param(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                    AddMatrix4Param(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                    AddColorParam(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
                    AddTextureParam(pkAttrDesc, strParameterDef, 
                        strRolloutDef, strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
                    AddArrayParam(pkAttrDesc, strParameterDef, strRolloutDef,
                        strSetupBody);
                    break;
                case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
                default:
                    break;
            }
            CHECK_MEMORY();

        }
        strParameterDef += "\n";
        strRolloutDef += "\n";

        if (uiSetupBodyLength < strSetupBody.Length())
            strSetupBody += "\n\t\t";
    }

    if (pkDesc->GetNumberOfAttributes() == 0)
    {
        strRolloutDef += "\t\t";
        strRolloutDef += "label NoAttrLabel \"There are no attributes for"
            " this shader\n\t\t\"";
        strRolloutDef += "\n";

    }

    strParameterDef += "\n\t)";
    strRolloutDef += "\n\t)";
    strSetupBody += "\n\t)";
    strAttributeDef += strParameterDef + "\n\n";
    strAttributeDef += strRolloutDef + "\n\n";
    strAttributeDef += strOnCreateHeader + strSetupBody + "\n\n";
    strAttributeDef += strOnUpdateHeader + strSetupBody + "\n\n";
    strAttributeDef += "\n)\n";

    NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();
#ifdef NIDEBUG
    bool bSet = 
#endif
        pkToolkit->SetAppStringForMaterialDesc(pcShaderName, 
        (const char*)strAttributeDef);
    NIASSERT(bSet);
    CHECK_MEMORY();
    return strAttributeDef;
}
//---------------------------------------------------------------------------
void NiMAXShader::UpdateShaderCustAttrib()
{
    CHECK_MEMORY();
    NiString strShaderName = GetShaderName();

    if (strcmp(strShaderName, NI_DEFAULT_SHADER_NAME) == 0)
    {
        RemoveShaderCustAttrib();
 
        return;
    }

    NiString strAttributeDef = GetAttributeDefinition(strShaderName);
        
    if (strAttributeDef.IsEmpty())
        return;

    NiString strMaxScript = 
        "include \"Gamebryo\\NiShaderHelpers.ms\"\n"
        "mtlTemp = GetInternalMatchingMaterial meditMaterials[activeMeditSlot]"
        " \"" + GetMtlName() + "\"\n"
        "if mtlTemp != undefined do\n"
        "(\n"
        "   AddOrRedefineShaderCustomAttributes mtlTemp \"" + strShaderName +
        "\"\n"
        ")\n"
        "\n";

#ifdef DEBUG_MAXSCRIPT
    DebugMaxScript(strAttributeDef);
#endif
    
    if(MAXScriptEvaluate(strMaxScript) == FALSE)
    {
        NiMessageBox("The automatic code generation for this NiShader"
            " failed.\n"
            "Launching MaxScript editor so that you may view the failed"
            " script...",
            "MaxScript generation failure");
        DebugMaxScript(strAttributeDef);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------

NiString NiMAXShader::GetMtlNameContainingThisShader(Mtl* pkMaterial)
{
    CHECK_MEMORY();
    if (pkMaterial == NULL)
        return "";

    if (pkMaterial->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
        pkMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        Shader *pShader = ((NiStdMat *) pkMaterial)->GetShader();
        if (pShader == this)
            return NiString(pkMaterial->GetName());
    }

    if (pkMaterial->IsMultiMtl())
    {
        Mtl *pSub;
        for (int i = 0; i < pkMaterial->NumSubMtls(); i++)
        {
            pSub = pkMaterial->GetSubMtl(i);
            NiString strName = GetMtlNameContainingThisShader(pSub);
            if (!strName.IsEmpty())
                return strName;
        }
    }

    return "";
}
//---------------------------------------------------------------------------

NiStdMat * NiMAXShader::GetMtlContainingThisShader(Mtl* pkMaterial)
{
    CHECK_MEMORY();
    if (pkMaterial == NULL)
        return NULL;

    if (pkMaterial->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
        pkMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        //Shader *pShader = ((NiStdMat *) pkMaterial)->GetShader();
        //if (pShader == this)
            return (NiStdMat *)pkMaterial;
    }

    if (pkMaterial->IsMultiMtl())
    {
        Mtl *pSub;
        for (int i = 0; i < pkMaterial->NumSubMtls(); i++)
        {
            pSub = pkMaterial->GetSubMtl(i);
            StdMat2* pkMtl = GetMtlContainingThisShader(pSub);
            if (pkMtl != NULL)
                return pkMtl;
        }
    }

    return NULL;
}

//---------------------------------------------------------------------------

NiString NiMAXShader::GetMtlName()
{
    CHECK_MEMORY();
    IMtlEditInterface* pkInterface = GetMtlEditInterface();
    if (pkInterface)
    {
        MtlBase * pkMtl = pkInterface->GetCurMtl();
        if (pkMtl && pkMtl->SuperClassID() == MATERIAL_CLASS_ID)
        {
            return GetMtlNameContainingThisShader((Mtl*)pkMtl);
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
NiStdMat * NiMAXShader::GetMtl()
{
    CHECK_MEMORY();
    IMtlEditInterface* pkInterface = GetMtlEditInterface();
    if (pkInterface)
    {
        MtlBase * pkMtl = pkInterface->GetCurMtl();
        if (pkMtl && pkMtl->SuperClassID() == MATERIAL_CLASS_ID)
        {
            return GetMtlContainingThisShader((Mtl*)pkMtl);
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
void NiMAXShader::AddRedefineShaderCustAttribCallback()
{
    CHECK_MEMORY();
    if (ms_bAddedRedefinitionCallback)
        return;

    /*NiString strCallbackMaxScript = 
        "include \"NiShaderHelpers.ms\"\n"
        "ReassignMatchingNiShadersInScene()\n"
        "ReassignMatchingNiShadersInBrowser()\n";

    NiString strMaxScript = "callbacks.addScript " + strCallbackMaxScript + 
        "#filepostopen id:#shaderredef  persistent:false\n"; 
        */
    NiString strMaxScript = "callbacks.addScript #filepostopen filename:"
            "\"Gamebryo\\NiShaderPostOpenCallback.ms\" id:#shaderredef"
            "  persistent:false\n";

    if(MAXScriptEvaluate(strMaxScript) == FALSE)
    {
        NiMessageBox("The automatic code generation for this Gamebryo Shader"
            " failed.\n"
            "Launching MaxScript editor so that you may view the failed"
            " script...",
            "MaxScript generation failure");
        DebugMaxScript(strMaxScript);
    }
    else
    {
        ms_bAddedRedefinitionCallback = true;
    }
    CHECK_MEMORY();

}

//---------------------------------------------------------------------------
NiString NiMAXShader::GetAttributeDefinition(const char* pcShaderName)
{
    CHECK_MEMORY();
    NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();

    if (strcmp(pcShaderName, NI_DEFAULT_SHADER_NAME) == 0)
        return "";

    NiShaderDesc* pkDesc = pkToolkit->GetMaterialDesc(pcShaderName);

    if (!pkDesc)
        return "";

    //if (pkDesc->GetNumberOfAttributes() == 0)
    //    return "";

    NiString strAttributeDef = 
        pkToolkit->GetAppStringForMaterialDesc(pcShaderName);
    
    if (strAttributeDef.IsEmpty())
        strAttributeDef = BuildAttributeDefinition(pkDesc, pcShaderName);

    CHECK_MEMORY();
    return strAttributeDef;
}
//---------------------------------------------------------------------------

Value* get_shader_def_source_cf(Value** arg_list, int count) 
{
    one_typed_value_local(Value* result);
    // check we have 1 arg and that it's a string
    check_arg_count(openFile, 1, count);
    type_check(arg_list[0], String, "GetSourceForShader shadername");
    char* pcShaderName = arg_list[0]->to_string();
    Value* result;

    NiString strDefinition = NiMAXShader::GetAttributeDefinition(pcShaderName);

    if (strDefinition.IsEmpty())
        result = &undefined;
    else
    {
        unsigned int uiLen = strDefinition.Length() + 1;
        char* pcNewString = NiAlloc(char, uiLen);
        NiSprintf(pcNewString, uiLen, "%s", (const char*) strDefinition);
        result = NiExternalNew String(pcNewString);
        NiFree(pcNewString);
    }

    // pop value locals
    return_value(result);
}


def_visible_primitive(get_shader_def_source, "GetSourceForShader");
//---------------------------------------------------------------------------
