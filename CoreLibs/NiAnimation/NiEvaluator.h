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

#ifndef NIEVALUATOR_H
#define NIEVALUATOR_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include <NiBool.h>
#include "NiPoseBufferHandle.h"
#include "NiScratchPadBlock.h"

class NiQuaternion;
class NiPoint3;
class NiColorA;
class NiQuatTransform;
class NiObjectNET;
class NiAVObjectPalette;
class NiBSplineBasisData;
class NiEvaluatorSPData;


class NIANIMATION_ENTRY NiEvaluator : public NiObject
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiEvaluator);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    enum EvalChannelIndex
    {
        // Indices for m_aiEvalChannelTypes. These also serve as channel 
        // indices in the methods below. Frequently, derived classes will 
        // give these indices new aliases to improve clarity.
        EVALBASEINDEX = 0,
        EVALPOSINDEX = EVALBASEINDEX,
        EVALROTINDEX = 1,
        EVALSCALEINDEX = 2,
        EVALMAXCHANNELS = 3,
        // *** begin Emergent internal use only ***
        EVALFLAGINDEX = 3,
        EVALMAXINDICES = EVALMAXCHANNELS + 1,
        // *** end Emergent internal use only ***
    };

    // ID Tag information.
    class NIANIMATION_ENTRY IDTag : public NiMemObject
    {
    public:
        IDTag(const NiFixedString& kAVObjectName = NULL,
            const NiFixedString& kPropertyType = NULL,
            const NiFixedString& kCtlrType = NULL,
            const NiFixedString& kCtlrID = NULL,
            const NiFixedString& kEvaluatorID = NULL);
        IDTag(const IDTag& kOther);

        IDTag& operator=(const IDTag& kTag);

        const NiFixedString& GetAVObjectName() const;
        void SetAVObjectName(const NiFixedString& kAVObjectName);

        const NiFixedString& GetPropertyType() const;
        void SetPropertyType(const NiFixedString& kPropertyType);

        const NiFixedString& GetCtlrType() const;
        void SetCtlrType(const NiFixedString& kCtlrType);

        const NiFixedString& GetCtlrID() const;
        void SetCtlrID(const NiFixedString& kCtlrID);

        const NiFixedString& GetEvaluatorID() const;
        void SetEvaluatorID(const NiFixedString& kEvaluatorID);

        inline void ClearValues();

        // *** begin Emergent internal use only ***

        static unsigned short GetHashValue(const char* pcAVObjectName, 
            const char* pcPropertyType, const char* pcCtlrType, 
            const char* pcCtlrID, const char* pcEvaluatorID);

        // These hash values are valid immediately after construction.
        unsigned short GetSmallHashTableValue() const;
        unsigned short GetLargeHashTableValue() const;

        // Streaming functions.
        void RegisterStreamables(NiStream& kStream);
        void SaveBinary(NiStream& kStream);
        void LoadBinary(NiStream& kStream);
        bool IsEqual(const IDTag& kDest);

        // *** end Emergent internal use only ***

    protected:
        inline void UpdateHashTableValues();

        NiFixedString m_kAVObjectName;
        NiFixedString m_kPropertyType;
        NiFixedString m_kCtlrType;
        NiFixedString m_kCtlrID;
        NiFixedString m_kEvaluatorID;
        unsigned short m_usSmallHashTableValue;
        unsigned short m_usLargeHashTableValue;
    };

    virtual ~NiEvaluator();

    // Functions to get and set the ID tag.
    void GetIDTag(const char*& pcAVObjectName, const char*& pcPropertyType, 
        const char*& pcCtlrType, const char*& pcCtlrID, 
        const char*& pcEvaluatorID) const;
    void GetIDTag(NiFixedString& kAVObjectName, NiFixedString& kPropertyType, 
        NiFixedString& kCtlrType, NiFixedString& kCtlrID, 
        NiFixedString& kEvaluatorID) const;
    const IDTag& GetIDTag() const;
    const NiFixedString& GetAVObjectName() const;
    const NiFixedString& GetPropertyType() const;
    const NiFixedString& GetCtlrType() const;
    const NiFixedString& GetCtlrID() const;
    const NiFixedString& GetEvaluatorID() const;
    void SetIDTag(const NiFixedString& kAVObjectName, 
        const NiFixedString& kPropertyType, const NiFixedString& kCtlrType, 
        const NiFixedString& kCtlrID, const NiFixedString& kEvaluatorID);
    inline void SetIDTag(const IDTag& kIDTag);

    // Description of channels for use with pose buffers.
    unsigned int GetMaxEvalChannels() const;
    bool IsEvalChannelInvalid(unsigned int uiChannel) const;
    NiPoseBufferChannelType GetEvalPBChannelType(unsigned int uiChannel) const;
    void GetEvalPBChannelType(unsigned int uiChannel, 
        NiPoseBufferChannelType& ePBChannelType, bool& bPosed) const;

    // These functions differ from GetEvalPBChannelType in that 
    // they deal exclusively with the underlying component channels 
    // without regard to IsReferencedEvaluator.
    NiPoseBufferChannelType GetRawEvalPBChannelType(
        unsigned int uiChannel) const;
    bool IsRawEvalChannelPosed(unsigned int uiChannel) const;

    // These functions dictate whether the evaluator is updated again
    // on a repeated update time.
    bool AlwaysUpdate() const;
    void SetAlwaysUpdate(bool bEnabled = true);

    // For use in reducing the memory footprint of an evaluator.
    virtual void Collapse();

    // *** begin Emergent internal use only ***

    // ID Tag information.
    enum
    {
        SMALLHASHTABLESIZE = 31,
        LARGEHASHTABLESIZE = 127,
    };

    // Functions dealing with internal evaluator flags.
    bool IsTransformEvaluator() const;
    inline bool IsReferencedEvaluator() const;
    void SetReferencedEvaluator();
    bool IsShutdownRequired() const;

    // Function to obtain the value of a posed (i.e. constant) channel.
    // This base class method always returns false.
    virtual bool GetChannelPosedValue(unsigned int uiChannel, 
        void* pvResult) const;

    // Function to obtain the time-based value of the specified evaluator 
    // channel through the use of the scratch pad. This base class method 
    // always asserts and returns false.
    virtual bool UpdateChannel(float fTime, unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData, void* pvResult) const;

    // Functions to set up, initialize, and shut down the scratch pad.
    virtual bool GetChannelScratchPadInfo(unsigned int uiChannel, 
        bool bForceAlwaysUpdate, NiAVObjectPalette* pkPalette, 
        unsigned int& uiFillSize, bool& bSharedFillData, 
        NiScratchPadBlock& eSPBSegmentData, 
        NiBSplineBasisData*& pkBasisData) const = 0;
    virtual bool InitChannelScratchPadData(unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData* pkSPBasisData, 
        bool bInitSharedData, NiAVObjectPalette* pkPalette, 
        NiPoseBufferHandle kPBHandle) const = 0;
    // ShutdownChannelScratchPadData is only called when 
    // IsShutdownRequired returns true.
    virtual void ShutdownChannelScratchPadData(unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData) const;

    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    virtual void GuaranteeTimeRange(float fStartTime, float fEndTime);
    virtual NiEvaluator* GetSequenceEvaluator(float fStartTime,
        float fEndTime);

    static const NiBool INVALID_BOOL;
    static const float INVALID_FLOAT;
    static const NiQuaternion INVALID_QUATERNION;
    static const NiPoint3 INVALID_POINT3;
    static const NiColorA INVALID_COLORA;

    // *** end Emergent internal use only ***

protected:
    NiEvaluator(const NiFixedString& kAVObjectName, 
        const NiFixedString& kPropertyType, const NiFixedString& kCtlrType, 
        const NiFixedString& kCtlrID, const NiFixedString& kEvaluatorID);
    NiEvaluator(const IDTag& kIDTag);
    NiEvaluator();

    void SetEvalChannelPosed(unsigned int uiChannel);

    enum EvalChannelType
    {
        // This enum contains base channel types to be stored at the 
        // following indices of m_aiEvalChannelTypes: 
        // EVALBASEINDEX, EVALPOSINDEX, EVALROTINDEX, EVALSCALEINDEX.
        EVALINVALIDCHANNEL = 0x00,
        EVALCOLORCHANNEL = 0x01,
        EVALBOOLCHANNEL = 0x02,
        EVALFLOATCHANNEL = 0x03,
        EVALPOINT3CHANNEL = 0x04,
        EVALROTCHANNEL = 0x05,
    };

    enum EvalChannelFlags
    {
        // This enum contains flags to be stored at the 
        // following indices of m_aiEvalChannelTypes: 
        // EVALBASEINDEX, EVALPOSINDEX, EVALROTINDEX, EVALSCALEINDEX.
        // These flags affect the specifed channel.
        EVALPOSEDFLAG = 0x40,
    };

    enum EvalFlags
    {
        // This enum contains flags to be stored in
        // m_aiEvalChannelTypes[EVALFLAGINDEX].
        // These flags affect the evaluator as a whole.
        EVALREFERENCEDFLAG = 0x01,
        EVALTRANSFORMFLAG = 0x02,
        EVALALWAYSUPDATEFLAG = 0x04,
        EVALSHUTDOWNFLAG = 0x08,
    };

    IDTag m_kIDTag;
    NiInt8 m_aiEvalChannelTypes[EVALMAXINDICES];
};

NiSmartPointer(NiEvaluator);

#include "NiEvaluator.inl"

#endif  // #ifndef NIEVALUATOR_H
