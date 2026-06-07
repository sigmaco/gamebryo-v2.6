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

#ifndef NISCRATCHPAD_H
#define NISCRATCHPAD_H

#include "NiAnimationLibType.h"
#include <NiColor.h>
#include <NiPoint3.h>
#include <NiQuaternion.h>
#include <NiAVObject.h>
#include "NiQuatTransform.h"
#include "NiBSplineData.h"
#include "NiColorKey.h"
#include "NiBoolKey.h"
#include "NiFloatKey.h"
#include "NiPosKey.h"
#include "NiRotKey.h"
#include "NiScratchPadBlock.h"
#include "NiEvaluator.h"
#include "NiEvaluatorSPData.h"

class NiPoseBuffer;
class NiControllerSequence;
class NiControllerManager;


NiSmartPointer(NiScratchPad);

class NIANIMATION_ENTRY NiScratchPad : public NiObject
{
    NiDeclareRTTI;

public:
    NiScratchPad(NiControllerSequence* pkSequence);
    virtual ~NiScratchPad();

    // *** begin Emergent internal use only ***

    enum
    {
        POSITION = NiEvaluator::EVALPOSINDEX,
        ROTATION = NiEvaluator::EVALROTINDEX,
        SCALE = NiEvaluator::EVALSCALEINDEX,
        MAXACCUMCHANNELS = NiEvaluator::EVALMAXCHANNELS,
        INVALIDOUTPUTINDEX = 0xffff,
    };

    // The following data structures define the various types of 
    // memory blocks held within the scratch pad as enumerated
    // in NiScratchPadBlock.

    // Common header for all segment data structs.

    struct SegmentDataHeader
    {
        NiInt16 m_sLOD;
        NiUInt16 m_usOutputIndex; // index to pose buffer entry
    };

    // Color data structures.

    struct LinearColorFillData
    {
        NiUInt32 m_uiLastIndex;
        NiUInt32 m_uiNumKeys;
        NiColorKey* m_pkKeys; // pointer to source data
        NiColorKey::KeyType m_eType;
        unsigned char m_ucKeySize;
        char m_cUnusedPadding[3]; // to maintain 4 byte alignment
    };

    struct LinearColorSegmentData
    {
        SegmentDataHeader m_kHeader;
        float m_fStartTime;
        float m_fInvDeltaTime;
        NiColorA m_kValue0;
        NiColorA m_kOutTangent0;
    };

    struct BSplineColorFillData
    {
        NiBSplineData::Handle m_kCPHandle;
        float m_fStartTime;
        float m_fInvDeltaTime;
        int m_iLastMin;
        NiBSplineData* m_pkData; // pointer to source control point data
    };

    struct BSplineCompColorFillData
    {
        BSplineColorFillData m_kBaseData;
        float m_fOffset; // to decompress control points
        float m_fHalfRange; // to decompress control points
    };

    struct BSplineColorSegmentData
    {
        SegmentDataHeader m_kHeader;
        NiBSplineBasisData* m_pkSPBasisData; // pointer to scratch pad entry
        float m_afSourceArray[16]; // 4 NiColorA control points
    };

    // Bool data structures.

    struct ConstantBoolFillData
    {
        NiUInt32 m_uiLastIndex;
        NiUInt32 m_uiNumKeys;
        NiBoolKey* m_pkKeys; // pointer to source data
        NiBoolKey::KeyType m_eType;
        unsigned char m_ucKeySize;
        char m_cUnusedPadding[3]; // to maintain 4 byte alignment
    };

    struct ConstantBoolTimelineFillData
    {
        ConstantBoolFillData m_kBaseData;
        NiUInt32 m_uiPreviousIndex;
        bool m_bPreviousValue;
    };

    struct ConstantBoolSegmentData
    {
        SegmentDataHeader m_kHeader;
        float m_fValue0;
    };

    // Float data structures.

    struct CubicFloatFillData
    {
        NiUInt32 m_uiLastIndex;
        NiUInt32 m_uiNumKeys;
        NiFloatKey* m_pkKeys; // pointer to source data
        NiFloatKey::KeyType m_eType;
        unsigned char m_ucKeySize;
        char m_cUnusedPadding[3]; // to maintain 4 byte alignment
    };

    struct CubicFloatSegmentData
    {
        SegmentDataHeader m_kHeader;
        float m_fStartTime;
        float m_fInvDeltaTime;
        float m_fValue0;
        float m_fOutTangent0;
        float m_fA0;
        float m_fB0;
    };

    struct BSplineFloatFillData
    {
        NiBSplineData::Handle m_kCPHandle;
        float m_fStartTime;
        float m_fInvDeltaTime;
        int m_iLastMin;
        NiBSplineData* m_pkData; // pointer to source control point data
    };

    struct BSplineCompFloatFillData
    {
        BSplineFloatFillData m_kBaseData;
        float m_fOffset; // to decompress control points
        float m_fHalfRange; // to decompress control points
    };

    struct BSplineFloatSegmentData
    {
        SegmentDataHeader m_kHeader;
        NiBSplineBasisData* m_pkSPBasisData; // pointer to scratch pad entry
        float m_afSourceArray[4]; // 4 float control points
    };

    // Point3 data structures.

    struct CubicPoint3FillData
    {
        NiUInt32 m_uiLastIndex;
        NiUInt32 m_uiNumKeys;
        NiPosKey* m_pkKeys; // pointer to source data
        NiPosKey::KeyType m_eType;
        unsigned char m_ucKeySize;
        char m_cUnusedPadding[3]; // to maintain 4 byte alignment
    };

    struct CubicPoint3SegmentData
    {
        SegmentDataHeader m_kHeader;
        float m_fStartTime;
        float m_fInvDeltaTime;
        NiPoint3 m_kValue0;
        NiPoint3 m_kOutTangent0;
        NiPoint3 m_kA0;
        NiPoint3 m_kB0;
    };

    struct BSplinePoint3FillData
    {
        NiBSplineData::Handle m_kCPHandle;
        float m_fStartTime;
        float m_fInvDeltaTime;
        int m_iLastMin;
        NiBSplineData* m_pkData; // pointer to source control point data
    };

    struct BSplineCompPoint3FillData
    {
        BSplinePoint3FillData m_kBaseData;
        float m_fOffset; // to decompress control points
        float m_fHalfRange; // to decompress control points
    };

    struct BSplinePoint3SegmentData
    {
        SegmentDataHeader m_kHeader;
        NiBSplineBasisData* m_pkSPBasisData; // pointer to scratch pad entry
        float m_afSourceArray[12]; // 4 NiPoint3 control points
    };

    // Rotation data structures.

    struct RotFillData // for slerp and squad segments
    {
        NiUInt32 m_uiLastIndex;
        NiUInt32 m_uiNumKeys;
        NiRotKey* m_pkKeys; // pointer to source data
        NiRotKey::KeyType m_eType;
        unsigned char m_ucKeySize;
        char m_cUnusedPadding[3]; // to maintain 4 byte alignment
    };

    struct EulerRotFillData // for EULERKEY channels
    {
        float m_afMinTime[3];
        float m_afMaxTime[3];
        CubicFloatFillData m_akFloatFill[3];
        CubicFloatSegmentData m_akFloatSeg[3];
    };

    struct SlerpRotSegmentData
    {
        SegmentDataHeader m_kHeader;
        float m_fStartTime;
        float m_fInvDeltaTime;
        NiQuaternion m_kQuat0;
        NiQuaternion m_kQuat1;
    };

    struct SquadRotSegmentData
    {
        SlerpRotSegmentData m_kBaseData;
        NiQuaternion m_kOutQuat0;
        NiQuaternion m_kInQuat1;
    };

    struct BSplineRotFillData
    {
        NiBSplineData::Handle m_kCPHandle;
        float m_fStartTime;
        float m_fInvDeltaTime;
        int m_iLastMin;
        NiBSplineData* m_pkData; // pointer to source control point data
    };

    struct BSplineCompRotFillData
    {
        BSplineRotFillData m_kBaseData;
        float m_fOffset; // to decompress control points
        float m_fHalfRange; // to decompress control points
    };

    struct BSplineRotSegmentData
    {
        SegmentDataHeader m_kHeader;
        NiBSplineBasisData* m_pkSPBasisData; // pointer to scratch pad entry
        float m_afSourceArray[16]; // 4 NiQuaternion control points
    };

    // Path data structures.

    struct PathFillData
    {
        float m_fLastTime;
        NiPoint3 m_kPosition;
        NiQuaternion m_kRotation;
        float m_fPctMinTime;
        float m_fPctMaxTime;
        CubicFloatFillData m_kPctFill;
        CubicFloatSegmentData m_kPctSeg;
        // The position data is parameterized by percentage, not time.
        float m_fPosMinPct;
        float m_fPosMaxPct;
        CubicPoint3FillData m_kPosFill;
        CubicPoint3SegmentData m_kPosSeg;
        NiUInt16 m_usFlags;
        NiInt16 m_sFollowAxis;
        float m_fMaxBankAngle;
        float m_fSmoothing;
        float m_fMaxCurvature;
        NiInt32 m_iBankDir;
        float m_fTotalLength;
        float* m_pfPartialLength;
        // When necessary this fill data struct is augmented with the 
        // partial length data. The data array is appended to 
        // this struct within the scratch pad.
    };

    // NiPathEvaluator uses CubicPoint3SegmentData and 
    // SlerpRotSegmentData to hold the final path values.

    // LookAt data structures.

    struct LookAtFillData
    {
        float m_fLastTime;
        NiPoint3 m_kPos;
        float m_fRoll;
        float m_fScale;
        NiUInt16 m_usFlags;
        NiUInt16 m_usPadding;
        NiAVObjectPtr m_spLookAtObject;
        NiAVObjectPtr m_spDrivenObject;
        NiEvaluatorSPData m_akEvalSPData[3];
        NiBSplineBasisDataPtr m_apkSPBasisData[3]; // pointers into scratch pad
        // When necessary this fill data struct is augmented with the 
        // extra fill and segment data for the internal evaluators. 
        // These are appended to this struct within the scratch pad.
    };

    // NiLookAtEvaluator uses CubicPoint3SegmentData, 
    // SlerpRotSegmentData, and CubicFloatSegmentData to 
    // hold the final transform component values.

    // Constructor to set up a scratch pad for use with an 
    // individual evaluator that is not associated with a sequence. 
    // This constructor is primarily intended for tools.
    NiScratchPad(NiEvaluator* pkEvaluator);

    // Functions to directly access the data blocks.
    void* GetDataBlock(NiScratchPadBlock eSPBlock) const;
    NiUInt16 GetNumBlockItems(NiScratchPadBlock eSPBlock) const;
    NiUInt16 GetBlockItemSize(NiScratchPadBlock eSPBlock) const;

    // Function to update the scratch pad and pose buffer to 
    // the specified time.
    void Update(float fTime, float fScaledTime, NiInt16 sLOD,
        NiPoseBuffer* pkPoseBuffer);

    void InitRecycledScratchPad(NiControllerSequence* pkSequence);
    void PrepareForRecycling();

    void SetLastAccumTransform(const NiQuatTransform& kTransform);
    void ClearAccumTransformData();

    // *** end Emergent internal use only ***

protected:
    // Default constructor for use in cloning and streaming only
    // (which are not implemented).
    NiScratchPad();
    void Init(NiControllerSequence* pkSequence);
    void Init(NiEvaluator* pkEvaluator);
    void Shutdown();

    void UpdatePoseBufferFromScratchPadSegmentData(
        float fScaledTime, NiInt16 sLOD, 
        NiBSplineBasisData* pkSPBasisDataBlock, 
        NiPoseBuffer* pkPoseBuffer);

    void UpdateAccumTransformDelta(float fTime, float fScaledTime, 
        NiInt16 sLOD, NiPoseBuffer* pkPoseBuffer);

    // The controller manager of the associated controller sequence.
    NiControllerManager* m_pkControllerManager;

    // This single memory buffer stores all items regardless of data type.
    NiUInt32 m_uiScratchPadBufferSize;
    char* m_pcScratchPadBuffer;

    // Description of distinct blocks within the memory buffer.
    char* m_apcBlock[SPBMAXSCRATCHPADBLOCKS];  // do not free held pointers
    NiUInt16 m_ausNumBlockItems[SPBMAXSCRATCHPADBLOCKS];
    NiUInt16 m_ausBlockItemSize[SPBMAXSCRATCHPADBLOCKS];

    // Data to maintain consistency with the pose buffer.
    NiUInt16 m_usNumPoseBufferItems;
    NiInt16 m_sLastPoseBufferUpdateLOD;
    float m_fLastPoseBufferUpdateTime;

    // Accum transform data.
    NiEvaluator* m_pkAccumEvaluator;
    NiEvaluatorSPData* m_apkAccumEvalSPData[MAXACCUMCHANNELS];
    float m_fLastAccumScaledTime;
    NiQuatTransform m_kLastAccumTransform;
    // m_kLoopAccumTransform, m_kStartAccumTransform, and m_kStartAccumRot
    // should never have invalid components after initialization.
    NiQuatTransform m_kLoopAccumTransform;
    NiQuatTransform m_kStartAccumTransform;
    NiMatrix3 m_kStartAccumRot;
};

#include "NiScratchPad.inl"

#endif // #ifndef NISCRATCHPAD_H
