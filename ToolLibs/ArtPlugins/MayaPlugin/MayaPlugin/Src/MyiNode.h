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

#ifndef MYINODE_H
#define MYINODE_H

#include "MyiObject.h"
#include "NiNode.h"
#include "MyiTextureEffect.h"


#include "MDt.h"
#include "MDtExt.h"
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
#include <NiTList.h>
#include <NiAVObject.h>
#include <NiLight.h>

NiSmartPointer(NiNode);

void SetAnimations(NiTimeController* pkCtrl);

class MyiNode : public MyiObject
{
public:
    MyiNode(int iShapeNum, NiTList<NiAVObject*>& pkSelectedObjects, 
        NiLightPtr* sppLights, 
        MyiTextureEffectManager* pkTextureEffectManager,
        MStringArray& akParentLights);

    ~MyiNode(void);
    inline NiNode *GetNode();

    static void AddZBufferProperties(NiAVObject* pkObject, 
        MFnDagNode& dgNode);

    static bool Equivalent(float fValue1, float fValue2, float fValue3);
    static bool Equivalent(float* pfValue);
    static bool EquivalentToZero(float fValue1, float fValue2, float fValue3);
    static bool EquivalentToZero(float* pfValue1);


protected:
    void SetTransforms(int iComponentNum, NiNode* pkNode);
    int  SetRotations(int iComponentNum, NiNode* pkNode, bool bFinal);
    int  SetTranslations(int iComponentNum, NiNode* pkNode, bool bFinal);
    int  SetPivots(NiNode *pkNode,float fX,float fY,float fZ);

    void SetInverseTransforms(int iComponentNum, NiNode* pkNode);
    int  SetInverseRotations(int iComponentNum, NiNode* pkNode);
    int  SetInverseTranslations(int iComponentNum,NiNode* pkNode);

    void GetPivot(int iComponentNum);
    void GetScalePivot(int iComponentNum);
    void GetOrientations(int iComponentNum);
    void GetRotateAxis(int iComponentNum);

    bool UsesQuaternionKeys(int iComponentNum);

    bool MakeChildren(int iComponentNum, NiNode* pkNode, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    inline bool AnyOrientationsOrPivots(void);
    inline bool AnyPivots(void);
    inline bool AnyScalePivots(void);
    inline bool AnyOrientations(void);
    inline bool AnyRotateAxis(void);

    bool CreateAnimations(int iComponent, NiNodePtr spNode);

    void AttachUserDefinedAttributeDataToNode(int iComponentNum, 
        EType eType);

    void HandleComponent(int iComponentNum, EType eType,  
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandleShape(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandleLight(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandleCamera(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandleJointBone(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandleParticleSystemEmitter(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandleParticleSystemParticles(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandleLevelOfDetail(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandlePhysXShape(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandlePhysXNode(int iComponentNum, EType eType,
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);

    bool HandleUnknown(int iComponentNum, 
        NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights);


    void HandleOrientations();
    void HandlePivots();
    void HandleScalePivots(int iComponentNum);

    NiNodePtr CreateNiNode(int iComponentNum, EType eType); 

    bool CreateBillboardNode(NiNodePtr &spNewNode, MFnDagNode& dgNode);
    bool CreateSwitchNode(NiNodePtr &spNewNode, MFnDagNode& dgNode);

    void AddSwitchNodeProperties(NiNode* pkNewNode, int iComponentNum);
    void AddPhysicsProperties(NiNode* pkNewNode, MFnDagNode& dgNode);
    void AddSgoProperties(NiNode* pkNewNode, MFnDagNode& dgNode);
    void AddWireFrameProperties(NiNode* pkNewNode, MFnDagNode& dgNode);
    void AddShaderProperties(NiNode* pkNewNode, MFnDagNode& dgNode);
    void AddStencilProperties(NiNode* pkNewNode, MFnDagNode& dgNode);

    void SetABV(int iComponentNum, EType eType);
    void AddABVNamingConvention(char* pcName, unsigned int uiNameLen, 
        int iComponentNum, EType eType);

    void AddNoStripifyAttributes(NiNode* pkNewNode, MFnDagNode& dgNode);

    void ApplyLightInclusionList(int iComponent, NiLightPtr* sppLights);

    void ApplyTextureEffects(int iComponent, NiLightPtr* sppLights);

    void CheckForAnimations(int iComponentNum);

    void HandleRotateAxis();
    NiNode* GetLastNode();

    void CreateMorphTagExtraData(int iComponentNum);
    void CreateTextKeyExtraData(int iComponentNum);

    void ModifyTranslationByScale(int iComponentNum, float* pfTranslate);
    bool CreateTransformsAndAnimation(int iComponentNum);

    void HandleSegmentScaleCompensate(MFnTransform& kTransform, 
        NiNode* pkNode);



    // Compacted Scene Graph I think :-)
    //
    //  Node = Rotation AND Translation + Rotate Pivot + 
    //          Rotate Translate AND No Pivot Scale
    //      Pivot = -Rotation Pivot + Scale Pivot + Scale Translate AND 
    //              Pivoted Scale 
    //          Scale Pivot = -Scale Pivot

    NiNodePtr   m_spNode;
    NiNodePtr   m_spJointOrientation;
    NiNodePtr   m_spPivot;
    NiNodePtr   m_spScalePivot;
    NiNodePtr   m_spRotateAxis;

    bool m_bAnyPosAnimations;
    bool m_bAnyRotAnimations;
    bool m_bAnyScaleAnimations;
    bool m_bVisibleAnimation;

    bool m_bAnimated;
    bool m_bIsQuaternion;

    float m_afPivot[3];
    float m_afPivotTranslation[3];
    float m_afOrientation[3];
    float m_afScalePivot[3];
    float m_afScalePivotTranslation[3];
    float m_afRotateAxis[3];

    NiTransformController* m_pkTransformController; 
    MyiTextureEffectManager* m_pkTextureEffectManager;

    MStringArray m_akParentLights;
    MStringArray m_akAffectedLights;

    bool m_bABVNode;
};

#include "MyiNode.inl"

#endif 
