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

#include "MSequence.h"
using namespace System::Collections;

namespace NiManagedToolInterface
{
    public __gc class MAnimation;
    public __gc class MSequenceGroup
    {
        public:
            
        __gc class MSequenceInfo
        {
        public:
            // Events.
            __value enum PropertyType
            {
                Prop_Priority,
                Prop_Weight,
                Prop_EaseInTime,
                Prop_EaseOutTime,
                Prop_SynchronizeToSequence
            };
            
            // Properties.
            __property unsigned int get_SequenceID();
            __property MSequence* get_Sequence();
            __property int get_Priority();
            __property void set_Priority(int iPriority);
            __property float get_Weight();
            __property void set_Weight(float fWeight);
            __property float get_EaseInTime();
            __property void set_EaseInTime(float fEaseInTime);
            __property float get_EaseOutTime();
            __property void set_EaseOutTime(float fEaseOutTime);
            __property bool get_Synchronized();
            __property unsigned int get_SynchronizeToSequenceID();
            __property void set_SynchronizeToSequenceID(
                unsigned int uiSynchronizeToSequenceID);
            __property String* get_Name();
            __property bool get_Activated();
            __property bool get_DefaultActiveValue();
            __property void set_DefaultActiveValue(bool bDefaultActiveValue);

            virtual String* ToString();

            MSequenceInfo(MAnimation* pkAnimation,
                MSequenceGroup* pkOwner, 
                NiKFMTool::SequenceGroup::SequenceInfo* pkInfo);
            void DeleteContents();

            bool ActivateImmediate();
            bool Activate();
            bool Deactivate();
            void DeactivateImmediate();

            bool CanSyncTo(unsigned int uiSynchronizeToSequenceID);
            void ResetSyncSequence();

            bool m_bDefaultActiveValue;

        protected:
            NiKFMTool::SequenceGroup::SequenceInfo* m_pkInfo;
            MAnimation* m_pkAnimation;
            MSequenceGroup* m_pkOwner;
            bool m_bDisposed;
        };

        __value enum PropertyType
        {
            Prop_GroupID,
            Prop_Name,
            Prop_SequenceInfo
        };

        // Properties.
        __property unsigned int get_GroupID();
        __property void set_GroupID(unsigned int uiGroupID);
        __property String* get_Name();
        __property void set_Name(String* strName);
        __property MSequenceInfo* get_SequenceInfo()[];

        MSequenceInfo* GetAt(int i);
        virtual String* ToString();

        MSequenceGroup(MAnimation* pkAnimation, 
            NiKFMTool::SequenceGroup* pkGroup);
        void DeleteContents();
        bool AddSequence(unsigned int uiSequenceID, int iPriority, 
            float fWeight, float fEaseIn, float fEaseOut);
        bool AddSequence(unsigned int uiSequenceID);
        bool RemoveSequence(unsigned int uiSequenceID);
        int GetSequenceInfoIndex(unsigned int uiSequenceID);

        void ActivateAll();
        void DeactivateAll();

        void FillSequenceInfoArray();
        void FillSequenceInfoArray(NiTMap<unsigned int, bool>* 
            pkActiveMap);
    protected:
        NiTMap<unsigned int, bool>* CreateDefaultActiveValuesArray();
        void ClearInfo();
        NiKFMTool::SequenceGroup* m_pkGroup;
        MSequenceInfo* m_aSequenceInfo[];
        MAnimation* m_pkAnimation;
        bool m_bDisposed;

    };

    public __gc class MSequenceGroupEventArgs: public EventArgs
    {
    public:

        MSequenceGroupEventArgs(MSequenceGroup::PropertyType eType);
        MSequenceGroup::PropertyType Type;
    };

    public __gc class MSequenceGroupChangedSequenceInfoEventArgs
        : public MSequenceGroupEventArgs
    {
    public:
        MSequenceGroupChangedSequenceInfoEventArgs(bool bAdded, 
            unsigned int uiSequenceID, float fEaseIn, float fEaseOut);
        bool Added;
        unsigned int SequenceID;
        float EaseIn;
        float EaseOut;
    };

    public __gc class MSequenceGroupChangedIDEventArgs
        : public MSequenceGroupEventArgs
    {
    public:

        MSequenceGroupChangedIDEventArgs(unsigned int uiOldID,
            unsigned int uiNewID);
        unsigned int OldID;
        unsigned int NewID;
    };

    public __gc class MSequenceGroupChangedNameEventArgs
        : public MSequenceGroupEventArgs
    {
    public:

        MSequenceGroupChangedNameEventArgs(String* pcOldName,
            String* pcNewName);
        String* OldName;
        String* NewName;
    };
}
