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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using WeifenLuo.WinFormsUI;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// EditViewForm is the form for editing based on context.
    /// </summary>
    public class EditViewForm : DockContent
    {
        private System.Windows.Forms.UserControl UserControl_Context;
        private System.ComponentModel.Container m_kComponents = null;
        private bool[] m_abRollbarSequenceGroupCheckStates = null;
        private bool[] m_abRollbarSequenceCheckStates = null;
        private bool[] m_abRollbarInteractivePreviewCheckStates = null;
        private NodeType m_eCurrentNodeType;

        public EditViewForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if(m_kComponents != null)
                {
                    m_kComponents.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(EditViewForm));
            // 
            // EditViewForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(488, 238);
            this.HideOnClose = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "EditViewForm";
            this.Text = "EditView";
            this.Load += new System.EventHandler(this.EditViewForm_Load);
        }
        #endregion

        public void SetHandlers()
        {
            AppEvents.ProjectViewSelectionChanged += new AppEvents
                .EventHandler_ProjectViewSelectionChanged(OnSelectionChanged);

            AppEvents.TransitionViewSelectionChanged += new
                AnimationToolUI.AppEvents
                .EventHandler_TransitionViewSelectionChanged(
                OnTransitionChanged); 

            MFramework.Instance.Animation.OnNewKFM +=
                new MAnimation.__Delegate_OnNewKFM(KFMChanged);
            MFramework.Instance.Animation.OnKFMLoaded +=
                new MAnimation.__Delegate_OnKFMLoaded(KFMChanged);
        }

        public void KFMChanged()
        {
            if (UserControl_Context != null)
            {
                UserControl_Context.Dispose();
            }

            m_eCurrentNodeType = NodeType.Undefined;
            NoEditableProperties kNoEditableProperties =
                new NoEditableProperties();
            UserControl_Context = kNoEditableProperties;
            UserControl_Context.Dock = System.Windows.Forms.DockStyle.Fill;
            Controls.Clear(); // remove the previous control
            Controls.Add(UserControl_Context);
            UserControl_Context.Show();
        }

        private void OnTransitionChanged(ArrayList kTransitionList)
        {
            m_eCurrentNodeType = NodeType.Undefined;
            
            if (kTransitionList.Count != 0)
            {
                if (UserControl_Context != null)
                {
                    UserControl_Context.Dispose();
                    UserControl_Context = null;
                }

                InputManager.Instance.DisableInputHandling();
                InputManager.Instance.RemoveForm(this);

                TransitionEdit ctrl = new TransitionEdit(kTransitionList);
                UserControl_Context = ctrl;
                UserControl_Context.Dock = System.Windows.Forms.DockStyle.Fill;
                Controls.Clear(); // remove the previous control
                Controls.Add(UserControl_Context);
                UserControl_Context.Show();

                // check if the IPostShow interface exists
                if ( ( UserControl_Context is IPostShow ) )
                {
                    ((IPostShow)UserControl_Context).PostShow();
                }
            }
        }

        private void OnSelectionChanged(ProjectTreeNode kNode)
        {
            bool bUndefined = false;

            if (m_eCurrentNodeType == NodeType.SequenceGroup)
            {
                m_abRollbarSequenceGroupCheckStates = ((RollBarControl) 
                    UserControl_Context).GetCheckStates();
            }
            else if (m_eCurrentNodeType == NodeType.Sequence)
            {
                m_abRollbarSequenceCheckStates = ((RollBarControl) 
                    UserControl_Context).GetCheckStates();
            }
            else if (m_eCurrentNodeType == NodeType.InteractivePreviewRoot)
            {
                m_abRollbarInteractivePreviewCheckStates = ((RollBarControl)
                    UserControl_Context).GetCheckStates();
            }

            if (UserControl_Context != null)
            {
                UserControl_Context.Dispose();
                UserControl_Context = null;
            }

            // Disable keyboard input handling.
            InputManager.Instance.DisableInputHandling();
            InputManager.Instance.RemoveForm(this);

            // Show the appropriate form... hide others.
            switch(kNode.NodeType)
            {
                case NodeType.CharacterModelRoot:
                {
                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_CHARACTER);
                    MLogger.LogGeneral(strLog);

                    CharacterEdit kCharacterEdit = new CharacterEdit();
                    UserControl_Context = kCharacterEdit;
                    break;
                }
                case NodeType.Sequence:
                {
                    MSequence seq = (MSequence)kNode.Tag;

                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_SEQUENCE);
                    MLogger.LogGeneral(strLog + seq.Name);

                    UserControl[] aucControls = new UserControl[]
                    {
                        new SequenceCommon(seq),
                        new SequenceEvaluators(seq),
                        new SequenceTextKeys(seq)
                    };

                    string[] astrNames = new string[]
                    {
                        "Sequence Properties",
                        "Sequence Evaluators",
                        "Sequence Text Keys"
                    };

                    bool[] abCheckStates = m_abRollbarSequenceCheckStates;
                    if (abCheckStates == null)
                    {
                        abCheckStates = new bool[]
                        {
                            true,
                            false,
                            false
                        };
                    }

                    RollBarControl rollbar = new RollBarControl(aucControls,
                        astrNames, abCheckStates, 4);
                    UserControl_Context = rollbar;
                    
                    break;
                }
                case NodeType.SequenceGroup:
                {
                    MSequenceGroup sg = (MSequenceGroup) kNode.Tag;

                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_SEQUENCEGRP);
                    MLogger.LogGeneral(strLog + sg.Name);

                    UserControl[] aucControls = new UserControl[]
                    {
                        new SequenceGroupCommon(sg),
                        new SequenceGroupProperties(sg),
                        new KeystrokeBindings(sg.GroupID)
                    };

                    string[] astrNames = new string[]
                    {
                        "Sequence Group General",
                        "Sequence Group Properties",
                        "Keystroke Settings"
                    };

                    bool[] abCheckStates = m_abRollbarSequenceGroupCheckStates;
                    if (abCheckStates == null)
                    {
                        abCheckStates = new bool[]
                        {
                            true,
                            true,
                            false
                        };
                    }

                    RollBarControl rollbar = new RollBarControl(aucControls,
                        astrNames, abCheckStates, 4);
                    UserControl_Context = rollbar;

                    InputManager.Instance.EnableInputHandling(InputManager
                        .InputMode.Toggle, ProjectData
                        .GetSeqInputInfoForGroup(sg.GroupID));
                    InputManager.Instance.AddForm(this);
                    break;
                }
                case NodeType.SequenceFolder:
                case NodeType.SequenceFolderRoot:
                {
                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_SEQUENCE_FOLDER);
                    MLogger.LogGeneral(strLog + kNode.Text);

                    SequenceFolderEdit kSequenceFolderEdit = 
                        new SequenceFolderEdit(kNode);
                    UserControl_Context = kSequenceFolderEdit;
                    break;
                }
                case NodeType.SequenceGroupFolder:
                case NodeType.SequenceGroupFolderRoot:
                {
                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_SEQUENCEGRP_FOLDER);
                    MLogger.LogGeneral(strLog + kNode.Text);

                    SequenceGroupFolderEdit kSequenceGroupFolderEdit = 
                        new SequenceGroupFolderEdit(kNode);
                    UserControl_Context = kSequenceGroupFolderEdit;
                    break;
                }
                case NodeType.SequenceIDOverviewRoot:
                {
                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_SEQID);
                    MLogger.LogGeneral(strLog);

                    SequenceIDOverview kSeqIDOverviewContext = new
                        SequenceIDOverview(SequenceIDOverview.ModeType
                        .SequenceID);
                    UserControl_Context = kSeqIDOverviewContext; 
                    break;
                }
                case NodeType.GroupIDOverviewRoot:
                {
                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_SEQGRPID);
                    MLogger.LogGeneral(strLog);

                    SequenceIDOverview kSeqIDOverviewContext = new
                        SequenceIDOverview(SequenceIDOverview.ModeType
                        .GroupID);
                    UserControl_Context = kSeqIDOverviewContext;
                    break;
                }
                case NodeType.InteractivePreviewRoot:
                {
                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_INTERACTIVE);
                    MLogger.LogGeneral(strLog);

                    if (AnimationHelpers.DoSequencesExist())
                    {
                        UserControl[] aucControls = new UserControl[]
                        {
                            new InteractivePreview(),
                            new KeystrokeBindings(ProjectData
                            .GetGlobalInputHash())
                        };

                        string[] astrNames = new string[]
                        {
                            "Interactive Preview",
                            "Keystroke Settings"
                        };

                        bool[] abCheckStates =
                            m_abRollbarInteractivePreviewCheckStates;
                        if (abCheckStates == null)
                        {
                            abCheckStates = new bool[]
                            {
                                true,
                                false
                            };
                        }

                        RollBarControl rollbar = new RollBarControl(
                            aucControls, astrNames, abCheckStates, 4);
                        UserControl_Context = rollbar;

                        InputManager.Instance.EnableInputHandling(InputManager
                            .InputMode.Interactive, ProjectData
                            .GetGlobalInputHash());

                        InputManager.Instance.AddForm(this);
                        AppEvents.RaiseInteractivePreviewSelected();
                    }
                    else
                    {
                        bUndefined = true;
                    }
                    break;
                }
                case NodeType.TransitionsPreviewRoot:
                {
                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_TRANSITIONS);
                    MLogger.LogGeneral(strLog);

                    if (AnimationHelpers.DoTransitionsExist() == true)
                    {
                        AppEvents.RaiseTransitionsPreviewSelected();
                    }
                    else
                    {
                        bUndefined = true;
                    }
                    break;
                }
                default:
                    bUndefined = true;
                    break;                   
            }

            if (bUndefined)
            {
                NoEditableProperties kNoEditableProperties =
                    new NoEditableProperties();
                UserControl_Context = kNoEditableProperties;
                m_eCurrentNodeType = NodeType.Undefined;
            }
            else
            {
                m_eCurrentNodeType = kNode.NodeType;
            }

            if (UserControl_Context != null)
            {
                UserControl_Context.Size = this.Size;
                UserControl_Context.Dock = System.Windows.Forms.DockStyle.Fill;
                Controls.Clear(); // remove the previous control
                Controls.Add(UserControl_Context);
                UserControl_Context.Show();
               
                // check if the IPostShow interface exists
                if ( ( UserControl_Context is IPostShow ) )
                {
                    ((IPostShow)UserControl_Context).PostShow();
                }
            }
        }

        protected override  void OnPaintBackground(PaintEventArgs pevent)
        {
            //Don't allow the background to paint
        }

        private void EditViewForm_Load(object sender, System.EventArgs e)
        {
            this.SetStyle( ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);

            if (UserControl_Context != null)
            {
                UserControl_Context.Dispose();
                UserControl_Context = null;
            }

            UserControl_Context = new NoEditableProperties();
            m_eCurrentNodeType = NodeType.Undefined;
            UserControl_Context.Dock = DockStyle.Fill;
            Controls.Clear();
            Controls.Add(UserControl_Context);
            UserControl_Context.Show();
        }

        public void RefreshFromData()
        {
        }
    }
}
