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

#ifndef NIMAXANIMATIONCONVERTER_H
#define NIMAXANIMATIONCONVERTER_H

#include "NiMAX.h"
#include "NiMAXConverter.h"
class Control;
//---------------------------------------------------------------------------

class NiMAXAnimationConverter
{
public:

    NiMAXAnimationConverter(TimeValue animStart, TimeValue animEnd);

    // convert all animation associated w/ "pMaxNode" and
    // apply to "pNode"
    int Convert(
        NiNode *pNode,
        INode *pMaxNode, 
        ScaleInfo *pParentScale,
        bool bIgnoreVisAnim);

    // convert a float animation
    NiInterpolator* ConvertFloatAnim(
        Control *pFc, 
        bool bAllowSingleKeyedAnimation = false);

    // convert a color animation
    NiInterpolator* ConvertPoint3Anim(Control *pCol);

    // Is "pMaxNode" path controlled?
    static Control* GetPathInterpolator(INode *pMaxNode);

    // Is "pMaxNode" animated by an IK controller?
    static bool IsIKControlled(INode *pMaxNode);

    int Finish();

    static void Init();
    static void Shutdown();

    static void SetConvertCSAsKeyframes(bool bConvertCSAsKeyframe);
    static bool GetConvertCSAsKeyframes();

    static void SetORT(
        Control *pCont,
        NiTimeController::CycleType& eType);
    static NiTimeController::CycleType SetORT(
        Control *pCont, 
        NiTimeController* pkController);

    static NiTimeController::CycleType GetORT(Control *pCont);

    void MakeSingleKey(
        Control *pCont, 
        TimeValue animStart, 
        unsigned int *pNumKeys,
        NiFloatKey **ppKeys,
        NiFloatKey::KeyType *pType);

    // float animations
    NiInterpolator* ConvertTCBFloat(IKeyControl *pIKeys);
    NiInterpolator* ConvertBezFloat(IKeyControl *pIKeys);
    NiInterpolator* ConvertLinFloat(IKeyControl *pIKeys);
    NiInterpolator* ConvertDumbFloat(Control *pFc);
    NiInterpolator* UnknownFloatAnim(Control *pFc);

    // scale animations
    NiInterpolator* ConvertTCBScale(
        Control *pPC,
        IKeyControl *pIKeys,
        ScaleInfo *pParentScale);
    NiInterpolator* ConvertBezScale(
        Control *pScale,
        IKeyControl *pIKeys, 
        ScaleInfo *pParentScale);
    NiInterpolator* ConvertLinScale(
        Control *pScale,
        IKeyControl *pIKeys, 
        ScaleInfo *pParentScale);
    NiInterpolator* ConvertDumbScale(
        Control* pControl,
        INode *pMaxNode,
        ScaleInfo *pParentScale);
    NiInterpolator* UnknownScaleAnim(
        Control *pSC,
        INode *pMaxNode,
        ScaleInfo *pParentScale);
    NiInterpolatorPtr ConvertScaleAnim(
        Control *pScale, 
        NiTimeController::CycleType& eCycleType,
        INode *pMaxNode,
        ScaleInfo *pParentScale);

    // position animations
    NiInterpolator* ConvertTCBPos(
        Control *pPC,
        IKeyControl *pIKeys, 
        ScaleInfo *pParentScale);
    NiInterpolator* ConvertLinPos(
        Control *pPC,
        IKeyControl *pIKeys, 
        ScaleInfo *pParentScale);
    NiInterpolator* ConvertBezPos(
        Control *pPC,
        IKeyControl *pIKeys, 
        ScaleInfo *pParentScale);

    NiInterpolator* ConvertDumbPos(
        Control *pPC,
        INode *pMaxNode,
        ScaleInfo *pParentScale);
    NiInterpolator* UnknownPosAnim(
        Control *pPC,
        INode *pMaxNode, 
        ScaleInfo *pParentScale);
    NiInterpolator* ConvertCompositePosAnim(
        Control* pParentC,
        Control* pXc,
        Control* pYc,
        Control* pZc,
        INode* pMaxNode,  
        ScaleInfo* pParentScale);
    NiInterpolatorPtr ConvertPositionAnim(
        Control *pPc, 
        NiTimeController::CycleType& eCycleType,
        INode *pMaxNode, 
        ScaleInfo *pParentScale,
        bool &bSampled);
    NiInterpolatorPtr ConvertPositionListAnim(
        IListControl *pPc, 
        NiTimeController::CycleType& eCycleType,
        INode *pMaxNode, 
        ScaleInfo *pParentScale);
    void ConvertBipSlaveAnim(
        Animatable *pControl,
        INode *pMaxNode,
        ScaleInfo *pParentScale,
        unsigned int &uiNumKeys, 
        NiLinPosKey **ppPosKeys,
        NiLinRotKey **ppRotKeys);

    void MergeHorizontalAndVerticalAnimations(
        NiLinPosKey* pPos0,
        unsigned int ui0,
        NiLinPosKey* pPos1,
        unsigned int ui1,
        NiLinPosKey*& pNewPos,
        unsigned int& uiNewPos);

    NiTransformInterpolator* CreateTransformInterp(
        NiInterpolator* pkTranslate,
        NiInterpolator* pkRotate,
        NiInterpolator* pkScale);

    static NiTextKeyExtraData* BuildNoteTrack(INode* pMaxNode);

    // path animations

    // a helper class to assist path controller conversion
    class PRSInfo : public NiMemObject
    {
    public:
        NiPoint3 t;
        Quat q;
        float s;
    };

    void XformVert(NiPoint3 *pPos, PRSInfo *pPRS);
    void XformVector(NiPoint3 *pVec, PRSInfo *pPRS);
    int GetXform(
        INode *pControlledMaxNode,
        INode *pShapeMaxNode,
        PRSInfo *pResult,
        TimeValue animStart,
        ScaleInfo *pParentScale);
    bool InvalidTans(NiPoint3 &in, NiPoint3 &out);
    void FixTans(Spline3D *pSpline, int iKnot, NiPoint3 *pIn, NiPoint3 *pOut);
    int ConvertSpline3D(
        Spline3D *pSpline,
        NiPosKey **ppPosKeys,
        unsigned int *pNumPosKeys,
        NiPosKey::KeyType *pPosKeyType,
        unsigned char& ucPosSize,
        PRSInfo *pPRS);
    int ConvertBezierShape(BezierShape *pBS,
        NiPosKey **ppPosKeys,
        unsigned int *pNumPosKeys,
        NiPosKey::KeyType *pPosKeyType,
        unsigned char& ucPosSize,
        PRSInfo *pPRS);
    int ConvertShape(ShapeObject *pShape,
        NiPosKey **ppPosKeys,
        unsigned int *pNumPosKeys,
        NiPosKey::KeyType *pPosKeyType,
        unsigned char& ucPosSize,
        PRSInfo *pPRS,
        TimeValue animStart);

    int ConvertPath(
        INode *pMaxNode,
        Control* pControl, 
        NiPathInterpolator *pPathInterpolator,
        NiTimeController::CycleType& eCycleType,
        ScaleInfo *pParentScale);

    // color animations
    NiInterpolator* ConvertTCBPoint3(Control *pCC, IKeyControl *pIKeys);
    NiInterpolator*  ConvertBezPoint3(Control *pCC, IKeyControl *pIKeys);
    NiInterpolator* ConvertDumbPoint3(Control *pCol);
    NiInterpolator* UnknownColAnim(Control *pCol);
                                        
    // rotation animations
    NiInterpolator* ConvertLinRot(IKeyControl *pIKeys);
    NiInterpolator* ConvertTCBRot(IKeyControl *pIKeys);
    NiInterpolator* ConvertBezRot(IKeyControl *pIKeys);
    NiInterpolator* ConvertEulerRot(
        Control *pRc,
        NiTimeController::CycleType& eCycleType);
    NiInterpolator* ConvertDumbRot(
        Control* pControl,
        INode *pMaxNode,
        ScaleInfo *pParentScale);
    NiInterpolator* UnknownRotAnim(
        Control *pRC,
        INode *pMaxNode, 
        ScaleInfo *pParentScale);
    NiInterpolatorPtr ConvertRotationAnim(
        Control *pRc, 
        NiTimeController::CycleType& eCyleType,
        INode *pMaxNode, 
        ScaleInfo *pParentScale,
        int uiMinNumKeys = 1);

    NiLookAtInterpolator* ConvertLookAt(
        Control* pControl,
        NiTimeController::CycleType& eCyleType);

    // visibility animations
    int ConvertVisAnim(Control *pVc, NiVisController *pVisController);

    static void CalculateGlobalTimeRange(
        TimeValue& animStart, 
        TimeValue& animEnd);

    void CalculateControlTimeRange(
        Control* pControl,
        TimeValue& animStart, 
        TimeValue& animEnd,
        unsigned int& uiNumFrames);
    static void CalculateAnimatableTimeRange(
        Animatable* pkAnim, 
        TimeValue& animStart,
        TimeValue& animEnd);
    static void CalculateNodeTimeRange(
        INode* pkAnim,
        TimeValue& animStart, 
        TimeValue& animEnd);

private:
    static bool CanConvertPosListControl(IListControl* pkControl);
    
    // class data
    TimeValue m_animStart, m_animEnd;
    unsigned int m_uiNumFrames;

    class LookAtInfo  : public NiMemObject
    {
    public:
        NiLookAtInterpolator *pControl;
        INode *pLookAt;
    };

    static NiTPointerList<LookAtInfo *> ms_lookAtList;
    
    class PosRotAnimInfo : public NiMemObject
    {
    public:
        NiTransformController* pTransformController;
        NiTransformInterpolator* pTransformInterp;
        INode *pMaxNode;
        ScaleInfo ParentScale;
        NiRotKey* pkRotKeys;
        NiPosKey* pkPosKeys;
    };
    
    static NiTPointerList<PosRotAnimInfo *> ms_PosRotAnimList;
    
    static bool ms_bConvertCSAsKeyframe;

    static void *ms_pUserData;
};

//---------------------------------------------------------------------------
#include "NiMAXAnimationConverter.inl"
//---------------------------------------------------------------------------

#endif
