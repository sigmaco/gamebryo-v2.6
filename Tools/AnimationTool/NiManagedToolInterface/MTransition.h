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

namespace NiManagedToolInterface
{
    public __gc class MAnimation;
    public __gc class MSequence;
    public __gc class MTransition
    {
    public:
        __gc class MBlendPair
        {
        public:
            //Events.
            __value enum PropertyType
            {
                Prop_StartKey,
                Prop_TargetKey
            };
            __event void OnBlendPairChanged(PropertyType ePropChanged,
                MBlendPair* pkBlendPair);

            // Properties.
            __property String* get_StartKey();
            __property void set_StartKey(String* strStartKey);
            __property String* get_TargetKey();
            __property void set_TargetKey(String* strTargetKey);

            MBlendPair(NiKFMTool::Transition::BlendPair* pkBlendPair);
            void DeleteContents();

        protected:
            NiKFMTool::Transition::BlendPair* m_pkBlendPair;
        };

        __gc class MChainInfo
        {
        public:
            // Events.
            __value enum PropertyType
            {
                Prop_Duration
            };
            __event void OnChainInfoChanged(PropertyType ePropChanged,
                MChainInfo* pkChainInfo);

            // Properties.
            __property unsigned int get_SequenceID();
            __property float get_Duration();
            __property void set_Duration(float fDuration);

            static const float MAX_DURATION = NiKFMTool::MAX_DURATION;

            MChainInfo(NiKFMTool::Transition::ChainInfo* pkChainInfo);
            void DeleteContents();

        protected:
            NiKFMTool::Transition::ChainInfo* m_pkChainInfo;
        };

        // Events.
        __value enum PropertyType
        {
            Prop_SrcID,
            Prop_DesID,
            Prop_Type,
            Prop_Duration,
            Prop_BlendPairs,
            Prop_ChainInfo,
            Prop_ImmediateOffsetTextKey
        };

        // Properties.
        __value enum TransitionType
        {
            Trans_None,
            Trans_ImmediateBlend,
            Trans_DelayedBlend,
            Trans_Morph,
            Trans_CrossFade,
            Trans_Chain,
            Trans_DefaultSync,
            Trans_DefaultNonSync,
            Trans_MultipleSelected
        };
        __property unsigned int get_SrcID();
        __property unsigned int get_DesID();
        __property MSequence* get_Source();
        __property MSequence* get_Destination();
        __property TransitionType get_Type();
        __property TransitionType get_StoredType();
        __property void set_StoredType(TransitionType eType);
        __property float get_Duration();
        __property void set_Duration(float fDuration);
        __property MBlendPair* get_BlendPairs()[];
        __property MChainInfo* get_ChainInfo()[];
        __property String* get_ImmediateOffsetTextKey();
        __property void set_ImmediateOffsetTextKey(String* strTargetKey);
        __property float get_DefaultTransitionTime();
        __property void set_DefaultTransitionTime(float fTime);
        __property float get_DefaultPositionTime();
        __property void set_DefaultPositionTime(float fTime);

        MTransition(MAnimation* pkAnimation, unsigned int uiSrcID,
            unsigned int uiDesID, NiKFMTool::Transition* pkTransition);
        void DeleteContents();

        MTransition::MBlendPair* GetNextBlendPair(float fTime);

        void ReplaceChain_NoEvent(MSequence* kSequences[], 
            float fDurations __gc []);
        void ReplaceChain(MSequence* kSequences[], float fDurations __gc []);
        void ConvertToChain(MSequence* kSequences[], float fDurations __gc[]);

        bool DoesBlendPairExistForSrcKey(String* strSrcKey);
        int GetBlendPairIndex(String* strSrcKey, String* strDestKey);
        void AddBlendPair(String* strSrcKey, String* strDestKey);
        void RemoveBlendPair(String* strSrcKey, String* strDestKey);
        
        int GetChainInfoIndex(unsigned int uiSequenceID, int iOccurance);

        void RebuildChainInfoArray();

        // This function should only be used when changing IDs to reflect when
        // a sequence's ID has been changed. It does not change the underlying
        // NiKFMTool::Transition settings.
        void ChangeSequenceID(unsigned int uiOldID, unsigned int uiNewID);
        
        static NiKFMTool::TransitionType TranslateTransitionType(
            TransitionType eType);
        static TransitionType TranslateTransitionType(
            NiKFMTool::TransitionType eKFMType, MTransition* pkTransition);
        static System::Collections::ArrayList* GetValidTypesForTransition(
            MSequence* pkSrc, MSequence* pkDest);

        NiKFMTool::Transition* GetTransition();

    protected:
        void BuildBlendPairs();
        void BuildChainInfo();
        TransitionType TranslateTransitionType(
            NiKFMTool::TransitionType eKFMType);

        void PushChainInfo(unsigned int uiSequenceID, float fDuration);
        void PopChainInfo();

        void OnBlendPairChanged(MBlendPair::PropertyType ePropChanged,
            MBlendPair* pkBlendPair);
        void OnChainInfoChanged(MChainInfo::PropertyType ePropChanged,
            MChainInfo* pkChainInfo);

        unsigned int m_uiSrcID;
        unsigned int m_uiDesID;
        NiKFMTool::Transition* m_pkTransition;
        MBlendPair* m_aBlendPairs[];
        MChainInfo* m_aChainInfo[];
        MAnimation* m_pkAnimation;

        float m_fDefaultTransitionTime;
        float m_fDefaultPositionTime;
    };

    public __gc class MTransitionData
    {
    public:
        MTransitionData(MTransition* pkTransition,
            MSequence* pkSource, MSequence* pkDestination);

        __property MSequence* get_Source();
        __property MSequence* get_Destination();
        __property MTransition* get_Transition();
        __property void set_Transition(MTransition* pkTran);

    protected:
        MTransition* m_pkTransition;
        MSequence* m_pkSource;
        MSequence* m_pkDestination;
    };
}
