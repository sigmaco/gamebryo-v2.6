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

using System;
using System.Collections;
using NiManagedToolInterface;


namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for Events.
    /// </summary>
    public class AppEvents
    {
        public AppEvents(){}

        #region Delegate Definitions
        public delegate void EventHandler_TransitionEditLoaded();
        public delegate void 
            EventHandler_ProjectViewSelectionChanged(ProjectTreeNode kNode);
        public delegate void EventHandler_TransitionViewSelectionChanged(
            ArrayList kTransitionList);
        public delegate void EventHandler_TransitionViewSelectionCountChanged(
            int iCount);
        public delegate void EventHandler_ProjectViewNewFolder();        
        public delegate void EventHandler_FrameworkDataChanged();
        public delegate void EventHandler_KeystrokeBindingsChanged();
        public delegate void EventHandler_TransitionTableSettingsChanged();
        public delegate void EventHandler_ToggleSequence(uint uiSequenceID);
        public delegate void EventHandler_InteractiveSequence(bool bIdle,
            uint uiSequenceID);
        public delegate void EventHandler_RollbarSplitterMoved();
        public delegate void EventHandler_TransitionsPreviewSelected();
        public delegate void EventHandler_InteractivePreviewSelected();
        public delegate void EventHandler_HandleTypeConversion(
            MTransition.TransitionType kTransitionType);
        public delegate void EventHandler_PhysicsSettingsChanged();
        #endregion

        #region Event Raising and Handlers
        static public event 
            EventHandler_HandleTypeConversion HandleTypeConversion;
        static public void RaiseHandleTypeConversion(
            MTransition.TransitionType kTransitionType)
        {
            if (HandleTypeConversion != null)
                HandleTypeConversion(kTransitionType);
        }

        static public event 
            EventHandler_TransitionEditLoaded TransitionEditLoaded;
        static public void RaiseTransitionEditLoaded()
        {
            if (TransitionEditLoaded != null)
                TransitionEditLoaded();
        }

        static public event 
            EventHandler_FrameworkDataChanged FrameworkDataChanged;
        static public void RaiseFrameworkDataChanged()
        {
            if (FrameworkDataChanged != null)
                FrameworkDataChanged();
        }

        static public event 
            EventHandler_ProjectViewNewFolder ProjectViewNewFolder;
        static public void RaiseProjectViewNewFolder()
        {
            if (ProjectViewNewFolder != null)
                ProjectViewNewFolder();
        }

        static public event 
            EventHandler_ProjectViewSelectionChanged 
            ProjectViewSelectionChanged;   
        static public void RaiseProjectViewSelectionChanged(
            ProjectTreeNode kNode)
        {
            if (ProjectViewSelectionChanged != null)
                ProjectViewSelectionChanged(kNode);
        }

        static public event 
            EventHandler_TransitionViewSelectionChanged 
            TransitionViewSelectionChanged;   
        static public void RaiseTransitionViewSelectionChanged(
            ArrayList kTransitionList)
        {
            if (TransitionViewSelectionChanged != null)
                TransitionViewSelectionChanged(kTransitionList);
        }

        static public event EventHandler_TransitionViewSelectionCountChanged
            TransitionViewSelectionCountChanged;   
        static public void RaiseTransitionViewSelectionCountChanged(int iCount)
        {
            if (TransitionViewSelectionCountChanged != null)
                TransitionViewSelectionCountChanged(iCount);
        }

        static public event EventHandler_KeystrokeBindingsChanged
            KeystrokeBindingsChanged;   
        static public void RaiseKeystrokeBindingsChanged()
        {
            if (KeystrokeBindingsChanged != null)
                KeystrokeBindingsChanged();
        }

        static public event EventHandler_TransitionTableSettingsChanged
            TransitionTableSettingsChanged;   
        static public void RaiseTransitionTableSettingsChanged()
        {
            if (TransitionTableSettingsChanged != null)
                TransitionTableSettingsChanged();
        }

        static public event EventHandler_ToggleSequence ToggleSequence;
        static public void RaiseToggleSequence(uint uiSequenceID)
        {
            if (ToggleSequence != null)
                ToggleSequence(uiSequenceID);
        }

        static public event EventHandler_InteractiveSequence
            InteractiveSequence;
        static public void RaiseInteractiveSequence(bool bIdle,
            uint uiSequenceID)
        {
            if (InteractiveSequence != null)
                InteractiveSequence(bIdle, uiSequenceID);
        }

        static public event EventHandler_RollbarSplitterMoved
            RollbarSplitterMoved;
        static public void RaiseRollbarSplitterMoved()
        {
            if (RollbarSplitterMoved != null)
                RollbarSplitterMoved();
        }
        static public int GetRollbarSplitterMovedCount()
        {
            if (RollbarSplitterMoved != null)
            {
                return RollbarSplitterMoved.GetInvocationList().Length;
            }
            
            return 0;
        }

        static public event EventHandler_TransitionsPreviewSelected
            TransitionsPreviewSelected;
        static public void RaiseTransitionsPreviewSelected()
        {
            if (TransitionsPreviewSelected != null)
                TransitionsPreviewSelected();
        }

        static public event EventHandler_InteractivePreviewSelected
            InteractivePreviewSelected;
        static public void RaiseInteractivePreviewSelected()
        {
            if (InteractivePreviewSelected != null)
                InteractivePreviewSelected();
        }

        static public event EventHandler_PhysicsSettingsChanged
            PhysicsSettingsChanged;
        static public void RaisePhysicsSettingsChanged()
        {
            if (PhysicsSettingsChanged != null)
                PhysicsSettingsChanged();
        }

        #endregion
    }
}
