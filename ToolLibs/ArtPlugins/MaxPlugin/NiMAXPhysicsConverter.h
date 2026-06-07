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


#ifndef NIMAXPHYSICSCONVERTER_H
#define NIMAXPHYSICSCONVERTER_H

#include "NiMAX.h"
#include "NiMAXConverter.h"


//---------------------------------------------------------------------------
class NiMAXPhysicsConverter
{
public:
    NiMAXPhysicsConverter();
    ~NiMAXPhysicsConverter() {};
    
    static void Init();
    static void Shutdown();

    static void PreChildrenProcess(INode* pMaxNode, NiNode* pkNiNode,
        Object* pObj, TimeValue m_animStart);
    static void PostChildrenProcess(INode* pMaxNode, NiNode* pkNiNode,
        Object* pObj, TimeValue m_animStart);
    static int ConvertGeom(Object* pObj, INode* pMaxNode, NiNode* pkNode,
        TimeValue fTime, ScaleInfo *pkParentScale);
    static bool IsPhysicsJoint(INode* pMaxNode, TimeValue fTime);
    
    static int Finish(TimeValue fTime);
   
protected:
    static bool IsPhysicsGroupHead(INode* pMaxNode);

    static int ConvertGroup(Object* pObj, INode* pMaxNode, NiNode* pkNode,
        TimeValue fTime);
    static int ConvertActor(INode* pMaxNode, NiNode* pkNode, int iPhysXType,
        bool& bHasProxy);
    static int ConvertCloth(INode* pMaxNode, NiNode* pkNode);
    static int ConvertGroupMember(Object* pObj, INode* pMaxNode,
        NiNode* pkNode, TimeValue fTime, ScaleInfo *pkParentScale);
    static int ConvertShape(Object* pObj, INode* pMaxNode, NiNode* pkNode,
        TimeValue fTime, ScaleInfo *pkParentScale);
    static int ConvertMaterial(INode* pMaxNode, NiNode* pkNode,
        TimeValue fTime);
    static int ConvertD6Joint(Object* pObj, INode* pMaxNode, TimeValue fTime);
    static int ConvertJoint(INode* pMaxNode, NiNode* pkNode, int iJointType);
    
    static int ProcessD6Joint(unsigned int uiID, INode* pkMaxNode,
        TimeValue fTime);
    static void ProcessScale(ScaleInfo *pkParentScale, bool& bFlipX,
        bool& bFlipY, bool& bFlipZ, bool& bUniform); 

    static void StringToPoint3(const char *pcStr, float* afPoint3);

    static void AppendIntExtraData(NiNode* pkNode, const char* pcTag,
        int iVal);
    static void AppendFloatExtraData(NiNode* pkNode,
        const char* pcTag, float fVal);
    static void AppendVectorExtraData(NiNode* pkNode,
        const char* pcTag, float* afAddVals);
    static void AppendStringExtraData(NiNode* pkNode,
        const char* pcTag, const char* pcVal);
   
    static void SetJointParent(INode* pMaxNode, NiNode* pkNiNode);

    static unsigned int GetProxyID(const char* pcName);
    static void SetProxyID(const char* pcName, unsigned int uiID);

    static unsigned int GetCompoundID(INode* pMaxNode);
    static void SetCompoundID(INode* pMaxNode, unsigned int uiID);

    static unsigned int ms_uiNextShapeID;
    
    static NiTStringMap<unsigned int>* ms_pkProxyMap;
    static NiTMap<INode*, unsigned int>* ms_pkCompoundMap;
    static NiTMap<unsigned int, INode*>* ms_pkJointMap;
    static NiTMap<unsigned int, INode*>* ms_pkD6JointMap;
};

#endif // #ifndef NIMAXPHYSICSCONVERTER_H
