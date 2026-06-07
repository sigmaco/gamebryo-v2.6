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

// Precompiled Headers
#include "MayaPluginPCH.h"

MyiUserData gUserData;

//---------------------------------------------------------------------------
sUserData::sUserData(void)
{
    Reset();
}
//---------------------------------------------------------------------------
sUserData::~sUserData(void)
{
    Reset();
}
//---------------------------------------------------------------------------
void sUserData::Reset(void)
{
    m_pNode         = NULL;
    m_pParent       = NULL;
    m_bHasParent    = false;
}
//---------------------------------------------------------------------------
void MyiUserData::InitSize(int iSize)
{
    NIASSERT(m_iMaxShape == 0);
    NIASSERT(m_pUserData == NULL);

    if (iSize==0)
        m_pUserData = NULL;
    else
    {
        m_pUserData = NiExternalNew sUserData[iSize];
        m_iMaxShape = iSize;
        NIASSERT(m_pUserData);
    }
}
//---------------------------------------------------------------------------
MyiUserData::MyiUserData(int iSize)
{
    m_iMaxShape = 0;
    m_pUserData = NULL;

    InitSize( iSize );
}
//---------------------------------------------------------------------------
void MyiUserData::Reset(void)
{
    if (m_pUserData)
        NiExternalDelete [] m_pUserData;
    m_pUserData = NULL;
    m_iMaxShape = 0;
}
//---------------------------------------------------------------------------
MyiUserData::~MyiUserData(void)
{
    Reset();
}
//---------------------------------------------------------------------------
void MyiUserData::SetNode(int iShapeNum, NiNode *pNode)
{
    sUserData User;
    
    ElementGetUserData( iShapeNum, &User );     // Make a Copy into User
    ElementDestroyUserData( iShapeNum );        // Clear    
    NIASSERT(User.m_pNode == NULL);   
    User.m_pNode = pNode;
    ElementSetUserData( iShapeNum, &User );     // Copy User Fields.
}
//---------------------------------------------------------------------------
void MyiUserData::SetParent(int iShapeNum,NiNode *pParent)
{
    sUserData User;
    
    ElementGetUserData( iShapeNum, &User);    
    ElementDestroyUserData( iShapeNum );
    NIASSERT(User.m_pParent == NULL);
    NIASSERT(User.m_bHasParent == false);    
    User.m_pParent = pParent;
    User.m_bHasParent = true;
    ElementSetUserData( iShapeNum, &User);
}
//---------------------------------------------------------------------------
NiNode *MyiUserData::GetNode(int iShapeNum)
{
    sUserData User;
    ElementGetUserData( iShapeNum, &User);
    return User.m_pNode;
}
//---------------------------------------------------------------------------
NiNode *MyiUserData::GetParent(int iShapeNum)
{
    sUserData User;
    ElementGetUserData( iShapeNum, &User );
    NIASSERT(User.m_bHasParent);
    return User.m_pParent;
}
//---------------------------------------------------------------------------
bool MyiUserData::HasParent( int iShapeNum)
{
    sUserData User;
    ElementGetUserData( iShapeNum, &User );
    return User.m_bHasParent;
}
//---------------------------------------------------------------------------
bool MyiUserData::ElementGetUserData( int iShapeNum, sUserData *pUser)
{
    NIASSERT(m_pUserData != NULL);

    if (iShapeNum >=0 && iShapeNum < m_iMaxShape)
    {
        pUser->m_bHasParent = m_pUserData[iShapeNum].m_bHasParent;
        pUser->m_pNode      = m_pUserData[iShapeNum].m_pNode;
        pUser->m_pParent    = m_pUserData[iShapeNum].m_pParent;
        return true;
    }
    
    NIASSERT(0 && "Invalid user data given");
    return false;
}
//---------------------------------------------------------------------------
bool MyiUserData::ElementDestroyUserData( int iShapeNum )
{
    m_pUserData[iShapeNum].Reset();
    return true;
}
//---------------------------------------------------------------------------
bool MyiUserData::ElementSetUserData( int iShapeNum, sUserData *pUser )
{
    NIASSERT(m_pUserData != NULL);
    NIASSERT(iShapeNum > -1);
    NIASSERT(iShapeNum < m_iMaxShape);

    m_pUserData[iShapeNum].m_bHasParent = pUser->m_bHasParent;
    m_pUserData[iShapeNum].m_pNode      = pUser->m_pNode;
    m_pUserData[iShapeNum].m_pParent    = pUser->m_pParent;
    
    return true;
}
//---------------------------------------------------------------------------
int MyiUserData::GetShapeNum(NiNode* pNode)
{
    // Convert back from a NiNode* to a ShapeNum
    for(int iLoop = 0; iLoop < m_iMaxShape; iLoop++)
    {
        if( m_pUserData[iLoop].m_pNode == pNode)
            return iLoop;
    }
    return -1;
}
//---------------------------------------------------------------------------
NiNode* MyiUserData::GetNode(MObject kObject)
{
    int iObj = gMDtObjectFindComponentIDByNode(kObject);

    return GetNode(iObj);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool IsColorAlphaAttribute(MPlug kPlug, MFnNumericAttribute AttrNumData,
        MFnDagNode dgNode)
{
    // Check to see if this is a color
    if(kPlug.isChild())
    {
        MObject MParent = kPlug.parent().attribute();
        MFnAttribute kAttribute( MParent );
        
        if (kAttribute.isUsedAsColor() &&
            (MParent.apiType() == MFn::kAttribute3Float))
            return true;

        if (!kAttribute.isUsedAsColor() &&
            (MParent.apiType() == MFn::kAttribute3Double))
            return true;

    }
    
    if(strstr(AttrNumData.name().asChar(), "_Alpha") != NULL)
    {
        char cAttributeName[256];
        NiStrcpy(cAttributeName, 256, AttrNumData.name().asChar());
        
        // Ends in Alpha so strip off the alpha
        char* pAlpha = cAttributeName + strlen(cAttributeName) - 
            strlen("_Alpha");
        
        if(strcmp(pAlpha, "_Alpha") == 0)
        {
            *pAlpha = '\0';
        }
        
        // Check for an Attribute with this name
        MObject MColor = dgNode.attribute(MString(cAttributeName));
        if(MColor != MObject::kNullObj)
        {
            MFnAttribute ColorAttribute(MColor);
            
            if (ColorAttribute.isUsedAsColor() &&
                (MColor.apiType() == MFn::kAttribute3Float))
            {
                return true;
            }
        }
        
    }
    return false;
}
//---------------------------------------------------------------------------
void HandleNumericAttribute(NiObjectNET *pObject, MObject AttributeName, 
    const char* pszAttributeName, int iAttributeNum, MFnDagNode dgNode,
    bool bFromTyped)
{
    MStatus s;
    MFnNumericAttribute AttrNumData;
    AttrNumData.setObject( AttributeName );

    bool bUsedAsColor = AttrNumData.isUsedAsColor();
    NI_UNUSED_ARG(bUsedAsColor);
    
    // Create the plug for this attribute
    MPlug kPlug(dgNode.object(), AttributeName);

    bool bIsChild = kPlug.isChild();
    NI_UNUSED_ARG(bIsChild);

    // Create the MFnNumericData
    MObject valueObj;
    kPlug.getValue(valueObj);
    MFnNumericData kPlugData(valueObj);

    MFnNumericData::Type mfntypeNumData;

        // Get the type data differently dependent on how we got here
    if (bFromTyped)
    {
        mfntypeNumData = kPlugData.numericType();
    }
    else
    {
        mfntypeNumData = AttrNumData.unitType();
    }

    double dVal = 0.0;
    double dVal2 = 0.0;
    double dVal3 = 0.0;

    float fVal = 0.0f;
    float fVal2 = 0.0f;
    float fVal3 = 0.0f;

    int    iVal = 0;
    int    iVal2 = 0;
    int    iVal3 = 0;

    short sVal = 0;
    short sVal2 = 0;
    short sVal3 = 0;

    bool   bVal = false;

    char szNumType[512];

    NiExtraData* pkExtraData = NULL;
    NiExtraDataController* pkController = NULL;
    int* piInts = NULL;
    float* pfFloats = NULL;
    const char* pcTrimmedAttributeName = pszAttributeName + 
        strlen("NiData_");
    
    switch( mfntypeNumData )
    {
    case MFnNumericData::kBoolean:
        s = kPlug.getValue(bVal);
        NIASSERT(s==MS::kSuccess);
        pkExtraData = (NiExtraData*)NiNew NiBooleanExtraData(bVal);
        break; 

    case MFnNumericData::kLong: 
        s = kPlug.getValue(iVal);
        NIASSERT(s==MS::kSuccess);
        pkExtraData = (NiExtraData*)NiNew NiIntegerExtraData(iVal);
        break; 

    case MFnNumericData::kDouble:

        // Check to see if this is a color Alpha value If it is then
        // We will handle the export with it's parent attribute
        // Check to see if this is a color or Alpha value for a color
        if (IsColorAlphaAttribute(kPlug, AttrNumData,dgNode))
        {
            break;
        }

        s = kPlug.getValue(dVal);
        NIASSERT(s==MS::kSuccess);
        pkExtraData = (NiExtraData*)NiNew NiFloatExtraData((float)dVal);

        // Check for Animation
        if(kPlug.isConnected())
        {
            unsigned int uiNumKeys;
            NiFloatKey* pkKeys;
            NiAnimationKey::KeyType eType;
            if(ConvertFloatAnim(kPlug, uiNumKeys, pkKeys, eType) == 
                MS::kSuccess)
            {
                // Create the Data
                NiFloatData* pkData = NiNew NiFloatData;
                pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

                // Create the Interpolator
                NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator
                    (pkData);
                pkInterp->Collapse();

                // Create the controller and reset the extrema
                pkController = 
                    (NiExtraDataController* )NiNew NiFloatExtraDataController(
                        pcTrimmedAttributeName);

                pkController->SetInterpolator(pkInterp);
                pkController->ResetTimeExtrema();
            }
        }
        break; 

    case MFnNumericData::kByte: 
        s = kPlug.getValue(iVal);
        NIASSERT(s==MS::kSuccess);
        pkExtraData = (NiExtraData*)NiNew NiIntegerExtraData(iVal);
        break; 

    case MFnNumericData::kChar: 
        s = kPlug.getValue(iVal);
        NIASSERT(s==MS::kSuccess);
        pkExtraData = (NiExtraData*)NiNew NiIntegerExtraData(iVal);
        break; 

    case MFnNumericData::kShort: 
        s = kPlug.getValue(sVal);
        NIASSERT(s==MS::kSuccess);
        pkExtraData = (NiExtraData*)NiNew NiIntegerExtraData(iVal);
        break; 

    case MFnNumericData::k2Short: 
        s = kPlugData.getData(sVal, sVal2);
        NIASSERT(s==MS::kSuccess);
        piInts = NiAlloc(int, 2);
        piInts[0] = sVal;
        piInts[1] = sVal2;
        pkExtraData = (NiIntegersExtraData*)NiNew NiIntegersExtraData(2, 
            piInts);
        NiFree(piInts);
        break; 

    case MFnNumericData::k3Short: 
        s = kPlugData.getData(sVal, sVal2, sVal3);
        NIASSERT(s==MS::kSuccess);
        piInts = NiAlloc(int, 3);
        piInts[0] = sVal;
        piInts[1] = sVal2;
        piInts[2] = sVal3;
        pkExtraData = (NiIntegersExtraData*)NiNew NiIntegersExtraData(3, 
            piInts);
        NiFree(piInts);
        break; 

    case MFnNumericData::k2Long: 
        s = kPlugData.getData(iVal, iVal2);
        NIASSERT(s==MS::kSuccess);
        piInts = NiAlloc(int, 2);
        piInts[0] = iVal;
        piInts[1] = iVal2;
        pkExtraData = (NiIntegersExtraData*)NiNew NiIntegersExtraData(2, 
            piInts);
        NiFree(piInts);
        break; 

    case MFnNumericData::k3Long: 
        s = kPlugData.getData(iVal, iVal2, iVal3);
        NIASSERT(s==MS::kSuccess);
        piInts = NiAlloc(int, 3);
        piInts[0] = iVal;
        piInts[1] = iVal2;
        piInts[2] = iVal3;
        pkExtraData = (NiIntegersExtraData*)NiNew NiIntegersExtraData(3, 
            piInts);
        NiFree(piInts);
        break; 

    case MFnNumericData::kFloat: 
        // Check to see if this is a color Alpha value If it is then
        // We will handle the export with it's parent attribute
        // Check to see if this is a color or Alpha value for a color
        if (IsColorAlphaAttribute(kPlug, AttrNumData,dgNode))
        {
            break;
        }

        s = kPlug.getValue(fVal);
        NIASSERT(s==MS::kSuccess);
        pkExtraData = (NiExtraData*)NiNew NiFloatExtraData(fVal);

        // Check for Animation
        if(kPlug.isConnected())
        {
            unsigned int uiNumKeys;
            NiFloatKey* pkKeys;
            NiAnimationKey::KeyType eType;
            if(ConvertFloatAnim(kPlug, uiNumKeys, pkKeys, eType) == 
                MS::kSuccess)
            {
                // Create the Data
                NiFloatData* pkData = NiNew NiFloatData;
                pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

                // Create the Interpolator
                NiFloatInterpolator* pkInterp = 
                    NiNew NiFloatInterpolator(pkData);

                pkInterp->Collapse();

                pkController = (NiExtraDataController* )
                    NiNew NiFloatExtraDataController(
                        pcTrimmedAttributeName);

                pkController->SetInterpolator(pkInterp);
                pkController->ResetTimeExtrema();
            }
        }
        break;

    case MFnNumericData::k2Float: 
        s = kPlugData.getData(fVal, fVal2);
        NIASSERT(s==MS::kSuccess);
        pfFloats = NiAlloc(float, 2);
        pfFloats[0] = fVal;
        pfFloats[1] = fVal2;
        pkExtraData = (NiFloatsExtraData*)NiNew NiFloatsExtraData(2, pfFloats);
        NiFree(pfFloats);
        break; 

    case MFnNumericData::k3Float: 
        s = kPlugData.getData(fVal, fVal2, fVal3);
        NIASSERT(s==MS::kSuccess);
        pfFloats = NiAlloc(float, 3);
        pfFloats[0] = fVal;
        pfFloats[1] = fVal2;
        pfFloats[2] = fVal3;
        pkExtraData = (NiFloatsExtraData*)NiNew NiFloatsExtraData(3, pfFloats);
        NiFree(pfFloats);
        break; 

    case MFnNumericData::k2Double: 
        s = kPlugData.getData(dVal, dVal2);
        NIASSERT(s==MS::kSuccess);
        pfFloats = NiAlloc(float, 2);
        pfFloats[0] = (float)dVal;
        pfFloats[1] = (float)dVal2;
        pkExtraData = (NiFloatsExtraData*)NiNew NiFloatsExtraData(2, pfFloats);
        NiFree(pfFloats);
        break; 

    case MFnNumericData::k3Double: 
        s = kPlugData.getData(dVal, dVal2, dVal3);
        NIASSERT(s==MS::kSuccess);
        pfFloats = NiAlloc(float, 3);
        pfFloats[0] = (float)dVal;
        pfFloats[1] = (float)dVal2;
        pfFloats[2] = (float)dVal3;
        pkExtraData = (NiFloatsExtraData*)NiNew NiFloatsExtraData(3, pfFloats);
        NiFree(pfFloats);
        break; 

    case MFnNumericData::kLast : NiStrcpy(szNumType, 512, "kLast ");break; 
    case MFnNumericData::kInvalid: NiStrcpy(szNumType, 512, "kInvalid");
    default:
        printf("WARNING: NiData Attribute #:%d  Name:%s  API:%s  "
            "NOT SUPPORTED\n", iAttributeNum, pcTrimmedAttributeName, 
            szNumType);
        return;
        break;
    }  

    // Add in the extra data if it was created
    if (pkExtraData != NULL)
    {

        pObject->AddExtraData( pcTrimmedAttributeName, pkExtraData );

        // Add the controller for any animated attribute if it exists
        if (pkController != NULL)
        {
            pkController->SetTarget(pObject);
        }
    }
}      
//---------------------------------------------------------------------------
void HandleTypedAttribute(  NiObjectNET *pObject, MObject AttributeName,
    const char* pszAttributeName, int iAttributeNum, 
    const char* szdgNodeName, MFnDagNode& dgNode)
{
    NI_UNUSED_ARG(szdgNodeName);
    MStatus s;

    NiExtraData* pData = NULL;

    MFnTypedAttribute TypedAttr;
    TypedAttr.setObject( AttributeName );
    
    bool bUsedAsColor = TypedAttr.isUsedAsColor();
    NI_UNUSED_ARG(bUsedAsColor);

    MFnData::Type AttributeType = TypedAttr.attrType(&s);
    
    MString StringVal;
    const char* pcTrimmedAttributeName = pszAttributeName + 
        strlen("NiData_");
    
    // Create the plug for this attribute
    MPlug kPlug(dgNode.object(), AttributeName);

    // Create the Value for this plug
    MObject valueObj;
    kPlug.getValue(valueObj);

    switch(AttributeType)
    {       
    case MFnData::kString:
        {
            s = kPlug.getValue(StringVal);
            if(s != MS::kSuccess)
                StringVal = MString("Uninitialize String");
            
            pData = (NiExtraData*)NiNew NiStringExtraData( StringVal.asChar());
            
            pObject->AddExtraData(pcTrimmedAttributeName, pData);
        }
        break; 
        
    case MFnData::kNumeric:
        {
            HandleNumericAttribute(pObject, AttributeName, pszAttributeName,
                            iAttributeNum, dgNode, true);
        }

        break;
    case MFnData::kDoubleArray:
        {
            MFnDoubleArrayData kData(valueObj, &s);
            if(s == MS::kSuccess)
            {
                float* pkFloats = NULL;
#ifdef NIDEBUG
                unsigned int length = kData.length();
                NI_UNUSED_ARG(length);
#endif
                if(kData.length() > 0)
                {
                    pkFloats = NiAlloc(float, kData.length());
                    
                    for(int iCount = 0; iCount < (int)kData.length(); 
                        iCount++)
                    {
                        pkFloats[iCount] = (float)kData[iCount];
                    }

                } 
                else
                {
                    //Default
                    pkFloats = NiAlloc(float, 1);
                    pkFloats[0] = 0; 
                }


                pData = (NiExtraData*)NiNew NiFloatsExtraData(
                    kData.length(), pkFloats);

                pObject->AddExtraData(pcTrimmedAttributeName, pData);
                NiFree(pkFloats);
            }
        }
        break;        
    case MFnData::kIntArray:
        {
            MFnIntArrayData kData(valueObj, &s);
            if(s == MS::kSuccess)
            {
                int* pkInts = NULL;
                if(kData.length() > 0)
                {
                    pkInts = NiAlloc(int, kData.length());
                    
                    for(int iCount = 0; iCount < (int)kData.length();
                        iCount++)
                    {
                        pkInts[iCount] = kData[iCount];
                    }

                    pData = (NiExtraData*)NiNew NiIntegersExtraData(
                        kData.length(), pkInts);
                    pObject->AddExtraData(pcTrimmedAttributeName, pData);
                    NiFree(pkInts);
                }
                
            }
        }
        break;
    case MFnData::kStringArray:
        {
            MFnStringArrayData kData(valueObj, &s);

            if(s != MS::kSuccess)
            {
                char** ppcStrings = NULL;
                if(kData.length() > 0)
                {
                    ppcStrings = NiAlloc(char*, kData.length());
                    
                    for(int iCount = 0; iCount < (int)kData.length(); 
                        iCount++)
                    {
                        unsigned int uiLen = kData[iCount].length() + 1;
                        ppcStrings[iCount] = NiAlloc(char, uiLen);
                        NiStrcpy(ppcStrings[iCount], uiLen, 
                            kData[iCount].asChar());
                    }

                    pData = (NiStringsExtraData*)NiNew NiStringsExtraData(
                        kData.length(), (const char**)ppcStrings);

                    pObject->AddExtraData(pcTrimmedAttributeName, pData);
                }
            }
        }
        break;
    case MFnData::kPlugin:
    case MFnData::kPluginGeometry:
    case MFnData::kMatrix:
    case MFnData::kPointArray:
    case MFnData::kVectorArray:
    case MFnData::kComponentList:
    case MFnData::kMesh:
    case MFnData::kLattice:
    case MFnData::kNurbsCurve:
    case MFnData::kNurbsSurface:
    case MFnData::kSphere:
    case MFnData::kDynArrayAttrs:
    case MFnData::kSubdSurface:
    case MFnData::kLast:
    default:
        printf("WARNING: NiData Attribute #:%d  Name:%s  NOT SUPPORTED\n", 
            iAttributeNum, pszAttributeName );
        break;
    }                    
}
//---------------------------------------------------------------------------
void AttachUserDefinedAttributes( MFnDagNode& dgNode, NiObjectNET *pObject )
{
    MStatus s;

    const char* szdgNodeName = dgNode.name(&s).asChar();
    NIASSERT(s == MS::kSuccess);

    int iNumAttributes = dgNode.attributeCount(&s);

    // Loop through each attribute attached to this node
    // looking for attributes which start with "NiData_"
    for(int iLoop = 0; iLoop < iNumAttributes; iLoop++)
    {
        MObject AttributeName = dgNode.attribute(iLoop, &s);    
        MFnAttribute Attribute;
        Attribute.setObject(AttributeName);

        bool bIsArray = Attribute.isArray(&s);
        NI_UNUSED_ARG(bIsArray);
        NIASSERT(s == MS::kSuccess);

        MPlug kMPlug(dgNode.object(), AttributeName);

        const char* pszAttributeName = Attribute.name().asChar();
        const char* pszResult = strstr( pszAttributeName, "NiData_" );

        if (pszResult)
        {
            MFn::Type mfntype = AttributeName.apiType();

            NiExtraData* pData = NULL;
            const char* pcTrimmedAttributeName = 
                pszAttributeName + strlen("NiData_");

            switch(mfntype)
            {
            case MFn::kNumericAttribute:
                HandleNumericAttribute(pObject, AttributeName, 
                    pszAttributeName, iLoop, dgNode, false);
                break;

            case MFn::kTypedAttribute:
                HandleTypedAttribute(pObject, AttributeName, 
                    pszAttributeName, iLoop, szdgNodeName, dgNode);
                break;
            case MFn::kTimeAttribute:
            case MFn::kDoubleAngleAttribute:
            case MFn::kDoubleLinearAttribute:
                {
                    double dVal;
                    s = kMPlug.getValue(dVal);
                    NIASSERT(s==MS::kSuccess);

                    pData = (NiExtraData*)NiNew NiFloatExtraData((float)dVal);
                    pObject->AddExtraData(pcTrimmedAttributeName, pData );
                }
                break; 
            case MFn::kFloatMatrixAttribute:
                {

                    // Create the Value for this plug
                    MObject valueObj;
                    kMPlug.getValue(valueObj);

                    MFnMatrixData kMatrixData(valueObj, &s);
                    NIASSERT(s == MS::kSuccess);

                    const MMatrix & kMatrix = kMatrixData.matrix(&s);
                    NIASSERT(s == MS::kSuccess);
            
                    float* pkFloats = NiAlloc(float, 16);
                    int iCurrent = 0;

                    for(unsigned int uiRow = 0; uiRow < 4; uiRow++)
                    {
                        for(unsigned int uiCol = 0; uiCol < 4; uiCol++)
                        {
                            pkFloats[iCurrent++] = 
                                (float)kMatrix.matrix[uiRow][uiCol];
                        }
                    }

                    pData = 
                        (NiExtraData*)NiNew NiFloatsExtraData(16, pkFloats);
                    pObject->AddExtraData(pcTrimmedAttributeName, pData);
                    NiFree(pkFloats);
                }
                break;
            case MFn::kEnumAttribute:
                {
                    // Create the Enum Attribute
                    MFnEnumAttribute kEnumAttribute(AttributeName);

                    // Get the Enum Value
                    int iValue;
                    s = kMPlug.getValue(iValue);
                    NIASSERT(s==MS::kSuccess);

                    pData = (NiExtraData*)NiNew NiIntegerExtraData( iValue );
                    pObject->AddExtraData(pcTrimmedAttributeName, pData);
                }
                break;
            case MFn::kAttribute3Double:
            case MFn::kAttribute3Float:
                {
                    NIASSERT(kMPlug.numChildren(&s) == 3);
                    NIASSERT(s == MS::kSuccess);

                    if ( Attribute.isUsedAsColor())
                    {
                        MPlug Red = kMPlug.child(0, &s);
                        MPlug Green = kMPlug.child(1, &s);
                        MPlug Blue = kMPlug.child(2, &s);
                        
                        float fRed = 1.0f;
                        float fGreen = 1.0f; 
                        float fBlue = 1.0f; 
                        float fAlpha = 1.0f;
                        
                        s = Red.getValue(fRed);
                        s = Green.getValue(fGreen);
                        s = Blue.getValue(fBlue);
                        
                        // Look For the Alpha Attribute
                        MObject MAlpha = dgNode.attribute(Attribute.name() + 
                            MString("_Alpha"));
                        if(MAlpha != MObject::kNullObj)
                        {
                            MPlug Alpha(dgNode.object(), MAlpha);
                            NIASSERT(s == MS::kSuccess);
                            
                            s = Alpha.getValue(fAlpha);
                            NIASSERT(s == MS::kSuccess);
                        }
                                                
                        NiColorA kColor(fRed, fGreen, fBlue, fAlpha);
                        
                        pData = (NiExtraData*)NiNew NiColorExtraData(kColor);
                        pObject->AddExtraData(pcTrimmedAttributeName, pData);

                        if(MAlpha == MObject::kNullObj)
                        {
                            // Animated Color No Alpha

                            if (Red.isConnected() || Green.isConnected() || 
                                Blue.isConnected())
                            {
                                unsigned int uiNumKeys;
                                NiColorKey* pkKeys;
                                NiAnimationKey::KeyType eType;
                                
                                if (ConvertColorAnim(Red, Green, Blue, 
                                    uiNumKeys, pkKeys, eType) == MS::kSuccess)
                                {
                                    // Create the Color Data
                                    NiColorData* pkColorData = 
                                        NiNew NiColorData();

                                    pkColorData->ReplaceAnim(pkKeys, 
                                        uiNumKeys, eType);

                                    // Create the Interpolator
                                    NiColorInterpolator* pkInterp = 
                                        NiNew NiColorInterpolator(pkColorData);

                                    pkInterp->Collapse();

                                    // Create the controller
                                  NiColorExtraDataController* pkController =
                                      NiNew NiColorExtraDataController(
                                            pData->GetName());

                                    pkController->SetInterpolator(pkInterp);
                                    pkController->ResetTimeExtrema();
                                    
                                    // Attach to the object
                                    pkController->SetTarget(pObject);
                                }
                            }
                        }
                        else
                        {
                            // Animated Color No Alpha

                            MPlug Alpha(dgNode.object(), MAlpha);
                            
                            if (Red.isConnected() || Green.isConnected() || 
                                Blue.isConnected() || Alpha.isConnected())
                            {
                                unsigned int uiNumKeys;
                                NiColorKey* pkKeys;
                                NiAnimationKey::KeyType eType;
                                
                                if (ConvertColorAlphaAnim(Red, Green, Blue, 
                                    Alpha, uiNumKeys, pkKeys, eType) == 
                                    MS::kSuccess)
                                {
                                    // Create the Color Data
                                    NiColorData* pkColorData = 
                                        NiNew NiColorData();

                                    pkColorData->ReplaceAnim(pkKeys, 
                                        uiNumKeys, eType);

                                    // Create the Interpolator
                                    NiColorInterpolator* pkInterp = 
                                        NiNew NiColorInterpolator(pkColorData);

                                    pkInterp->Collapse();

                                    // Create the controller
                                 NiColorExtraDataController* pkController = 
                                     NiNew NiColorExtraDataController(
                                        pData->GetName());

                                    pkController->SetInterpolator(pkInterp);
                                    pkController->ResetTimeExtrema();
                                                                        
                                    // Attach to the object
                                    pkController->SetTarget(pObject);
                                }
                            }
                        }
                    }
                    else
                    {
                        MPlug MPlugX = kMPlug.child(0, &s);
                        MPlug MPlugY = kMPlug.child(1, &s);
                        MPlug MPlugZ = kMPlug.child(2, &s);

                        float* pfFloats = NiAlloc(float, 3);
                    
                        s = MPlugX.getValue(pfFloats[0]);
                        s = MPlugY.getValue(pfFloats[1]);
                        s = MPlugZ.getValue(pfFloats[2]);

                        pData = (NiExtraData*)NiNew NiFloatsExtraData(3, 
                            pfFloats);
                        NiFree(pfFloats);

                        pObject->AddExtraData(pcTrimmedAttributeName, pData);

                        if (MPlugX.isConnected())
                        {   
                            unsigned int uiNumKeys;
                            NiFloatKey* pkKeys;
                            NiAnimationKey::KeyType eType;

                            if ( ConvertFloatAnim(MPlugX, uiNumKeys, pkKeys, 
                                eType) == MS::kSuccess)
                            {
                                // Create the Data
                                NiFloatData* pkFloatData = NiNew NiFloatData();
                                pkFloatData->ReplaceAnim (pkKeys, uiNumKeys,
                                    eType);
                                
                                // Create the Interpolator
                                NiFloatInterpolator* pkInterp = 
                                    NiNew NiFloatInterpolator(pkFloatData);

                                pkInterp->Collapse();

                                // Create the Controller
                                NiFloatsExtraDataController* pkController = 
                                    NiNew NiFloatsExtraDataController(
                                        pData->GetName(), 0);

                                pkController->SetInterpolator(pkInterp);
                                pkController->ResetTimeExtrema();

                                pkController->SetTarget(pObject);
                            }
                        }

                        if (MPlugY.isConnected())
                        {   
                            unsigned int uiNumKeys;
                            NiFloatKey* pkKeys;
                            NiAnimationKey::KeyType eType;

                            if ( ConvertFloatAnim(MPlugY, uiNumKeys, pkKeys, 
                                eType) == MS::kSuccess)
                            {
                                // Create the Data
                                NiFloatData* pkFloatData = NiNew NiFloatData();
                                pkFloatData->ReplaceAnim (pkKeys, uiNumKeys, 
                                    eType);

                                // Create the Interpolator
                                NiFloatInterpolator* pkInterp = 
                                    NiNew NiFloatInterpolator(pkFloatData);

                                pkInterp->Collapse();

                                // Create the Controller
                                NiFloatsExtraDataController* pkController = 
                                    NiNew NiFloatsExtraDataController(
                                        pData->GetName(), 1);
                                
                                pkController->SetInterpolator(pkInterp);
                                pkController->ResetTimeExtrema();

                                pkController->SetTarget(pObject);
                            }
                        }

                        if (MPlugZ.isConnected())
                        {   
                            unsigned int uiNumKeys;
                            NiFloatKey* pkKeys;
                            NiAnimationKey::KeyType eType;

                            if ( ConvertFloatAnim(MPlugZ, uiNumKeys, pkKeys, 
                                eType) == MS::kSuccess)
                            {
                                // Create the Data
                                NiFloatData* pkFloatData = NiNew NiFloatData();
                                pkFloatData->ReplaceAnim (pkKeys, uiNumKeys, 
                                    eType);
                                
                                // Create the Interpolator
                                NiFloatInterpolator* pkInterp = 
                                    NiNew NiFloatInterpolator(pkFloatData);

                                pkInterp->Collapse();

                                // Create the Conroller
                                NiFloatsExtraDataController* pkController = 
                                    NiNew NiFloatsExtraDataController(
                                    pData->GetName(), 2);

                                pkController->SetInterpolator(pkInterp);
                                pkController->ResetTimeExtrema();

                                pkController->SetTarget(pObject);
                            }
                        }

                    }
                }
                break;
            case MFn::kMessageAttribute:
            default:
                {
                    printf("WARNING: NIData Attribute %s NOT SUPPORTED\n",
                        AttributeName.apiTypeStr());
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
bool CheckForExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase)
{
    MStatus s;

    const char* szCurrentAttributeName = NULL;

    int iNumAttributes = dgNode.attributeCount(&s);

        // Loop through each of the attributes looking for ours
        // the one passed in.
    for(int iLoop = 0; iLoop < iNumAttributes; iLoop++)
    {
        MObject AttributeName = dgNode.attribute(iLoop, &s);    
        MFnAttribute Attribute;
        Attribute.setObject(AttributeName);

        szCurrentAttributeName = Attribute.name().asChar();

        if(bIgnoreCase)
        {
            if( NiStricmp(szCurrentAttributeName, szAttributeName) == 0)
                return true;
        }
        else
        {
            if( strcmp(szCurrentAttributeName, szAttributeName) == 0)
                return true;
        }

    }
    
    // Attribute not found
    return false;
}
//---------------------------------------------------------------------------
bool CheckForExtraAttribute(MFnDependencyNode &kDepNode, 
    char *szAttributeName, bool bIgnoreCase)
{
    MStatus s;

    const char* szCurrentAttributeName = NULL;

    int iNumAttributes = kDepNode.attributeCount(&s);

        // Loop through each of the attributes looking for ours
        // the one passed in.
    for(int iLoop = 0; iLoop < iNumAttributes; iLoop++)
    {
        MObject AttributeName = kDepNode.attribute(iLoop, &s);    
        MFnAttribute Attribute;
        Attribute.setObject(AttributeName);

        szCurrentAttributeName = Attribute.name().asChar();

        if(bIgnoreCase)
        {
            if( NiStricmp(szCurrentAttributeName, szAttributeName) == 0)
                return true;
        }
        else
        {
            if( strcmp(szCurrentAttributeName, szAttributeName) == 0)
                return true;
        }
    }
    
    // Attribute not found
    return false;
}
//---------------------------------------------------------------------------
bool GetExtraDataAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, MFnAttribute& FoundAttribute)
{
    MStatus s;

    const char* szCurrentAttributeName = NULL;

    int iNumAttributes = dgNode.attributeCount(&s);

        // Loop through each of the attributes looking for ours
        // the one passed in.
    for(int iLoop = 0; iLoop < iNumAttributes; iLoop++)
    {
        MObject AttributeName = dgNode.attribute(iLoop, &s);    
        MFnAttribute Attribute;
        Attribute.setObject(AttributeName);

        szCurrentAttributeName = Attribute.name().asChar();

        if(bIgnoreCase)
        {
            if( NiStricmp(szCurrentAttributeName, szAttributeName) == 0)
            {
                FoundAttribute.setObject(AttributeName);
                return true;
            }
        }
        else
        {
            if( strcmp(szCurrentAttributeName, szAttributeName) == 0)
            {
                FoundAttribute.setObject(AttributeName);
                return true;
            }
        }

    }
    
    // Attribute not found
    return false;
}
//---------------------------------------------------------------------------
bool GetExtraDataAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, MFnAttribute& FoundAttribute)
{
    MStatus s;

    const char* szCurrentAttributeName = NULL;

    int iNumAttributes = kDepNode.attributeCount(&s);

        // Loop through each of the attributes looking for ours
        // the one passed in.
    for(int iLoop = 0; iLoop < iNumAttributes; iLoop++)
    {
        MObject AttributeName = kDepNode.attribute(iLoop, &s);    
        MFnAttribute Attribute;
        Attribute.setObject(AttributeName);

        szCurrentAttributeName = Attribute.name().asChar();

        if(bIgnoreCase)
        {
            if( NiStricmp(szCurrentAttributeName, szAttributeName) == 0)
            {
                FoundAttribute.setObject(AttributeName);
                return true;
            }
        }
        else
        {
            if( strcmp(szCurrentAttributeName, szAttributeName) == 0)
            {
                FoundAttribute.setObject(AttributeName);
                return true;
            }
        }

    }
    
    // Attribute not found
    return false;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, bool &bValue)
{
    // Check for the existance of the attribute
    if(!CheckForExtraAttribute(dgNode, szAttributeName, bIgnoreCase))
        return false;

    MStatus s;

    // Create the command to get our Attribute
    MString command = MString("getAttr ") + dgNode.fullPathName(&s) + 
        MString(".") + MString(szAttributeName) + MString(";");
    
    if(s != MS::kSuccess)
        return false;

    // Get this attribute
    int iVal = 0;
    s = MGlobal::executeCommand(command, iVal);

    if(s != MS::kSuccess)
        return false;


    // Out of Range
    if((iVal <0) || (iVal>1))
        return false;

    bValue = (iVal != 0);
    return true;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, int &iValue)
{
    // Check for the existance of the attribute
    if(!CheckForExtraAttribute(dgNode, szAttributeName, bIgnoreCase))
        return false;

    MStatus s;

    // Create the command to get our Attribute
    MString command = MString("getAttr ") + dgNode.fullPathName(&s) + 
        MString(".") + MString(szAttributeName) + MString(";");
    
    if(s != MS::kSuccess)
        return false;

    // Get this attribute
    int iVal = false;
    s = MGlobal::executeCommand(command, iVal);

    if(s != MS::kSuccess)
        return false;

    iValue = iVal;
    return true;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, double &dValue)
{
    // Check for the existance of the attribute
    if(!CheckForExtraAttribute(dgNode, szAttributeName, bIgnoreCase))
        return false;

    MStatus s;
    MFnAttribute FoundAttribute;

    // Create the command to get our Attribute
    MString command = MString("getAttr ") + dgNode.fullPathName(&s) + 
        MString(".") + MString(szAttributeName) + MString(";");
    
    if(s != MS::kSuccess)
        return false;

    // Get this attribute
    double dVal = false;
    s = MGlobal::executeCommand(command, dVal);

    if(s != MS::kSuccess)
        return false;

    dValue = dVal;
    return true;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, float &fValue)
{
    // Check for the existance of the attribute
    if(!CheckForExtraAttribute(dgNode, szAttributeName, bIgnoreCase))
        return false;

    double dVal;
    
    if( GetExtraAttribute(dgNode, szAttributeName, bIgnoreCase, dVal) )
    {
        fValue = (float)dVal;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, MString sValue)
{
    // Check for the existance of the attribute
    if(!CheckForExtraAttribute(dgNode, szAttributeName, bIgnoreCase))
        return false;

    MStatus s;
    MFnAttribute FoundAttribute;

    // Create the command to get our Attribute
    MString command = MString("getAttr ") + dgNode.fullPathName(&s) + 
        MString(".") + MString(szAttributeName) + MString(";");
    
    if(s != MS::kSuccess)
        return false;

    // Get this attribute
    MString sVal = "";
    s = MGlobal::executeCommand(command, sVal);

    if(s != MS::kSuccess)
        return false;

    sValue = sVal;
    return true;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, bool &bValue)
{
    MFnAttribute kAttribute;
    MStatus kStatus;
    
    // Find the attribute
    if (!GetExtraDataAttribute(kDepNode, szAttributeName, bIgnoreCase, 
        kAttribute))
    {
        return false;
    }
    
    // Verify the type
    if (kAttribute.object().apiType() != MFn::kNumericAttribute)
    {
        return false;
    }
    
    MFnNumericAttribute AttrNumData(kAttribute.object(), &kStatus);
    
    // Make sure we are an integer type
    if (AttrNumData.unitType() != MFnNumericData::kBoolean)
    {
        return false;
    }
    
    // Create the plug for this attribute
    MPlug kPlug(kDepNode.object(), kAttribute.object());
    
    kStatus = kPlug.getValue(bValue);
    
    NIASSERT(kStatus == MS::kSuccess);
    return true;    
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, int &iValue)
{
    MFnAttribute kAttribute;
    MStatus kStatus;

    // Find the attribute
    if (!GetExtraDataAttribute(kDepNode, szAttributeName, bIgnoreCase, 
            kAttribute))
    {
        return false;
    }

    // Verify the type
    if ((kAttribute.object().apiType() != MFn::kNumericAttribute) &&
        (kAttribute.object().apiType() != MFn::kEnumAttribute))
    {
        return false;
    }

    if (kAttribute.object().apiType() == MFn::kNumericAttribute)
    {
        MFnNumericAttribute AttrNumData(kAttribute.object(), &kStatus);

        // Make sure we are an integer type
        if ((AttrNumData.unitType() != MFnNumericData::kLong) &&
            (AttrNumData.unitType() != MFnNumericData::kByte) &&
            (AttrNumData.unitType() != MFnNumericData::kChar) &&
            (AttrNumData.unitType() != MFnNumericData::kShort))
        {
            return false;
        }

        // Create the plug for this attribute
        MPlug kPlug(kDepNode.object(), kAttribute.object());

        kStatus = kPlug.getValue(iValue);

        NIASSERT(kStatus == MS::kSuccess);
        return true;
    }

    if (kAttribute.object().apiType() == MFn::kTypedAttribute)
    {
        // Create the MFnNumericData
        MPlug kPlug(kDepNode.object(), kAttribute.object());

        MObject valueObj;
        kPlug.getValue(valueObj);

        MFnNumericData kPlugData(valueObj);

        // Make sure we are an integer type
        if ((kPlugData.numericType() != MFnNumericData::kLong) &&
            (kPlugData.numericType() != MFnNumericData::kByte) &&
            (kPlugData.numericType() != MFnNumericData::kChar) &&
            (kPlugData.numericType() != MFnNumericData::kShort))
        {
            return false;
        }

        kStatus = kPlug.getValue(iValue);

        NIASSERT(kStatus == MS::kSuccess);
        return true;
    }

    if (kAttribute.object().apiType() == MFn::kEnumAttribute)
    {
        // Create the Enum Attribute
        MFnEnumAttribute kEnumAttribute(kAttribute.object(), &kStatus);
        NIASSERT(kStatus==MS::kSuccess);

         // Create the plug for this attribute
        MPlug kPlug(kDepNode.object(), kAttribute.object());
       
        // Get the Enum Value
        kStatus = kPlug.getValue(iValue);
        NIASSERT(kStatus==MS::kSuccess);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, float &fValue)
{
    MFnAttribute kAttribute;
    MStatus kStatus;
    
    // Find the attribute
    if (!GetExtraDataAttribute(kDepNode, szAttributeName, bIgnoreCase, 
        kAttribute))
    {
        return false;
    }
    
    // Verify the type
    if (kAttribute.object().apiType() != MFn::kNumericAttribute)
    {
        return false;
    }
    
    MFnNumericAttribute AttrNumData(kAttribute.object(), &kStatus);
    
    // Make sure we are an integer type
    if ((AttrNumData.unitType() != MFnNumericData::kFloat) &&
        (AttrNumData.unitType() != MFnNumericData::kDouble))
    {
        return false;
    }
    
    // Create the plug for this attribute
    MPlug kPlug(kDepNode.object(), kAttribute.object());
    
    kStatus = kPlug.getValue(fValue);
    
    NIASSERT(kStatus == MS::kSuccess);
    return true;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, double &dValue)
{
    MFnAttribute kAttribute;
    MStatus kStatus;
    
    // Find the attribute
    if (!GetExtraDataAttribute(kDepNode, szAttributeName, bIgnoreCase, 
        kAttribute))
    {
        return false;
    }
    
    // Verify the type
    if (kAttribute.object().apiType() != MFn::kNumericAttribute)
    {
        return false;
    }
    
    MFnNumericAttribute AttrNumData(kAttribute.object(), &kStatus);
    
    // Make sure we are an integer type
    if ((AttrNumData.unitType() != MFnNumericData::kFloat) &&
        (AttrNumData.unitType() != MFnNumericData::kDouble))
    {
        return false;
    }
    
    // Create the plug for this attribute
    MPlug kPlug(kDepNode.object(), kAttribute.object());
    
    kStatus = kPlug.getValue(dValue);
    
    NIASSERT(kStatus == MS::kSuccess);
    return true;
}
//---------------------------------------------------------------------------
bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, MString &sValue)
{
    MFnAttribute kAttribute;
    MStatus kStatus;
    
    // Find the attribute
    if (!GetExtraDataAttribute(kDepNode, szAttributeName, bIgnoreCase, 
        kAttribute))
    {
        return false;
    }
    
    // Verify the type
    if (kAttribute.object().apiType() != MFn::kTypedAttribute)
    {
        return false;
    }
    
    MFnTypedAttribute TypedAttr(kAttribute.object(), &kStatus);

    
    // Make sure we are an integer type
    if (TypedAttr.attrType() != MFnData::kString)
    {
        return false;
    }
    
    // Create the plug for this attribute
    MPlug kPlug(kDepNode.object(), kAttribute.object());
    
    kStatus = kPlug.getValue(sValue);
    if(kStatus != MS::kSuccess)
        sValue = MString("Uninitialize String");

    return true;
}
//---------------------------------------------------------------------------
