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

// MDtAnim.cpp
// The purpose of this module is to extend the functionality of MDt
// to support simple animations

// Precompiled Headers
//#include "MDtAnim.h"
#include "MayaPluginPCH.h"
#include "MDtAnim.h"
#include "crtdbg.h"
#include <malloc.h>
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnIkHandle.h"
#include "maya/MItDag.h"

bool g_bSampleAllRotation;
bool g_bSampleRotationAsQuaternion;
bool g_bSampleAllTranslation;

//#include "MDtAnim.h"
extern void BezierInterpolate(float *x, float *y, float time);


bool floatDistanceCompare(float fA, float fB, int iMaxFloatsDifference)
{
    NIASSERT(iMaxFloatsDifference > 0);
    NIASSERT(sizeof(int) == sizeof(float));

    bool returnValue;
    int iFloatAAsInt, iFloatBAsInt;
    
    _asm
    {
        //copy the float
        mov eax, [fA]
        //copy the float again
        mov ebx, eax
        //sign extend whats in eax into edx
        //if it is negative the value is ffffffff otherwise 0
        cdq
        //copy 0x80000000 to use for subtraction
        mov ecx, 0x80000000
        //convert from signed magnitude to two's complement if it is negative
        sub ecx, eax
        //if it is negative we will use this value otherwise we make it zero
        and ecx, edx
        //if its positive we get the un-subtracted version
        not edx
        and ebx, edx
        //One of these will be zero and the other will be a converted 
        //two's complement number.
        add ecx, ebx
        mov [iFloatAAsInt], ecx
    }

    //Here we go again for fB

    _asm
    {
        //copy the float
        mov eax, [fB]
        //copy the again
        mov ebx, eax
        //sign extend into edx
        cdq
        //copy 0x80000000 to use for subtraction
        mov ecx, 0x80000000  
        sub ecx, eax
        and ecx, edx
        //if its positive we get the unsubtracted version
        not edx
        and ebx, edx
        add ecx, ebx
        mov [iFloatBAsInt], ecx
    }

    //This is the c code it is based on
    //int iAmountApart = abs(iFloatAAsInt - iFloatBAsInt);

    //this is our fast integer abs
    //note that there is nothing to handle the weird number 0x80000000
    //which would cause an overflow. We assume that the numbers that are being 
    //tested would never be that far apart
    _asm
    {
        mov eax, [iFloatAAsInt]
        sub eax, [iFloatBAsInt]
        mov ebx, eax
        //see if the result was negative
        cdq
        //neg in case it is negative
        neg eax
        and eax, edx
        //if it was positive edx will be zero so we not it
        not edx
        //in the positive case this gives use the result of 
        //iFloatAAsInt - iFloatBAsInt
        and ebx, edx
        //One of these are going to be a positive number and one is zero
        add eax, ebx 
        //eax now holds abs(iFloatAAsInt - iFloatBAsInt) or iIntDiff

        //old c code
        //the assembly removes the branch
        //
        //if(iIntDiff <= iMaxFloatsDifference)
        //{
        //    return true;
        //}

        //eax is the abs(iFloatAAsInt - iFloatBAsInt)
        //we copy it to ebx for later
        mov ebx, eax
        
        mov eax, [iMaxFloatsDifference]
        //this will be negative if iIntDiff is greater
        //then iMaxFloatsDifference. We assume iMaxFloatsDifference is 
        //greater then zero
        sub eax, ebx
        cdq
        //if edx is 0xffffffff then difference is outside our tolerance
        //we should return false so we not and then and it with one
        not edx
        and edx, 0x00000001
        //bool is 8 bit so we just want the bottom part of edx
        mov [returnValue], dl
    }

    return returnValue;
}

struct ColinearTestData
{
    bool m_bIsColinear;

    double m_kLastValue;
    double m_kCurrentValue;
    double m_kNextValue;

    MTime m_kLastTime;
    MTime m_kCurrentTime;
    MTime m_kNextTime;
    
};

char szPlugType[10][16] = 
{
    "translateX",
    "translateY",
    "translateZ",
    "rotateX",
    "rotateY",
    "rotateZ",
    "scaleX",
    "scaleY",
    "scaleZ",
    "visibility",
};

enum PlugType
{
    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,
    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,
    SCALE_X,
    SCALE_Y,
    SCALE_Z,
    VISIBILITY,
    QUATERNION_W,
    PLUG_TYPE_MAX
};

float kMaxTan = 5729577.9485111479f;

#define XMASK 1
#define YMASK 2
#define ZMASK 4


//---------------------------------------------------------------------------
//   ANIMATION CONSTANTS
//---------------------------------------------------------------------------

MTime kANIMATION_START_TIME;
MTime kANIMATION_END_TIME;


// Same as SKeyInfo, but this structure is
// for when we collapse all three axis...

MTime::Unit GetTimeUnits()
{
    MStatus s;
    MString sValue;
    s = MGlobal::executeCommand(MString("currentUnit -q -t;"), sValue);

    NIASSERT(s == MS::kSuccess);

    if( strcmp(sValue.asChar(), "game") == 0)
    {
        return MTime::kGames;
    }
    else if( strcmp(sValue.asChar(), "film") == 0)
    {
        return MTime::kFilm;
    }
    else if( strcmp(sValue.asChar(), "pal") == 0)
    {
        return MTime::kPALFrame;
    }
    else if( strcmp(sValue.asChar(), "ntsc") == 0)
    {
        return MTime::kNTSCFrame;
    }
    else if( strcmp(sValue.asChar(), "show") == 0)
    {
        return MTime::kShowScan;
    }
    else if( strcmp(sValue.asChar(), "palf") == 0)
    {
        return MTime::kPALField;
    }
    else if( strcmp(sValue.asChar(), "ntscf") == 0)
    {
        return MTime::kNTSCField;
    }
    else 
    {
        char szBuffer[512];
        NiSprintf(szBuffer, 512,
            "Error Reading current working unit for time: %s is "
            "not supported.\n", sValue.asChar());
        DtExt_Err(szBuffer);

        return MTime::kNTSCFrame;
    }
}

void LoadAnimationConstants()
{

#ifdef MAYA60
    kANIMATION_START_TIME = MAnimControl::animationStartTime();
    kANIMATION_END_TIME = MAnimControl::animationEndTime();
#else

    MStatus s;

    MTime::Unit kTimeUnit = GetTimeUnits();
    kANIMATION_START_TIME.setUnit(kTimeUnit);
    kANIMATION_END_TIME.setUnit(kTimeUnit);
    
    // Get the Animation Start Time
    double dValue;
    s = MGlobal::executeCommand(MString("playbackOptions -q -ast;"), dValue);

    NIASSERT(s == MS::kSuccess);
    kANIMATION_START_TIME.setValue(dValue);

    // Get the Animation End Time
    s = MGlobal::executeCommand(MString("playbackOptions -q -aet;"), dValue);

    NIASSERT(s == MS::kSuccess);
    kANIMATION_END_TIME.setValue(dValue);

#endif
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct SCombinedKeyInfo : public NiMemObject
{
    int m_iOrigIndex[3];        // Indexs into uncombined key info.

    float m_fInTanSlope[3];
    float m_fOutTanSlope[3];
    float m_fInWeight[3];
    float m_fOutWeight[3];

    float m_fKeyTime;
    double m_dValue[3];

    SCombinedKeyInfo(void);
} SCombinedKeyInfo;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SCombinedKeyInfo::SCombinedKeyInfo(void)
{
    for (int i=0; i<3; i++)
    {
        m_iOrigIndex[i]     = -1;
        m_fInTanSlope[i]    = 0;
        m_fOutTanSlope[i]   = 0;
        m_dValue[i]         = 0;
    }
    m_fKeyTime = 0.0f;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class kCombinedAnimCurveInfo : public NiMemObject
{
    public:
        bool    bInUse;
        bool    bIsWeighted;
        MString msTypeName;
        MString msName;

        unsigned int m_NumKeyFrames;

        SCombinedKeyInfo *pKeyInfo;

        MFnAnimCurve::AnimCurveType eCurveType;

        kCombinedAnimCurveInfo(void);
        ~kCombinedAnimCurveInfo(void);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
kCombinedAnimCurveInfo::~kCombinedAnimCurveInfo(void)
{
    if (pKeyInfo)
        NiDelete [] pKeyInfo;
    pKeyInfo = NULL;
}
//---------------------------------------------------------------------------
kCombinedAnimCurveInfo::kCombinedAnimCurveInfo(void)
{
    bInUse = false;
    pKeyInfo = NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct SKeyInfo : public NiMemObject
{
    int m_iCombIndex;       // Indices into Combined key info.

    MFnAnimCurve::TangentType   m_eInTangentType;
    MFnAnimCurve::TangentType   m_eOutTangentType;

    float m_fInTanX;
    float m_fInTanY;
    float m_fOutTanX;
    float m_fOutTanY;

    float m_fInWeight;
    float m_fOutWeight;

    float m_fKeyTime;
    double m_dValue;

    SKeyInfo(void);
} SKeyInfo;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SKeyInfo::
SKeyInfo(void)
{
    m_iCombIndex        = -1;
    m_fKeyTime          = 0.0f;
    m_dValue            = 0.0f;
    m_fInTanX           = 0.0f;
    m_fInTanY           = 0.0f;
    m_fOutTanX          = 0.0f;
    m_fOutTanY          = 0.0f;
}
//---------------------------------------------------------------------------
class kAnimCurveInfo: public NiMemObject
{
    public:
        bool    bInUse;
        bool    bIsWeighted;
        MString msTypeName;
        MString msName;

        unsigned int m_NumKeyFrames;

        SKeyInfo *pKeyInfo;

        MFnAnimCurve::AnimCurveType eCurveType;

        kAnimCurveInfo(void);
        ~kAnimCurveInfo(void);
};
//---------------------------------------------------------------------------
kAnimCurveInfo::~kAnimCurveInfo(void)
{
    if (pKeyInfo)
    {
        NiDelete[] pKeyInfo;
    }
    pKeyInfo = NULL;
}
//---------------------------------------------------------------------------
kAnimCurveInfo::kAnimCurveInfo(void)
{
    bInUse = false;
    pKeyInfo = NULL;
    m_NumKeyFrames = 0; 
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class kDtAnimInfo
{
    public:
        kAnimCurveInfo          **m_ppModelAnimCurve;
        kCombinedAnimCurveInfo  **m_ppCombinedModelAnimCurve;

        int m_iComponents;

        kDtAnimInfo(void);
        ~kDtAnimInfo(void);
        int AnimNew(void);
        void Reset(void);
        void ArchiveAnimInfoFromPlug( int iComponentID, 
                             MFnDependencyNode &transformDependNode,
                             int iPlugType );
        void CombineKeyFrames(int iComponentID, int iGroup);

        void MergeTransAxis(kCombinedAnimCurveInfo *pCCurves,
               kAnimCurveInfo *pCurves, int *pTags, unsigned int *pTotalCnt, 
               int axis, int iComponentID); 

        void EvalTangentsAndPos(int iComponentID, int iGroup, int iAxis, 
            int iMask, int *pTags,const MFnAnimCurve& kAC);
        
        void InterpolateHermite(SKeyInfo *pLeft,SKeyInfo *pRight,
                 SCombinedKeyInfo *pMiddle, int iAxis);
        void InterpolateBezier(SKeyInfo *pLeft,SKeyInfo *pRight,
                 SCombinedKeyInfo *pMiddle, int iAxis);

    protected:

        class BakedAnimCurve
        {
        public:
            MObject m_kObject;
            MObject Object() const { return m_kObject; }
            void Object(MObject val) { m_kObject = val; }
            int m_iPlugType;
            MPlug m_kPlug;
            MFnAnimCurve m_kCurve;
            bool m_bCreatedCurve;
            
            MTime GetStartTime()
            {
                return m_kStartTime;
            }

            MTime GetEndTime()
            {
                return m_kEndTime;
            }

            void SetStartTime(MTime startTime)
            {
                m_kStartTime = startTime;
            }

            void SetEndTime(MTime endTime)
            {
                m_kEndTime = endTime;
            }

            virtual double GetValue()
            {
                double dValue;
                m_kPlug.getValue(dValue);
                return dValue;
            }

#ifdef NIDEBUG
            const char* GetDebugName()
            {
                return &m_debugName[0];
            }

            void CopyDebugName(const char* name)
            {
                NIASSERT(name);
                if(name != NULL)
                {
                    NiStrcpy(m_debugName, 256, name);
                }
            }
#endif


            BakedAnimCurve() { m_bCreatedCurve = false; }
            virtual ~BakedAnimCurve();

        private:
            MTime m_kStartTime;
            MTime m_kEndTime;

#ifdef NIDEBUG
            char m_debugName[256];
#endif

        };

        class QuaternionBakedAnimCurve : public BakedAnimCurve
        {
        public:

            QuaternionBakedAnimCurve() { m_bCreatedCurve = false; }
            virtual ~QuaternionBakedAnimCurve();

            virtual double GetValue()
            {
                MStatus kStat = MStatus::kSuccess;
                double returnValue = 0.0;

                NIASSERT(m_kObject.hasFn(MFn::kDagNode));
                if(m_kObject.hasFn(MFn::kDagNode))
                {
                    MFnDagNode kDagNodeFn (m_kObject);
#if NIDEBUG
                    MString name = kDagNodeFn.name();
                    const char* debugName = name.asChar();
                    NI_UNUSED_ARG(debugName);
#endif
                    MMatrix kMatrix = 
                        kDagNodeFn.transformationMatrix(&kStat);
                    
                    NIASSERT(kStat == MStatus::kSuccess);
                    if(kStat != MStatus::kSuccess)
                    {
                        //Need some way of handling errors
                        return 0.0;
                    }
                    
                    MTransformationMatrix kTransformationMatrix(kMatrix);
                    
                    double x;
                    double y;
                    double z;
                    double w;
                    kTransformationMatrix.getRotationQuaternion(x, y, z, w);

                    switch(m_iPlugType)
                    {
                    case ROTATE_X:
                        returnValue = 1.0f * x;
                        break;
                    case ROTATE_Y:
                        returnValue = 1.0f * y;
                        break;
                    case ROTATE_Z:
                        returnValue = 1.0f * z;
                        break;
                    case QUATERNION_W:
                        returnValue = 1.0f * w;
                        break;
                    default:
                        NIASSERT(0);
                    }
                }
                else
                {
                    NIASSERT(0);
                    returnValue = 0.0;
                }

                return returnValue;
            }
            
        };

       BakedAnimCurve** m_kBakedCurves;
       unsigned int m_uiNumBakedAnimations;

        void RemoveSingleKeyEulerRotations(int iComponentID);
        void AllocateBakedAnimationCurves();
        int CreateBakedAnimationCurves();
        void DeleteBakedAnimationCurves();
        bool GetBakedAnimationCurve(MObject kObject, int iCurveType, 
            MFnAnimCurve& kAnimCurve);
        bool IsColinear(const ColinearTestData& colinearTestData);
        bool IsColinear(const ColinearTestData& colinearTestDataX, 
            const ColinearTestData& colinearTestDataY, 
            const ColinearTestData& colinearTestDataZ, 
            const ColinearTestData& colinearTestDataW);

        bool GetBakedAnimationCurve(int iComponentID, int iCurveType, 
            MFnAnimCurve& kAnimCurve);

        void GetMaximumTimeRange(const MFnAnimCurve& kAnimCurve, 
            MTime& kStartTime, MTime& kEndTime, MStatus& kStatus);

        bool DisconnectPhysics(MDGModifier& kDGModifier);
        void ReconnectPhysics(MDGModifier& kDGModifier);
};
//---------------------------------------------------------------------------
kDtAnimInfo::BakedAnimCurve::~BakedAnimCurve()
{
    MStatus kStatus;
    if (m_bCreatedCurve)
    {
        MObject obj = m_kCurve.object();
        kStatus = MGlobal::deleteNode(obj);
    }
#ifdef NIDEBUG
    if(kStatus != MStatus::kSuccess)
    {
        NIASSERT(0);
    }
#endif
    
}
//---------------------------------------------------------------------------
kDtAnimInfo::QuaternionBakedAnimCurve::~QuaternionBakedAnimCurve()
{

}
//---------------------------------------------------------------------------
bool CheckForExtraAttribute_Dup(MFnDependencyNode &kDepNode, 
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
bool GetExtraAttribute_Dup(MFnDependencyNode& kDepNode, char *szAttributeName, 
                       bool bIgnoreCase, bool &bValue)
{
    NI_UNUSED_ARG(szAttributeName);
    NI_UNUSED_ARG(bIgnoreCase);
    MFnAttribute kAttribute;
    MStatus kStatus;


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
//---------------------------------------------------------------------------
bool DtExt_GetAnimCurve(MObject MTransform, const char* pcPlugName, 
    MFnAnimCurve& kAnimCurve, bool& bBake, bool& bFound)
{
    MStatus status;
    MFnDependencyNode transformDependNode(MTransform,&status);

#ifdef NIDEBUG
    char testNameBuffer[256];
    NiStrcpy(testNameBuffer, 255,"l_middle01_jnt");

    if(strcmp(transformDependNode.name().asChar(), testNameBuffer) == 0)
    {
        NIASSERT(0);
    }
#endif

    if (status!=MS::kSuccess)
        return false;

    // Find the plug
    MPlug transPlug = transformDependNode.findPlug(pcPlugName, &status);

#ifdef NIDEBUG
    MString kTransPlugName = transPlug.name();
    const char* pcTransPlugName = kTransPlugName.asChar();
    NI_UNUSED_ARG(pcTransPlugName);
#endif

    if (status!=MS::kSuccess)
        return false;

    MPlugArray plugArr;
    MPlug destPlug;
    MObject destNodeObj;

    // If the plug isn't connected it isn't animated
    if (!transPlug.isConnected()) 
    {
        // Check for a parent plug connection
        if (!transPlug.isChild())
        {
            return false;
        }

        MPlug kParent = transPlug.parent();
        
#ifdef NIDEBUG
        MString kTransPlugParenName = kParent.name();
        const char* pcTransPlugParenName = kTransPlugParenName.asChar();
        NI_UNUSED_ARG(pcTransPlugParenName);
#endif

        // See if the parent is connected to something
        kParent.connectedTo(plugArr, true, false, &status);

        if (plugArr.length() != 1)
            return false;

        destPlug = plugArr[0];
#ifdef NIDEBUG
        MString kDestPlugName = destPlug.name();
        const char* pcDestPlugName = kDestPlugName.asChar();
        NI_UNUSED_ARG(pcDestPlugName);
#endif

        destNodeObj = destPlug.node();

        // Find the plug index for the transPlug
        unsigned int uiIndex = 0;
        for(uiIndex = 0; uiIndex < kParent.numChildren(); uiIndex++)
        {
            if (kParent.child(uiIndex) == transPlug)
            {
                break;
            }
        }

        if (uiIndex == kParent.numChildren())
            return false;

        if(destPlug.numChildren() > 0)
        {
            // Set the destPlug to be the appropriate child
            destPlug = destPlug.child(uiIndex, &status);
        }

        if( status != MS::kSuccess)
            return false;
    }
    else
    {
        transPlug.connectedTo(plugArr, true, false, &status);

        int iLength = plugArr.length();
        NI_UNUSED_ARG(iLength);

        if (plugArr.length() != 1)
        {
            //if length > 1, plug assigned to multiple shapes?  Not supported
            //NIASSERT(0);
            return false;
        }

        destPlug = plugArr[0];
        destNodeObj = destPlug.node();
    }


    char szBuffer[256];
    MFnDependencyNode DestNode(destNodeObj,&status);
    NiStrcpy(szBuffer, 256, destNodeObj.apiTypeStr());
    NiStrcpy(szBuffer, 256, DestNode.name().asChar());



    // Check for a character
    if (destNodeObj.hasFn(MFn::kCharacter))
    {
        destPlug.connectedTo(plugArr, true, false, &status);

        if (plugArr.length() == 1)
        {
            destPlug = plugArr[0];
            destNodeObj = destPlug.node();
        }
    }


    DestNode.setObject(destNodeObj);
    NiStrcpy(szBuffer, 256, destNodeObj.apiTypeStr());
    NiStrcpy(szBuffer, 256, DestNode.name().asChar());

    // Check for a Handle
    if (destNodeObj.hasFn(MFn::kTransform))
    {
        destPlug.connectedTo(plugArr, true, false, &status);

        if (plugArr.length() == 1)
        {
            destPlug = plugArr[0];
            destNodeObj = destPlug.node();
        }
    }

    DestNode.setObject(destNodeObj);
    NiStrcpy(szBuffer, 256, destNodeObj.apiTypeStr());
    NiStrcpy(szBuffer, 256, DestNode.name().asChar());

    //Should we sample positions
    bool bSamplePosition = false;
    bool hasDoPositionSampling = false;
    bool hasDontPositionSampling = false;

    //I need the functionality of this but it is in plugin so I duplicated.
    //Utterly retarted, but I am not trying to throw more stuff in the 
    //MdtLayer 

    if(CheckForExtraAttribute_Dup(transformDependNode, 
        "GamebryoSamplePosition", true))
    {
        bool bValue;
        GetExtraAttribute_Dup(transformDependNode, "GamebryoSamplePosition", 
            true, bValue);

        if(bValue)
        {
            hasDoPositionSampling = true;
        }
        else
        {
            hasDontPositionSampling = true;
        }
    }

    if(hasDoPositionSampling)
    {
        bSamplePosition = true;
    }
    else if(g_bSampleAllTranslation && !hasDontPositionSampling)
    {
        bSamplePosition = true;
    }

    //Should we sample rotations
    bool bSampleRotations = false;
    bool hasDoRotationSampling = false;
    bool hasDontRotationSampling = false;

    if(CheckForExtraAttribute_Dup(transformDependNode, 
        "GamebryoSampleRotation", true))
    {
        bool bValue;
        GetExtraAttribute_Dup(transformDependNode, "GamebryoSampleRotation", 
            true, bValue);

        if(bValue)
        {
            hasDoRotationSampling = true;
        }
        else
        {
            hasDontRotationSampling = true;
        }
    }

    if(hasDoRotationSampling)
    {
        bSampleRotations = true;
    }
    else if(g_bSampleAllRotation && !hasDontRotationSampling)
    {
        bSampleRotations = true;
    }

    if (destNodeObj.hasFn(MFn::kAnimCurve))
    {
      
        status = kAnimCurve.setObject(destNodeObj);
        if (status!=MS::kSuccess)
            return false;

        // Check for Animation Curves we recognize and can convert directly
        if ((kAnimCurve.animCurveType() == MFnAnimCurve::kAnimCurveTA) ||
            (kAnimCurve.animCurveType() == MFnAnimCurve::kAnimCurveTL) ||
            (kAnimCurve.animCurveType() == MFnAnimCurve::kAnimCurveTU))
        {

            if(strncmp(pcPlugName, szPlugType[TRANSLATE_X], 11) == 0 || 
                strncmp(pcPlugName, szPlugType[TRANSLATE_Y], 11) == 0 ||
                strncmp(pcPlugName, szPlugType[TRANSLATE_Z], 11) == 0)
            {
                if(bSamplePosition)
                {
                    bBake = true;
                }
                else
                {
                    bBake = false;
                }

                bFound = true;
            }
            else if(strncmp(pcPlugName, szPlugType[ROTATE_X], 11) == 0 || 
                strncmp(pcPlugName, szPlugType[ROTATE_Y], 11) == 0 ||
                strncmp(pcPlugName, szPlugType[ROTATE_Z], 11) == 0)
            {
                if(bSampleRotations)
                {
                    bBake = true;
                }
                else
                {
                    bBake = false;
                }
                
                bFound = true;
            }
        }
        else
        {
            bFound = false;
            bBake = true;
        }
    }
    else
    {
        // We are connected but we can't resolve to what so we should bake.
        bFound = false;
        bBake = true;
        
    }

    return true;
}
//---------------------------------------------------------------------------
bool DtExt_GetAnimCurve(int iComponentID, int iCurveType, 
    MFnAnimCurve& kAnimCurve)
{
    // Get a particular animcurve from a shape...
    MObject mobject;
    
    int iResult = gMDtObjectGetTransform(iComponentID, mobject);

    if (iResult==false) 
        return false;

    bool bBake = false;
    bool bFound = false;
    if (!DtExt_GetAnimCurve(mobject, szPlugType[iCurveType], kAnimCurve, 
        bBake, bFound))
        return false;

    // only return true if we found the curve
    return bFound;

}
//---------------------------------------------------------------------------
kDtAnimInfo::~kDtAnimInfo(void)
{
    // Allocate 10 TranslateXYZ, RotateXYZ, ScaleXYZ, Visibility
    // for each model.
    Reset();
}
//---------------------------------------------------------------------------
void kDtAnimInfo::Reset(void)
{
    if (m_ppModelAnimCurve)
    {
        for(int i=0; i<m_iComponents; i++)
        {
            // 10 elements
            kAnimCurveInfo *pCurves = m_ppModelAnimCurve[i];
            if (pCurves)
                NiExternalDelete [] pCurves;
            pCurves = NULL;
        }
        NiExternalDelete [] m_ppModelAnimCurve;
    }
    m_ppModelAnimCurve = NULL;

    if (m_ppCombinedModelAnimCurve)
    {
        for(int i=0; i<m_iComponents; i++)
        {
            // 10 elements
            kCombinedAnimCurveInfo *pCurves = m_ppCombinedModelAnimCurve[i];
            if (pCurves)
                NiDelete [] pCurves;
            pCurves = NULL;
        }
        NiFree(m_ppCombinedModelAnimCurve);
    }
    m_ppCombinedModelAnimCurve = NULL;

    m_iComponents = 0;
}
//---------------------------------------------------------------------------
kDtAnimInfo::kDtAnimInfo(void)
{
    m_iComponents = 0;
    m_ppModelAnimCurve          = NULL;
    m_ppCombinedModelAnimCurve  = NULL;

    m_kBakedCurves = NULL;
    m_uiNumBakedAnimations = 0;
}
//---------------------------------------------------------------------------
int kDtAnimInfo::AnimNew(void)
{
    // Return value:
    //  0: Success
    //  1: Cancellation
    //  2: Error

    // LOAD THE ANIMATION CONSTANTS
    LoadAnimationConstants();


    MStatus status;
    // For every shape we have in MDt
    // We will grab it's transform node
    // and pass it on to DtProcessAnimations

    // Animation Info will be accessable by shapeID

    m_iComponents = gMDtObjectGetUsed();

    if (m_iComponents < 1)
        return 0;

    // This shouldn't be done unless there is animations...
    // we will have to come back to make this more efficient
    
    m_ppModelAnimCurve = NiExternalNew kAnimCurveInfo*[m_iComponents];

    int i=0;
    for (; i<m_iComponents; i++)
        m_ppModelAnimCurve[i] = NULL;

    MDGModifier kDGModifier;
    bool bDidPhysics = DisconnectPhysics(kDGModifier);

    // Allocate the "BAKED" ANIMATIONS
    AllocateBakedAnimationCurves();

    // Create the "BAKED" ANIMATIONS
    int iBakedReturn = CreateBakedAnimationCurves();
    if (iBakedReturn)
    {
        DeleteBakedAnimationCurves();

        if (bDidPhysics)
            ReconnectPhysics(kDGModifier);
        
        return iBakedReturn;
    }
    
    if(DtExt_HaveGui() == true)
    {
        if (MProgressWindow::isCancelled())
        {
            DeleteBakedAnimationCurves();

            if (bDidPhysics)
                ReconnectPhysics(kDGModifier);
            
            return 1;
        }

        // Update the progress window
        MProgressWindow::setProgressStatus("Extracting Animation Curves");
        MProgressWindow::setProgressRange(0, m_iComponents);
        MProgressWindow::setProgress(0);
    }

    int iComponentID=0;
    for (; iComponentID < m_iComponents; iComponentID++)
    {
        if(DtExt_HaveGui()== true)
        {
            if (MProgressWindow::isCancelled())
            {
                DeleteBakedAnimationCurves();

                if (bDidPhysics)
                    ReconnectPhysics(kDGModifier);
                
                return 1;
            }

            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        MObject mobject;
        int bResult = gMDtObjectGetTransform(iComponentID, mobject);

        if (bResult==false)
        {
            NIASSERT(bResult);

            DeleteBakedAnimationCurves();

            if (bDidPhysics)
                ReconnectPhysics(kDGModifier);
            
            return 2;
        }

        MFnDependencyNode transformDependNode(mobject,&status);

#ifdef NIDEBUG
        const char* name = transformDependNode.name().asChar();
        if(strcmp(name, "CyberMonkey_Rig:rootJoint") == 0)
        {
            //int a = 0;
        }
#endif

        for (int iPlugType =0; iPlugType <11; iPlugType++)
        {
            ArchiveAnimInfoFromPlug( iComponentID, transformDependNode, 
                iPlugType );
        }

        RemoveSingleKeyEulerRotations(iComponentID);
    }

    // This shouldn't be done unless there is animations...
    // we will have to come back to make this more efficient
    m_ppCombinedModelAnimCurve = NiAlloc(kCombinedAnimCurveInfo*, 
        m_iComponents);

    for (i=0; i<m_iComponents; i++)
        m_ppCombinedModelAnimCurve[i] = NULL;

    if(DtExt_HaveGui() == true)
    {
        if (MProgressWindow::isCancelled())
        {
            DeleteBakedAnimationCurves();

            if (bDidPhysics)
                ReconnectPhysics(kDGModifier);
            
            return 1;
        }

        MProgressWindow::setProgressStatus("Combining Animation Curves");
        MProgressWindow::setProgressRange(0, m_iComponents);
        MProgressWindow::setProgress(0);
    }

    for (iComponentID=0; iComponentID < m_iComponents; iComponentID++)
    {
        if(DtExt_HaveGui()== true)
        {      
            if (MProgressWindow::isCancelled())
            {
                DeleteBakedAnimationCurves();

                if (bDidPhysics)
                    ReconnectPhysics(kDGModifier);
                
                return 1;
            }

            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        NIASSERT(m_ppCombinedModelAnimCurve[iComponentID]==NULL);

        // allocate space for TranslationXYZ, RotationXYZ, ScaleXYZ)
        m_ppCombinedModelAnimCurve[iComponentID] = 
            NiNew kCombinedAnimCurveInfo[3]; 

        // Now Combine our keyframes which may be different on different 
        // axis into one set of keyframes (one set for translation, one 
        // for rotation...)
        CombineKeyFrames(iComponentID,0); // Translation
        //CombineKeyFrames(iShapeID,1); // Rotation
        //CombineKeyFrames(iShapeID,2); // Scale
    }

    if(DtExt_HaveGui() == true)
    {
        MProgressWindow::setProgressStatus("Deleting Baked Curves");
    }

    // DELETE the "BAKED" ANIMATIONS
    DeleteBakedAnimationCurves();

    if (bDidPhysics)
        ReconnectPhysics(kDGModifier);
    
    return 0;
}
//---------------------------------------------------------------------------
void kDtAnimInfo::AllocateBakedAnimationCurves()
{
    MStatus kStatus;
    MPlugArray plugArr;

    // First Count the number of Baked Curves

    // Loop through each component
    for (int iComponentID=0; iComponentID < m_iComponents; iComponentID++)
    {
        MObject kObject;
        
        if (gMDtObjectGetTransform(iComponentID, kObject))
        {

            MFnDependencyNode transformDependNode(kObject);

#ifdef NIDEBUG
            char szBuffer[256];
            static char szNameToCheck[256];
            NiStrcpy(szBuffer, 256, transformDependNode.name().asChar());
            if (strncmp(szBuffer, szNameToCheck, 256) == 0)
            {
                //int iFoo = 1;
            }
#endif
            int iSampleRotation = 0;
            bool bHasRotationAnimationCurve = false;

            // Examine each plug
            for(int iPlugType = 0; iPlugType < 10; iPlugType++)
            {
                MFnAnimCurve kAnimCurve;
                bool bBake = false;
                bool bFound = false;

                if (DtExt_GetAnimCurve(kObject, szPlugType[iPlugType], 
                    kAnimCurve, bBake, bFound) &&  bBake)
                {
                    //Need to take into account if we are sampling rotations as
                    //Quaternions there is an additional curve
                    if(iPlugType > 2 && iPlugType < 6)
                    {
                        iSampleRotation++;

                        if(bFound)
                        {
                            bHasRotationAnimationCurve = true;
                        }

                        continue;
                    }
                    
                    m_uiNumBakedAnimations++;
                }
            }

            //what type of sampling should we do

            bool bForceQuaternion = false;
            bool hasDoQuatSampling = false;
            bool hasDoEulerSampling = false;

            if(CheckForExtraAttribute_Dup(transformDependNode, 
                "GamebryoSampleAsQuat", true))
            {
                bool bValue;
                GetExtraAttribute_Dup(transformDependNode, 
                    "GamebryoSampleAsQuat", true, bValue);

                if(bValue)
                {
                    hasDoQuatSampling = true;
                }
                else
                {
                    hasDoEulerSampling = true;
                }
            }


            if(hasDoQuatSampling)
            {
                bForceQuaternion = true;
            }
            else if(g_bSampleRotationAsQuaternion && !hasDoEulerSampling)
            {
                bForceQuaternion = true;
            }


            if(iSampleRotation > 0)
            {
                if(bHasRotationAnimationCurve && !bForceQuaternion)
                {
                    //three euler curves
                    m_uiNumBakedAnimations += 3;
                }
                else
                {
                    //four quaternion
                    m_uiNumBakedAnimations += 4;
                }
            }
        }
    }


    // Search for the ikHandles
    MItDag dagIterator( 
        MItDag::kDepthFirst,    // Traversal Type
        //MItDag::kBreadthFirst,    // Traversal Type
        MFn::kIkHandle,          // Filter - ie. what the iterator selects
        &kStatus );

    for( ; !dagIterator.isDone(); dagIterator.next() )
    {
        MFnIkHandle kIKHandle(dagIterator.item(), &kStatus);

        MDagPath kStartJoint;
        MDagPath kEndEffector;


        if ((kIKHandle.getStartJoint(kStartJoint) == MS::kSuccess) &&
            (kIKHandle.getEffector(kEndEffector) == MS::kSuccess))
        {
            // Start at the end effector and work our way up.
            MDagPath kCurrentPath = kEndEffector;
            kCurrentPath.pop();

            bool bFinished = false;

            // Verify the IK blend Value is near zero then the
            // IK Handle has no effect and we will skip it.
            MPlug kIKBlendPlug = kIKHandle.findPlug("ikBlend", &kStatus);
            if (kStatus == MS::kSuccess)
            {
                // Read the plug
                float fIKBlend = 0.0f;
                kIKBlendPlug.getValue(fIKBlend);

                // If the plug is near zero then we have finishe processing 
                // this IK handle
                if (fIKBlend <= 0.0001f)
                    bFinished = true;
            }

            while (!bFinished)
            {
                MFnDagNode kDagNode(kCurrentPath.node(), &kStatus);
                NIASSERT(kStatus == MS::kSuccess);


#ifdef NIDEBUG
                char szBuffer[256];
                static char szNameToCheck[256];
                NiStrcpy(szBuffer, 256, kDagNode.name().asChar());
                if (strncmp(szBuffer, szNameToCheck, 256) == 0)
                {
                    //int iFoo = 1;
                }
#endif // NIDEBUG

                int iRotationCurveIds[4] = {ROTATE_X, ROTATE_Y, ROTATE_Z, 
                    QUATERNION_W};
                NI_UNUSED_ARG(iRotationCurveIds);

                // Always sample joints connected to an IK Handle
                for (int iRotationCurveIndex = 0; iRotationCurveIndex < 4; 
                    iRotationCurveIndex++)
                {
                    m_uiNumBakedAnimations++;
                }

                bFinished = (kCurrentPath == kStartJoint);

                // Move to the next node up
                kCurrentPath.pop();
            }
        }
    }
    

    // Allocate the curves and then reset our counter
    m_kBakedCurves = NULL;

    //This will probably need to be sped up
    if (m_uiNumBakedAnimations)
        m_kBakedCurves = NiExternalNew BakedAnimCurve*[m_uiNumBakedAnimations];
}
//---------------------------------------------------------------------------
int kDtAnimInfo::CreateBakedAnimationCurves()
{
    // Return:
    // 0 Success
    // 1 Cancelled
    // 2 Error
    
    if (m_uiNumBakedAnimations == 0)
        return 0;

#ifdef NIDEBUG
    static char szNameToCheck[256];
    char szBuffer[256];
#endif // NIDEBUG
    MStatus kStatus;
    MPlugArray plugArr;
    unsigned int uiNumBakedAnimations = 0;



    // Collect the infor
    // Loop through each component
    for (int iComponentID=0; iComponentID < m_iComponents; iComponentID++)
    {
        MObject kObject;
        
        if (gMDtObjectGetTransform(iComponentID, kObject))
        {
            MFnDependencyNode transformDependNode(kObject);

#ifdef NIDEBUG
            NiStrcpy(szBuffer, 256, transformDependNode.name().asChar());
            if (strncmp(szBuffer, szNameToCheck, 256) == 0)
            {
                //int iFoo = 1;
            }
#endif // NIDEBUG

            bool bSampleRotation = false;
            bool bHasRotationAnimationCurve = false;
            bool bHasTranslationAnimationCurve = false;
            bool bHasScaleAnimationCurve = false;

            bool bForceQuaternion = false;
            bool hasDoQuatSampling = false;
            bool hasDoEulerSampling = false;

            if(CheckForExtraAttribute_Dup(transformDependNode, 
                "GamebryoSampleAsQuat", true))
            {
                bool bValue;
                GetExtraAttribute_Dup(transformDependNode, 
                    "GamebryoSampleAsQuat", true, bValue);

                if(bValue)
                {
                    hasDoQuatSampling = true;
                }
                else
                {
                    hasDoEulerSampling = true;
                }
            }

            if(hasDoQuatSampling)
            {
                bForceQuaternion = true;
            }
            else if(g_bSampleRotationAsQuaternion && !hasDoEulerSampling)
            {
                bForceQuaternion = true;
            }

            //this seems like a total waste
            //This code really needs to be reworked
            MTime::Unit kTimeUnit = GetTimeUnits();
            MTime kRotationCurveMaxStart(0.0, kTimeUnit);
            MTime kRotationCurveMaxEnd(0.0, kTimeUnit);

            MTime kTranslationCurveMaxStart(0.0, kTimeUnit);
            MTime kTranslationCurveMaxEnd(0.0, kTimeUnit);

            MTime kScaleCurveMaxStart(0.0, kTimeUnit);
            MTime kScaleCurveMaxEnd(0.0, kTimeUnit);

            int iTranslateXIndex = 0;
            int iTranslateYIndex = 0;
            int iTranslateZIndex = 0;

            int iScaleXIndex = 0;
            int iScaleYIndex = 0;
            int iScaleZIndex = 0;



            // Examine each plug
            for(int iPlugType = 0; iPlugType < 10; iPlugType++)
            {
                MFnAnimCurve kAnimCurve;
                bool bBake = false;
                bool bFound = false;

                if (DtExt_GetAnimCurve(kObject, szPlugType[iPlugType], 
                    kAnimCurve, bBake, bFound) && bBake)
                {
                    //We need to handle rotation differently
                    //if there any animations driving the rotation
                    //we don't sample it as quaternions. All other cases
                    //use quaternions

                    switch(iPlugType)
                    {
                    case TRANSLATE_X:
                        iTranslateXIndex = uiNumBakedAnimations;
                    case TRANSLATE_Y:
                        iTranslateYIndex = uiNumBakedAnimations;
                    case TRANSLATE_Z:
                        iTranslateZIndex = uiNumBakedAnimations;

                        if(bFound)
                        {
                            bHasTranslationAnimationCurve = true;

                            GetMaximumTimeRange(kAnimCurve, 
                                kTranslationCurveMaxStart, 
                                kTranslationCurveMaxEnd, kStatus);
                        }
                        break;

                    case ROTATE_X:
                    case ROTATE_Y:
                    case ROTATE_Z:
                        bSampleRotation = true;

                        if(bFound)
                        {
                            bHasRotationAnimationCurve = true;

                            GetMaximumTimeRange(kAnimCurve, 
                                kRotationCurveMaxStart, 
                                kRotationCurveMaxEnd, kStatus);
                        }

                        continue;
                        break;

                    case SCALE_X:
                        iScaleXIndex = uiNumBakedAnimations;
                    case SCALE_Y:
                        iScaleYIndex = uiNumBakedAnimations;
                    case SCALE_Z:
                        iScaleZIndex = uiNumBakedAnimations;

                        if(bFound)
                        {
                            bHasScaleAnimationCurve = true;

                            GetMaximumTimeRange(kAnimCurve, 
                                kScaleCurveMaxStart, 
                                kScaleCurveMaxEnd, kStatus);
                        }

                        break;
                                
                    }

                    m_kBakedCurves[uiNumBakedAnimations] = NiExternalNew 
                        BakedAnimCurve();


                    m_kBakedCurves[uiNumBakedAnimations]->m_kObject = kObject;
                    m_kBakedCurves[uiNumBakedAnimations]->m_kPlug = 
                        transformDependNode.findPlug(szPlugType[iPlugType], 
                        &kStatus);

                    m_kBakedCurves[uiNumBakedAnimations]->m_iPlugType = 
                        iPlugType;

                    // Create the Curve
                    m_kBakedCurves[uiNumBakedAnimations]->m_kCurve.create(
                        MFnAnimCurve::kAnimCurveTU, NULL, &kStatus);
                    m_kBakedCurves[uiNumBakedAnimations]->m_bCreatedCurve =
                        true;

                    m_kBakedCurves[uiNumBakedAnimations]->SetStartTime(
                        kANIMATION_START_TIME);
                    m_kBakedCurves[uiNumBakedAnimations]->SetEndTime(
                        kANIMATION_END_TIME);
#ifdef NIDEBUG
                    m_kBakedCurves[uiNumBakedAnimations]->CopyDebugName(
                        szBuffer);
#endif
                    uiNumBakedAnimations++;
                }

                // Delete the AnimCurve if one was created
            }

             if(bHasTranslationAnimationCurve)
            {
                //I need to get the translation curve indices
                //and reset the time range
                m_kBakedCurves[iTranslateXIndex]->SetStartTime(
                    kTranslationCurveMaxStart);
                m_kBakedCurves[iTranslateXIndex]->SetEndTime(
                    kTranslationCurveMaxEnd);

                m_kBakedCurves[iTranslateYIndex]->SetStartTime(
                    kTranslationCurveMaxStart);
                m_kBakedCurves[iTranslateYIndex]->SetEndTime(
                    kTranslationCurveMaxEnd);

                m_kBakedCurves[iTranslateZIndex]->SetStartTime(
                    kTranslationCurveMaxStart);
                m_kBakedCurves[iTranslateZIndex]->SetEndTime(
                    kTranslationCurveMaxEnd);
            }
            
            if(bHasScaleAnimationCurve)
            {
                //I need to get the translation curve indices
                //and reset the time range
                m_kBakedCurves[iScaleXIndex]->SetStartTime(
                    kTranslationCurveMaxStart);
                m_kBakedCurves[iScaleXIndex]->SetEndTime(
                    kTranslationCurveMaxEnd);

                m_kBakedCurves[iScaleYIndex]->SetStartTime(
                    kTranslationCurveMaxStart);
                m_kBakedCurves[iScaleYIndex]->SetEndTime(
                    kTranslationCurveMaxEnd);

                m_kBakedCurves[iScaleZIndex]->SetStartTime(
                    kTranslationCurveMaxStart);
                m_kBakedCurves[iScaleZIndex]->SetEndTime(
                    kTranslationCurveMaxEnd);
            }


            if(bSampleRotation == true)
            {
                int iRotationCurveIds[4] = {ROTATE_X, ROTATE_Y, ROTATE_Z, 
                    QUATERNION_W};

                if(bHasRotationAnimationCurve)
                {
                    int iMaxCurves = bForceQuaternion ? 4 : 3;

                    for(int iRotationIndex = 0; iRotationIndex < iMaxCurves; 
                        iRotationIndex++)
                    {
                        int iCurrentRotationId = iRotationCurveIds[
                            iRotationIndex];
                        
                        if(bForceQuaternion)
                        {
                            m_kBakedCurves[uiNumBakedAnimations] = 
                                NiExternalNew QuaternionBakedAnimCurve();
                        }
                        else
                        {
                            //use euler
                            m_kBakedCurves[uiNumBakedAnimations] = 
                                NiExternalNew BakedAnimCurve();
                        }

                        m_kBakedCurves[uiNumBakedAnimations]->m_kObject = 
                            kObject;
                        m_kBakedCurves[uiNumBakedAnimations]->m_kPlug = 
                            transformDependNode.findPlug(szPlugType[
                                iCurrentRotationId], 
                            &kStatus);;

                        m_kBakedCurves[uiNumBakedAnimations]->m_iPlugType = 
                            iCurrentRotationId;

                        // Create the Curve
                        m_kBakedCurves[uiNumBakedAnimations]->m_kCurve.create(
                            MFnAnimCurve::kAnimCurveTU, NULL, &kStatus);
                        m_kBakedCurves[uiNumBakedAnimations]->m_bCreatedCurve =
                            true;

                        m_kBakedCurves[uiNumBakedAnimations]->SetStartTime(
                            kRotationCurveMaxStart);
                        m_kBakedCurves[uiNumBakedAnimations]->SetEndTime(
                            kRotationCurveMaxEnd);
#ifdef NIDEBUG
                        m_kBakedCurves[uiNumBakedAnimations]->CopyDebugName(
                            szBuffer);
#endif

                        uiNumBakedAnimations++;
                    }

                }
                else
                {
                    for (int iRotationCurveIndex = 0; iRotationCurveIndex < 4; 
                        iRotationCurveIndex++)
                    {
                        int iPlugType = iRotationCurveIds[iRotationCurveIndex];

                        m_kBakedCurves[uiNumBakedAnimations] = NiExternalNew 
                            QuaternionBakedAnimCurve();

                        // X, Y, and Z rotation for each
                        m_kBakedCurves[uiNumBakedAnimations]->m_kObject = 
                            kObject;

                        m_kBakedCurves[uiNumBakedAnimations]->m_iPlugType = 
                            iPlugType;

                        // Create the Curve
                        m_kBakedCurves[uiNumBakedAnimations]->m_kCurve.create(
                            MFnAnimCurve::kAnimCurveTU, NULL, &kStatus);
                        m_kBakedCurves[uiNumBakedAnimations]->m_bCreatedCurve =
                            true;

                        m_kBakedCurves[uiNumBakedAnimations]->SetStartTime(
                            kANIMATION_START_TIME);
                        m_kBakedCurves[uiNumBakedAnimations]->SetEndTime(
                            kANIMATION_END_TIME);
#ifdef NIDEBUG
                        m_kBakedCurves[uiNumBakedAnimations]->CopyDebugName(
                            szBuffer);
#endif

                        uiNumBakedAnimations++;
                    }
                }
            }
        }
    }


    // Search for the ikHandles
    MItDag dagIterator( 
        MItDag::kDepthFirst,    // Traversal Type
        //MItDag::kBreadthFirst,    // Traversal Type
        MFn::kIkHandle,          // Filter - ie. what the iterator selects
        &kStatus );


    for( ; !dagIterator.isDone(); dagIterator.next() )
    {
        MFnIkHandle kIKHandle(dagIterator.item(), &kStatus);

        MDagPath kStartJoint;
        MDagPath kEndEffector;
        
        if ((kIKHandle.getStartJoint(kStartJoint) == MS::kSuccess) &&
            (kIKHandle.getEffector(kEndEffector) == MS::kSuccess))
        {

            MDagPath kCurrentPath = kEndEffector;
            kCurrentPath.pop();
            bool bFinished = false;

            // Verify the IK blend Value is near zero then the
            // IK Handle has no effect and we will skip it.
            MPlug kIKBlendPlug = kIKHandle.findPlug("ikBlend", &kStatus);
            if (kStatus == MS::kSuccess)
            {
                // Read the plug
                float fIKBlend = 0.0f;
                kIKBlendPlug.getValue(fIKBlend);

                // If the plug is near zero then we have finishe processing 
                // this IK handle
                if (fIKBlend <= 0.0001f)
                    bFinished = true;
            }


            while (!bFinished)
            {
                MObject kObject = kCurrentPath.node();
                MFnDagNode kNode(kObject);


#ifdef NIDEBUG
                char szBuffer[256];
                NiStrcpy(szBuffer, 256, kNode.name().asChar());
                if (strcmp(szBuffer, "CyberMonkey_Rig:l_kneeJoint") == 0)
                    //int iFoo = 1;
#endif // NIDEBUG

                int iIdx = 0;
                int iRotationCurveIds[4] = {ROTATE_X, ROTATE_Y, ROTATE_Z, 
                    QUATERNION_W};
                // Sample the rotations
                //Need to change
                for (int iRotationCurveIndex = 0; iRotationCurveIndex < 4; 
                    iRotationCurveIndex++)
                {
                    int iPlugType = iRotationCurveIds[iRotationCurveIndex];

                    m_kBakedCurves[uiNumBakedAnimations] = NiExternalNew 
                        QuaternionBakedAnimCurve();

                    // X, Y, and Z rotation for each
                    m_kBakedCurves[uiNumBakedAnimations]->m_kObject = 
                        kObject;

                    m_kBakedCurves[uiNumBakedAnimations]->m_iPlugType = 
                        iPlugType;

                    // Create the Curve
                    m_kBakedCurves[uiNumBakedAnimations]->m_kCurve.create(
                        MFnAnimCurve::kAnimCurveTU, NULL, &kStatus);
                    m_kBakedCurves[uiNumBakedAnimations]->m_bCreatedCurve =
                        true;

                    m_kBakedCurves[uiNumBakedAnimations]->SetStartTime(
                        kANIMATION_START_TIME);
                    m_kBakedCurves[uiNumBakedAnimations]->SetEndTime(
                        kANIMATION_END_TIME);
#ifdef NIDEBUG
                    m_kBakedCurves[uiNumBakedAnimations]->CopyDebugName(
                        szBuffer);
#endif

                    uiNumBakedAnimations++;
                }

                bFinished = (kCurrentPath == kStartJoint);
                kCurrentPath.pop();
            }
        }
    }

    // Get the Animation Range
    MTime kMinTime = kANIMATION_START_TIME;
    MTime kMaxTime = kANIMATION_END_TIME;
    //
    kMaxTime += 2;
    MTime kCurrentTime = MAnimControl::currentTime();
    MTime kLoopTime = kMinTime;

    MTime::Unit kUnti = kMinTime.unit();
    NI_UNUSED_ARG(kUnti);

    if(DtExt_HaveGui() == true)
    {
        // Update the progress window
        MProgressWindow::setProgressStatus("Baking Animations");
        MProgressWindow::setProgressRange(0, 
            (int)(kMaxTime -kMinTime).value());
        MProgressWindow::setProgress(0);
    }

    // Create a linear key for each plug for each frame.
    while (kLoopTime < kMaxTime)
    {

        if(DtExt_HaveGui() == true)
        {
            if (MProgressWindow::isCancelled())
            {
                return 1;
            }

            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        MAnimControl::setCurrentTime(kLoopTime);

        //this code compares three keyframes to allow for optimizations if
        //the keys are on a line

        for(unsigned int uiLoop = 0; uiLoop < uiNumBakedAnimations; uiLoop++)
        {

            m_kBakedCurves[uiLoop]->m_kCurve.addKey(
                    kLoopTime, 
                    m_kBakedCurves[uiLoop]->GetValue(), 
                    MFnAnimCurve::kTangentLinear, 
                    MFnAnimCurve::kTangentLinear, 
                    NULL, &kStatus);

        }

        kLoopTime++;
    }

    
    return 0;
}
//---------------------------------------------------------------------------
void kDtAnimInfo::GetMaximumTimeRange(const MFnAnimCurve& kAnimCurve, 
                                      MTime& kStartTime, MTime& kEndTime, 
                                      MStatus& kStatus)
{

    int iMaxKeys = kAnimCurve.numKeys(&kStatus);

    MTime startTime = kAnimCurve.time(0, &kStatus);
    MTime endTime = kAnimCurve.time(iMaxKeys - 1, &kStatus);

    kStartTime = startTime < kStartTime ? startTime : kStartTime;

    kEndTime = endTime > kEndTime ? endTime : kEndTime;

    kStartTime = kStartTime < kANIMATION_START_TIME ? 
        kANIMATION_START_TIME : kStartTime;
    kEndTime = kEndTime > kANIMATION_END_TIME ? 
        kANIMATION_END_TIME : kEndTime;
}

//---------------------------------------------------------------------------
bool kDtAnimInfo::IsColinear(const ColinearTestData& colinearTestData)
{
    bool returnValue = false;
    


    double currentTimeValue = colinearTestData.m_kCurrentTime.value();
    double nextTimeValue = colinearTestData.m_kNextTime.value();
    double lastTimeValue = colinearTestData.m_kLastTime.value();

    NIASSERT((currentTimeValue != nextTimeValue) 
        && (nextTimeValue != lastTimeValue));

    double dNextValue = colinearTestData.m_kNextValue;
    double dLastValue = colinearTestData.m_kLastValue;
    double dCurrentValue = colinearTestData.m_kCurrentValue;

    //test for colinear
    //equation of a line
    //v = bt + m
    //b = (v1 - v0)/(t1 - t0)
    //m = v - bt

    //protect against division by zero
    if(nextTimeValue - lastTimeValue == 0)
    {
        //we just don't optimize in this case
        return false;
    }

    double slope = (dNextValue - dLastValue)/(nextTimeValue - lastTimeValue);

    double intercept = dLastValue - (slope * lastTimeValue);

    double linearlyInterpolatedValue = (slope * currentTimeValue) + intercept; 

    if(floatDistanceCompare((float)dCurrentValue, 
        (float)linearlyInterpolatedValue, 8))
    {
        returnValue = true;
    }
    else
    {
        returnValue = false;
    }

    return returnValue;
}
//---------------------------------------------------------------------------
bool kDtAnimInfo::IsColinear(const ColinearTestData& colinearTestDataX, 
                const ColinearTestData& colinearTestDataY, 
                const ColinearTestData& colinearTestDataZ, 
                const ColinearTestData& colinearTestDataW)
{
    //it has to colinear for all projects
    bool returnValue = IsColinear(colinearTestDataX);
    returnValue &= IsColinear(colinearTestDataY);
    returnValue &= IsColinear(colinearTestDataZ);
    returnValue &= IsColinear(colinearTestDataW);
    
    return returnValue;
}

//---------------------------------------------------------------------------
void kDtAnimInfo::DeleteBakedAnimationCurves()
{
    for(unsigned int i = 0; i < m_uiNumBakedAnimations; i++)
    {
        NiExternalDelete m_kBakedCurves[i];
    }
    NiExternalDelete[] m_kBakedCurves;
    m_kBakedCurves = NULL;
    m_uiNumBakedAnimations = 0;
}
//---------------------------------------------------------------------------
bool kDtAnimInfo::GetBakedAnimationCurve(MObject kObject, int iCurveType,
    MFnAnimCurve& kAnimCurve)
{
    if (!m_kBakedCurves)
        return false;

    for(unsigned int uiLoop = 0; uiLoop < m_uiNumBakedAnimations; uiLoop++)
    {
        if ((m_kBakedCurves[uiLoop]->m_kObject == kObject) &&
            (m_kBakedCurves[uiLoop]->m_iPlugType == iCurveType))
        {
            kAnimCurve.setObject(m_kBakedCurves[uiLoop]->m_kCurve.object());
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool kDtAnimInfo::GetBakedAnimationCurve(int iComponentID, int iCurveType,
    MFnAnimCurve& kAnimCurve)
{
    MObject kObject;
    
    int iResult = gMDtObjectGetTransform(iComponentID, kObject);

    if (iResult==false) 
        return false;

    return GetBakedAnimationCurve(kObject, iCurveType, kAnimCurve);
}
//---------------------------------------------------------------------------
void kDtAnimInfo::ArchiveAnimInfoFromPlug( int iComponentID, 
    MFnDependencyNode &transformDependNode,
    int iPlugType )
{
    MStatus status;

#ifdef NIDEBUG
    char szBuffer2[256];
    NiStrcpy(szBuffer2, 255, transformDependNode.name().asChar());
#endif // NIDEBUG

    kAnimCurveInfo *pCurves = m_ppModelAnimCurve[iComponentID];

    MFnAnimCurve kAnimCurve;

    // Check for No animations
    if (!GetBakedAnimationCurve(transformDependNode.object(), iPlugType, 
        kAnimCurve))
    {
        bool bBake = false;
        bool bFound = false;

        
        // Find the Animation Curve
        if (!DtExt_GetAnimCurve(transformDependNode.object(), 
            szPlugType[iPlugType], kAnimCurve, bBake, bFound))
        {    
            return;
        }
    }
    // Allocate the curves if they are needed
    if (pCurves == NULL)
    {
        // This is our first time... we know our shape is animated
        m_ppModelAnimCurve[iComponentID] = NiNew kAnimCurveInfo[11];
        pCurves = m_ppModelAnimCurve[iComponentID];
    }

    // Info from AnimCurve
    pCurves = &pCurves[iPlugType];
    pCurves->bInUse = true;
    pCurves->bIsWeighted = kAnimCurve.isWeighted (&status ); 
    pCurves->msTypeName = kAnimCurve.typeName(&status);
    pCurves->msName     = kAnimCurve.name(&status);
    pCurves->eCurveType = kAnimCurve.animCurveType(&status);

    unsigned int uiNumMayaKeys = kAnimCurve.numKeyframes(&status);

    NIASSERT(pCurves->pKeyInfo == NULL);

    // Determine the number of keys adding an extra for each stepped key
    // Don't add extra keys for a final stepped key
    int iTotalKeys = uiNumMayaKeys;
    unsigned int uiLoop;
    for(uiLoop = 0; (int)uiLoop < (int)(uiNumMayaKeys - 1); uiLoop++)
    {
        // Add an extra key if it is a step key
        if(kAnimCurve.outTangentType(uiLoop,&status) == 
            MFnAnimCurve::kTangentStep)
            iTotalKeys++;
    }

    pCurves->m_NumKeyFrames = iTotalKeys;
    pCurves->pKeyInfo = NiNew SKeyInfo[iTotalKeys];

    // Index.
    bool bInTangent = true;
    int iCurrentKey = 0;
    for (unsigned int index=0; index < uiNumMayaKeys; index++,iCurrentKey++)
    {
        pCurves->pKeyInfo[iCurrentKey].m_eInTangentType  = 
            kAnimCurve.inTangentType(index,&status);

        pCurves->pKeyInfo[iCurrentKey].m_eOutTangentType = 
            kAnimCurve.outTangentType(index,&status);

        // Just to make sure we havn't used this slot already (debugging)
        NIASSERT(pCurves->pKeyInfo[iCurrentKey].m_fKeyTime == 0.0f);

        status = kAnimCurve.getTangent ( index, 
            pCurves->pKeyInfo[iCurrentKey].m_fInTanX,
            pCurves->pKeyInfo[iCurrentKey].m_fInTanY,
            bInTangent );
 
        NIASSERT(status==MS::kSuccess);

        if ((pCurves->pKeyInfo[iCurrentKey].m_fInTanX == 0.0f) &&
            (pCurves->pKeyInfo[iCurrentKey].m_fInTanY == 0.0f))
        {
            DtExt_Err("Warning: Animation curve %s has an invalid tangent."
                " Making flat.", pCurves->msName.asChar());
            pCurves->pKeyInfo[iCurrentKey].m_fInTanX = 1.0f;
        }

        status = kAnimCurve.getTangent ( index,
            pCurves->pKeyInfo[iCurrentKey].m_fOutTanX,
            pCurves->pKeyInfo[iCurrentKey].m_fOutTanY,
            !bInTangent );

        if ((pCurves->pKeyInfo[iCurrentKey].m_fOutTanX == 0.0f) &&
            (pCurves->pKeyInfo[iCurrentKey].m_fOutTanY == 0.0f))
        {
            DtExt_Err("Warning: Animation curve %s has an invalid tangent."
                " Making flat.", pCurves->msName.asChar());
            pCurves->pKeyInfo[iCurrentKey].m_fOutTanX = 1.0f;
        }


        NIASSERT(status==MS::kSuccess);

        pCurves->pKeyInfo[iCurrentKey].m_fKeyTime = 
            (float) kAnimCurve.time( index, &status ).as(MTime::kSeconds);
        pCurves->pKeyInfo[iCurrentKey].m_dValue = kAnimCurve.value( index,
            &status);

        // If the out tangent is stepped then we need to add an extra key
        if(pCurves->pKeyInfo[iCurrentKey].m_eOutTangentType == 
            MFnAnimCurve::kTangentStep)
        {
            // Flatten the out tangent for the stepped key
            pCurves->pKeyInfo[iCurrentKey].m_fOutTanX = 1.0f;
            pCurves->pKeyInfo[iCurrentKey].m_fOutTanY = 0.0f;
            pCurves->pKeyInfo[iCurrentKey].m_eOutTangentType = 
                MFnAnimCurve::kTangentFlat;

            // Create a stepped key
            if(index < (uiNumMayaKeys - 1))
            {
                iCurrentKey++;

                pCurves->pKeyInfo[iCurrentKey].m_dValue = 
                    pCurves->pKeyInfo[iCurrentKey-1].m_dValue;

                pCurves->pKeyInfo[iCurrentKey].m_fInTanX = 1.0f;
                pCurves->pKeyInfo[iCurrentKey].m_fInTanY = 0.0f;

                pCurves->pKeyInfo[iCurrentKey].m_fOutTanX = 1.0f;
                pCurves->pKeyInfo[iCurrentKey].m_fOutTanY = 0.0f;

                // Create this new key immediately before the next key.  
                pCurves->pKeyInfo[iCurrentKey].m_fKeyTime = 
                    (float) kAnimCurve.time( index+1, &status ).
                    as(MTime::kSeconds) - 0.001f; 

                pCurves->pKeyInfo[iCurrentKey].m_eInTangentType  = 
                    MFnAnimCurve::kTangentFlat;

                pCurves->pKeyInfo[iCurrentKey].m_eOutTangentType = 
                    MFnAnimCurve::kTangentFlat;
            }
        }
    }

    if (iTotalKeys > 0)
    {
        // Initialize the First Key In Tangent to Flat and the Last Key out 
        // tangent to flat
        pCurves->pKeyInfo[0].m_fInTanX = 1.0f;
        pCurves->pKeyInfo[0].m_fInTanY = 0.0f;
        pCurves->pKeyInfo[0].m_eInTangentType = MFnAnimCurve::kTangentFlat;

        pCurves->pKeyInfo[iTotalKeys - 1].m_fOutTanX = 1.0f;
        pCurves->pKeyInfo[iTotalKeys - 1].m_fOutTanY = 0.0f;
        pCurves->pKeyInfo[iTotalKeys - 1].m_eOutTangentType = 
            MFnAnimCurve::kTangentFlat;
    }

}
//---------------------------------------------------------------------------
void SetCombinedKeyInfo( SCombinedKeyInfo *pCKeyInfo, SKeyInfo *pKeyInfo)
{
    pCKeyInfo->m_fKeyTime = pKeyInfo->m_fKeyTime;
}   
//---------------------------------------------------------------------------
void kDtAnimInfo::CombineKeyFrames(int iComponentID,int iGroup)
{
    kAnimCurveInfo *pCurves = m_ppModelAnimCurve[iComponentID]; 
    kCombinedAnimCurveInfo *pCCurves = 
        m_ppCombinedModelAnimCurve[iComponentID];

    pCCurves = &pCCurves[iGroup];
    NIASSERT(pCCurves != NULL);

    int iX = 0, iY = 0, iZ = 0;
    switch(iGroup)
    {
        case 0: // Translation
            iX = 0; iY = 1; iZ = 2;
            pCCurves->eCurveType = MFnAnimCurve::kAnimCurveTL;
            break;
        case 1:
            iX = 3; iY = 4; iZ = 5;
            pCCurves->eCurveType = MFnAnimCurve::kAnimCurveTL;
            break;
        case 2:
            iX = 6; iY = 7; iZ = 8;
            pCCurves->eCurveType = MFnAnimCurve::kAnimCurveTL;
            break;
        default:
            NIASSERT(0);
            break;
    }
    // We will take each axis keyframes and combine them into one...
    
    int iXKeys, iYKeys, iZKeys;
    if (pCurves)
    {
        iXKeys = pCurves[iX].m_NumKeyFrames;
        iYKeys = pCurves[iY].m_NumKeyFrames;
        iZKeys = pCurves[iZ].m_NumKeyFrames;
    }
    else return;

    // First, we'll assume the worst an allocate mem as if each 3 axis
    // had a different set of key frames... which is possible.
    int iWorst = iXKeys + iYKeys + iZKeys;

    // Should be all the same... if in use...
    // We'll just make it of type CurveTL
    NIASSERT(pCCurves->pKeyInfo == NULL);
    pCCurves->pKeyInfo = NiNew SCombinedKeyInfo[iWorst];

    // Simple tag to let us know which xyz values are valid
    // x is bit 0.  y is bit 1.  z is bit 2.
    int *pTags = NiAlloc(int, iWorst);
    memset(pTags, 0, sizeof(int) * iWorst);

    // Assuming 1) that Maya keeps the entries sorted by time
    //          2) no two entries have the same time for a single axis
    pCCurves->m_NumKeyFrames = 0;
    MergeTransAxis(pCCurves, pCurves, pTags, &pCCurves->m_NumKeyFrames, iX, 
        iComponentID); 

    MergeTransAxis(pCCurves, pCurves, pTags, &pCCurves->m_NumKeyFrames, iY, 
        iComponentID); 

    MergeTransAxis(pCCurves, pCurves, pTags, &pCCurves->m_NumKeyFrames, iZ, 
        iComponentID); 


    // Zero out the In Tangent for the First Key and the Out Tangent for 
    // the Last Key
    if(pCCurves->m_NumKeyFrames > 0)
    {
        pCCurves->pKeyInfo[0].m_fInTanSlope[0] = 0.0f;
        pCCurves->pKeyInfo[0].m_fInTanSlope[1] = 0.0f;
        pCCurves->pKeyInfo[0].m_fInTanSlope[2] = 0.0f;
pCCurves->pKeyInfo[pCCurves->m_NumKeyFrames - 1].m_fOutTanSlope[0] = 0.0f;
pCCurves->pKeyInfo[pCCurves->m_NumKeyFrames - 1].m_fOutTanSlope[1] = 0.0f;
pCCurves->pKeyInfo[pCCurves->m_NumKeyFrames - 1].m_fOutTanSlope[2] = 0.0f;
    }

    if (pCurves[iX].bInUse)
        pCCurves->bIsWeighted = pCurves[iX].bIsWeighted;
    else if (pCurves[iY].bInUse)
        pCCurves->bIsWeighted = pCurves[iY].bIsWeighted;
    else if (pCurves[iZ].bInUse)
        pCCurves->bIsWeighted = pCurves[iZ].bIsWeighted;
    else
    {
        //NIASSERT(0); // want to observe this if it happens during testing.
        NiFree(pTags);
        return;
    }

    // process each axis values
    for (int iAxis = 0; iAxis<3; iAxis++)
    {
        MFnAnimCurve kAC;
        

        if ((DtExt_GetAnimCurve(iComponentID, iX+iAxis, kAC) == true) ||
            (GetBakedAnimationCurve(iComponentID, iX+iAxis, kAC) == true))
        {
            int iMask = 1<<iAxis;

            EvalTangentsAndPos( iComponentID, iGroup, iAxis, iMask, pTags, 
                kAC);

            /*if (pAC)
                NiExternalDelete pAC;*/
            //pAC = NULL;
        }
    }

    // Set the In Tangents and Out Tangents to the ZERO
//    pCCurves;
    NiFree(pTags);
    return;
}
//---------------------------------------------------------------------------
void kDtAnimInfo::EvalTangentsAndPos( int iComponentID, int iGroup, 
    int iAxis, int iMask, int *pTags, const MFnAnimCurve& kAC)
{
    kAnimCurveInfo *pCurves = m_ppModelAnimCurve[iComponentID]; 
    kCombinedAnimCurveInfo *pCCurves = 
        m_ppCombinedModelAnimCurve[iComponentID];

    pCCurves                            = &pCCurves[iGroup];
    NIASSERT(pCCurves != NULL);


    if(pCCurves->m_NumKeyFrames == 1)
    {
        pCCurves->pKeyInfo->m_fInTanSlope[0] = 0.0f;
        pCCurves->pKeyInfo->m_fInTanSlope[1] = 0.0f;
        pCCurves->pKeyInfo->m_fInTanSlope[2] = 0.0f;

        pCCurves->pKeyInfo->m_fOutTanSlope[0] = 0.0f;
        pCCurves->pKeyInfo->m_fOutTanSlope[1] = 0.0f;
        pCCurves->pKeyInfo->m_fOutTanSlope[2] = 0.0f;

        pCCurves->pKeyInfo->m_fInWeight[0] = 0.0f;
        pCCurves->pKeyInfo->m_fInWeight[1] = 0.0f;
        pCCurves->pKeyInfo->m_fInWeight[2] = 0.0f;

        pCCurves->pKeyInfo->m_fOutWeight[0] = 0.0f;
        pCCurves->pKeyInfo->m_fOutWeight[1] = 0.0f;
        pCCurves->pKeyInfo->m_fOutWeight[2] = 0.0f;
    }

    int iLastGood = -1;
    for (unsigned int i=0; i<pCCurves->m_NumKeyFrames;i++)
    {
        if (pTags[i] & iMask)
        {
            iLastGood = i;
            continue; // because entry is good.
        }

        // This entry needs an evaluation for this time...
        double dValue;
        double dTimeAsDouble = (double)pCCurves->pKeyInfo[i].m_fKeyTime;
        const MTime mTime(dTimeAsDouble);



        NIASSERT( pCCurves->eCurveType == MFnAnimCurve::kAnimCurveTA ||
                pCCurves->eCurveType == MFnAnimCurve::kAnimCurveTL ||
                pCCurves->eCurveType == MFnAnimCurve::kAnimCurveTU);

        // Evaluate Curve at Time mTime and interpolate the position value...
        MStatus status;
        dValue = kAC.evaluate(mTime, &status);
        pCCurves->pKeyInfo[i].m_dValue[iAxis] = dValue;

        //
        // Now Evaluate Tangents in,out for all axis at time m_fKeyTime...
        //
        int iNextGood = 0;
        if (iLastGood != -1)
        {
            unsigned int oi = 
                pCCurves->pKeyInfo[iLastGood].m_iOrigIndex[iAxis]+1;

            if (oi >= pCurves[iAxis].m_NumKeyFrames)
                iNextGood = -1;
            else
                iNextGood = pCurves[iAxis].pKeyInfo[oi].m_iCombIndex;
        }

        // Now Evaluate the Curve and determine the Tangent values...
        if (iLastGood == -1)
        {
            // Special case, we'll just mirror the tangent of the first
            // good one we find.
        }
        else
        {
            // pCCurves->pKeyInfo[i].m_fInTanX
            if (iNextGood == -1)
            {
                // just mirror last good
            }
            else
            {
                // i: entry for which interpolation is needed.
                // iLastGood and iNextGood are the left,right endpnts
                //      from which to grab our end tangents.
                SKeyInfo *pLeft,*pRight;
                SCombinedKeyInfo *pCMid;

                int iLindex = 
                    pCCurves->pKeyInfo[iLastGood].m_iOrigIndex[iAxis];

                pLeft = &pCurves[iAxis].pKeyInfo[iLindex];

                int iNindex = 
                    pCCurves->pKeyInfo[iNextGood].m_iOrigIndex[iAxis];

                pRight = &pCurves[iAxis].pKeyInfo[iNindex];

                pCMid = &pCCurves->pKeyInfo[i];

                if (pCCurves->bIsWeighted)
                    InterpolateBezier(pLeft,pRight,pCMid,iAxis);
                else
                    InterpolateHermite(pLeft,pRight,pCMid,iAxis);
            }
        }
    }
}
//---------------------------------------------------------------------------
void kDtAnimInfo::MergeTransAxis(kCombinedAnimCurveInfo *pCCurves,
    kAnimCurveInfo *pCurves, int *pTags, unsigned int *pTotalCnt, int axis, 
    int iComponentID) 
{
    // Axis = 0=x,1=y,2=z
    // Default Values
    float fDefaultX;
    float fDefaultY;
    float fDefaultZ;
    gMDtObjectGetTranslation(iComponentID, &fDefaultX, &fDefaultY, &fDefaultZ);


    // Setup
    int iKeys = pCurves[axis].m_NumKeyFrames;

    SKeyInfo  *pKeyInfo;
    SCombinedKeyInfo *pCKeyInfo;
    int i;

    // Now merge Y values over...
    for (i=0; i<iKeys;i++)
    {
        pKeyInfo  = &pCurves[axis].pKeyInfo[i];

        // Need to see if this key is unique in time.
        // If it is, we need to insert it.
        // otherwise, we use the already existing key.
        
        // Brute force search
        unsigned int x;
        int InsertAt = *pTotalCnt;
        for (x=0; x<*pTotalCnt;x++)
        {
            pCKeyInfo = &pCCurves->pKeyInfo[x];

            if (pKeyInfo->m_fKeyTime < pCKeyInfo->m_fKeyTime)
            {
                // No chance now.  We have to insert this key
                // into the list.  Push array over.  push tags over 
                // as well.
                InsertAt = x;
                break;
            }

            if (pKeyInfo->m_fKeyTime == pCKeyInfo->m_fKeyTime)
            {
                // Have a match.  Hurray.
                // Questions.  Are the tangent types the same?
                //             Are the In and Out values the same?
                pCKeyInfo->m_dValue[axis] = pKeyInfo->m_dValue;
                pCKeyInfo->m_iOrigIndex[axis]   = i;
                pKeyInfo->m_iCombIndex          = x;

                // Set tangents...
                pCKeyInfo->m_fInTanSlope[axis]  = 
                    FindSlope(pKeyInfo->m_fInTanX, pKeyInfo->m_fInTanY);

                pCKeyInfo->m_fOutTanSlope[axis] = 
                    FindSlope(pKeyInfo->m_fOutTanX,pKeyInfo->m_fOutTanY);

                pTags[x] |= (1<<axis);
                InsertAt = -1;
                break;
            }
        }

        if (InsertAt >= 0)
        {
            // We have to insert this node.
            int y;
            for (y=*pTotalCnt;y>InsertAt;y--)
            {
                pCCurves->pKeyInfo[y] = pCCurves->pKeyInfo[y-1];

                // Notify orignal of change
                for (int a=0; a<3; a++)
                {
                    int orig = pCCurves->pKeyInfo[y].m_iOrigIndex[a];
                    if (orig >= 0)
                    {
                        kAnimCurveInfo *pC = 
                            m_ppModelAnimCurve[iComponentID];

                        pC = &pC[a];
                        pC->pKeyInfo[orig].m_iCombIndex = y;
                    }
                }

                pTags[y] = pTags[y-1];
            }

            pCKeyInfo = &pCCurves->pKeyInfo[InsertAt];
            SetCombinedKeyInfo( pCKeyInfo, pKeyInfo);
            
            // Set the Defaults
            pCKeyInfo->m_dValue[0] = (double)fDefaultX;
            pCKeyInfo->m_dValue[1] = (double)fDefaultY;
            pCKeyInfo->m_dValue[2] = (double)fDefaultZ;


            pCKeyInfo->m_dValue[axis]       = pKeyInfo->m_dValue;
            pCKeyInfo->m_iOrigIndex[axis]   = i;
            pKeyInfo->m_iCombIndex          = InsertAt;

            // Set tangents... and check for infinity slope!!!
            pCKeyInfo->m_fInTanSlope[axis]  = 
                FindSlope(pKeyInfo->m_fInTanX, pKeyInfo->m_fInTanY);

            pCKeyInfo->m_fOutTanSlope[axis] = 
                FindSlope(pKeyInfo->m_fOutTanX,pKeyInfo->m_fOutTanY);

            pTags[InsertAt] = (1<<axis);
            (*pTotalCnt)++;
        }
    }
}
//---------------------------------------------------------------------------
void kDtAnimInfo::InterpolateBezier(SKeyInfo *pLeft,SKeyInfo *pRight,
    SCombinedKeyInfo *pMiddle, int iAxis)
{
    NI_UNUSED_ARG(iAxis);
    // First, Caclulate delta and normalize the time
    float fLOutTanX = pLeft->m_fOutTanX;
    float fLOutTanY = pLeft->m_fOutTanY;
    float fRInTanX  = pRight->m_fInTanX;
    float fRInTanY  = pRight->m_fInTanY;

    float x[4],y[4];
    x[0] = pLeft->m_fKeyTime;
    x[1] = x[0] + fLOutTanX; 
    
    y[0] = (float)pLeft->m_dValue;
    y[1] = y[0] + fLOutTanY; 

    x[3] = pRight->m_fKeyTime;
    x[2] = x[3] - fRInTanX;
    
    y[3] = (float)pRight->m_dValue;
    y[2] = y[3] - fRInTanY;

    // Now do bezier...
    BezierInterpolate(x,y,pMiddle->m_fKeyTime);
}
//---------------------------------------------------------------------------
float FindSlope(float fX, float fY)
{
    if (fX != 0.0)
        return (fY/fX);
    else
        return kMaxTan;
}
//---------------------------------------------------------------------------
void kDtAnimInfo::InterpolateHermite(SKeyInfo *pLeft,SKeyInfo *pRight,
    SCombinedKeyInfo *pMiddle, int iAxis)
{
    float fOldDeltaX = (float)pRight->m_dValue - (float)pLeft->m_dValue;
    float fOldDeltaT = pRight->m_fKeyTime - pLeft->m_fKeyTime;
    float fNewDeltaTA = pMiddle->m_fKeyTime - pLeft->m_fKeyTime;

    // calculate normalized time
    float t = fNewDeltaTA / fOldDeltaT;
    
    float fLOutSlope = FindSlope(pLeft->m_fOutTanX, pLeft->m_fOutTanY);
    float fRInSlope = FindSlope(pRight->m_fInTanX, pRight->m_fInTanY);

    // Premultiply the Slopes because the algorithm only works with 
    // Normalized tangents
    fLOutSlope *= fOldDeltaT;
    fRInSlope *= fOldDeltaT;

    float a = -2.0f * fOldDeltaX + fLOutSlope + fRInSlope;
    float b = 3.0f * fOldDeltaX - 2.0f * fLOutSlope - fRInSlope;
    float c = fLOutSlope;
    float d = (float)pLeft->m_dValue;

    // Calculate the new Result
    pMiddle->m_dValue[iAxis] = ((a * t + b) * t + c) * t + d;

    // calculate tangents
    pMiddle->m_fInTanSlope[iAxis] = (((3.0f * a * t + 2.0f * b) * t + 
        fLOutSlope) / fOldDeltaT);

    pMiddle->m_fOutTanSlope[iAxis] = pMiddle->m_fInTanSlope[iAxis];
}
//---------------------------------------------------------------------------
void kDtAnimInfo::RemoveSingleKeyEulerRotations(int iComponentID)
{
    // Check if there are no curves for this component
    if(m_ppModelAnimCurve[iComponentID] == NULL)
        return;

    kAnimCurveInfo* pCurves = m_ppModelAnimCurve[iComponentID];

    // Check for ANIMATIONS with 1 key and turn them into two key animations

    for(int iLoop = 0; iLoop < 11; iLoop++)
    {
        if(pCurves[iLoop].m_NumKeyFrames == 1) 
        {
            SKeyInfo* pNewKeyInfo = NiNew SKeyInfo[2];

            // Add an extra Frame
            pNewKeyInfo[0].m_iCombIndex = pNewKeyInfo[1].m_iCombIndex = 
                pCurves[iLoop].pKeyInfo->m_iCombIndex;

            pNewKeyInfo[0].m_eInTangentType = 
                pNewKeyInfo[1].m_eInTangentType = 
                    MFnAnimCurve::kTangentFlat; 
                        // pCurves[iLoop].pKeyInfo->m_eInTangentType;

            pNewKeyInfo[0].m_eOutTangentType = 
                pNewKeyInfo[1].m_eOutTangentType = 
                MFnAnimCurve::kTangentFlat; 
                        //pCurves[iLoop].pKeyInfo->m_eOutTangentType;

            pNewKeyInfo[0].m_fInTanX = pNewKeyInfo[1].m_fInTanX = 1.0f;
            pNewKeyInfo[0].m_fInTanY = pNewKeyInfo[1].m_fInTanY = 0.0f;

            pNewKeyInfo[0].m_fOutTanX = pNewKeyInfo[1].m_fOutTanX = 1.0f;
            pNewKeyInfo[0].m_fOutTanY = pNewKeyInfo[1].m_fOutTanY = 0.0f;

            pNewKeyInfo[0].m_fInWeight = pNewKeyInfo[1].m_fInWeight = 0.0f;
            pNewKeyInfo[0].m_fOutWeight = pNewKeyInfo[1].m_fOutWeight = 0.0f;

            pNewKeyInfo[0].m_dValue = 
                pNewKeyInfo[1].m_dValue = 
                    pCurves[iLoop].pKeyInfo->m_dValue;

            pNewKeyInfo[0].m_fKeyTime = 
                pNewKeyInfo[1].m_fKeyTime = 
                    pCurves[iLoop].pKeyInfo->m_fKeyTime;

            // Add One Frame
            pNewKeyInfo[1].m_fKeyTime += 
                1.0f / (float)gAnimControlGetFramesPerSecond();

            // Replace the single key with the new dupicated keys
            NiExternalDelete[] pCurves[iLoop].pKeyInfo;
            pCurves[iLoop].pKeyInfo = pNewKeyInfo;

            pCurves[iLoop].m_NumKeyFrames = 2;
        }
    }
}
//---------------------------------------------------------------------------
bool kDtAnimInfo::DisconnectPhysics(MDGModifier& kDGModifier)
{
    // SJC 04/19/06 COde to turn of Ageia PhysX solving during playback.
    MStatus kStatus;
    MSelectionList kSelection;

    kStatus = MGlobal::getSelectionListByName("nxRigidSolver1", kSelection);
    MObject kSolverObj;
    if (kStatus &&
        kSelection.length() >= 1 &&
        kSelection.getDependNode(0, kSolverObj))
    {
        if (!MFnPlugin::isNodeRegistered("nxRigidSolver")) 
        {
            DtExt_Err("Error:: nxRigidSolver not registered.");
            return false;
        }

        kSelection.clear();
        kStatus = MGlobal::getSelectionListByName("time1", kSelection);
        if (kStatus != MS::kSuccess || kSelection.length() != 1)
        {
            DtExt_Err("Error:: Could not find time1 to connect to physics\n");
            return false;
        }
        
        MObject kTimeObj;
        kStatus = kSelection.getDependNode(0, kTimeObj);
        if (kStatus != MS::kSuccess)
        {
            DtExt_Err("Error:: time1 node not in selection for physics\n");
            return false;
        }
        MFnDependencyNode kTimeNode(kTimeObj);
        
        MPlug kOutTimePlug = kTimeNode.findPlug("outTime", &kStatus);
        if (kStatus != MS::kSuccess)
        {
            DtExt_Err("Error:: Could not get outTime from time1\n");
            return false;
        }

        // Connect time to solver
        MFnDependencyNode kSolverDepFn(kSolverObj);
        MPlug kSolverTimePlug = kSolverDepFn.findPlug("currentTime", kStatus);
        if (kStatus != MS::kSuccess)
        {
            kSolverTimePlug = kSolverDepFn.findPlug("ct", kStatus);
            if (kStatus != MS::kSuccess)
            {
                DtExt_Err("Error:: Could not fine solver time in plug\n");
                return false;
            }
        }
        kStatus = kDGModifier.disconnect(kOutTimePlug, kSolverTimePlug);
        if (kStatus != MS::kSuccess)
        {
            DtExt_Err("Error:: Could not disconnect time (disconnect)\n");
            return false;
        }
        kStatus = kDGModifier.doIt();
        if (kStatus != MS::kSuccess)
        {
            DtExt_Err("Error:: Could not disconnect time (doIt)\n");
            return false;
        }
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
void kDtAnimInfo::ReconnectPhysics(MDGModifier& kDGModifier)
{
    MStatus kStatus = kDGModifier.undoIt();
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not reconnect PhysX time\n");
    }   
}
//---------------------------------------------------------------------------
//
// C Wrappers
//
//---------------------------------------------------------------------------
kDtAnimInfo gAnimData;

void gAnimDataGetSlopeInOut( int iShapeID, int iCurveType, int iFrame, 
    float *pIn, float *pOut)
{
    kAnimCurveInfo *pCurves = gAnimData.m_ppModelAnimCurve[iShapeID]; 
    pCurves = &pCurves[iCurveType];

    float fInTanX  = pCurves->pKeyInfo[iFrame].m_fInTanX;
    float fInTanY  = pCurves->pKeyInfo[iFrame].m_fInTanY;
    float fOutTanX = pCurves->pKeyInfo[iFrame].m_fOutTanX;
    float fOutTanY = pCurves->pKeyInfo[iFrame].m_fOutTanY;

    float tanx,m1,m2;
    
    tanx = fInTanX;
    m1 = m2 = kMaxTan;

    if (tanx != 0.0)
        m1 = fInTanY / tanx;

    tanx = fOutTanX;
    if (tanx != 0.0)
        m2 = fOutTanY / tanx;

    *pIn  = m1;
    *pOut = m2;
}
//---------------------------------------------------------------------------
void gAnimDataGetCSlopeIn( int iShapeID, int iGroupID, int iFrame, float *pIn)
{
    kCombinedAnimCurveInfo *pCCurves = 
        gAnimData.m_ppCombinedModelAnimCurve[iShapeID];

    pCCurves = &pCCurves[iGroupID]; // 0=Translations,1=Rotations,2=Scale
    pIn[0] = pCCurves->pKeyInfo[iFrame].m_fInTanSlope[0];
    pIn[1] = pCCurves->pKeyInfo[iFrame].m_fInTanSlope[1];
    pIn[2] = pCCurves->pKeyInfo[iFrame].m_fInTanSlope[2];
}
//---------------------------------------------------------------------------
void gAnimDataGetCSlopeOut( int iShapeID, int iGroupID, int iFrame, 
    float *pOut)
{
    kCombinedAnimCurveInfo *pCCurves = 
        gAnimData.m_ppCombinedModelAnimCurve[iShapeID];

    pCCurves = &pCCurves[iGroupID]; // 0=Translations,1=Rotations,2=Scale
    pOut[0] = pCCurves->pKeyInfo[iFrame].m_fOutTanSlope[0];
    pOut[1] = pCCurves->pKeyInfo[iFrame].m_fOutTanSlope[1];
    pOut[2] = pCCurves->pKeyInfo[iFrame].m_fOutTanSlope[2];
}
//---------------------------------------------------------------------------
int gAnimDataGetCKeyFrames(int iShapeID, int iGroupID)
{
    // This returns the Combined Translation KeyFrame number
    kCombinedAnimCurveInfo *pCCurves = 
        gAnimData.m_ppCombinedModelAnimCurve[iShapeID];

    pCCurves = &pCCurves[iGroupID]; // 0=Translations,1=Rotations,2=Scale
    return pCCurves->m_NumKeyFrames;
}
//---------------------------------------------------------------------------
float gAnimDataGetCKeyFrameTime(int iShapeID,int iGroupID, int iFrame)
{
    kCombinedAnimCurveInfo *pCCurves = 
        gAnimData.m_ppCombinedModelAnimCurve[iShapeID];

    pCCurves = &pCCurves[iGroupID]; // 0=Translations,1=Rotations,2=Scale
    return pCCurves->pKeyInfo[iFrame].m_fKeyTime;
}
//---------------------------------------------------------------------------
void gAnimDataGetCKeyFrameValue( int iShapeID, int iGroupID, int iFrame, 
    float *fvals)
{
    kCombinedAnimCurveInfo *pCCurves = 
        gAnimData.m_ppCombinedModelAnimCurve[iShapeID];

    pCCurves = &pCCurves[iGroupID]; // 0=Translations,1=Rotations,2=Scale

    fvals[0] = (float)pCCurves->pKeyInfo[iFrame].m_dValue[0];
    fvals[1] = (float)pCCurves->pKeyInfo[iFrame].m_dValue[1];
    fvals[2] = (float)pCCurves->pKeyInfo[iFrame].m_dValue[2];
}
//---------------------------------------------------------------------------
void gAnimDataReset(void)
{
    gAnimData.Reset();
}
//---------------------------------------------------------------------------
int gAnimDataAnimNew(void)
{
    // Return value:
    //  0: Success
    //  1: Cancellation
    //  2: Error

    return gAnimData.AnimNew();

}
//---------------------------------------------------------------------------
int gAnimDataGetNumKeyFrames(int iComponentID, int iCurveType )
{
    kAnimCurveInfo *pCurves = gAnimData.m_ppModelAnimCurve[iComponentID]; 
    pCurves = &pCurves[iCurveType];
    return pCurves->m_NumKeyFrames;
}
//---------------------------------------------------------------------------
float gAnimDataGetKeyFrameTime(int iComponentID, int iCurveType, int iFrame)
{
    kAnimCurveInfo *pCurves = gAnimData.m_ppModelAnimCurve[iComponentID]; 
    pCurves = &pCurves[iCurveType];
    if (pCurves->pKeyInfo == NULL)
        return -1.0f;
    return pCurves->pKeyInfo[iFrame].m_fKeyTime;
}
//---------------------------------------------------------------------------
double gAnimDataGetKeyFrameValue(int iComponentID, int iCurveType, int iFrame)
{
    kAnimCurveInfo *pCurves = gAnimData.m_ppModelAnimCurve[iComponentID]; 
    pCurves = &pCurves[iCurveType];
    return pCurves->pKeyInfo[iFrame].m_dValue;
}
//---------------------------------------------------------------------------
bool gAnimDataIsObjectAnimated(int iComponentID, bool *bPos, bool *bRot,
                               bool *bScale, bool *bVisible)
{
    for (int i=0; i<3; i++)
    {
        bPos[i] = false;
        bRot[i] = false;
        bScale[i] = false;
    }
    *bVisible = false;


    // The shape is animated if we have any animation curves.
    if (gAnimData.m_ppModelAnimCurve)
    {
        if (gAnimData.m_ppModelAnimCurve[iComponentID] != NULL)
        {
            kAnimCurveInfo *pCurves = 
                gAnimData.m_ppModelAnimCurve[iComponentID];

            if (pCurves[0].m_NumKeyFrames > 0)
                bPos[0] = true;

            if (pCurves[1].m_NumKeyFrames > 0)
                bPos[1] = true;
        
            if (pCurves[2].m_NumKeyFrames > 0)
                bPos[2] = true;

            if (pCurves[3].m_NumKeyFrames > 0)
                bRot[0] = true;

            if (pCurves[4].m_NumKeyFrames > 0)
                bRot[1] = true;

            if (pCurves[5].m_NumKeyFrames > 0)
                bRot[2] = true;

            if (pCurves[6].m_NumKeyFrames > 0)
                bScale[0] = true;

            if (pCurves[7].m_NumKeyFrames > 0)
                bScale[1] = true;

            if (pCurves[8].m_NumKeyFrames > 0)
                bScale[2] = true;

            if (pCurves[9].m_NumKeyFrames > 0)
                *bVisible = true;
        }
    }

    return (bPos[0] || bPos[1] || bPos[2] || bRot[0] || bRot[1] || bRot[2] ||
        bScale[0] || bScale[1] || bScale[2] || *bVisible);
}
//---------------------------------------------------------------------------
double gAnimControlGetPlayBackSpeed(void)
{
    double dPlaybackSpeed = MAnimControl::playbackSpeed();

    // Check for Invalid playback speed of "Play every frame"
    if(dPlaybackSpeed <= 0.0001)
    {
         dPlaybackSpeed = 1.0;   
    }

    return dPlaybackSpeed;
}
//---------------------------------------------------------------------------
double gAnimControlGetFramesPerSecond(void)
{

    MStatus s;
    MString sValue;
    s = MGlobal::executeCommand(MString("currentUnit -q -t;"), sValue);

    NIASSERT(s == MS::kSuccess);

    if( strcmp(sValue.asChar(), "game") == 0)
    {
        return 15.0;
    }
    else if( strcmp(sValue.asChar(), "film") == 0)
    {
        return 24.0;
    }
    else if( strcmp(sValue.asChar(), "pal") == 0)
    {
        return 25.0;
    }
    else if( strcmp(sValue.asChar(), "ntsc") == 0)
    {
        return 30.0;
    }
    else if( strcmp(sValue.asChar(), "show") == 0)
    {
        return 48.0;
    }
    else if( strcmp(sValue.asChar(), "palf") == 0)
    {
        return 50.0;
    }
    else if( strcmp(sValue.asChar(), "ntscf") == 0)
    {
        return 60.0;
    }
    else 
    {
        char szBuffer[512];
        NiSprintf(szBuffer, 512, "Error Reading current working unit for "
            "time: %s is not supported.\n", sValue.asChar());

        DtExt_Err(szBuffer);
    }

    return 0.0; 
}
//---------------------------------------------------------------------------
double gMDtGetLinearUnitMultiplier(void)
{
    MStatus s;
    MString sValue;
    s = MGlobal::executeCommand(MString("currentUnit -q -l -f;"), sValue);

    NIASSERT(s == MS::kSuccess);


    if( strcmp(sValue.asChar(), "millimeter") == 0)
    {
        return 10.0;
    }
    else if( strcmp(sValue.asChar(), "centimeter") == 0)
    {
        return 1.0;
    }
    else if( strcmp(sValue.asChar(), "meter") == 0)
    {
        return 0.01;
    }
    else if( strcmp(sValue.asChar(), "inch") == 0)
    {
        return (1.0 / 2.54);
    }
    else if( strcmp(sValue.asChar(), "foot") == 0)
    {
        return (1.0 / 30.48);
    }
    else if( strcmp(sValue.asChar(), "yard") == 0)
    {
        return (1.0 / 91.44);
    }
    else 
    {
        char szBuffer[512];
        NiSprintf(szBuffer, 512, 
            "Error Reading current working unit for linear "
            "distance: %s is not supported.\n", sValue.asChar());

        DtExt_Err(szBuffer);
    }

    return 0.0; 
}
//---------------------------------------------------------------------------
double gAnimControlGetMinTime(void)
{
    MTime mtime = MAnimControl::minTime();

    return mtime.as( mtime.unit());
}
//---------------------------------------------------------------------------
double gAnimControlGetMaxTime(void)
{
    MTime mtime = MAnimControl::maxTime();
    return mtime.as( mtime.unit() );
}
//---------------------------------------------------------------------------
int gAnimControlGetPlayBackMode(void)
{
    //
    // Various Playback Modes include:
    //
    //      o kPlaybackOnce 
    //      o kPlaybackLoop 
    //      o kPlaybackOscillate  
    //

    return ((int)(MAnimControl::playbackMode()));
} 
//---------------------------------------------------------------------------
