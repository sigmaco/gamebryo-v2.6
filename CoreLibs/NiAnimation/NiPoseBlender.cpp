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

#include "NiPoseBlender.h"
#include "NiControllerSequence.h"
#include "NiControllerManager.h"

NiImplementRTTI(NiPoseBlender,NiObject);

//---------------------------------------------------------------------------
NiPoseBlender::NiPoseBlender(NiControllerManager* pkOwner) : 
    m_pkOwner(NULL), m_spFinalPoseBuffer(NULL), m_uiNumLODs(0), 
    m_psLODs(NULL), m_uiActivePoseSize(0), m_pkActivePoseList(NULL),
    m_uiNumBitPatterns(0), m_uiFlagWeightSize(0), m_puiFlagWeightArray(NULL)
{
    Init(pkOwner);
}
//---------------------------------------------------------------------------
NiPoseBlender::~NiPoseBlender()
{
    Shutdown();
}
//---------------------------------------------------------------------------
bool NiPoseBlender::Update(unsigned int uiNumContributingSequences, int iLOD, 
    NiPoseBuffer*& pkFinalPoseBuffer, 
    NiControllerSequence*& pkSoleSequence)
{
    // This function computes the prioritized weighted blend of multiple 
    // active sequences playing on the owning controller manager. 
    // 
    // uiNumContributingSequences specifies the number of sequences in the
    // owner's NiControllerSequence list that should be processed. This may be
    // less than the size of the list if the sequences at the tail of the list
    // make no contribution (due to priority, weight, and animation state) to 
    // the final result.
    //
    // iLOD specifies the current level of detail for the controller manager.
    // As an optimization, the pose blender does not blend data items which 
    // are hidden due to the specified level of detail. 
    //
    // The pose buffer containing the final blended result is returned via 
    // pkFinalPoseBuffer. If a single sequence contributes to the final result,
    // the contributing sequence will be returned via pkSoleSequence. In this 
    // case, pkFinalPoseBuffer will point to the pose buffer associated with 
    // pkSoleSequence as opposed to m_spFinalPoseBuffer, the pose buffer 
    // maintained by the pose blender. 
    //
    // This function returns true if at least one sequence contributes to the 
    // final result. false is returned when all of the sequences are hidden 
    // (say, due to invalid data in all pose buffers).

    NIASSERT(uiNumContributingSequences > 1);
    NIASSERT(uiNumContributingSequences <= m_pkOwner->GetSequenceCount());
    NIASSERT(m_pkOwner);
    NIASSERT(m_pkOwner->GetPoseBinding());

    // Limit the number of contributing sequences to simplify the algorithm.
    // It's unlikely that 30+ sequences contribute to the final animation.
    // Extra sequences at the tail of the list (i.e. the least significant)
    // will be ignored.
    if (uiNumContributingSequences > MAXBLENDEDSEQUENCES)
    {
        NILOG("NiPoseBlender::Update attempted to blend %d sequences. "
            "Only the first %d will be blended.\n", 
            uiNumContributingSequences, MAXBLENDEDSEQUENCES);
        uiNumContributingSequences = MAXBLENDEDSEQUENCES;
    }

    // Ensure the flag weight array is large enough.
    unsigned int uiRequiredSize = uiNumContributingSequences * MAXBITPATTERNS;
    if (uiRequiredSize > m_uiFlagWeightSize)
    {
        NiFree(m_puiFlagWeightArray);
        m_puiFlagWeightArray = (unsigned int*)NiMalloc(
            uiRequiredSize * sizeof(unsigned int));
        m_uiFlagWeightSize = uiRequiredSize;
    }

    // Ensure LOD list is the correct size.
    NiPoseBinding* pkPoseBinding = m_pkOwner->GetPoseBinding();
    unsigned int uiNumTotalBindings = pkPoseBinding->GetNumTotalBindings();
    if (uiNumTotalBindings > m_uiNumLODs)
    {
        AddNewLODsFromPoseBinding();
    }
    NIASSERT(m_uiNumLODs == uiNumTotalBindings);

    // Ensure the active pose list is large enough.
    if (uiNumContributingSequences > m_uiActivePoseSize)
    {
        NiFree(m_pkActivePoseList);
        m_pkActivePoseList = (ActivePose*)NiMalloc(
            uiNumContributingSequences * sizeof(ActivePose));
        m_uiActivePoseSize = uiNumContributingSequences;
    }

    // Initialize the active pose list.
    for (unsigned int ui = 0; ui < uiNumContributingSequences; ui++)
    {
        NiControllerSequence* pkSequence = m_pkOwner->GetSequenceAt(ui);
        NIASSERT(pkSequence);
        NiPoseBuffer* pkPoseBuffer = pkSequence->GetPoseBuffer();
        NIASSERT(pkPoseBuffer);
        NIASSERT(pkPoseBuffer->GetPoseBinding());
        NIASSERT(pkPoseBuffer->GetPoseBinding() == pkPoseBinding);
        NIASSERT(pkPoseBuffer->GetNumTotalItems() == 
            pkPoseBuffer->GetPoseBinding()->GetNumTotalBindings());

        ActivePose& kActivePose = m_pkActivePoseList[ui];
        kActivePose.m_pkPoseBuffer = pkPoseBuffer;
        kActivePose.m_puiFlagWeight = pkPoseBuffer->GetFlagWeightArray();
        kActivePose.m_bHasContributingItems = false;
        kActivePose.m_iPriority = pkSequence->GetPriority();
        kActivePose.m_fWeight = pkSequence->GetSpinnerScaledWeight();
        kActivePose.m_pkSequence = pkSequence;
    }

    // Determine the finalized weight for each entry in the 
    // contributing pose buffers.
    m_uiNumBitPatterns = 0;
    short sLOD = (short)iLOD;
    short* psLOD = m_psLODs;
    for (unsigned int ui = 0; ui < m_uiNumLODs; ui++, psLOD++)
    {
        if (sLOD <= *psLOD)
        {
            // Data entry is visible at the specified LOD.

            // Create bit pattern of valid items.
            unsigned int uiBitPattern = 0;
            unsigned int uiCurBit = 1 << (uiNumContributingSequences - 1);
            for (unsigned int uj = 0; uj < uiNumContributingSequences; uj++)
            {
                if (NiPoseBuffer::IsItemValid(
                    *m_pkActivePoseList[uj].m_puiFlagWeight))
                {
                    uiBitPattern |= uiCurBit;
                }
                uiCurBit >>= 1;
            }

            // Check if this matches a known pattern.
            unsigned int uk = 0;
            for (; uk < m_uiNumBitPatterns; uk++)
            {
                if ((uiBitPattern & m_auiBitMaskList[uk]) == 
                    m_auiBitPatternList[uk])
                {
                    // Match found. Copy associated finalized weights.
                    unsigned int* puiFlagWeight = m_puiFlagWeightArray + 
                        uk * uiNumContributingSequences;
                    for (unsigned int uj = 0; uj < uiNumContributingSequences;
                        uj++)
                    {
                        if (NiPoseBuffer::IsItemValid(
                            *m_pkActivePoseList[uj].m_puiFlagWeight))
                        {
                            *m_pkActivePoseList[uj].m_puiFlagWeight = 
                                *puiFlagWeight;
                        }
                        puiFlagWeight++;
                    }
                    break;
                }
            }

            // Compute the finalized weights, if needed.
            if (uk >= m_uiNumBitPatterns)
            {
                int iHighPriority = -INT_MAX;
                int iSecondPriority = -INT_MAX;
                int iHighCount = 0;
                int iSecondCount = 0;
                float fTotalHighWeight = 0.0f;
                float fTotalSecondWeight = 0.0f;
                float fMaxHighEaseSpinner = 0.0f;
                unsigned int uiNonCulledValidItems = 0;
                unsigned int uiCurrentBit =
                    1 << (uiNumContributingSequences - 1);
                for (unsigned int uj = 0; uj < uiNumContributingSequences; 
                    uj++)
                {
                    ActivePose& kActivePose = m_pkActivePoseList[uj];
                    if ((uiBitPattern & uiCurrentBit) == 0)
                    {
                        // Item is invalid: no need to cull it.
                        NIASSERT(!NiPoseBuffer::IsItemValid(
                            *kActivePose.m_puiFlagWeight));
                        NIASSERT(*kActivePose.m_puiFlagWeight == 0);
                        uiCurrentBit >>= 1;
                        continue;
                    }

                    // Clear any cull flag from the last update.
                    // Overwrite flags and finalized weight.
                    *kActivePose.m_puiFlagWeight = 
                        (unsigned int)NiPoseBuffer::VALIDITEMFLAG;
                    uiNonCulledValidItems++;

                    // This code relies upon the sorted order of sequences
                    // from most to least importance as defined by 
                    // NiControllerSequence::IsMoreImportantThan().

                    if (iHighPriority == -INT_MAX)
                    {
                        iHighPriority = kActivePose.m_iPriority;
                        fMaxHighEaseSpinner = 
                            kActivePose.m_pkSequence->GetEaseSpinner();
                        NIASSERT(fMaxHighEaseSpinner >= 0.0f);
                        NIASSERT(fMaxHighEaseSpinner <= 1.0f);
                        fTotalHighWeight = kActivePose.m_fWeight;
                        iHighCount = 1;
                    }
                    else if (kActivePose.m_iPriority == iHighPriority)
                    {
                        fTotalHighWeight += kActivePose.m_fWeight;
                        iHighCount++;
                    }
                    else if (fMaxHighEaseSpinner < 1.0f &&
                        kActivePose.m_iPriority >= iSecondPriority)
                    {
                        NIASSERT(kActivePose.m_iPriority < iHighPriority);
                        iSecondPriority = kActivePose.m_iPriority;
                        fTotalSecondWeight += kActivePose.m_fWeight;
                        iSecondCount++;
                    }
                    else
                    {
                        // No other sequences contribute to the result.
                        NIASSERT(NiPoseBuffer::IsItemValid(
                            *kActivePose.m_puiFlagWeight));
                        *kActivePose.m_puiFlagWeight = (unsigned int)
                            (NiPoseBuffer::VALIDITEMFLAG |
                            NiPoseBuffer::CULLEDVALIDITEMFLAG);
                        uj++;

                        // Counted one too many.
                        uiNonCulledValidItems--;

                        // Cull remaining valid items.
                        while (uj < uiNumContributingSequences)
                        {
                            if (NiPoseBuffer::IsItemValid(
                                *m_pkActivePoseList[uj].m_puiFlagWeight))
                            {
                                *m_pkActivePoseList[uj].m_puiFlagWeight = 
                                    (unsigned int)
                                    (NiPoseBuffer::VALIDITEMFLAG |
                                    NiPoseBuffer::CULLEDVALIDITEMFLAG);
                            }
                            else
                            {
                                NIASSERT(
                                    *m_pkActivePoseList[uj].m_puiFlagWeight 
                                    == 0);
                            }
                            uj++;
                        }
                        break;
                    }

                    uiCurrentBit >>= 1;
                }

                // Compute normalization scaler for second priority sequences.
                float fSecondNormalizer = 0.0f;
                if (fTotalSecondWeight > 0.0f)
                {
                    fSecondNormalizer = 
                        (1.0f - fMaxHighEaseSpinner) / fTotalSecondWeight;
                }
                else
                {
                    if (iSecondCount == 0)
                    {
                        // No second priority sequences contribute to the 
                        // result.
                        // Force the high priority ease spinner to full value.
                        fMaxHighEaseSpinner = 1.0f;
                    }
                    else
                    {
                        // Normalize the weight across all sequences at
                        // this priority.
                        fSecondNormalizer = 
                            (1.0f - fMaxHighEaseSpinner) / (float)iSecondCount;
                    }
                }

                // Compute normalization scaler for high priority sequences.
                float fHighNormalizer = 0.0f;
                if (fTotalHighWeight > 0.0f)
                {
                    fHighNormalizer = 
                        fMaxHighEaseSpinner / fTotalHighWeight;
                }
                else
                {
                    if (iHighCount > 0)
                    {
                        // Normalize the weight across all sequences at
                        // this priority.
                        fHighNormalizer = 
                            fMaxHighEaseSpinner / (float)iHighCount;
                    }
                }

                // Keep tab of the remaining weight to avoid round-off errors.
                unsigned int uiRemainingWeight = 
                    NiPoseBuffer::FINALIZEDWEIGHTSCALER;
                unsigned int uiActivePoseIndex = 0;
                while (uiNonCulledValidItems > 0)
                {
                    NIASSERT(uiActivePoseIndex < uiNumContributingSequences);
                    ActivePose& kActivePose = 
                        m_pkActivePoseList[uiActivePoseIndex];
                    uiActivePoseIndex++;
                    if (NiPoseBuffer::IsItemValidAndNotCulled(
                        *kActivePose.m_puiFlagWeight))
                    {
                        unsigned int uiWeight = uiRemainingWeight;
                        uiNonCulledValidItems--;
                        if (uiNonCulledValidItems > 0)
                        {
                            float fWeight = kActivePose.m_fWeight;
                            if (kActivePose.m_iPriority == iHighPriority)
                            {
                                if (fTotalHighWeight == 0.0f)
                                {
                                    fWeight = fHighNormalizer;
                                }
                                else
                                {
                                    fWeight *= fHighNormalizer;
                                }
                            }
                            else
                            {
                                if (fTotalSecondWeight == 0.0f)
                                {
                                    fWeight = fSecondNormalizer;
                                }
                                else
                                {
                                    fWeight *= fSecondNormalizer;
                                }
                            }
                            NIASSERT(fWeight >= 0.0f && fWeight <= 1.0f);
                            uiWeight = (unsigned int)(fWeight * 
                                (float)NiPoseBuffer::FINALIZEDWEIGHTSCALER);
                            if (uiWeight > uiRemainingWeight)
                            {
                                NIASSERT(uiWeight - uiRemainingWeight < 4);
                                uiWeight = uiRemainingWeight;
                            }
                        }

                        if (uiWeight > 0)
                        {
                            // This item contributes to the final result.
                            kActivePose.m_bHasContributingItems = true;
                            *kActivePose.m_puiFlagWeight = 
                                NiPoseBuffer::VALIDITEMFLAG | uiWeight;
                        }
                        else
                        {
                            // The weight is zero. Cull this item.
                            NIASSERT(NiPoseBuffer::IsItemValid(
                                *kActivePose.m_puiFlagWeight));
                            *kActivePose.m_puiFlagWeight = (unsigned int)
                                (NiPoseBuffer::VALIDITEMFLAG |
                                NiPoseBuffer::CULLEDVALIDITEMFLAG);
                        }
                        NIASSERT(uiRemainingWeight >= uiWeight);
                        uiRemainingWeight -= uiWeight;
                    }
                }
                NIASSERT(uiRemainingWeight == 0 || 
                    uiRemainingWeight == NiPoseBuffer::FINALIZEDWEIGHTSCALER);

                // Save the new pattern and associated weights.
                if (m_uiNumBitPatterns < MAXBITPATTERNS)
                {
                    unsigned int uiBitMask = 0;
                    if (uiCurrentBit > 0)
                    {
                        uiBitMask = (uiCurrentBit << 1) - 1;
                    }
                    uiBitMask = ~uiBitMask;
                    m_auiBitMaskList[m_uiNumBitPatterns] = uiBitMask;
                    m_auiBitPatternList[m_uiNumBitPatterns] = 
                        uiBitPattern & uiBitMask;
                    unsigned int* puiFlagWeight = m_puiFlagWeightArray + 
                        m_uiNumBitPatterns * uiNumContributingSequences;
                    uiCurrentBit = 1 << (uiNumContributingSequences - 1);
                    for (unsigned int uj = 0; uj < uiNumContributingSequences; 
                        uj++)
                    {
                        if ((uiBitMask & uiCurrentBit) != 0)
                        {
                            // Save the weight of visible sequences.
                            *puiFlagWeight =
                                *m_pkActivePoseList[uj].m_puiFlagWeight;
                        }
                        else
                        {
                            // Treat hidden sequences as valid, but culled.
                            *puiFlagWeight = (unsigned int)
                                (NiPoseBuffer::VALIDITEMFLAG |
                                NiPoseBuffer::CULLEDVALIDITEMFLAG);
                        }
                        uiCurrentBit >>= 1;
                        puiFlagWeight++;
                    }
                    m_uiNumBitPatterns++;
                }
            }

            // Advance the flag weight pointers.
            for (unsigned int uj = 0; uj < uiNumContributingSequences; uj++)
            {
                m_pkActivePoseList[uj].m_puiFlagWeight++;
            }
        }
        else
        {
            // Data entry is hidden at the specified LOD.
            for (unsigned int uj = 0; uj < uiNumContributingSequences; uj++)
            {
                // Cull valid items.
                if (NiPoseBuffer::IsItemValid(
                    *m_pkActivePoseList[uj].m_puiFlagWeight))
                {
                    *m_pkActivePoseList[uj].m_puiFlagWeight = (unsigned int)
                        (NiPoseBuffer::VALIDITEMFLAG |
                        NiPoseBuffer::CULLEDVALIDITEMFLAG);
                }
                else
                {
                    NIASSERT(*m_pkActivePoseList[uj].m_puiFlagWeight == 0);
                }

                // Advance the flag weight pointer.
                m_pkActivePoseList[uj].m_puiFlagWeight++;
            }
        }
    }

    // Now that the finalized weights have been computed, 
    // check if there's less than two contributing sequences.
    ActivePose* pkContributingActivePose = m_pkActivePoseList;
    ActivePose* pkActivePose = m_pkActivePoseList;
    ActivePose* pkEndActivePose = m_pkActivePoseList + 
        uiNumContributingSequences;
    while (pkActivePose < pkEndActivePose)
    {
        if (pkActivePose->m_bHasContributingItems)
        {
            if (pkActivePose != pkContributingActivePose)
            {
                *pkContributingActivePose = *pkActivePose;
            }
            pkContributingActivePose++;
        }
        pkActivePose++;
    }

    uiNumContributingSequences = 
        (unsigned int)(pkContributingActivePose - m_pkActivePoseList);
    if (uiNumContributingSequences == 0)
    {
        // No sequences contribute to the final result.
        // Thus, there's no pose buffer to return.
        pkSoleSequence = NULL;
        pkFinalPoseBuffer = NULL;
        return false;
    }

    if (uiNumContributingSequences == 1)
    {
        // Just one sequence contributes to the final result.
        // Return the sequence and associated pose buffer.
        NIASSERT(m_pkActivePoseList[0].m_bHasContributingItems);
        pkSoleSequence = m_pkActivePoseList[0].m_pkSequence;
        pkFinalPoseBuffer = m_pkActivePoseList[0].m_pkPoseBuffer;
        return true;
    }

    // Multiple sequences contribute to the final result.
    // Compute the blended result in the final pose buffer.

    // Ensure the final pose buffer is the correct size.
    pkFinalPoseBuffer = m_spFinalPoseBuffer;
    NIASSERT(pkFinalPoseBuffer);
    NIASSERT(pkFinalPoseBuffer->GetPoseBinding() == 
        m_pkOwner->GetPoseBinding());
    if (!pkFinalPoseBuffer->AddNewItemsFromPoseBinding(false, false))
    {
        // Invalidate all items in the final pose buffer.
        pkFinalPoseBuffer->InvalidateAllItems();
    }

    // Reset the flag weight pointers to the start of their lists.
    pkActivePose = m_pkActivePoseList;
    pkEndActivePose = m_pkActivePoseList + uiNumContributingSequences;
    while (pkActivePose < pkEndActivePose)
    {
        pkActivePose->m_puiFlagWeight = 
            pkActivePose->m_pkPoseBuffer->GetFlagWeightArray();
        pkActivePose++;
    }

    // Blend the color items.
    unsigned int uiNumColors = pkFinalPoseBuffer->GetNumColors();
    for (unsigned int ui = 0; ui < uiNumColors; ui++)
    {
        bool bValid = false;
        NiColorA kFinalColor(NiColorA::BLACK);
        NiPoseBufferHandle kPBHandle(PBCOLORCHANNEL, (unsigned short)ui);
        pkActivePose = m_pkActivePoseList;
        while (pkActivePose < pkEndActivePose)
        {
            NiColorA kColor;
            if (pkActivePose->m_pkPoseBuffer->GetColorIfNotCulled(
                kPBHandle, kColor))
            {
                bValid = true;
                float fWeight = NiPoseBuffer::GetItemFinalizedWeight(
                    *pkActivePose->m_puiFlagWeight);
                kFinalColor += kColor * fWeight;
            }
            pkActivePose->m_puiFlagWeight++;
            pkActivePose++;
        }

        if (bValid)
        {
            pkFinalPoseBuffer->SetColorValid(kPBHandle, true);
            pkFinalPoseBuffer->SetColor(kPBHandle, kFinalColor);
        }
    }

    // Blend the bool items.
    unsigned int uiNumBools = pkFinalPoseBuffer->GetNumBools();
    for (unsigned int ui = 0; ui < uiNumBools; ui++)
    {
        bool bValid = false;
        float fFinalBool = 0.0f;
        NiPoseBufferHandle kPBHandle(PBBOOLCHANNEL, (unsigned short)ui);
        pkActivePose = m_pkActivePoseList;
        while (pkActivePose < pkEndActivePose)
        {
            float fBool;
            if (pkActivePose->m_pkPoseBuffer->GetBoolIfNotCulled(
                kPBHandle, fBool))
            {
                bValid = true;
                float fWeight = NiPoseBuffer::GetItemFinalizedWeight(
                    *pkActivePose->m_puiFlagWeight);
                fFinalBool += fBool * fWeight;
            }
            pkActivePose->m_puiFlagWeight++;
            pkActivePose++;
        }

        if (bValid)
        {
            pkFinalPoseBuffer->SetBoolValid(kPBHandle, true);
            pkFinalPoseBuffer->SetBool(kPBHandle, fFinalBool);
        }
    }

    // Blend the float items.
    unsigned int uiNumFloats = pkFinalPoseBuffer->GetNumFloats();
    for (unsigned int ui = 0; ui < uiNumFloats; ui++)
    {
        bool bValid = false;
        float fFinalFloat = 0.0f;
        NiPoseBufferHandle kPBHandle(PBFLOATCHANNEL, (unsigned short)ui);
        pkActivePose = m_pkActivePoseList;
        while (pkActivePose < pkEndActivePose)
        {
            float fFloat;
            if (pkActivePose->m_pkPoseBuffer->GetFloatIfNotCulled(
                kPBHandle, fFloat))
            {
                bValid = true;
                float fWeight = NiPoseBuffer::GetItemFinalizedWeight(
                    *pkActivePose->m_puiFlagWeight);
                fFinalFloat += fFloat * fWeight;
            }
            pkActivePose->m_puiFlagWeight++;
            pkActivePose++;
        }

        if (bValid)
        {
            pkFinalPoseBuffer->SetFloatValid(kPBHandle, true);
            pkFinalPoseBuffer->SetFloat(kPBHandle, fFinalFloat);
        }
    }

    // Blend the point3 items.
    unsigned int uiNumPoint3s = pkFinalPoseBuffer->GetNumPoint3s();
    for (unsigned int ui = 0; ui < uiNumPoint3s; ui++)
    {
        bool bValid = false;
        NiPoint3 kFinalPoint3(NiPoint3::ZERO);
        NiPoseBufferHandle kPBHandle(PBPOINT3CHANNEL, (unsigned short)ui);
        pkActivePose = m_pkActivePoseList;
        while (pkActivePose < pkEndActivePose)
        {
            NiPoint3 kPoint3;
            if (pkActivePose->m_pkPoseBuffer->GetPoint3IfNotCulled(
                kPBHandle, kPoint3))
            {
                bValid = true;
                float fWeight = NiPoseBuffer::GetItemFinalizedWeight(
                    *pkActivePose->m_puiFlagWeight);
                kFinalPoint3 += kPoint3 * fWeight;
            }
            pkActivePose->m_puiFlagWeight++;
            pkActivePose++;
        }

        if (bValid)
        {
            pkFinalPoseBuffer->SetPoint3Valid(kPBHandle, true);
            pkFinalPoseBuffer->SetPoint3(kPBHandle, kFinalPoint3);
        }
    }

    // Blend the rot items.
    //
    // Generally speaking, when you want to blend between quaternions,
    // you use slerp. There are three qualities that any rotation
    // interpolation needs to satisfy.
    // 1) Commutativity
    // 2) Constant Velocity
    // 3) Minimal Torque (ie shortest path on great sphere)
    // Slerp has 2 and 3. For blending like this, we really want 1.
    // Therefore, we will use normalized lerp. This algorithm has the
    // properties of 1 and 3. 
    unsigned int uiNumRots = pkFinalPoseBuffer->GetNumRots();
    for (unsigned int ui = 0; ui < uiNumRots; ui++)
    {
        bool bValid = false;
        NiQuaternion kFinalRot(0.0f, 0.0f, 0.0f, 0.0f);
        NiPoseBufferHandle kPBHandle(PBROTCHANNEL, (unsigned short)ui);
        pkActivePose = m_pkActivePoseList;
        while (pkActivePose < pkEndActivePose)
        {
            NiQuaternion kRot;
            if (pkActivePose->m_pkPoseBuffer->GetRotIfNotCulled(
                kPBHandle, kRot))
            {
                // Dot only represents the angle between quats 
                // when they are unitized.
                if (bValid)
                {
                    float fCos = NiQuaternion::Dot(kFinalRot, kRot);

                    // If the angle is negative, we need to 
                    // invert the quat to get the best path.
                    if (fCos < 0.0f)
                    {
                        kRot = -kRot;
                    }
                }
                bValid = true;

                float fWeight = NiPoseBuffer::GetItemFinalizedWeight(
                    *pkActivePose->m_puiFlagWeight);

                // Accumulate the total weighted values.
                //
                // Multiply in the weights to the quaternions.
                // Note that this makes them non-rotations.
                kFinalRot.SetValues(
                    kRot.GetW() * fWeight + kFinalRot.GetW(), 
                    kRot.GetX() * fWeight + kFinalRot.GetX(), 
                    kRot.GetY() * fWeight + kFinalRot.GetY(),
                    kRot.GetZ() * fWeight + kFinalRot.GetZ());
            }
            pkActivePose->m_puiFlagWeight++;
            pkActivePose++;
        }

        if (bValid)
        {
            // Need to re-normalize the quaternion.
            kFinalRot.Normalize();
            pkFinalPoseBuffer->SetRotValid(kPBHandle, true);
            pkFinalPoseBuffer->SetRot(kPBHandle, kFinalRot);
        }
    }

    // Set the highest weighted referenced item for each entry.
    unsigned int uiNumReferencedItems = 
        pkFinalPoseBuffer->GetNumReferencedItems();
    for (unsigned int ui = 0; ui < uiNumReferencedItems; ui++)
    {
        float fMaxWeight = 0.0f;
        NiPoseBuffer::ReferencedItem kFinalReferencedItem;
        kFinalReferencedItem.m_pkControllerSequence = NULL;
        kFinalReferencedItem.m_pkReferencedEvaluator = NULL;
        NiPoseBufferHandle kPBHandle(PBREFERENCEDCHANNEL, (unsigned short)ui);
        pkActivePose = m_pkActivePoseList;
        while (pkActivePose < pkEndActivePose)
        {
            NiPoseBuffer::ReferencedItem kReferencedItem;
            if (pkActivePose->m_pkPoseBuffer->GetReferencedItemIfNotCulled(
                kPBHandle, kReferencedItem))
            {
                float fWeight = NiPoseBuffer::GetItemFinalizedWeight(
                    *pkActivePose->m_puiFlagWeight);
                if (fWeight > fMaxWeight)
                {
                    fMaxWeight = fWeight;
                    kFinalReferencedItem = kReferencedItem;
                }
            }
            pkActivePose->m_puiFlagWeight++;
            pkActivePose++;
        }

        if (fMaxWeight > 0.0f)
        {
            pkFinalPoseBuffer->SetReferencedItemValid(kPBHandle, true);
            pkFinalPoseBuffer->SetReferencedItem(kPBHandle, 
                kFinalReferencedItem);
        }
    }

    // More than one sequence contributes to the result.
    pkSoleSequence = NULL;
    return true;
}
//---------------------------------------------------------------------------
void NiPoseBlender::Init(NiControllerManager* pkOwner)
{
    NIASSERT(pkOwner && !m_pkOwner);
    NIASSERT(pkOwner->GetPoseBinding());
    NIASSERT(!m_spFinalPoseBuffer);
    NIASSERT(!m_psLODs && m_uiNumLODs == 0);

    m_pkOwner = pkOwner;

    NiPoseBinding* pkPoseBinding = pkOwner->GetPoseBinding();
    m_spFinalPoseBuffer = NiNew NiPoseBuffer(pkPoseBinding);

    unsigned int uiNumTotalBindings = pkPoseBinding->GetNumTotalBindings();
    if (uiNumTotalBindings > 0)
    {
        AddNewLODsFromPoseBinding();
    }
}
//---------------------------------------------------------------------------
void NiPoseBlender::Shutdown()
{
    NiFree(m_pkActivePoseList);
    m_pkActivePoseList = NULL;
    m_uiActivePoseSize = 0;

    NiFree(m_psLODs);
    m_psLODs = NULL;
    m_uiNumLODs = 0;

    NiFree(m_puiFlagWeightArray);
    m_puiFlagWeightArray = NULL;
    m_uiFlagWeightSize = 0;
    m_uiNumBitPatterns = 0;

    m_spFinalPoseBuffer = NULL;
    m_pkOwner = NULL;
}
//---------------------------------------------------------------------------
void NiPoseBlender::AddNewLODsFromPoseBinding()
{
    NIASSERT(m_pkOwner);
    NIASSERT(m_pkOwner->GetPoseBinding());

    // Delete existing data.
    NiFree(m_psLODs);

    // Determine new size of array.
    NiPoseBinding* pkPoseBinding = m_pkOwner->GetPoseBinding();
    NIASSERT(m_uiNumLODs < pkPoseBinding->GetNumTotalBindings());
    m_uiNumLODs = pkPoseBinding->GetNumTotalBindings();

    // Allocate larger array.
    m_psLODs = (short*)NiMalloc(m_uiNumLODs * sizeof(short));

    // Fill in the new array.
    NiPoseBinding::BindInfo* pkBindInfos = pkPoseBinding->GetBindInfos();
    unsigned short* pusIndexList = pkPoseBinding->GetBindInfoIndexList();
    for (unsigned int ui = 0; ui < m_uiNumLODs; ui++)
    {
        unsigned short usIndex = pusIndexList[ui];
        NIASSERT(usIndex < pkPoseBinding->GetNumBindInfos());
        m_psLODs[ui] = pkBindInfos[usIndex].m_sLOD;
    }
}
//---------------------------------------------------------------------------
