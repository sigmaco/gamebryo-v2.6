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

// Precompiled Header
#include "NiAnimationPCH.h"

#include "NiMultiTargetPoseHandler.h"
#include "NiAnimationConstants.h"
#include <NiNode.h>
#include <NiAVObjectPalette.h>
#include "NiConstTransformEvaluator.h"
#include "NiTransformController.h"
#include "NiControllerManager.h"


NiImplementRTTI(NiMultiTargetPoseHandler, NiObject);


//---------------------------------------------------------------------------
NiMultiTargetPoseHandler::NiMultiTargetPoseHandler(
    NiControllerManager* pkOwner, bool bMoveAccumTransformToChild) : 
    m_pkOwner(NULL), m_fLastUpdateTime(-NI_INFINITY),
    m_kAccumRootName(), m_pkAccumRoot(NULL), m_kAccumTransform(), 
    m_kStartAccumTransform(), m_kStartAccumRot(), m_spSoleSequence(),
    m_uiNumInterpCtlrInfos(0), m_pkInterpCtlrInfos(NULL),
    m_usColorStartIndex(0), m_usBoolStartIndex(0), 
    m_usFloatStartIndex(0), m_usPoint3StartIndex(0),
    m_usRotStartIndex(0), m_usReferencedStartIndex(0)
{
    Init(pkOwner, bMoveAccumTransformToChild);
}
//---------------------------------------------------------------------------
NiMultiTargetPoseHandler::NiMultiTargetPoseHandler() : 
    m_pkOwner(NULL), m_fLastUpdateTime(-NI_INFINITY),
    m_kAccumRootName(), m_pkAccumRoot(NULL), m_kAccumTransform(), 
    m_kStartAccumTransform(), m_kStartAccumRot(), m_spSoleSequence(),
    m_uiNumInterpCtlrInfos(0), m_pkInterpCtlrInfos(NULL),
    m_usColorStartIndex(0), m_usBoolStartIndex(0), 
    m_usFloatStartIndex(0), m_usPoint3StartIndex(0),
    m_usRotStartIndex(0), m_usReferencedStartIndex(0)
{
}
//---------------------------------------------------------------------------
NiMultiTargetPoseHandler::~NiMultiTargetPoseHandler()
{
    Shutdown();
}
//---------------------------------------------------------------------------
void NiMultiTargetPoseHandler::Update(float fTime, int iLOD, 
    NiPoseBuffer* pkPoseBuffer, NiControllerSequence* pkSoleSequence)
{
    // This function propagates data values contained within pkPoseBuffer to 
    // interp controllers and scene graph objects of the owning controller 
    // manager. 
    //
    // fTime typically is the application's global animation time. 
    //
    // iLOD specifies the current level of detail for the controller manager.
    // As an optimization, this function does not distribute data values 
    // whose effects are not visible due to level of detail.
    //
    // pkSoleSequence should be NULL if multiple sequences contributed to the 
    // data contained within pkPoseBuffer; otherwise, pkSoleSequence should 
    // point to the sole contributing sequence.
    //
    // When transform accumulation is active the pkSoleSequence parameter 
    // determines whether this function uses the absolute transform value 
    // stored at index 0 in pkPoseBuffer or adds the delta value contained 
    // at index 1 to the current accumulation transform. The absolute transform
    // minimizes accumulated floating point round-off error, but can only be 
    // used when a single sequence contributes to the pose buffer.

    NIASSERT(pkPoseBuffer);
    NIASSERT(pkPoseBuffer->GetPoseBinding());
    NIASSERT(m_pkOwner);
    NIASSERT(pkPoseBuffer->GetPoseBinding() == m_pkOwner->GetPoseBinding());

    short sLOD = (short)iLOD;

    // Push color data into the scene graph.
    NiPoseBuffer::ColorItem* pkColorItem = pkPoseBuffer->GetColorDataBlock();
    NiPoseBuffer::ColorItem* pkEndColorItem = pkColorItem + 
        pkPoseBuffer->GetNumColors();
    NIASSERT((pkEndColorItem - pkColorItem) <= 
        (m_usBoolStartIndex - m_usColorStartIndex));
    unsigned int* puiFlagWeight = pkPoseBuffer->GetFlagWeightArray();
    InterpCtlrInfo* pkInterpCtlrInfo = m_pkInterpCtlrInfos + m_usColorStartIndex;
    while (pkColorItem < pkEndColorItem)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            NIASSERT(!pkInterpCtlrInfo->IsTransformEvaluator());
            NIASSERT(pkInterpCtlrInfo->m_pkInterpCtlr);
            pkInterpCtlrInfo->m_pkInterpCtlr->UpdateValue(
                fTime, pkColorItem->m_kColor, 
                pkInterpCtlrInfo->m_usInterpolatorIndex);
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkColorItem++;
    }

    // Push bool data into the scene graph.
    NiPoseBuffer::BoolItem* pkBoolItem = pkPoseBuffer->GetBoolDataBlock();
    NiPoseBuffer::BoolItem* pkEndBoolItem = pkBoolItem + 
        pkPoseBuffer->GetNumBools();
    NIASSERT((pkEndBoolItem - pkBoolItem) <= 
        (m_usFloatStartIndex - m_usBoolStartIndex));
    NIASSERT(puiFlagWeight == pkPoseBuffer->GetFlagWeightArray() + 
        pkPoseBuffer->GetNumColors());
    pkInterpCtlrInfo = m_pkInterpCtlrInfos + m_usBoolStartIndex;
    while (pkBoolItem < pkEndBoolItem)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            NIASSERT(!pkInterpCtlrInfo->IsTransformEvaluator());
            NIASSERT(pkInterpCtlrInfo->m_pkInterpCtlr);
            pkInterpCtlrInfo->m_pkInterpCtlr->UpdateValue(
                fTime, (pkBoolItem->m_fBool >= 0.5f), 
                pkInterpCtlrInfo->m_usInterpolatorIndex);
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkBoolItem++;
    }

    // Get scale delta for accum root.
    unsigned int uiNonAccumStartIndex = pkPoseBuffer->GetNonAccumStartIndex();
    NIASSERT(uiNonAccumStartIndex <= ACCUMENDINDEX);
    NiQuatTransform kSeqAccumTransform[ACCUMENDINDEX];
    NiPoseBuffer::FloatItem* pkFloatItem = pkPoseBuffer->GetFloatDataBlock();
    NiPoseBuffer::FloatItem* pkEndFloatItem = pkFloatItem + 
        pkPoseBuffer->GetNumFloats();
    NIASSERT((pkEndFloatItem - pkFloatItem) <= 
        (m_usPoint3StartIndex - m_usFloatStartIndex));
    NIASSERT(puiFlagWeight == pkPoseBuffer->GetFlagWeightArray() + 
        pkPoseBuffer->GetNumColors() + pkPoseBuffer->GetNumBools());
    pkInterpCtlrInfo = m_pkInterpCtlrInfos + m_usFloatStartIndex;
    unsigned int uj = 0;
    while (uj < uiNonAccumStartIndex && pkFloatItem < pkEndFloatItem)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            NIASSERT(pkInterpCtlrInfo->IsTransformEvaluator());
            kSeqAccumTransform[uj].SetScale(pkFloatItem->m_fFloat);
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkFloatItem++;
        uj++;
    }

    // Push float data into the scene graph.
    while (pkFloatItem < pkEndFloatItem)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            if (pkInterpCtlrInfo->IsTransformEvaluator())
            {
                NIASSERT(pkInterpCtlrInfo->m_pkAVObject);
                pkInterpCtlrInfo->m_pkAVObject->SetScale(
                    pkFloatItem->m_fFloat);
            }
            else
            {
                NIASSERT(pkInterpCtlrInfo->m_pkInterpCtlr);
                pkInterpCtlrInfo->m_pkInterpCtlr->UpdateValue(
                    fTime, pkFloatItem->m_fFloat, 
                    pkInterpCtlrInfo->m_usInterpolatorIndex);
            }
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkFloatItem++;
    }

    // Get position delta for accum root.
    NiPoseBuffer::Point3Item* pkPoint3Item = 
        pkPoseBuffer->GetPoint3DataBlock();
    NiPoseBuffer::Point3Item* pkEndPoint3Item = pkPoint3Item + 
        pkPoseBuffer->GetNumPoint3s();
    NIASSERT((pkEndPoint3Item - pkPoint3Item) <= 
        (m_usRotStartIndex - m_usPoint3StartIndex));
    NIASSERT(puiFlagWeight == pkPoseBuffer->GetFlagWeightArray() + 
        pkPoseBuffer->GetNumColors() + pkPoseBuffer->GetNumBools() +
        pkPoseBuffer->GetNumFloats());
    pkInterpCtlrInfo = m_pkInterpCtlrInfos + m_usPoint3StartIndex;
    uj = 0;
    while (uj < uiNonAccumStartIndex && pkPoint3Item < pkEndPoint3Item)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            NIASSERT(pkInterpCtlrInfo->IsTransformEvaluator());
            kSeqAccumTransform[uj].SetTranslate(pkPoint3Item->m_kPoint3);
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkPoint3Item++;
        uj++;
    }

    // Push point3 data into the scene graph.
    while (pkPoint3Item < pkEndPoint3Item)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            if (pkInterpCtlrInfo->IsTransformEvaluator())
            {
                NIASSERT(pkInterpCtlrInfo->m_pkAVObject);
                pkInterpCtlrInfo->m_pkAVObject->SetTranslate(
                    pkPoint3Item->m_kPoint3);
            }
            else
            {
                NIASSERT(pkInterpCtlrInfo->m_pkInterpCtlr);
                pkInterpCtlrInfo->m_pkInterpCtlr->UpdateValue(
                    fTime, pkPoint3Item->m_kPoint3, 
                    pkInterpCtlrInfo->m_usInterpolatorIndex);
            }
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkPoint3Item++;
    }

    // Get rotation delta for accum root.
    NiPoseBuffer::RotItem* pkRotItem = pkPoseBuffer->GetRotDataBlock();
    NiPoseBuffer::RotItem* pkEndRotItem = pkRotItem + 
        pkPoseBuffer->GetNumRots();
    NIASSERT((pkEndRotItem - pkRotItem) <= 
        (m_usReferencedStartIndex - m_usRotStartIndex));
    NIASSERT(puiFlagWeight == pkPoseBuffer->GetFlagWeightArray() + 
        pkPoseBuffer->GetNumColors() + pkPoseBuffer->GetNumBools() +
        pkPoseBuffer->GetNumFloats() + pkPoseBuffer->GetNumPoint3s());
    pkInterpCtlrInfo = m_pkInterpCtlrInfos + m_usRotStartIndex;
    uj = 0;
    while (uj < uiNonAccumStartIndex && pkRotItem < pkEndRotItem)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            NIASSERT(pkInterpCtlrInfo->IsTransformEvaluator());
            kSeqAccumTransform[uj].SetRotate(pkRotItem->m_kRot);
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkRotItem++;
        uj++;
    }

    // Push rot data into the scene graph.
    while (pkRotItem < pkEndRotItem)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            if (pkInterpCtlrInfo->IsTransformEvaluator())
            {
                NIASSERT(pkInterpCtlrInfo->m_pkAVObject);
                pkInterpCtlrInfo->m_pkAVObject->SetRotate(pkRotItem->m_kRot);
            }
            else
            {
                NIASSERT(pkInterpCtlrInfo->m_pkInterpCtlr);
                pkInterpCtlrInfo->m_pkInterpCtlr->UpdateValue(
                    fTime, pkRotItem->m_kRot, 
                    pkInterpCtlrInfo->m_usInterpolatorIndex);
            }
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkRotItem++;
    }

    // Push referenced items into the scene graph.

    NiPoseBuffer::ReferencedItem* pkReferencedItem = 
        pkPoseBuffer->GetReferencedDataBlock();
    NiPoseBuffer::ReferencedItem* pkEndReferencedItem = pkReferencedItem + 
        pkPoseBuffer->GetNumReferencedItems();
    NIASSERT((pkEndReferencedItem - pkReferencedItem) <= 
        (int)(m_uiNumInterpCtlrInfos - m_usReferencedStartIndex));
    NIASSERT(puiFlagWeight == pkPoseBuffer->GetFlagWeightArray() + 
        pkPoseBuffer->GetNumColors() + pkPoseBuffer->GetNumBools() +
        pkPoseBuffer->GetNumFloats() + pkPoseBuffer->GetNumPoint3s() +
        pkPoseBuffer->GetNumRots());
    pkInterpCtlrInfo = m_pkInterpCtlrInfos + m_usReferencedStartIndex;
    while (pkReferencedItem < pkEndReferencedItem)
    {
        if (sLOD <= pkInterpCtlrInfo->m_sLOD && 
            NiPoseBuffer::IsItemValid(*puiFlagWeight))
        {
            NIASSERT(!pkInterpCtlrInfo->IsTransformEvaluator());
            NIASSERT(pkInterpCtlrInfo->m_pkInterpCtlr);
            NIASSERT(pkReferencedItem->m_pkReferencedEvaluator);
            pkInterpCtlrInfo->m_pkInterpCtlr->UpdateValue(fTime, 
                pkReferencedItem->m_pkControllerSequence, 
                pkReferencedItem->m_pkReferencedEvaluator, 
                &pkReferencedItem->m_apkEvalSPData[0],
                pkInterpCtlrInfo->m_usInterpolatorIndex);
        }
        puiFlagWeight++;
        pkInterpCtlrInfo++;
        pkReferencedItem++;
    }
    NIASSERT(puiFlagWeight == pkPoseBuffer->GetFlagWeightArray() + 
        pkPoseBuffer->GetNumColors() + pkPoseBuffer->GetNumBools() +
        pkPoseBuffer->GetNumFloats() + pkPoseBuffer->GetNumPoint3s() +
        pkPoseBuffer->GetNumRots() + pkPoseBuffer->GetNumReferencedItems());

    // Clear previous sole sequence if inactive.
    if (m_spSoleSequence && m_spSoleSequence->GetState() == INACTIVE)
    {
        m_spSoleSequence = NULL;
    }

    // If this is a new update time and the accum transform delta is valid,
    // update the accum transform and push it into the accum root node.
    // Index 0 holds the current sequence accum value (for when 
    //   there's only one contributing sequence). 
    // Index 1 contains the accum transform delta.
    if (fTime != m_fLastUpdateTime && 
        !kSeqAccumTransform[1].IsTransformInvalid())
    {
        // Check if a single sequence contributes to the pose buffer.
        if (pkSoleSequence)
        {
            if (pkSoleSequence != m_spSoleSequence)
            {
                m_spSoleSequence = pkSoleSequence;

                if (kSeqAccumTransform[0].IsTransformInvalid())
                {
                    // Set the start accum transform to identity.
                    m_kStartAccumTransform.SetScale(1.0f);
                    m_kStartAccumTransform.SetRotate(NiQuaternion::IDENTITY);
                    m_kStartAccumTransform.SetTranslate(NiPoint3::ZERO);
                    m_kStartAccumRot.MakeIdentity();
                }
                else
                {
                    // Compute the inverse of the last accum transform.
                    NiQuatTransform kCurrentInv;
                    kSeqAccumTransform[0].Invert(kCurrentInv);
                    NiQuatTransform kLastInv = 
                        kSeqAccumTransform[1] * kCurrentInv;
                    if (kLastInv.IsRotateValid())
                    {
                        NiMatrix3 kRefRot;
                        NiQuaternion kLastRot = NiQuaternion::UnitInverse(
                            kLastInv.GetRotate());
                        kLastRot.ToRotation(kRefRot);
                        kLastInv.SetTranslate(
                            kCurrentInv.GetTranslate() + 
                            kRefRot * kSeqAccumTransform[1].GetTranslate());
                    }

                    // Compute the start accum transform. 
                    // Ensure all components are valid.
                    NIASSERT(m_kAccumTransform.IsScaleValid());
                    float fScale = m_kAccumTransform.GetScale();
                    if (kLastInv.IsScaleValid())
                    {
                        fScale *= kLastInv.GetScale();
                    }
                    m_kStartAccumTransform.SetScale(fScale);

                    NIASSERT(m_kAccumTransform.IsRotateValid());
                    NiQuaternion kRot = m_kAccumTransform.GetRotate();
                    if (kLastInv.IsRotateValid())
                    {
                        kRot = kRot * kLastInv.GetRotate();
                    }
                    m_kStartAccumTransform.SetRotate(kRot);
                    kRot.ToRotation(m_kStartAccumRot);

                    NIASSERT(m_kAccumTransform.IsTranslateValid());
                    NiPoint3 kPos = m_kAccumTransform.GetTranslate();
                    if (kLastInv.IsTranslateValid())
                    {
                        kPos += m_kStartAccumRot * kLastInv.GetTranslate();
                    }
                    m_kStartAccumTransform.SetTranslate(kPos);
                }
            }

            // Compute the accum transform. 
            // Ensure all components are valid.
            if (kSeqAccumTransform[0].IsTranslateValid())
            {
                kSeqAccumTransform[0].SetTranslate(
                    m_kStartAccumRot * kSeqAccumTransform[0].GetTranslate());
            }
            NiQuatTransform kTransform = m_kStartAccumTransform * 
                kSeqAccumTransform[0];
            if (kTransform.IsScaleValid())
            {
                m_kAccumTransform.SetScale(kTransform.GetScale());
            }
            if (kTransform.IsRotateValid())
            {
                m_kAccumTransform.SetRotate(kTransform.GetRotate());
            }
            if (kTransform.IsTranslateValid())
            {
                m_kAccumTransform.SetTranslate(kTransform.GetTranslate());
            }
        }
        else
        {
            // More than one sequence contributes to the pose buffer.
            // Index 1 in the pose buffer contains the accum transform delta.
            m_spSoleSequence = NULL;

            // Compute the accum transform. 
            // Ensure all components are valid.
            if (kSeqAccumTransform[1].IsTranslateValid())
            {
                NIASSERT(m_kAccumTransform.IsRotateValid());
                NiMatrix3 kAccumRot;
                m_kAccumTransform.GetRotate().ToRotation(kAccumRot);
                kSeqAccumTransform[1].SetTranslate(
                    kAccumRot * kSeqAccumTransform[1].GetTranslate());
            }
            NiQuatTransform kTransform = m_kAccumTransform * 
                kSeqAccumTransform[1];
            if (kTransform.IsScaleValid())
            {
                m_kAccumTransform.SetScale(kTransform.GetScale());
            }
            if (kTransform.IsRotateValid())
            {
                m_kAccumTransform.SetRotate(kTransform.GetRotate());
            }
            if (kTransform.IsTranslateValid())
            {
                m_kAccumTransform.SetTranslate(kTransform.GetTranslate());
            }
        }

        // Push the updated accum transform into the accum root node.
        if (m_pkAccumRoot)
        {
            NIASSERT(m_kAccumTransform.IsScaleValid());
            m_pkAccumRoot->SetScale(m_kAccumTransform.GetScale());
            NIASSERT(m_kAccumTransform.IsRotateValid());
            m_pkAccumRoot->SetRotate(m_kAccumTransform.GetRotate());
            NIASSERT(m_kAccumTransform.IsTranslateValid());
            m_pkAccumRoot->SetTranslate(m_kAccumTransform.GetTranslate());
        }
    }

    m_fLastUpdateTime = fTime;
}
//---------------------------------------------------------------------------
bool NiMultiTargetPoseHandler::FindTarget(const NiFixedString& kAVObjectName, 
    const NiFixedString& kPropertyType, const NiFixedString& kCtlrType, 
    const NiFixedString& kCtlrID, const NiFixedString& kEvaluatorID, 
    NiInterpController*& pkCtlr, NiAVObject*& pkAVObject)
{
    // This function returns true when the specified evaluator is mapped 
    // to a specific target object, and false otherwise. Upon success, 
    // the function returns the target interp controller via pkCtlr.
    // 
    // When searching for an NiTransformController target the returned 
    // pkCtlr will always be NULL. In this situation, the target 
    // transform node is returned via pkAVObject. This is the result of 
    // an optimization whereby the multi-target pose handler communicates 
    // directly with the NiAVObject associated with transform evaluators.

    NIASSERT(m_pkOwner);
    NIASSERT(m_pkOwner->GetPoseBinding());
    NIASSERT(m_uiNumInterpCtlrInfos == 
        m_pkOwner->GetPoseBinding()->GetNumTotalBindings());

    pkCtlr = NULL;
    pkAVObject = NULL;

    NiPoseBinding* pkPoseBinding = m_pkOwner->GetPoseBinding();
    NiPoseBinding::BindInfo* pkBindInfo = pkPoseBinding->GetBindInfo(
        kAVObjectName, kPropertyType, kCtlrType, kCtlrID, kEvaluatorID);
    if (pkBindInfo)
    {
        // Convert the bind info into an index.
        NiPoseBufferHandle kPBHandle = pkBindInfo->GetFirstPBHandle();
        if (kPBHandle.IsValid())
        {
            unsigned short usIndex = kPBHandle.GetChannelIndex();
            NiPoseBufferChannelType ePBChannelType = 
                kPBHandle.GetChannelType();
            switch(ePBChannelType)
            {
            case PBCOLORCHANNEL:
                usIndex = usIndex + m_usColorStartIndex;
                break;
            case PBBOOLCHANNEL:
                usIndex = usIndex + m_usBoolStartIndex;
                break;
            case PBFLOATCHANNEL:
                usIndex = usIndex + m_usFloatStartIndex;
                break;
            case PBPOINT3CHANNEL:
                usIndex = usIndex + m_usPoint3StartIndex;
                break;
            case PBROTCHANNEL:
                usIndex = usIndex + m_usRotStartIndex;
                break;
            case PBREFERENCEDCHANNEL:
                usIndex = usIndex + m_usReferencedStartIndex;
                break;
            default:
                NIASSERT(false);
            }

            // Transform attributes are handled internally as a special, 
            // optimized case. In this situation, the associated AVObject
            // should be returned.
            NIASSERT(usIndex < m_uiNumInterpCtlrInfos);
            if (m_pkInterpCtlrInfos[usIndex].IsTransformEvaluator())
            {
                pkAVObject = m_pkInterpCtlrInfos[usIndex].m_pkAVObject;
            }
            else
            {
                pkCtlr = m_pkInterpCtlrInfos[usIndex].m_pkInterpCtlr;
            }
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiMultiTargetPoseHandler::FillPoseSequenceData(
    const NiSequenceData* pkSeqDataToSynchronize, 
    NiSequenceData* pkPoseSeqData)
{
    // This function fills pkPoseSeqData with new evaluators 
    // containing constant data which matches the current value 
    // of scene graph attributes. 
    //
    // pkSeqDataToSynchronize dictates which scene graph attributes 
    // are captured and stored within pkPoseSeqData.

    NIASSERT(pkSeqDataToSynchronize);
    NIASSERT(pkPoseSeqData);
    NIASSERT(m_pkOwner);

    NiPoseBinding* pkPoseBinding = m_pkOwner->GetPoseBinding();
    NIASSERT(pkPoseBinding);
    unsigned int uiNumEvaluators = pkSeqDataToSynchronize->GetNumEvaluators();
    for (unsigned int ui = 0; ui < uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = pkSeqDataToSynchronize->GetEvaluatorAt(ui);
        NIASSERT(pkEvaluator);

        const NiEvaluator::IDTag& kIDTag = pkEvaluator->GetIDTag();
        const NiPoseBinding::BindInfo* pkBindInfo = 
            pkPoseBinding->GetBindInfo(kIDTag);

        NiPoseBufferHandle kPBHandle;
        unsigned int uiMaxEvalChannels = pkEvaluator->GetMaxEvalChannels();
        bool bFoundChannelType[EVALMAXCHANNELS];
        if (pkBindInfo)
        {
            for (unsigned int uj = 0; uj < uiMaxEvalChannels; uj++)
            {
                bFoundChannelType[uj] = false;
                NiPoseBufferChannelType ePBChannelType = 
                    pkEvaluator->GetEvalPBChannelType(uj);
                if (ePBChannelType != PBINVALIDCHANNEL)
                {
                    NiPoseBufferHandle kPBHandleTemp = 
                        pkBindInfo->GetPBHandle(ePBChannelType);
                    if (kPBHandleTemp.IsValid())
                    {
                        kPBHandle = kPBHandleTemp;
                        bFoundChannelType[uj] = true;
                    }
                    else
                    {
                        NILOG("NiMultiTargetPoseHandler::FillPoseSequenceData "
                            "failed to find target with the following "
                            "identifiers:\n\tm_pcAVObjectName\t%s\n\t"
                            "m_pcPropertyType\t%s\n\tm_pcCtlrType\t\t%s\n\t"
                            "m_pcCtlrID\t\t\t%s\n\tm_pcEvaluatorID\t%s\n\t"
                            "ePBChannelType\t\t%d\n",
                            (const char*)kIDTag.GetAVObjectName(),
                            (const char*)kIDTag.GetPropertyType(), 
                            (const char*)kIDTag.GetCtlrType(),
                            (const char*)kIDTag.GetCtlrID(), 
                            (const char*)kIDTag.GetEvaluatorID(),
                            ePBChannelType);
                    }
                }
            }
        }

        if (!kPBHandle.IsValid())
        {
            NILOG("NiMultiTargetPoseHandler::FillPoseSequenceData "
                "failed to find any targets with the following identifiers:"
                "\n\tm_pcAVObjectName\t%s\n\tm_pcPropertyType\t%s\n\t"
                "m_pcCtlrType\t\t%s\n\tm_pcCtlrID\t\t\t%s\n\t"
                "m_pcEvaluatorID\t%s\n",
                (const char*)kIDTag.GetAVObjectName(),
                (const char*)kIDTag.GetPropertyType(), 
                (const char*)kIDTag.GetCtlrType(),
                (const char*)kIDTag.GetCtlrID(), 
                (const char*)kIDTag.GetEvaluatorID());
            continue;
        }

        unsigned short usIndex = kPBHandle.GetChannelIndex();
        NiPoseBufferChannelType ePBChannelType = kPBHandle.GetChannelType();
        switch(ePBChannelType)
        {
        case PBCOLORCHANNEL:
            usIndex = usIndex + m_usColorStartIndex;
            break;
        case PBBOOLCHANNEL:
            usIndex = usIndex + m_usBoolStartIndex;
            break;
        case PBFLOATCHANNEL:
            usIndex = usIndex + m_usFloatStartIndex;
            break;
        case PBPOINT3CHANNEL:
            usIndex = usIndex + m_usPoint3StartIndex;
            break;
        case PBROTCHANNEL:
            usIndex = usIndex + m_usRotStartIndex;
            break;
        case PBREFERENCEDCHANNEL:
            usIndex = usIndex + m_usReferencedStartIndex;
            break;
        default:
            NIASSERT(false);
        }

        NiEvaluator* pkPoseEvaluator;
        NIASSERT(usIndex < m_uiNumInterpCtlrInfos);
        InterpCtlrInfo* pkInterpCtlrInfo = m_pkInterpCtlrInfos + usIndex;
        if (pkInterpCtlrInfo->IsTransformEvaluator())
        {
            NIASSERT(kIDTag.GetCtlrType() == 
                NiAnimationConstants::GetTransformCtlrType());

            NiAVObject* pkTarget = pkInterpCtlrInfo->m_pkAVObject;
            NIASSERT(pkTarget);

            // Only sync the channels that are present in the template.
            NiQuatTransform kTransform;
            if (bFoundChannelType[EVALPOSINDEX])
            {
                kTransform.SetTranslate(pkTarget->GetTranslate());
            }
            if (bFoundChannelType[EVALROTINDEX])
            {
                NiQuaternion kRotate;
                pkTarget->GetRotate(kRotate);
                kTransform.SetRotate(kRotate);
            }
            if (bFoundChannelType[EVALSCALEINDEX])
            {
                kTransform.SetScale(pkTarget->GetScale());
            }
            pkPoseEvaluator = NiNew NiConstTransformEvaluator(kTransform);
        }
        else
        {
            NIASSERT(kIDTag.GetCtlrType() != 
                NiAnimationConstants::GetTransformCtlrType());

            NiInterpController* pkInterpCtlr = 
                pkInterpCtlrInfo->m_pkInterpCtlr;
            NIASSERT(pkInterpCtlr);
            NIASSERT(pkInterpCtlrInfo->m_usInterpolatorIndex != 
                NiInterpController::INVALID_INDEX);
            NIASSERT(pkInterpCtlrInfo->m_usInterpolatorIndex == 
                pkInterpCtlr->GetInterpolatorIndex(
                kIDTag.GetEvaluatorID()));
            pkPoseEvaluator = pkInterpCtlr->CreatePoseEvaluator(
                pkInterpCtlrInfo->m_usInterpolatorIndex);
        }

        if (ePBChannelType == PBREFERENCEDCHANNEL)
        {
            pkPoseEvaluator->SetReferencedEvaluator();
        }

        pkPoseEvaluator->SetIDTag(kIDTag);
        pkPoseSeqData->AddEvaluator(pkPoseEvaluator);
    }
}
//---------------------------------------------------------------------------
void NiMultiTargetPoseHandler::OnRemoveSequence(
    NiControllerSequence* pkSequence)
{
    // Clear the sole sequence if it's being removed from the manager.
    if (pkSequence == m_spSoleSequence)
    {
        m_spSoleSequence = NULL;
    }
}
//---------------------------------------------------------------------------
void NiMultiTargetPoseHandler::Init(NiControllerManager* pkOwner,
    bool bMoveAccumTransformToChild)
{
    // This function initializes the multi-target pose handler for use with 
    // the specified controller manager. The internal interp controller array 
    // is updated with new scene graph targets based on the manager's 
    // associated NiPoseBinding.
    // 
    // If animation accumulation is active on the manager, the scene graph 
    // is traversed to locate the accumulation root node.
    //
    // When bMoveAccumTransformToChild is true the transform on the 
    // accumulation root is pushed to its non-accum child node. This 
    // adjustment ensures the initial transform on the accumulation root 
    // is identity (as if no accum flags had been set on the accum root node 
    // during export).
    //
    // The accumulation transform stored in the multi-target pose handler is 
    // initialized from the final transform value on the accumulation root. 
    // In general, this is the identity transform.

    NIASSERT(pkOwner && !m_pkOwner);
    NIASSERT(pkOwner->GetPoseBinding());

    m_pkOwner = pkOwner;
    NiPoseBinding* pkPoseBinding = pkOwner->GetPoseBinding();
    bool bCumulativeAnimations = pkOwner->GetCumulativeAnimations();
    NIASSERT(bCumulativeAnimations == 
        (pkPoseBinding->GetNonAccumStartIndex() == ACCUMENDINDEX));
    if (bCumulativeAnimations)
    {
        const NiFixedString& kAccumRootName = 
            pkPoseBinding->GetAccumRootName();
        if (kAccumRootName.Exists())
        {
            NiAVObjectPalette* pkObjectPalette = pkOwner->GetObjectPalette();
            if (pkObjectPalette)
            {
                m_pkAccumRoot = pkObjectPalette->GetAVObject(kAccumRootName);
            }
            else
            {
                m_pkAccumRoot = NiControllerManager::FindAccumRoot(
                    (NiAVObject*)pkOwner->GetTarget());
            }
            if (m_pkAccumRoot)
            {
                NIASSERT(m_pkAccumRoot->GetName() == kAccumRootName);

                m_kAccumRootName = kAccumRootName;

                // For backwards compatibility with pre-Gamebryo 2.6:
                // Move the root transform if the root is not identity.
                NiQuaternion kRot;
                m_pkAccumRoot->GetRotate(kRot);
                if (bMoveAccumTransformToChild &&
                    (kRot != NiQuaternion::IDENTITY ||
                    m_pkAccumRoot->GetTranslate() != NiPoint3::ZERO ||
                    m_pkAccumRoot->GetScale() != 1.0f))
                {
                    NiAVObject* pkNonAccumRoot = 0;
                    NiNode* pkNode = NiDynamicCast(NiNode, m_pkAccumRoot);
                    NIASSERT(pkNode);
                    const char* pcNonAccumSuffix = 
                        NiAnimationConstants::GetNonAccumSuffix();
                    int iNonAccumLength = 
                        NiAnimationConstants::GetNonAccumSuffixLength();
                    unsigned int uiNumChildren = pkNode->GetArrayCount();
                    for (unsigned int ui = 0; ui < uiNumChildren; ui++)
                    {
                        NiAVObject* pkChild = pkNode->GetAt(ui);
                        if (pkChild)
                        {
                            const NiFixedString& kChildName = 
                                pkChild->GetName();
                            int length = (int)kChildName.GetLength() - 
                                iNonAccumLength;
                            if (length >= 0)
                            {
                                const char* pcSuffix = &kChildName[length];
                                if (strcmp(pcSuffix, pcNonAccumSuffix) == 0)
                                {
                                    // Found the " NonAccum" suffix.
                                    // Compare child name to accum root name.
                                    if (strncmp(kChildName, kAccumRootName, 
                                        length) == 0)
                                    {
                                        pkNonAccumRoot = pkChild;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    if (pkNonAccumRoot)
                    {
                        pkNonAccumRoot->SetRotate(m_pkAccumRoot->GetRotate());
                        pkNonAccumRoot->SetTranslate(
                            m_pkAccumRoot->GetTranslate());
                        pkNonAccumRoot->SetScale(m_pkAccumRoot->GetScale());
                        m_pkAccumRoot->SetRotate(NiQuaternion::IDENTITY);
                        m_pkAccumRoot->SetTranslate(NiPoint3::ZERO);
                        m_pkAccumRoot->SetScale(1.0f);
                    }
                }

                // Initialize accum transform with scene graph value.
                m_kAccumTransform.SetScale(m_pkAccumRoot->GetScale());
                NiQuaternion kRotate;
                m_pkAccumRoot->GetRotate(kRotate);
                m_kAccumTransform.SetRotate(kRotate);
                m_kAccumTransform.SetTranslate(m_pkAccumRoot->GetTranslate());

                // Ensure all accum components are valid.
                if (!m_kAccumTransform.IsScaleValid())
                {
                    m_kAccumTransform.SetScale(1.0f);
                }
                if (!m_kAccumTransform.IsRotateValid())
                {
                    m_kAccumTransform.SetRotate(NiQuaternion::IDENTITY);
                }
                if (!m_kAccumTransform.IsTranslateValid())
                {
                    m_kAccumTransform.SetTranslate(NiPoint3::ZERO);
                }
            }
        }
    }

    AddInterpControllerInfos();
}
//---------------------------------------------------------------------------
void NiMultiTargetPoseHandler::Shutdown()
{
    m_spSoleSequence = NULL;

    RemoveAllInterpControllerInfos();

    m_pkOwner = NULL;
    m_pkAccumRoot = NULL;
}
//---------------------------------------------------------------------------
void NiMultiTargetPoseHandler::AddInterpControllerInfos()
{
    // This function updates the interp controller (i.e. target) array 
    // to match the bindings used by the owning controller manager.

    NIASSERT(m_pkOwner);
    NIASSERT(m_pkOwner->GetPoseBinding());

    NiAVObjectPalette* pkObjectPalette = m_pkOwner->GetObjectPalette();
    if (!pkObjectPalette)
    {
        NILOG("NiMultiTargetPoseHandler::AddInterpControllerInfos "
            "missing AVObjectPalette -- cannot locate any targets "
            "without it\n");
        return;
    }

    // Determine new size of array.
    NiPoseBinding* pkPoseBinding = m_pkOwner->GetPoseBinding();
    unsigned int uiNumTotalBindings = pkPoseBinding->GetNumTotalBindings();
    NIASSERT(m_uiNumInterpCtlrInfos <= uiNumTotalBindings);
    if (m_uiNumInterpCtlrInfos == uiNumTotalBindings)
    {
        return;
    }

    // Allocate larger array.
    InterpCtlrInfo* pkNewInfos = NiNew InterpCtlrInfo[uiNumTotalBindings];

    // Copy existing data.
    InterpCtlrInfo* pkNewInfo = pkNewInfos;
    InterpCtlrInfo* pkOldInfo = m_pkInterpCtlrInfos;
    InterpCtlrInfo* pkEndOldInfo = m_pkInterpCtlrInfos;
    unsigned int uiChannelType = 0;
    NIASSERT(m_usColorStartIndex == 0);
    for (unsigned int ui = 0; ui < m_uiNumInterpCtlrInfos; ui++)
    {
        while (pkOldInfo == pkEndOldInfo)
        {
            switch(uiChannelType)
            {
            case 0:
                pkEndOldInfo = m_pkInterpCtlrInfos + m_usBoolStartIndex;
                break;
            case 1:
                pkEndOldInfo = m_pkInterpCtlrInfos + m_usFloatStartIndex;
                pkNewInfo += pkPoseBinding->GetNumColorBindings() - 
                    (m_usBoolStartIndex - m_usColorStartIndex);
                break;
            case 2:
                pkEndOldInfo = m_pkInterpCtlrInfos + m_usPoint3StartIndex;
                pkNewInfo += pkPoseBinding->GetNumBoolBindings() - 
                    (m_usFloatStartIndex - m_usBoolStartIndex);
                break;
            case 3:
                pkEndOldInfo = m_pkInterpCtlrInfos + m_usRotStartIndex;
                pkNewInfo += pkPoseBinding->GetNumFloatBindings() - 
                    (m_usPoint3StartIndex - m_usFloatStartIndex);
                break;
            case 4:
                pkEndOldInfo = m_pkInterpCtlrInfos + m_usReferencedStartIndex;
                pkNewInfo += pkPoseBinding->GetNumPoint3Bindings() - 
                    (m_usRotStartIndex - m_usPoint3StartIndex);
                break;
            case 5:
                pkEndOldInfo = m_pkInterpCtlrInfos + m_uiNumInterpCtlrInfos;
                pkNewInfo += pkPoseBinding->GetNumRotBindings() - 
                    (m_usReferencedStartIndex - m_usRotStartIndex);
                break;
            default:
                NIASSERT(false);
            }
            uiChannelType++;
        }
        *pkNewInfo = *pkOldInfo;
        pkNewInfo++;
        pkOldInfo++;
    }

    // Delete existing data.
    NiDelete[] m_pkInterpCtlrInfos;

    // Use the new array.
    m_pkInterpCtlrInfos = pkNewInfos;

    // Fill in new entries in array.
    bool bNeedToUpdateSceneGraphFlags = false;
    NiPoseBinding::BindInfo* pkBindInfos = pkPoseBinding->GetBindInfos();
    unsigned short* pusIndexList = pkPoseBinding->GetBindInfoIndexList();
    uiChannelType = 0;
    InterpCtlrInfo* pkEndNewInfo = m_pkInterpCtlrInfos;
    InterpCtlrInfo* pkEndNewInfos = m_pkInterpCtlrInfos + uiNumTotalBindings;
    for (pkNewInfo = m_pkInterpCtlrInfos; pkNewInfo < pkEndNewInfos; 
        pkNewInfo++)
    {
        while (pkNewInfo == pkEndNewInfo && pkNewInfo < pkEndNewInfos)
        {
            switch(uiChannelType)
            {
            case 0:
                pkNewInfo += (m_usBoolStartIndex - m_usColorStartIndex);
                pkEndNewInfo += pkPoseBinding->GetNumColorBindings();
                break;
            case 1:
                pkNewInfo += (m_usFloatStartIndex - m_usBoolStartIndex);
                pkEndNewInfo += pkPoseBinding->GetNumBoolBindings();
                break;
            case 2:
                pkNewInfo += (m_usPoint3StartIndex - m_usFloatStartIndex);
                pkEndNewInfo += pkPoseBinding->GetNumFloatBindings();
                break;
            case 3:
                pkNewInfo += (m_usRotStartIndex - m_usPoint3StartIndex);
                pkEndNewInfo += pkPoseBinding->GetNumPoint3Bindings();
                break;
            case 4:
                pkNewInfo += (m_usReferencedStartIndex - m_usRotStartIndex);
                pkEndNewInfo += pkPoseBinding->GetNumRotBindings();
                break;
            case 5:
                pkNewInfo += (m_uiNumInterpCtlrInfos - 
                    m_usReferencedStartIndex);
                pkEndNewInfo = pkEndNewInfos;
                break;
            default:
                NIASSERT(false);
            }
            uiChannelType++;
        }

        if (pkNewInfo >= pkEndNewInfos)
        {
            // All new entries have been filled.
            NIASSERT(pkNewInfo == pkEndNewInfos);
            break;
        }

        // Find the matching AVObject.
        NIASSERT(!pkNewInfo->m_pkInterpCtlr);
        NIASSERT(pkNewInfo->m_sLOD == LODSKIPINTERP);
        unsigned short usIndex = pusIndexList[pkNewInfo - m_pkInterpCtlrInfos];
        NIASSERT(usIndex < pkPoseBinding->GetNumBindInfos());
        NiPoseBinding::BindInfo& kBindInfo = pkBindInfos[usIndex];
        NiAVObject* pkAVObj = pkObjectPalette->GetAVObject(
            kBindInfo.m_kAVObjectName);
        if (!pkAVObj)
        {
            NILOG("NiMultiTargetPoseHandler::AddInterpControllerInfos "
                "failed to find target with the following identifiers:\n\t"
                "m_kAVObjectName\t%s\n",
                (const char*)kBindInfo.m_kAVObjectName);
            continue;
        }

        // Check if the binding is associated with a transform controller.
        // Transform attributes are handled internally as a special, 
        // optimized case.
        if (kBindInfo.m_kCtlrType.Exists() && 
            kBindInfo.m_kCtlrType == 
            NiAnimationConstants::GetTransformCtlrType())
        {
            NiTransformControllerPtr spTransCtlr = NiGetController(
                NiTransformController, pkAVObj);
            if (spTransCtlr)
            {
                // Remove the transform controller since transform attributes
                // are handled internally as a special, optimized case.
                pkAVObj->RemoveController(spTransCtlr);
                bNeedToUpdateSceneGraphFlags = true;
            }

            // Initialize the interp controller info.
            pkNewInfo->m_sLOD = kBindInfo.m_sLOD;
            pkNewInfo->m_usInterpolatorIndex = 
                NiInterpController::INVALID_INDEX;  // indicates a transform
            pkNewInfo->m_pkAVObject = pkAVObj;
            // Set override flag so scene graph world transforms are updated.
            pkAVObj->SetSelectiveUpdateTransformsOverride(true);
            continue;
        }

        // Search for the associated interp controller.
        NiInterpController* pkInterpCtlr = NULL;

        // Retrieve the property target, if specified.
        NiObjectNET* pkTarget = pkAVObj;
        if (kBindInfo.m_kPropertyType.Exists())
        {
            NiTListIterator kIter = 
                pkAVObj->GetPropertyList().GetHeadPos();
            while (kIter)
            {
                NiProperty* pkProp = pkAVObj->GetPropertyList().GetNext(
                    kIter);
                if (strcmp(pkProp->GetRTTI()->GetName(),
                    kBindInfo.m_kPropertyType) == 0)
                {
                    pkTarget = pkProp;
                    break;
                }
            }
            if (!pkTarget)
            {
                NILOG("NiMultiTargetPoseHandler::"
                    "AddInterpControllerInfos failed to find target "
                    "with the following identifiers:\n\t"
                    "m_kAVObjectName\t%s\n\tm_kPropertyType\t%s\n",
                    (const char*)kBindInfo.m_kAVObjectName, 
                    (const char*)kBindInfo.m_kPropertyType);
                continue;
            }
        }

        // Retrieve the associated controller.
        NiTimeController* pkTargetCtlr = pkTarget->GetControllers();
        while (pkTargetCtlr)
        {
            if (strcmp(pkTargetCtlr->GetRTTI()->GetName(), 
                kBindInfo.m_kCtlrType) == 0)
            {
                NIASSERT(NiIsKindOf(NiInterpController, pkTargetCtlr));
                const char* pcTargetCtlrID = 
                    ((NiInterpController*)pkTargetCtlr)->GetCtlrID();
                if ((!pcTargetCtlrID && !kBindInfo.m_kCtlrID.Exists()) ||
                    (pcTargetCtlrID && kBindInfo.m_kCtlrID.Exists() &&
                        strcmp(pcTargetCtlrID, kBindInfo.m_kCtlrID) == 0))
                {
                    pkInterpCtlr = (NiInterpController*)pkTargetCtlr;
                    break;
                }
            }
            pkTargetCtlr = pkTargetCtlr->GetNext();
        }

        if (!pkInterpCtlr)
        {
            NILOG("NiMultiTargetPoseHandler::AddInterpControllerInfos "
                "failed to find target with the following "
                "identifiers:\n\tm_kAVObjectName\t%s\n\tm_kPropertyType"
                "\t%s\n\tm_kCtlrType\t\t%s\n\tm_kCtlrID\t\t\t%s\n",
                (const char*)kBindInfo.m_kAVObjectName, 
                (const char*)kBindInfo.m_kPropertyType, 
                (const char*)kBindInfo.m_kCtlrType, 
                (const char*)kBindInfo.m_kCtlrID);
            continue;
        }

        // Retrieve the interpolator index associated with the evaluator ID.
        unsigned short usInterpIdx = 
            pkInterpCtlr->GetInterpolatorIndex(kBindInfo.m_kEvaluatorID);
        if (usInterpIdx == NiInterpController::INVALID_INDEX)
        {
            NILOG("NiMultiTargetPoseHandler::AddInterpControllerInfos "
                "failed to find target with the following identifiers:\n\t"
                "m_kAVObjectName\t%s\n\tm_kPropertyType\t%s\n\tm_kCtlrType"
                "\t\t%s\n\tm_kCtlrID\t\t\t%s\n\tm_kEvaluatorID\t%s\n",
                (const char*)kBindInfo.m_kAVObjectName, 
                (const char*)kBindInfo.m_kPropertyType, 
                (const char*)kBindInfo.m_kCtlrType, 
                (const char*)kBindInfo.m_kCtlrID, 
                (const char*)kBindInfo.m_kEvaluatorID);
            continue;
        }

        // Initialize the interp controller info.
        pkInterpCtlr->SetManagerControlled(true);
        pkNewInfo->m_sLOD = kBindInfo.m_sLOD;
        pkNewInfo->m_usInterpolatorIndex = usInterpIdx;
        pkNewInfo->m_pkInterpCtlr = pkInterpCtlr;
    }

    // Update the start indices for each data type.
    m_uiNumInterpCtlrInfos = uiNumTotalBindings;
    NIASSERT(m_usColorStartIndex == 0);
    m_usBoolStartIndex =
        (unsigned short)pkPoseBinding->GetNumColorBindings() + 
        m_usColorStartIndex;
    m_usFloatStartIndex =
        (unsigned short)pkPoseBinding->GetNumBoolBindings() + 
        m_usBoolStartIndex;
    m_usPoint3StartIndex = 
        (unsigned short)pkPoseBinding->GetNumFloatBindings() + 
        m_usFloatStartIndex;
    m_usRotStartIndex = 
        (unsigned short)pkPoseBinding->GetNumPoint3Bindings() + 
        m_usPoint3StartIndex;
    m_usReferencedStartIndex = 
        (unsigned short)pkPoseBinding->GetNumRotBindings() + 
        m_usRotStartIndex;
    NIASSERT(m_uiNumInterpCtlrInfos == m_usReferencedStartIndex +
        pkPoseBinding->GetNumReferencedBindings());

    // Update the SelectiveUpdate flags if any controllers were removed.
    if (bNeedToUpdateSceneGraphFlags)
    {
        NIASSERT(m_pkOwner->GetTarget());
        NIASSERT(NiIsKindOf(NiAVObject, m_pkOwner->GetTarget()));
        bool bSelectiveUpdate, bRigid;
        ((NiAVObject*)m_pkOwner->GetTarget())->SetSelectiveUpdateFlags(
            bSelectiveUpdate, true, bRigid);
    }
}
//---------------------------------------------------------------------------
void NiMultiTargetPoseHandler::RemoveAllInterpControllerInfos()
{
    for (unsigned int ui = 0; ui < m_uiNumInterpCtlrInfos; ui++)
    {
        m_pkInterpCtlrInfos[ui].ClearValues();
    }

    NiDelete[] m_pkInterpCtlrInfos;
    m_pkInterpCtlrInfos = NULL;

    m_uiNumInterpCtlrInfos = 0;
    m_usColorStartIndex = 0;
    m_usBoolStartIndex = 0;
    m_usFloatStartIndex = 0;
    m_usPoint3StartIndex = 0;
    m_usRotStartIndex = 0; 
    m_usReferencedStartIndex = 0;
}
//---------------------------------------------------------------------------
