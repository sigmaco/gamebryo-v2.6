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

#pragma once

#include "MTransition.h"

using namespace System::Collections;

namespace NiManagedToolInterface
{
    public __gc class MAnimation;
    public __gc class MSequence : public System::IComparable
    {
    public:
        int CompareTo(Object* pkObject);

        static const float INVALID_TIME = -NI_INFINITY;

        __gc class MTextKey
        {
        public:
            MTextKey(NiTextKey* pkKey);
            __property float get_Time();
            __property String* get_Text();
            
        protected:
            float m_fTime;
            String* m_strText;
        };

        __gc class MInvalidSequenceInfo
        {
        public:
            unsigned int SequenceID;
            String* Filename;
            String* OriginalFilename;
            int AnimIndex;
            String* SequenceName;
        };

        __gc class MEvaluatorInfo
        {
        public:
            __value enum KeyContent: unsigned char
            {
                FLOATKEY = NiAnimationKey::FLOATKEY,
                POSKEY = NiAnimationKey::POSKEY,
                ROTKEY = NiAnimationKey::ROTKEY,
                COLORKEY = NiAnimationKey::COLORKEY,
                TEXTKEY = NiAnimationKey::TEXTKEY,
                BOOLKEY = NiAnimationKey::BOOLKEY,
                NUMKEYCONTENTS  = NiAnimationKey::NUMKEYCONTENTS
            };

            __value enum KeyType : unsigned char
            {
                NOINTERP  = NiAnimationKey::NOINTERP,
                LINKEY  = NiAnimationKey::LINKEY,
                BEZKEY  = NiAnimationKey::BEZKEY,
                TCBKEY  = NiAnimationKey::TCBKEY,
                EULERKEY  = NiAnimationKey::EULERKEY,
                STEPKEY = NiAnimationKey::STEPKEY,
                NUMKEYTYPES = NiAnimationKey::NUMKEYTYPES
            };
            MEvaluatorInfo(NiEvaluator* pkEvaluator);
            // Properties.
            __property String* get_RTTI();
            __property String* get_AVObjectName();
            __property String* get_PropertyType();
            __property String* get_ControllerType();
            __property String* get_ControllerID();
            __property String* get_EvaluatorID();
                        
            unsigned short GetKeyChannelCount();
            unsigned int GetKeyCount(unsigned short usChannel);
            KeyType GetKeyType(unsigned short usChannel);
            KeyContent GetKeyContent(unsigned short usChannel);
            bool GetChannelPosed(unsigned short usChannel);
            unsigned int GetAllocatedSize(unsigned short usChannel);
            bool IsBSplineEvaluator();
            unsigned int GetDimension(unsigned short usChannel);
            unsigned int GetDegree(unsigned short usChannel);
            
        protected:
            NiEvaluator* m_pkEvaluator;
        };

        // Events.
        __value enum PropertyType : unsigned char
        {
            Prop_SequenceID,
            Prop_Name,
            Prop_Filename,
            Prop_AnimIndex,
            Prop_Activated
        };

        __value enum AnimState : unsigned char
        {
            INACTIVE = INACTIVE,
            ANIMATING  = ANIMATING,
            EASEIN = EASEIN,
            EASEOUT = EASEOUT,
            TRANSSOURCE = TRANSSOURCE,
            TRANSDEST = TRANSDEST,
            MORPHSOURCE  = MORPHSOURCE
        };
        
        // Properties.
        __property unsigned int get_SequenceID();
        __property void set_SequenceID(unsigned int uiSequenceID);
        __property String* get_Name();
        __property String* get_Filename();
        __property void set_Filename(String* strFilename);
        __property int get_AnimIndex();
        __property void set_AnimIndex(int iAnimIndex);
        __property float get_Duration();
        __property bool get_Loop();
        __property float get_Frequency();
        __property float get_LastScaledTime();
        __property MTransition* get_Transitions()[];
        __property MEvaluatorInfo* get_EvaluatorInfo()[];
        __property MTextKey* get_TextKeys()[];

        __property float get_DurationDivFreq();

        __property float get_DefaultPositionTime();
        __property void set_DefaultPositionTime(float fTime);

#ifdef NDL_DEBUG
        __property void set_Frequency(float fFrequency);
#endif
        AnimState GetAnimState();

        MSequence(MAnimation* pkAnimation, 
            NiKFMTool::Sequence* pkSequence,
            NiSequenceData* pkSeqData);
        void DeleteContents();

        void RebuildTransitionsArray();
        MTransition* GetTransition(unsigned int uiDesID);

        float GetKeyTimeAt(String* strKeyText);
        float GetKeyTimeAtDivFreq(String* strKeyText);
        float TimeDivFreq(float fTime);
        float TimeMultFreq(float fTime);

        // To support animation accumulation caching.
        void StoreAccum();
        void RestoreAccum();

        bool IsDeleted();
        virtual String* ToString();

        bool CanSyncTo(MSequence* pkMSequence);
        float FindCorrespondingMorphFrame(MSequence* pkPartnerMSequence,
            float fFrameTime);

        NiSequenceData* GetSequenceData();
        NiControllerSequence* GetControllerSequence();
        void SetControllerSequence(NiControllerSequence* pkControllerSequence);

        static ArrayList* GetSequenceNamesFromKF(String* pkFileName);

    protected:
        NiKFMTool::Sequence* m_pkKFMSequence;
        NiSequenceData* m_pkSeqData;       // hold a ref
        NiControllerSequence* m_pkControllerSequence;   // hold a ref

        MTransition* m_aTransitions[];

        NiQuatTransform* m_pkStoredAccum;
        MAnimation* m_pkAnimation;
        bool m_bDisposed;

        void FillArrays();
        void ClearArrays();
        MEvaluatorInfo* m_aEvaluatorInfo[];
        MTextKey* m_aTextKeys[];

        float m_fDefaultPositionTime;
    };
}
