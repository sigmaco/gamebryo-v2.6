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

#ifndef NIPOSEBINDING_H
#define NIPOSEBINDING_H

#include "NiAnimationLibType.h"
#include "NiObject.h"
#include "NiStream.h"
#include "NiAnimationConstants.h"
#include "NiPoseBufferHandle.h"
#include "NiEvaluator.h"

class NiSequenceData;
class NiControllerManager;


NiSmartPointer(NiPoseBinding);

class NIANIMATION_ENTRY NiPoseBinding : public NiObject
{
    NiDeclareRTTI;
    NiDeclareClone(NiPoseBinding);
    NiDeclareStream;

public:
    // destruction
    virtual ~NiPoseBinding();

    // member access
    bool GetCumulativeAnimations() const;
    const NiFixedString& GetAccumRootName() const;
    unsigned int GetNonAccumStartIndex() const;

    unsigned int GetNumTotalBindings() const;
    unsigned int GetNumColorBindings() const;
    unsigned int GetNumBoolBindings() const;
    unsigned int GetNumFloatBindings() const;
    unsigned int GetNumPoint3Bindings() const;
    unsigned int GetNumRotBindings() const;
    unsigned int GetNumReferencedBindings() const;

    bool GetHandleAndLOD(const NiFixedString& kAVObjectName, 
        const NiFixedString& kPropertyType, const NiFixedString& kCtlrType, 
        const NiFixedString& kCtlrID, const NiFixedString& kEvaluatorID, 
        NiPoseBufferChannelType ePBChannelType, 
        NiPoseBufferHandle& kPBHandle, short& sLOD) const;

    bool GetHandleAndLOD(const NiEvaluator::IDTag& kIDTag,
        NiPoseBufferChannelType ePBChannelType, 
        NiPoseBufferHandle& kPBHandle, short& sLOD) const;

    // *** begin Emergent internal use only ***

    enum
    {
        ACCUMENDINDEX = 2, // index 0: accum value; index 1: accum delta
        INVALIDCHANNELINDEX = 0xffff,
        INVALIDHASHINDEX = 0xffff,
        SMALLHASHTABLESIZE = NiEvaluator::SMALLHASHTABLESIZE,
        LARGEHASHTABLESIZE = NiEvaluator::LARGEHASHTABLESIZE,
        EVALBASEINDEX = NiEvaluator::EVALBASEINDEX,
        EVALPOSINDEX = NiEvaluator::EVALPOSINDEX,
        EVALROTINDEX = NiEvaluator::EVALROTINDEX,
        EVALSCALEINDEX = NiEvaluator::EVALSCALEINDEX,
    };

    NiPoseBinding(NiControllerManager* pkManager, 
        const NiAVObject* pkAccumRoot);

    void AddBindings(const NiSequenceData* pkSeqData, 
        NiControllerManager* pkManager);

    class NIANIMATION_ENTRY BindInfo : public NiMemObject
    {
        // This class stores the mapping of evaluator ID tags 
        // to pose buffer handles. Additionally, it holds the
        // level of detail setting for each evaluator.
    public:
        BindInfo();

        void AddPBHandle(NiPoseBufferHandle kPBHandle);
        NiPoseBufferHandle GetFirstPBHandle() const;
        NiPoseBufferHandle GetPBHandle(
            NiPoseBufferChannelType ePBChannelType) const;
        bool MatchesIDTag(const NiEvaluator::IDTag& kIDTag) const;
        bool MatchesIDTag(const NiFixedString& kAVObjectName, 
            const NiFixedString& kPropertyType, 
            const NiFixedString& kCtlrType, 
            const NiFixedString& kCtlrID, 
            const NiFixedString& kEvaluatorID) const;
        bool IsTransformData() const;

        // Streaming functions.
        void RegisterStreamables(NiStream& kStream);
        void SaveBinary(NiStream& kStream);
        void LoadBinary(NiStream& kStream);
        bool IsEqual(const BindInfo& kDest);

        NiFixedString m_kAVObjectName;
        NiFixedString m_kPropertyType;
        NiFixedString m_kCtlrType;
        NiFixedString m_kCtlrID;
        NiFixedString m_kEvaluatorID;
        short m_sLOD;
        unsigned short m_usNextHashIndex;
        NiPoseBufferHandle m_kPBHandle; // Primary handle for the evaluator.
        unsigned short m_usPBIndices[2]; // Extra handles for transforms.
    };

    NiPoseBufferHandle AddBinding(BindInfo* pkBindInfo, 
        NiPoseBufferChannelType ePBChannelType);

    unsigned int GetNumBindInfos() const;
    BindInfo* GetBindInfos();
    unsigned short* GetBindInfoIndexList();
    
    // Functions to obtain the BindInfo associated with a 
    // given pose buffer handle or evaluator ID tags.
    const BindInfo* GetBindInfo(NiPoseBufferHandle kPBHandle) const;
    BindInfo* GetBindInfo(const NiEvaluator::IDTag& kIDTag) const;
    BindInfo* GetBindInfo(const NiFixedString& kAVObjectName, 
        const NiFixedString& kPropertyType, const NiFixedString& kCtlrType, 
        const NiFixedString& kCtlrID, const NiFixedString& kEvaluatorID
        ) const;

    // cloning support
    virtual void ProcessClone(NiCloningProcess& kCloning);
    // *** end Emergent internal use only ***

protected:
    // Default constructor for use in cloning and streaming only.
    NiPoseBinding();

    void Init(NiControllerManager* pkManager, const NiAVObject* pkAccumRoot);
    void Shutdown();
    void InitAccumRootBindings(NiControllerManager* pkManager,
        const NiAVObject* pkAccumRoot);

    void ReallocBindings(unsigned int uiNumColors,
        unsigned int uiNumBools, unsigned int uiNumFloats, 
        unsigned int uiNumPoint3s, unsigned int uiNumRots,
        unsigned int uiNumReferencedItems, unsigned int uiNumBindInfos);
    void ReallocHashTable(unsigned int uiTableSize);

    NiFixedString m_kAccumRootName;
    // 0 (no accum root), ACCUMENDINDEX (accum root)
    unsigned short m_usNonAccumStartIndex; 

    // This single list stores indices to all the bindings, and defines the
    // order of bindings within any dependent pose buffer. The list contains
    // m_usNumTotalBindings indices which reference items in m_pkBindInfoList.
    unsigned short m_usNumTotalBindings;
    unsigned short* m_pusBindInfoIndexList;

    // This list holds the actual bind infos. A single entry can map to
    // multiple pose buffer handles, such as translate, rotate, and scale
    // for a transform evaluator. Generally, there is one BindInfo per 
    // unique evaluator IDTag in the sequences that have been bound.
    BindInfo* m_pkBindInfoList;
    unsigned short m_usNumBindInfos;

    // This hash table provides a mechanism to locate a specific 
    // bind info more quickly.
    unsigned short m_usTableSize;
    unsigned short* m_pusHashTable;

    unsigned short m_usNumColorBindings;
    unsigned short m_usColorStartIndex;
    unsigned short m_usNumBoolBindings;
    unsigned short m_usBoolStartIndex;
    unsigned short m_usNumFloatBindings;
    unsigned short m_usFloatStartIndex;
    unsigned short m_usNumPoint3Bindings;
    unsigned short m_usPoint3StartIndex;
    unsigned short m_usNumRotBindings;
    unsigned short m_usRotStartIndex;
    unsigned short m_usNumReferencedBindings;
    unsigned short m_usReferencedStartIndex;

    unsigned short m_usNumLODs;
};

#include "NiPoseBinding.inl"

#endif
