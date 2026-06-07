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

#ifndef NIPOSEBUFFER_H
#define NIPOSEBUFFER_H

#include "NiAnimationLibType.h"
#include <NiPoint3.h>
#include <NiColor.h>
#include <NiQuaternion.h>
#include <NiObject.h>
#include <NiStream.h>
#include "NiPoseBufferHandle.h"
#include "NiPoseBinding.h"
#include "NiEvaluator.h"
#include "NiSequenceData.h"

class NiInterpolator;
class NiEvaluatorSPData;
class NiControllerSequence;


NiSmartPointer(NiPoseBuffer);

class NIANIMATION_ENTRY NiPoseBuffer : public NiObject
{
    NiDeclareRTTI;
    NiDeclareClone(NiPoseBuffer);

public:
    static bool IsItemValid(unsigned int uiWeightAndFlags);
    static bool IsItemValidAndNotCulled(unsigned int uiWeightAndFlags);
    static float GetItemFinalizedWeight(unsigned int uiWeightAndFlags);
    static void SetItemFinalizedWeight(float fFinalizedWeight, 
        unsigned int& uiWeightAndFlags);

    NiPoseBuffer(NiPoseBinding* pkPoseBinding);
    virtual ~NiPoseBuffer();

    NiPoseBinding* GetPoseBinding() const;
    unsigned int GetNumTotalItems() const;
    unsigned int GetNumValidItems() const;
    unsigned int GetNonAccumStartIndex() const;
    bool IsFullBodyPose() const;

    unsigned int GetNumColors() const;
    bool GetColor(NiPoseBufferHandle kPBHandle, NiColorA& kColor) const;
    bool GetColorIfNotCulled(NiPoseBufferHandle kPBHandle, 
        NiColorA& kColor) const;
    bool IsColorValid(NiPoseBufferHandle kPBHandle) const;
    void SetColor(NiPoseBufferHandle kPBHandle, const NiColorA& kColor);
    void SetColorValid(NiPoseBufferHandle kPBHandle, bool bValid);

    unsigned int GetNumBools() const;
    bool GetBool(NiPoseBufferHandle kPBHandle, float& fBool) const;
    bool GetBoolIfNotCulled(NiPoseBufferHandle kPBHandle, 
        float& fBool) const;
    bool IsBoolValid(NiPoseBufferHandle kPBHandle) const;
    void SetBool(NiPoseBufferHandle kPBHandle, float fBool);
    void SetBoolValid(NiPoseBufferHandle kPBHandle, bool bValid);

    unsigned int GetNumFloats() const;
    bool GetFloat(NiPoseBufferHandle kPBHandle, float& fValue) const;
    bool GetFloatIfNotCulled(NiPoseBufferHandle kPBHandle, 
        float& fValue) const;
    bool IsFloatValid(NiPoseBufferHandle kPBHandle) const;
    void SetFloat(NiPoseBufferHandle kPBHandle, float fValue);
    void SetFloatValid(NiPoseBufferHandle kPBHandle, bool bValid);

    unsigned int GetNumPoint3s() const;
    bool GetPoint3(NiPoseBufferHandle kPBHandle, NiPoint3& kPoint3) const;
    bool GetPoint3IfNotCulled(NiPoseBufferHandle kPBHandle, 
        NiPoint3& kPoint3) const;
    bool IsPoint3Valid(NiPoseBufferHandle kPBHandle) const;
    void SetPoint3(NiPoseBufferHandle kPBHandle, const NiPoint3& kPoint3);
    void SetPoint3Valid(NiPoseBufferHandle kPBHandle, bool bValid);

    unsigned int GetNumRots() const;
    bool GetRot(NiPoseBufferHandle kPBHandle, NiQuaternion& kRot) const;
    bool GetRotIfNotCulled(NiPoseBufferHandle kPBHandle, 
        NiQuaternion& kRot) const;
    bool IsRotValid(NiPoseBufferHandle kPBHandle) const;
    void SetRot(NiPoseBufferHandle kPBHandle, const NiQuaternion& kRot);
    void SetRotValid(NiPoseBufferHandle kPBHandle, bool bValid);

    struct ReferencedItem
    {
        NiControllerSequence* m_pkControllerSequence; // do not hold a ref
        NiEvaluator* m_pkReferencedEvaluator; // do not hold a ref
        NiEvaluatorSPData* m_apkEvalSPData[NiEvaluator::EVALMAXCHANNELS];
    };
    unsigned int GetNumReferencedItems() const;
    bool GetReferencedItem(NiPoseBufferHandle kPBHandle, 
        ReferencedItem& kItem) const;
    bool GetReferencedItemIfNotCulled(NiPoseBufferHandle kPBHandle, 
        ReferencedItem& kItem) const;
    bool IsReferencedItemValid(NiPoseBufferHandle kPBHandle) const;
    void SetReferencedItem(NiPoseBufferHandle kPBHandle, 
        const ReferencedItem& kItem);
    void SetReferencedItemValid(NiPoseBufferHandle kPBHandle, bool bValid);

    bool IsValid(NiPoseBufferHandle kPBHandle) const;
    void SetValid(NiPoseBufferHandle kPBHandle, bool bValid);

    bool GetHandleAndLOD(const NiFixedString& kAVObjectName,
        const NiFixedString& kPropertyType, const NiFixedString& kCtlrType,
        const NiFixedString& kCtlrID, const NiFixedString& kInterpolatorID, 
        NiPoseBufferChannelType ePBChannelType, 
        NiPoseBufferHandle& kPBHandle, short& sLOD) const;

    bool GetHandleAndLOD(const NiEvaluator::IDTag& kIDTag,
        NiPoseBufferChannelType ePBChannelType, 
        NiPoseBufferHandle& kPBHandle, short& sLOD) const;

    // *** begin Emergent internal use only ***

    // Format of m_puiFlagWeightArray
    //   Bits 0-24: finalized weight * FINALIZEDWEIGHTSCALER
    //   Bit 30: culled valid item flag (i.e. hidden by higher priority items)
    //   Bit 31: valid item flag
    enum
    {
        VALIDITEMFLAG = 0x80000000,
        CULLEDVALIDITEMFLAG = 0x40000000,
        FINALIZEDWEIGHTMASK = 0x01ffffff,
        FINALIZEDWEIGHTSCALER = (1 << 24),
    };

    struct ColorItem
    {
        NiColorA m_kColor;
    };
    struct BoolItem
    {
        // BoolItem is mapped to a float to match the data type utilized when 
        // blending boolean channels from multiple active sequences.
        float m_fBool;
    };
    struct FloatItem
    {
        float m_fFloat;
    };
    struct Point3Item
    {
        NiPoint3 m_kPoint3;
    };
    struct RotItem
    {
        NiQuaternion m_kRot;
    };

    // Functions to directly access the data blocks.
    // The master data block is subdivided into smaller blocks with 
    // one section per bound data type. The returned pointers will 
    // remain valid until new bindings are added to the 
    // associated pose binding.
    float* GetDataBlock();
    ColorItem* GetColorDataBlock();
    BoolItem* GetBoolDataBlock();
    FloatItem* GetFloatDataBlock();
    Point3Item* GetPoint3DataBlock();
    RotItem* GetRotDataBlock();
    ReferencedItem* GetReferencedDataBlock();

    // Function to directly access the data item specified by kPBHandle.
    float* GetDataPointer(NiPoseBufferHandle kPBHandle) const;

    // Function to access the flag and weight array.
    // The indices to this array match the global indices of data items 
    // within the composite data block returned by GetDataBlock. 
    unsigned int* GetFlagWeightArray();

    void InvalidateAllItems();
    void ValidateAccumDeltaItemsFromValidAccumItems();
    bool AddNewItemsFromPoseBinding(bool bCopyFloatWeightArray, 
        bool bCopyDataItems);

    // cloning support
    virtual void ProcessClone(NiCloningProcess& kCloning);

    // *** end Emergent internal use only ***

protected:
    // Default constructor for use in cloning and streaming only.
    NiPoseBuffer();
    void Init(NiPoseBinding* pkPoseBinding);
    void Shutdown();

    NiPoseBindingPtr m_spPoseBinding;
    unsigned short m_usNumTotalItems;
    unsigned short m_usNumValidItems;
    // 0 (no accum root), ACCUMENDINDEX (accum root)
    unsigned int m_uiNonAccumStartIndex;

    unsigned int* m_puiFlagWeightArray;

    // This single memory buffer stores all items regardless of data type.
    float* m_pkDataBlock;

    // Info regarding contents of m_pkDataBlock.
    unsigned short m_usNumColorItems;
    unsigned short m_usColorStartIndex;
    ColorItem* m_pkColorItems; // do not free
    unsigned short m_usNumBoolItems;
    unsigned short m_usBoolStartIndex;
    BoolItem* m_pkBoolItems; // do not free
    unsigned short m_usNumFloatItems;
    unsigned short m_usFloatStartIndex;
    FloatItem* m_pkFloatItems; // do not free
    unsigned short m_usNumPoint3Items;
    unsigned short m_usPoint3StartIndex;
    Point3Item* m_pkPoint3Items; // do not free
    unsigned short m_usNumRotItems;
    unsigned short m_usRotStartIndex;
    RotItem* m_pkRotItems; // do not free
    unsigned short m_usNumReferencedItems;
    unsigned short m_usReferencedStartIndex;
    ReferencedItem* m_pkReferencedItems; // do not free
};

#include "NiPoseBuffer.inl"

#endif
