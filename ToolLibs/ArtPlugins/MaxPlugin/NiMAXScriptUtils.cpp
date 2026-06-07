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

#include "NiMaxScriptUtils.h"
#include <NiMeshProfileProcessor.h>
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
#include <guplib.h>

#include <custattrib.h>
#include <icustattribcontainer.h>

#include "maxscrpt\Strings.h"
#include "maxscrpt\Parser.h"

#pragma warning(pop)

//---------------------------------------------------------------------------
static NiMAXScriptUtils* s_pkMAXScriptUtils = NULL;
//---------------------------------------------------------------------------
NiMAXScriptUtils::NiMAXScriptUtils()
{
    bool bNeedsShutdown = false;
    if (!NiShadowManager::GetShadowManager())
    {
        NiShadowManager::Initialize();
        bNeedsShutdown = true;
    }

    for (NiUInt32 ui = 0; ui < NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES;
        ui++)
    {
        NiShadowTechnique* pkShadowTechnique =
            NiShadowManager::GetKnownShadowTechnique((unsigned short)ui);
        if (pkShadowTechnique)
        {
            m_apShadowTechniqueNames[ui] = 
                NiStrdup(pkShadowTechnique->GetName());
        }
        else
        {
            m_apShadowTechniqueNames[ui] = NULL;
        }
    }

    if (bNeedsShutdown)
    {
        NiShadowManager::Shutdown();
    }
}
//---------------------------------------------------------------------------
NiMAXScriptUtils::~NiMAXScriptUtils()
{
    for (NiUInt32 ui = 0; ui < NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES;
        ui++)
    {
        NiFree(m_apShadowTechniqueNames[ui]);
        m_apShadowTechniqueNames[ui] = NULL;
    }
}
//---------------------------------------------------------------------------
void NiMAXScriptUtils::Init()
{
    NIASSERT(!s_pkMAXScriptUtils);
    s_pkMAXScriptUtils = NiExternalNew NiMAXScriptUtils;
}
//---------------------------------------------------------------------------
void NiMAXScriptUtils::Shutdown()
{
    NiExternalDelete s_pkMAXScriptUtils;
    s_pkMAXScriptUtils = NULL;
}
//---------------------------------------------------------------------------
Value* NiMAXScriptUtils::GetShadowTechniqueNames()
{
    NIASSERT(s_pkMAXScriptUtils);
    Value* pkReturnValue = NiExternalNew 
        Array(NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES);
    Array* pkArray = (Array*)pkReturnValue;

    // collect a list of strings from the shadow manager
    for (unsigned short us = 0;
        us < NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES; us++)
    {
        if (s_pkMAXScriptUtils->m_apShadowTechniqueNames[us])
        {
            Value* pTechniqueName = NiExternalNew 
                String(s_pkMAXScriptUtils->m_apShadowTechniqueNames[us]);
            pkArray->append(pTechniqueName);
        }
    }
    return pkReturnValue;
}
//---------------------------------------------------------------------------
Value* LocalExecuteScript(CharStream* source, bool *res) 
{

    *res = true;

    init_thread_locals();
    push_alloc_frame();
    three_typed_value_locals(Parser* parser, Value* code, Value* result);
    CharStream* out = thread_local(current_stdout);
    vl.parser = NiExternalNew Parser (out);
    
    try {

        source->flush_whitespace();
        while (!source->at_eos()) {
            vl.code     = vl.parser->compile(source);
            vl.result   = vl.code->eval()->get_heap_ptr();
            source->flush_whitespace();
        }
        source->close();

    } catch (...) {
        *res = false;
    }

    if (vl.result == NULL)
        vl.result = &ok;
    
    pop_alloc_frame();
    return_value(vl.result);

}

//---------------------------------------------------------------------------
BOOL MAXScriptEvaluate(const char *command)
{
/*    CHECK_MEMORY();
    GUP *gp = (GUP*)CreateInstance( GUP_CLASS_ID,Class_ID(470000002,0));
    unsigned int uiSize = strlen(command);
    char* pcString = NiAlloc(char, uiSize + 1);
    sprintf(pcString, "%s", command);
    BOOL bReturn = gp->ExecuteStringScript(pcString);
    gp->DeleteThis();
    NiFree(pcString);
    CHECK_MEMORY();
    return bReturn;*/
    TCHAR *s = (TCHAR*)command;
    static bool script_initialized = false;
    if (!script_initialized) {
        init_MAXScript();
        script_initialized = TRUE;
    }
    init_thread_locals();

    push_alloc_frame();
    two_typed_value_locals(StringStream* ss, Value* result);

    vl.ss = NiExternalNew StringStream (s);
    bool res = false;
    try {
        vl.result = LocalExecuteScript(vl.ss,&res);
    } catch (...) {
        return FALSE;
    }
    thread_local(current_result) = vl.result;
    thread_local(current_locals_frame) = vl.link;
    pop_alloc_frame();
    return TRUE;
}
//---------------------------------------------------------------------------
Value* NiAddBlankNoteKey_cf(Value** arg_list, int count) 
{
    check_arg_count("NiAddKey", 2, count);

    Value* pkNoteTrackTabValue = arg_list[0];
    Value* pkTimeValue = arg_list[1];

    if (!pkNoteTrackTabValue || !pkTimeValue)
        return &ok;

    if (pkNoteTrackTabValue == &undefined ||  
        pkNoteTrackTabValue == &unsupplied ||
        pkTimeValue == &undefined ||  
        pkTimeValue == &unsupplied)
    {
        return &ok;
    }

    TimeValue kTime = pkTimeValue->to_timevalue();

    ReferenceTarget* pkNoteTrackRefTarget = pkNoteTrackTabValue->to_reftarg();
    Class_ID cID = pkNoteTrackRefTarget->ClassID();
    if (cID == Class_ID(NOTETRACK_CLASS_ID,0))
    {
        DefNoteTrack* pkNoteTrack = (DefNoteTrack*) pkNoteTrackRefTarget;
        pkNoteTrack->AddNewKey(kTime, 0);
    }

    return &ok;
}

def_visible_primitive(NiAddBlankNoteKey, "NiAddBlankNoteKey");
//---------------------------------------------------------------------------
Value* NiGetNoteKeyCount_cf(Value** arg_list, int count) 
{
    one_typed_value_local(Value* result);
    check_arg_count("NiGetNoteKeyCount", 1, count);

    Value* pkNoteTrackTabValue = arg_list[0];

    if (!pkNoteTrackTabValue)
    {
        Value* result = NiExternalNew Integer(0);
        return_value(result);
    }

    if (pkNoteTrackTabValue == &undefined ||  
        pkNoteTrackTabValue == &unsupplied)
    {
        Value* result = NiExternalNew Integer(0);
        return_value(result);
    }

    ReferenceTarget* pkNoteTrackRefTarget = pkNoteTrackTabValue->to_reftarg();
    Class_ID cID = pkNoteTrackRefTarget->ClassID();
    int iNumKeys = 0;
    if (cID == Class_ID(NOTETRACK_CLASS_ID,0))
    {
        DefNoteTrack* pkNoteTrack = (DefNoteTrack*) pkNoteTrackRefTarget;
        iNumKeys = pkNoteTrack->NumKeys();
    }

    Value* result = NiExternalNew Integer(iNumKeys);
    return_value(result);
}

def_visible_primitive(NiGetNoteKeyCount, "NiGetNoteKeyCount");
//---------------------------------------------------------------------------
Value* NiGetCustAttrib_cf(Value** arg_list, int count) 
{
    two_typed_value_locals(Value* result, Value* item);

    Value* pkCAObj = arg_list[0];
    Value* pkCAName = arg_list[1];
    // there is an optional third argument
    Value* pkCAType = NULL;
    if (count == 3)
        pkCAType = arg_list[2];

    // Make sure the arguments exist
    if (!pkCAName || !pkCAObj)
    {
        return_value(&undefined);
    }

    // Make sure that they are valid
    if (pkCAName == &undefined || pkCAObj == &undefined ||
        pkCAName == &unsupplied || pkCAObj == &unsupplied )
    {
        return_value(&undefined);
    }

    // Convert the second argument to its C string representation
    const char* pcName = NULL;
    const char* pcType = NULL;
    int iIndex = 0;
    if (is_int(pkCAName))
    {
        // if the second arg is an int, then we have the alternate call method
        // the primary way is: NiGetCustAttrib obj attribName
        // in the case that the name is not known, find by index using type:
        // NiGetCustAttrib obj index CAType.  An example type is 
        // "NiShaderCustAttribs"
        iIndex = pkCAName->to_int();
        pcType = pkCAType->to_string();
    }
    else
    {
        pcName = pkCAName->to_string();
    }

    // Convert the first argument to a MAX object
    ReferenceTarget* pkCAObjRefTarget = pkCAObj->to_reftarg();

    if (pkCAObjRefTarget == NULL)
    {
        return_value(&undefined);
    }

    // Check out any custom attributes on the object
    Class_ID cID = pkCAObjRefTarget->ClassID();
    ICustAttribContainer* cc = pkCAObjRefTarget->GetCustAttribContainer();
    if ( cc ) 
    {
        int num_attribs = cc->GetNumCustAttribs();
        CustAttrib * pkAttribute = NULL;

        for (int attrib = 0; attrib < num_attribs; attrib ++ )
        {
            // Get the custom attribute at this location in the array
            // and check its parameter blocks
            pkAttribute = (CustAttrib *) cc->GetCustAttrib( attrib );
            if (pkAttribute)
            {
                if (pcType != NULL)
                {
                    // if we are searching by index, make sure that we are
                    // searching the right CA group
                    TSTR strName = pkAttribute->GetName();
                    if (NiStricmp((char*) strName, pcType) != 0)
                        continue;
                }
                Class_ID cId = pkAttribute->ClassID();

                for (int iWhichPB = 0; iWhichPB < 
                     pkAttribute->NumParamBlocks(); iWhichPB++)
                {
                    IParamBlock2* pkParamBlock = 
                        pkAttribute->GetParamBlock(iWhichPB);
                    if (pkParamBlock != NULL)
                    {
                        // Get the parameter block and check its
                        // values one-by-one to see if they match
                        // the name
                        pkParamBlock->GetDesc();
                        int num_params = pkParamBlock->NumParams();

                        int param_type;

                        for ( int i = 0 ; i < num_params; i++ )
                        {
                            param_type = 
                                pkParamBlock->GetParameterType((ParamID) i );
                            
                            if (pcName != NULL)
                            {
                                // if we are searching by attribute name
                                TSTR strName = pkParamBlock->GetLocalName((ParamID) i );
                                if (NiStricmp((char*) strName, pcName) != 0)
                                    continue;
                            }
                            else
                            {
                                // if we are searching by attribute index
                                if (i != iIndex)
                                    continue;
                            }

                            // Now that we have a matching name, convert its 
                            // value into a MaxScript-wrapped value and 
                            // return
                            Interval kValid;
                            switch ( param_type ) 
                            {
                                case TYPE_INODE:
                                {
                                    INode* pkValue;
                                    if (pkParamBlock->GetValue((ParamID)i,0, pkValue,
                                        kValid))
                                    {
                                        if (pkValue == NULL)
                                        {
                                            return_value(&undefined);
                                        }

                                        Value* result = NiExternalNew 
                                            MAXNode(pkValue);
                                        NiOutputDebugString("Converted "
                                            "INODE cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_STRING:
                                {
                                    TCHAR* pcValue;
                                    if (pkParamBlock->GetValue((ParamID)i,0, pcValue, 
                                        kValid))
                                    {
                                        Value* result = NiExternalNew 
                                            String(pcValue);
                                        NiOutputDebugString("Converted "
                                            "STRING cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_FLOAT:
                                {
                                    float fValue;
                                    if (pkParamBlock->GetValue((ParamID)i,0, fValue,
                                        kValid))
                                    {
                                        Value* result = NiExternalNew 
                                            Float(fValue);
                                        NiOutputDebugString("Converted"
                                            " FLOAT cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_INT:
                                {
                                    int iValue;
                                    if (pkParamBlock->GetValue((ParamID)i,0, iValue,
                                        kValid))
                                    {
                                        Value* result = NiExternalNew
                                            Integer(iValue);
                                        NiOutputDebugString("Converted"
                                            " INT cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_RGBA:
                                {
                                    Color kValue;
                                    if (pkParamBlock->GetValue((ParamID)i,0, kValue,
                                        kValid))
                                    {
                                        Value* result=NiExternalNew 
                                            ColorValue(kValue);
                                        NiOutputDebugString("Converted"
                                            "RGBA cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_POINT3:
                                {
                                    Point3 kValue;
                                    if (pkParamBlock->GetValue((ParamID)i,0, kValue,
                                        kValid))
                                    {
                                        Value* result = 
                                            NiExternalNew Point3Value(kValue);
                                        NiOutputDebugString("Converted"
                                            " POINT3 cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_POINT4:
                                case TYPE_FRGBA:
                                {
                                    Point4 kValue;
                                    if (pkParamBlock->GetValue((ParamID)i,0, kValue,
                                        kValid))
                                    {
                                        Value* result = 
                                            NiExternalNew Point4Value(kValue);
                                        NiOutputDebugString("Converted"
                                            " POINT4 or FRGBA cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_BOOL:
                                {
                                    BOOL bValue;
                                    if (pkParamBlock->GetValue((ParamID)i,0, bValue,
                                        kValid))
                                    {
                                        NiOutputDebugString("Converted"
                                            "BOOL cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        
                                        if (bValue == TRUE)
                                        {
                                            return_value(&true_value);
                                        }
                                        else
                                        {
                                            return_value(&false_value);
                                        }
                                    }
                                    break;
                                }
                                case TYPE_INT_TAB:
                                {
                                    int iCount = pkParamBlock->Count((ParamID)i);
                                    if (iCount > 0)
                                    {
                                        int iValue;
                                        Value* result = NiExternalNew 
                                            Array(iCount);
                                        Array* pkArray = (Array*)result;
                                        
                                        for (int iSub = 0; iSub < iCount;
                                            iSub++)
                                        {
                                            if (pkParamBlock->GetValue((ParamID)i,
                                                0, iValue, kValid, iSub))
                                            {
                                                Value* item = NiExternalNew 
                                                    Integer(iValue);
                                                pkArray->append(item);
                                            }
                                            else
                                            {
                                                pkArray->append(&undefined);
                                            }
                                        }
                                        NiOutputDebugString("Converted"
                                            " INT_TAB cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_BITMAP_TAB:
                                {
                                    int iCount = pkParamBlock->Count((ParamID)i);
                                    if (iCount > 0)
                                    {
                                        Value* result = NiExternalNew 
                                            Array(iCount);
                                        Array* pkArray = (Array*)result;
                                        for (int iSub = 0; iSub < iCount;
                                            iSub++)
                                        {
                                            PBBitmap* pkBitmap = 
                                                pkParamBlock->GetBitmap((ParamID)i, 
                                                0, iSub);
                                            if (pkBitmap)
                                            {
                                                Value* item = NiExternalNew 
                                                    MAXBitMap(pkBitmap->bi, 
                                                    pkBitmap->bm);
                                                pkArray->append(item);
                                            }
                                            else
                                            {
                                                pkArray->append(&undefined);
                                            }
                                        }
                                        NiOutputDebugString("Converted"
                                            " BITMAP_TAB cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        
                                        return_value(result);
                                    }
                                    break;
                                }
                                case TYPE_TEXMAP:
                                {
                                    Texmap* pkValue = NULL;
                                    if (pkParamBlock->GetValue((ParamID)i,0, pkValue,
                                        kValid))
                                    {
                                        if (pkValue == NULL)
                                        {
                                            return_value(&undefined);
                                        }

                                        Value* result = 
                                            MAXTexture::intern(pkValue);
                                        NiOutputDebugString("Converted "
                                            "INODE cust attrib:");
                                        NiOutputDebugString(pcName);
                                        NiOutputDebugString("\n");
                                        return_value(result);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // We failed finding the value, just return undefined
    NiOutputDebugString("WARNING! \"NiGetCustAttrib\" Could not find"
        " or didn't convert: ");
    NiOutputDebugString(pcName);
    NiOutputDebugString("\n");

    return_value(&undefined);
}

def_visible_primitive( NiGetCustAttrib,
                      "NiGetCustAttrib");
//---------------------------------------------------------------------------
Value* NiSetCustAttrib_cf(Value** arg_list, int count)
{
    two_typed_value_locals(Value* result, Value* item);
    check_arg_count("NiSetCustAttrib", 4, count);

    Value* pkCAObj = arg_list[0];       //object to set the CustAttribs of
    Value* pkCAName = arg_list[1];      //name of the CustAttrib set to analyze
    Value* pkCAIndex = arg_list[2];     //index into the CustAttrib's Paramblk
    Value* pkCANewValue = arg_list[3];  //value to set it to

    // Make sure the arguments exist
    if (!pkCAName || !pkCAObj || !pkCAIndex || !pkCANewValue)
    {
        return_value(&undefined);
    }

    // Make sure that they are valid
    if (pkCAName == &undefined || pkCAObj == &undefined ||
        pkCAName == &unsupplied || pkCAObj == &unsupplied ||
        pkCAIndex == &undefined || pkCAIndex == &unsupplied ||
        pkCANewValue == &undefined || pkCAIndex == &unsupplied)
    {
        return_value(&undefined);
    }

    // Convert the second argument to its C string representation
    const char* pcName = pkCAName->to_string();
    int iIndex = pkCAIndex->to_int();

    // Convert the first argument to a MAX object
    ReferenceTarget* pkCAObjRefTarget = pkCAObj->to_reftarg();

    if (pkCAObjRefTarget == NULL)
    {
        return_value(&undefined);
    }

    // Check out any custom attributes on the object
    Class_ID cID = pkCAObjRefTarget->ClassID();
    ICustAttribContainer* cc = pkCAObjRefTarget->GetCustAttribContainer();
    if (cc) 
    {
        int num_attribs = cc->GetNumCustAttribs();
        CustAttrib * pkAttribute = NULL;

        for (int attrib = 0; attrib < num_attribs; attrib ++ )
        {
            // Get the custom attribute at this location in the array
            // and check its parameter blocks
            pkAttribute = (CustAttrib *) cc->GetCustAttrib(attrib);
            if (pkAttribute)
            {
                // search for attribute by name
                TSTR strName = pkAttribute->GetName();
                if (NiStricmp((char*) strName, pcName) != 0)
                    continue;
                Class_ID cId = pkAttribute->ClassID();

                for (int iWhichPB = 0; iWhichPB < 
                     pkAttribute->NumParamBlocks(); iWhichPB++)
                {
                    IParamBlock2* pkParamBlock = 
                        pkAttribute->GetParamBlock(iWhichPB);
                    if (pkParamBlock != NULL)
                    {
                        pkParamBlock->GetDesc();
                        pkParamBlock->NumParams();

                        int param_type;

                        param_type = 
                            pkParamBlock->GetParameterType((ParamID)iIndex);
                        
                        // convert value into a MaxScript-wrapped value and 
                        // return
                        Interval kValid;
                        switch ( param_type ) 
                        {
                            case TYPE_FLOAT:
                            {
                                float fNewValue = pkCANewValue->to_float();
                                pkParamBlock->SetValue((ParamID)iIndex, 0, fNewValue);
                                break;
                            }
                            case TYPE_INT:
                            {
                                int iNewValue = pkCANewValue->to_int();
                                pkParamBlock->SetValue((ParamID)iIndex, 0, iNewValue);
                                break;
                            }
                            case TYPE_TEXMAP:
                            {
                                Texmap* pkNewValue = pkCANewValue->to_texmap();
                                pkParamBlock->SetValue((ParamID)iIndex, 0, pkNewValue);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // We failed finding the value, just return undefined
    NiOutputDebugString("WARNING! \"NiGetCustAttrib\" Could not find"
        " or didn't convert: ");
    NiOutputDebugString(pcName);
    NiOutputDebugString("\n");

    return_value(&undefined);
}

def_visible_primitive( NiSetCustAttrib,
                      "NiSetCustAttrib");
//---------------------------------------------------------------------------
Value* NiGetCustAttribCount_cf(Value** arg_list, int count) 
{
    two_typed_value_locals(Value* result, Value* item);
    check_arg_count("NiSetCustAttrib", 2, count);

    Value* pkCAObj = arg_list[0];
    Value* pkCAType = arg_list[1];

    // Make sure the arguments exist and are valid
    if (!pkCAObj || !pkCAType || pkCAObj == &undefined || 
        pkCAType == &undefined || pkCAObj == &unsupplied || 
        pkCAType == &unsupplied)
    {
        return_value(&undefined);
    }

    // Convert the second argument to its C string representation
    const char* pcType = pkCAType->to_string();
    
    // Convert the first argument to a MAX object
    ReferenceTarget* pkCAObjRefTarget = pkCAObj->to_reftarg();

    if (pkCAObjRefTarget == NULL)
        return_value(&undefined);

    // Check out any custom attributes on the object
    Class_ID cID = pkCAObjRefTarget->ClassID();
    ICustAttribContainer* cc = pkCAObjRefTarget->GetCustAttribContainer();
    if (cc) 
    {
        int num_attribs = cc->GetNumCustAttribs();
        CustAttrib * pkAttribute = NULL;

        for (int attrib = 0; attrib < num_attribs; attrib ++ )
        {
            // Get the custom attribute at this location in the array
            // and check its parameter blocks
            pkAttribute = (CustAttrib *) cc->GetCustAttrib(attrib);
            if (pkAttribute)
            {
                // make sure that we are searching the right CA group
                TSTR strName = pkAttribute->GetName();
                if (NiStricmp((char*) strName, pcType) != 0)
                    continue;

                // this method assumes the user wants info from only the first
                // parameter block
                IParamBlock2* pkParamBlock = 
                    pkAttribute->GetParamBlock(0);
                if (pkParamBlock != NULL)
                {
                    int num_params = pkParamBlock->NumParams();
                    Value* retVal = NiExternalNew Integer(num_params);
                    return_value(retVal);
                }
            }
        }
    }

    // We failed finding the value, just return undefined
    return_value(&undefined);
}

def_visible_primitive(NiGetCustAttribCount, "NiGetCustAttribCount");

//---------------------------------------------------------------------------
Value* NiGetShadowTechniqueList_cf(Value**, int)
{
    // define the return array
    one_typed_value_local(Value* pkReturnValue);
    Value* pkReturnValue = NiMAXScriptUtils::GetShadowTechniqueNames();
    return_value(pkReturnValue);
}
def_visible_primitive(NiGetShadowTechniqueList, "NiGetShadowTechniqueList");

//---------------------------------------------------------------------------
Value* NiGetMeshProfileList_cf(Value**, int)
{
    // define the return array
    one_typed_value_local(Value* pkReturnValue);
    NiTPrimitiveSet<const char*> kProfiles;
    NiMeshProfileProcessor::GetAvailableProfiles(kProfiles);
    Value* pkReturnValue = NiExternalNew 
        Array(kProfiles.GetSize());
    Array* pkArray = (Array*)pkReturnValue;

    // collect a list of strings from the mesh profile processor
    for (unsigned short us = 0;
        us < kProfiles.GetSize(); us++)
    {
        Value* pTechniqueName = NiExternalNew 
            String((TCHAR*)(const char*)kProfiles.GetAt(us));
        pkArray->append(pTechniqueName);
    }

    return_value(pkReturnValue);
}
def_visible_primitive(NiGetMeshProfileList, "NiGetMeshProfileList");

//---------------------------------------------------------------------------
Value* NiSetXRay_cf(Value** arg_list, int count)
{
    // retrieve the arguments
    check_arg_count("NiSetXRay", 2, count);
    Value* pkNodeValue = arg_list[0];
    Value* pkXRayValue = arg_list[1];

    // ensure arguments are valid
    if ((pkNodeValue == NULL) || (pkNodeValue == &undefined) ||
        (pkXRayValue == NULL) || (pkXRayValue == &undefined)) 
    {
        return &ok;
    }

    // cast the arguments
    INode* pkNode = pkNodeValue->to_node();
    BOOL bXRay = pkXRayValue->to_bool();

    // perform the operation
    if (pkNode != NULL)
    {
        pkNode->XRayMtl(bXRay);
        Interface* pkInterface = GetCOREInterface();
        pkInterface->NodeInvalidateRect(pkNode);
        pkInterface->RedrawViews(pkInterface->GetTime());
    }

    return &ok;
}
def_visible_primitive(NiSetXRay, "NiSetXRay");
//---------------------------------------------------------------------------
