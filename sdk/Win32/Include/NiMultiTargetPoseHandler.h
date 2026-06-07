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

#ifndef NIMULTITARGETPOSEHANDLER_H
#define NIMULTITARGETPOSEHANDLER_H

#include "NiAnimationLibType.h"
#include "NiObject.h"
#include "NiQuatTransform.h"
#include "NiInterpController.h"
#include "NiControllerSequence.h"

class NiPoseBuffer;
class NiSequenceData;


NiSmartPointer(NiMultiTargetPoseHandler);

class NIANIMATION_ENTRY NiMultiTargetPoseHandler : public NiObject
{
public:
    NiDeclareRTTI;

    virtual ~NiMultiTargetPoseHandler();

    // Get and set the accumulation root.
    const NiFixedString& GetAccumRootName() const;
    NiAVObject* GetAccumRoot() const;
    const NiQuatTransform& GetAccumulatedTransform() const;
    void SetAccumulatedTransform(const NiQuatTransform& kTransform);
    void ClearAccumulatedTransform();

    // *** begin Emergent internal use only ***

    enum
    {
        EVALPOSINDEX = NiEvaluator::EVALPOSINDEX,
        EVALROTINDEX = NiEvaluator::EVALROTINDEX,
        EVALSCALEINDEX = NiEvaluator::EVALSCALEINDEX,
        EVALMAXCHANNELS = NiEvaluator::EVALMAXCHANNELS,
    };

    NiMultiTargetPoseHandler(NiControllerManager* pkOwner,
        bool bMoveAccumTransformToChild = true);

    NiControllerManager* GetOwner() const;
    float GetLastUpdateTime() const;

    void Update(float fTime, int iLOD, NiPoseBuffer* pkPoseBuffer, 
        NiControllerSequence* pkSoleSequence);

    // Find the scene graph target object associated with an interpolator.
    bool FindTarget(const NiFixedString& kAVObjectName, 
        const NiFixedString& kPropertyType, const NiFixedString& kCtlrType, 
        const NiFixedString& kCtlrID, const NiFixedString& kInterpolatorID, 
        NiInterpController*& pkCtlr, NiAVObject*& pkAVObject);

    // Add new pose targets based on the owner's pose binding.
    void AddNewTargetsFromPoseBinding();

    // Fill the pose sequence data with constant interpolators derived
    // from scene graph data.
    void FillPoseSequenceData(const NiSequenceData* pkSeqDataToSynchronize, 
        NiSequenceData* pkPoseSeqData);

    void OnRemoveSequence(NiControllerSequence* pkSequence);

    // *** end Emergent internal use only ***

protected:
    // Default constructor for use in cloning and streaming only.
    NiMultiTargetPoseHandler();

    // Internal storage structures.
    enum
    {
        LODSKIPINTERP = -2,
        ACCUMENDINDEX = NiPoseBinding::ACCUMENDINDEX,
    };

    class InterpCtlrInfo : public NiMemObject
    {
    public:
        InterpCtlrInfo();

        void ClearValues();
        bool IsTransformEvaluator() const;

        short m_sLOD;
        unsigned short m_usInterpolatorIndex;   // INVALID_INDEX: transform
        union
        {
            NiInterpController* m_pkInterpCtlr; // non-transform controller
            NiAVObject* m_pkAVObject;   // in lieu of a transform controller
        };
    };

    void Init(NiControllerManager* pkOwner, bool bMoveAccumTransformToChild);
    void Shutdown();

    void AddInterpControllerInfos();
    void RemoveAllInterpControllerInfos();

    // The controller manager that owns this multi-target pose handler.
    NiControllerManager* m_pkOwner;
    float m_fLastUpdateTime;

    // Accum root data.
    NiFixedString m_kAccumRootName;
    NiAVObject* m_pkAccumRoot;
    // m_kAccumTransform, m_kStartAccumTransform, and m_kStartAccumRot
    // should never have invalid components after initialization.
    NiQuatTransform m_kAccumTransform;
    NiQuatTransform m_kStartAccumTransform;
    NiMatrix3 m_kStartAccumRot;
    NiControllerSequencePtr m_spSoleSequence;

    // Storage array for interp controller infos.
    // There should be one InterpCtlrInfo for each bind info index 
    // (as opposed to bind info) in the associated pose binding.
    unsigned int m_uiNumInterpCtlrInfos;
    InterpCtlrInfo* m_pkInterpCtlrInfos;
    unsigned short m_usColorStartIndex;
    unsigned short m_usBoolStartIndex;
    unsigned short m_usFloatStartIndex;
    unsigned short m_usPoint3StartIndex;
    unsigned short m_usRotStartIndex;
    unsigned short m_usReferencedStartIndex;
};

#include "NiMultiTargetPoseHandler.inl"

#endif
