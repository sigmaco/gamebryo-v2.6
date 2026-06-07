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
using System.IO;
using System.Collections;
using System.Windows.Forms;
using System.Diagnostics;
using NiManagedToolInterface;
using Microsoft.Win32;
using System.Reflection;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for ProjectData.
    /// </summary>
    public class ProjectData
    {
        #region Enumerations for Roots
        public enum Position : int
        {
            CharacterRoot = 0,
            SequenceRoot,
            SequenceGroupRoot,
            SequenceIDOverviewRoot,
            GroupIDOverviewRoot,
            TransitionsPreviewRoot,
            InteractivePreviewRoot,
            Max
        };
        #endregion

        #region Project Settings
        
        private string ms_strProjectDataVersion =
            MUtility.GetGamebryoVersion();
        private static string ms_strOriginalBackgroundFilename;
        static private ProjectData ms_kThis = null;  
        private ProjectTreeNode[] m_akProjectRoot = new ProjectTreeNode[(int)
            Position.Max];

        public static string RegistryPath;
        public static bool m_bDefaultInvertPan = false;
        MCamera.UpAxisType m_kDefaultUpAxis = MCamera.UpAxisType.Z_AXIS;

        // Physics default properties
        public static bool m_bDefaultPhysicsDebugEnabled = false;
        public static float m_fDefaultPhysicsDebugScale = 1.0f;
        public static float m_fDefaultPhysicsTimestep = 0.01f;
        public static float m_fDefaultPhysicsGravityX = 0.0f;
        public static float m_fDefaultPhysicsGravityY = 0.0f;
        public static float m_fDefaultPhysicsGravityZ = -9.8f;
        public static float m_fDefaultPhysicsGroundFriction = 0.3f;
        public static float m_fDefaultPhysicsGroundBounce = 0.3f;
        public static float m_fDefaultPhysicsForcePower = 1.0f;

        public bool DefaultInvertPan
        {
            get 
            {
                return m_bDefaultInvertPan; 
            }
            set
            {
                m_bDefaultInvertPan = value;
            }
        }

        public MCamera.UpAxisType DefaultUpAxis
        {
            get {return m_kDefaultUpAxis;}
            set 
            {
                m_kDefaultUpAxis = value;
            }
        }

        public bool DefaultPhysicsDebugEnabled
        {
            get 
            {
                return m_bDefaultPhysicsDebugEnabled; 
            }
            set
            {
                m_bDefaultPhysicsDebugEnabled = value;
            }
        }

        public float DefaultPhysicsDebugScale
        {
            get 
            {
                return m_fDefaultPhysicsDebugScale; 
            }
            set
            {
                m_fDefaultPhysicsDebugScale = value;
            }
        }

        public float DefaultPhysicsTimestep
        {
            get 
            {
                return m_fDefaultPhysicsTimestep; 
            }
            set
            {
                m_fDefaultPhysicsTimestep = value;
            }
        }

        public float DefaultPhysicsGravityX
        {
            get 
            {
                return m_fDefaultPhysicsGravityX; 
            }
            set
            {
                m_fDefaultPhysicsGravityX = value;
            }
        }

        public float DefaultPhysicsGravityY
        {
            get 
            {
                return m_fDefaultPhysicsGravityY; 
            }
            set
            {
                m_fDefaultPhysicsGravityY = value;
            }
        }

        public float DefaultPhysicsGravityZ
        {
            get 
            {
                return m_fDefaultPhysicsGravityZ; 
            }
            set
            {
                m_fDefaultPhysicsGravityZ = value;
            }
        }

        public float DefaultPhysicsGroundFriction
        {
            get 
            {
                return m_fDefaultPhysicsGroundFriction; 
            }
            set
            {
                m_fDefaultPhysicsGroundFriction = value;
            }
        }

        public float DefaultPhysicsGroundBounce
        {
            get 
            {
                return m_fDefaultPhysicsGroundBounce; 
            }
            set
            {
                m_fDefaultPhysicsGroundBounce = value;
            }
        }

        public float DefaultPhysicsForcePower
        {
            get 
            {
                return m_fDefaultPhysicsForcePower; 
            }
            set
            {
                m_fDefaultPhysicsForcePower = value;
            }
        }

        public bool InvertPan
        {
            get {return MFramework.Instance.Input.InvertPan;}
            set 
            {
                if (MFramework.Instance.Input.InvertPan != value)
                {
                    MFramework.Instance.Input.InvertPan = value;
                    SetNeedToSave(true);
                }
            }
        }

        public MCamera.UpAxisType UpAxis
        {
            get {return MFramework.Instance.Input.UpAxis;}
            set 
            {
                if (MFramework.Instance.Input.UpAxis != value)
                {
                    MFramework.Instance.Input.UpAxis = value;
                    SetNeedToSave(true);
                }
            }
        }

        public bool PhysicsDebugEnabled
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.DebugState;
                else
                    return DefaultPhysicsDebugEnabled; 
            }
            set
            {
                MFramework.Instance.Physics.DebugState = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }

        public float PhysicsDebugScale
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.DebugScale;
                else
                    return DefaultPhysicsDebugScale; 
            }
            set
            {
                MFramework.Instance.Physics.DebugScale = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }

        public float PhysicsTimestep
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.TimeStepLength;
                else
                    return DefaultPhysicsTimestep; 
            }
            set
            {
                MFramework.Instance.Physics.TimeStepLength = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }

        public float PhysicsGravityX
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.GravityX;
                else
                    return DefaultPhysicsGravityX; 
            }
            set
            {
                MFramework.Instance.Physics.GravityX = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }

        public float PhysicsGravityY
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.GravityY;
                else
                    return DefaultPhysicsGravityY; 
            }
            set
            {
                MFramework.Instance.Physics.GravityY = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }

        public float PhysicsGravityZ
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.GravityZ;
                else
                    return DefaultPhysicsGravityZ; 
            }
            set
            {
                MFramework.Instance.Physics.GravityZ = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }

        public float PhysicsGroundFriction
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.GroundFriction;
                else
                    return DefaultPhysicsGroundFriction; 
            }
            set
            {
                MFramework.Instance.Physics.GroundFriction = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }

        public float PhysicsGroundBounce
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.GroundBounce;
                else
                    return DefaultPhysicsGroundBounce; 
            }
            set
            {
                MFramework.Instance.Physics.GroundBounce = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }
            
        public float PhysicsForcePower
        {
            get 
            {
                if (MFramework.Instance.Physics.Present)
                    return MFramework.Instance.Physics.ForceExponent;
                else
                    return DefaultPhysicsForcePower; 
            }
            set
            {
                MFramework.Instance.Physics.ForceExponent = value;
                AppEvents.RaisePhysicsSettingsChanged();
                SetNeedToSave(true);
            }
        }

        private ProjectTreeNode m_ptnCurrentFolder = null;
        public ProjectTreeNode CurrentFolder
        {
            get {return m_ptnCurrentFolder;}
            set {m_ptnCurrentFolder = value;}
        }

        static public ProjectData Instance
        {
            get {return ms_kThis;}
        }

        public MColor RendererClearColor
        {
            get {return MFramework.Instance.Renderer.Color;}
            set 
            {
                if (MFramework.Instance.Renderer.Color != value) 
                {
                    MFramework.Instance.Renderer.Color = value;
                    SetNeedToSave(true);
                }
            }
        }

        public float BackgroundHeight
        {
            get {return MFramework.Instance.Input.BackgroundHeight;}
            set 
            {
                if (MFramework.Instance.Input.BackgroundHeight != value)
                {
                    MFramework.Instance.Input.BackgroundHeight = value;
                    MFramework.Instance.Input.SubmitCommand(
                        MUICommand.CommandType.ZOOM_EXTENTS);
                    MFramework.Instance.Input.SubmitCommand(
                        MUICommand.CommandType.RESET_BACKGROUND_HEIGHT);
                    SetNeedToSave(true);
                }
            }
        }

        private bool m_bNeedToSave;
        public bool NeedToSave
        {
            get {return ProjectData.Instance.m_bNeedToSave;}
        }
        private void SetNeedToSave(bool bNeedToSave)
        {
            m_bNeedToSave = bNeedToSave;
        }

        public string BackgroundFilename
        {
            get {return ms_strBackgroundFilename;}
            set
            {
                string TempBackgroundFilename = null;
                string ValueBackgroundFilename = null;
                if (ms_strBackgroundFilename != null)
                    ValueBackgroundFilename = 
                        ms_strBackgroundFilename.ToUpper();
                if (value != null)
                    TempBackgroundFilename = value.ToUpper();

                if (TempBackgroundFilename != ValueBackgroundFilename)
                {
                    ms_strBackgroundFilename = value;
                    MFramework.Instance.SceneGraph.LoadNIF(MFramework.Instance
                        .SceneGraph.BackgroundIndex, ms_strBackgroundFilename);
                }
            }
        }

        private static string ms_strBackgroundFilename;

        public string ImageSubFolder
        {
            get {return ms_strImageSubFolder;}
            set
            {
                string TempImageSubFolder = null;
                if (ms_strImageSubFolder != null)
                    TempImageSubFolder = ms_strImageSubFolder.ToUpper();
                string ValueImageSubFolder = null;
                if (value != null)
                    ValueImageSubFolder = value.ToUpper();

                if (TempImageSubFolder != ValueImageSubFolder &&
                    ValueImageSubFolder != null)
                {
                    MUtility.SetImageSubFolder(value);
                    ms_strImageSubFolder = value;
                    MFramework.Instance.Animation.ReloadNIF();
                    SetNeedToSave(true);
                }
            }
        }

        private static string ms_strImageSubFolder;

        public uint IdleSequenceID
        {
            get {return ms_uiIdleSequenceID;}
            set 
            {
                if (ms_uiIdleSequenceID == value)
                    return;

                ms_uiIdleSequenceID = value;
                SetNeedToSave(true);
            }
        }
        
        private static uint ms_uiIdleSequenceID;

        // Hash tables index on ID's. This is useful because we can quickly 
        // determine if an ID is in use without traversing the list of objects.
        // If not for this feature, we would just hash on the MSequence or 
        // MSequenceGroup itself.
        private Hashtable m_hashGlobalInput = new Hashtable();
        private Hashtable m_hashGroup = new Hashtable();

        private static int ms_iNumTransitionTypes = 9;
        private string[] m_astrTransitionTypeNames = new string[9];
        private System.Drawing.Color[] m_akTransitionTypeBackColors =
            new System.Drawing.Color[9];
        private System.Drawing.Color[] m_akTransitionTypeForeColors =
            new System.Drawing.Color[9];

        private void InitTypes()
        {
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_None] = "None";
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_ImmediateBlend] = "Immediate Blend";
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_DelayedBlend] = "Delayed Blend";
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_Morph] = "Morph";
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_CrossFade] = "Cross Fade";
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_Chain] = "Chain";
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_DefaultSync] = "Default Sync";
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_DefaultNonSync] = "Default NonSync";
            m_astrTransitionTypeNames[(int) MTransition.TransitionType
                .Trans_MultipleSelected] = "Multiple Selected";

            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_None] = WorkSheet.Instance.BackColor;
            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_ImmediateBlend] = System.Drawing.Color.Beige;
            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_DelayedBlend] = System.Drawing.Color.LightBlue;
            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_Morph] = System.Drawing.Color.Moccasin;
            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_CrossFade] = System.Drawing.Color.Cyan;
            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_Chain] = System.Drawing.Color.LightCoral;
            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_DefaultSync] = System.Drawing.Color.LightSalmon;
            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_DefaultNonSync] = System.Drawing.Color.YellowGreen;
            m_akTransitionTypeBackColors[(int) MTransition.TransitionType
                .Trans_MultipleSelected] = System.Drawing.Color.Black;

            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_None] = System.Drawing.Color.Black;
            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_ImmediateBlend] = System.Drawing.Color.Black;
            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_DelayedBlend] = System.Drawing.Color.Black;
            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_Morph] = System.Drawing.Color.Black;
            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_CrossFade] = System.Drawing.Color.Black;
            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_Chain] = System.Drawing.Color.Black;
            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_DefaultSync] = System.Drawing.Color.Black;
            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_DefaultNonSync] = System.Drawing.Color.Black;
            m_akTransitionTypeForeColors[(int) MTransition.TransitionType
                .Trans_MultipleSelected] = System.Drawing.Color.White;

            AppEvents.RaiseTransitionTableSettingsChanged();
        }

        static public string GetTypeNameForTransition(MTransition kT)
        {
            return GetTypeNameForTransitionType(kT.StoredType);
        }

        static public string GetTypeNameForTransitionType(int iType)
        {
            return Instance.m_astrTransitionTypeNames[iType];
        }

        static public string GetTypeNameForTransitionType(
            MTransition.TransitionType kTranType)
        {
            return GetTypeNameForTransitionType((int)kTranType);
        }

        static public System.Drawing.Color GetColorForTransitionType(
            int iType)
        {
            return Instance.m_akTransitionTypeBackColors[iType];
        }

        static public System.Drawing.Color GetAppDefaultColorForTransitionType(
            int iType)
        {
            MTransition.TransitionType eType = 
                (MTransition.TransitionType) iType;
            switch (eType)
            {
                case MTransition.TransitionType.Trans_None: 
                    return System.Drawing.Color.FromArgb(224,223,227); 
                case MTransition.TransitionType.Trans_ImmediateBlend: 
                    return  System.Drawing.Color.Beige;
                case MTransition.TransitionType.Trans_DelayedBlend: 
                    return  System.Drawing.Color.LightBlue;
                case MTransition.TransitionType.Trans_Morph: 
                    return  System.Drawing.Color.Moccasin;
                case MTransition.TransitionType.Trans_CrossFade: 
                    return  System.Drawing.Color.Cyan;
                case MTransition.TransitionType.Trans_Chain: 
                    return  System.Drawing.Color.LightCoral;
                case MTransition.TransitionType.Trans_DefaultSync: 
                    return  System.Drawing.Color.LightSalmon;
                case MTransition.TransitionType.Trans_DefaultNonSync: 
                    return  System.Drawing.Color.YellowGreen;
                case MTransition.TransitionType.Trans_MultipleSelected: 
                    return  System.Drawing.Color.Black;
            }

            Debug.Assert(false, "Should not reach here!");
            return System.Drawing.Color.Fuchsia;
        }

        static public System.Drawing.Color GetColorForTransitionType(
            MTransition.TransitionType kTranType)
        {
            return GetColorForTransitionType((int)kTranType);
        }

        static public System.Drawing.Color GetTextColorForTransitionType(
            int iType)
        {
            return Instance.m_akTransitionTypeForeColors[iType];
        }

        static public System.Drawing.Color 
            GetAppDefaultTextColorForTransitionType(int iType)
        {
            MTransition.TransitionType eType = 
                (MTransition.TransitionType) iType;
            switch (eType)
            {
                case MTransition.TransitionType.Trans_None: 
                    return System.Drawing.Color.Black;
                case MTransition.TransitionType.Trans_ImmediateBlend: 
                    return  System.Drawing.Color.Black;
                case MTransition.TransitionType.Trans_DelayedBlend: 
                    return System.Drawing.Color.Black;
                case MTransition.TransitionType.Trans_Morph: 
                    return System.Drawing.Color.Black;
                case MTransition.TransitionType.Trans_CrossFade: 
                    return System.Drawing.Color.Black;
                case MTransition.TransitionType.Trans_Chain: 
                    return System.Drawing.Color.Black;
                case MTransition.TransitionType.Trans_DefaultSync: 
                    return System.Drawing.Color.Black;
                case MTransition.TransitionType.Trans_DefaultNonSync: 
                    return System.Drawing.Color.Black;
                case MTransition.TransitionType.Trans_MultipleSelected: 
                    return  System.Drawing.Color.White;
            }

            Debug.Assert(false, "Should not reach here!");
            return System.Drawing.Color.Fuchsia;
        }

        static public System.Drawing.Color GetTextColorForTransitionType(
            MTransition.TransitionType kTranType)
        {
            return GetTextColorForTransitionType((int)kTranType);
        }

        static public System.Drawing.Color GetColorForTransition(
            MTransition kT)
        {
            return GetColorForTransitionType(kT.StoredType);
        }

        static public System.Drawing.Color GetTextColorForTransition(
            MTransition kT)
        {
            return GetTextColorForTransitionType(kT.StoredType);
        }

        static public void SetTextColorForTransitionType(int iType,
            System.Drawing.Color kColor)
        {
            Instance.m_akTransitionTypeForeColors[iType] = kColor;
            AppEvents.RaiseTransitionTableSettingsChanged();
        }

        static public void SetColorForTransitionType(int iType,
            System.Drawing.Color kColor)
        {
            Instance.m_akTransitionTypeBackColors[iType] = kColor;
            AppEvents.RaiseTransitionTableSettingsChanged();
        }

        static public int GetNumTransitionTypes()
        {
            return 9;
        }

        #endregion

        #region Static Methods

        private ProjectData()
        {
            ms_kThis = this;
            InitTypes();
            InitCallbacks();
            SetNeedToSave(true);
        }

        static public void Create()
        {
            if (ms_kThis != null)
            {
                return; // already created.
            }

            new ProjectData();
            ms_kThis.ResetContents(null, null);
            ms_strOriginalBackgroundFilename = null;
            //ms_kThis.BackgroundFilename = ms_strOriginalBackgroundFilename;
            ms_kThis.ImageSubFolder = "";
            ms_kThis.IdleSequenceID = uint.MaxValue;
            AppEvents.RaiseFrameworkDataChanged();
        }

        static public void InitBackground()
        {
            // Must occur post-render
            ms_strOriginalBackgroundFilename = MFramework.Instance
                .AppStartupPath + @"\..\..\Ground.nif";
            ms_kThis.BackgroundFilename = ms_strOriginalBackgroundFilename;
        }

        static public Hashtable GetGlobalInputHash()
        {
            ProjectData kPD = ProjectData.Instance;
            return kPD.m_hashGlobalInput;
        }

        static public Hashtable GetGroupHash()
        {
            ProjectData kPD = ProjectData.Instance;
            return kPD.m_hashGroup;
        }

        static private void OnNewKFM()
        {
            try
            {
                ms_kThis.DeleteContents();
                ms_kThis.ResetContents(null, null);
                AppEvents.RaiseFrameworkDataChanged();
                ProjectData.Instance.SetNeedToSave(true);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        static private void OnKFMLoaded()
        {
            try
            {
                ms_kThis.DeleteContents();

                ms_kThis.ResetContents(
                    MFramework.Instance.Animation.Sequences,
                    MFramework.Instance.Animation.SequenceGroups);

                ProjectData.Instance.SetNeedToSave(false);
                MFramework.Instance.Physics.NeedToSave = false;
                AppEvents.RaiseFrameworkDataChanged();
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        static public ProjectTreeNode GetSequenceNode(uint sequenceID)
        {
            ProjectData kData = ProjectData.Instance;
            ProjectTreeNode kNode = kData.GetRootSequenceNode();
            return kNode.FindNodeByID(sequenceID);
        }

        static public ProjectTreeNode GetSequenceGroupNode(
            uint sequenceGroupID)
        {
            ProjectData kData = ProjectData.Instance;
            ProjectTreeNode kNode = kData.GetRootSequenceGroupNode();
            return kNode.FindNodeByID(sequenceGroupID);
        }
        
        #endregion

        #region Initialization

        public void DeleteContents()
        {
            m_hashGlobalInput.Clear();
            m_hashGroup.Clear();
        }

        public void ResetContents(ArrayList aMSeq, ArrayList aMSeqGrp)
        {
            ////
            /// This constuctor is used when no previous project has 
            /// been set up.
            ////
            
            if (MFramework.InstanceIsValid())
            {
                if (BackgroundHeight != 0.0f)
                {
                    BackgroundHeight = 0.0f;
                }

                UpAxis = DefaultUpAxis;
                InvertPan = DefaultInvertPan;
            }
            
            InitHashTables();
            InitRoots();

            ProjectTreeNode kRootChar = GetRootCharacterNode();
            ProjectTreeNode kRootSeq = GetRootSequenceNode();
            ProjectTreeNode kRootSeqGrp = GetRootSequenceGroupNode();

            kRootChar.Nodes.Clear();
            kRootSeq.Nodes.Clear();
            kRootSeqGrp.Nodes.Clear();

            AddSequences(aMSeq, kRootSeq);
            AddSequenceGroups(aMSeqGrp, kRootSeqGrp);
        }

        private void InitHashTables()
        {
            m_hashGlobalInput.Clear();
            m_hashGroup.Clear();

            ArrayList aMSeq = MFramework.Instance.Animation.Sequences;
            if (aMSeq != null && aMSeq.Count != 0)
            {
                foreach (MSequence kSeq in aMSeq)
                {
                    SeqInputInfo kEntry = new SeqInputInfo(kSeq);
                    AddSequenceToHash(kSeq, kEntry);
                }
            }

            ArrayList aSeqGrp = MFramework.Instance.Animation.SequenceGroups;
            if (aSeqGrp != null && aSeqGrp.Count != 0)
            {
                foreach (MSequenceGroup kSeqGrp in aSeqGrp)
                {
                    GrpSeqInputInfo kEntry = 
                        new GrpSeqInputInfo(kSeqGrp);
                    AddSequenceGroupToHash(kSeqGrp, kEntry);
                }
            }
        }

        private void RemoveSequenceFromHash(uint uiSequenceID)
        {
            m_hashGlobalInput.Remove(uiSequenceID);
        }

        private void AddSequenceToHash(MSequence kSeq, SeqInputInfo kEntry)
        {
            m_hashGlobalInput.Add(kSeq.SequenceID, kEntry);
        }

        private void AddSequenceGroupToHash(MSequenceGroup kSeqGrp,
            GrpSeqInputInfo kEntry)
        {
            m_hashGroup.Add(kSeqGrp.GroupID, kEntry);
        }

        private void InitCallbacks()
        {
            MFramework.Instance.Animation.OnNewKFM +=
                new MAnimation.__Delegate_OnNewKFM(OnNewKFM);
            MFramework.Instance.Animation.OnKFMLoaded +=
                new MAnimation.__Delegate_OnKFMLoaded(OnKFMLoaded);

            MFramework.Instance.Animation.OnSequenceModified += new
                MAnimation.__Delegate_OnSequenceModified(OnSequenceChanged);
        
            MFramework.Instance.Animation.OnSequenceGroupModified += new
                MAnimation.__Delegate_OnSequenceGroupModified(
                OnSequenceGroupChanged);
            
            MFramework.Instance.Animation.OnSequenceAdded += new
                MAnimation.__Delegate_OnSequenceAdded(OnSequenceAdded);

            MFramework.Instance.Animation.OnSequenceRemoved += new
                MAnimation.__Delegate_OnSequenceRemoved(OnSequenceRemoved);

            MFramework.Instance.Animation.OnTransitionAdded += new
                MAnimation.__Delegate_OnTransitionAdded(OnTransitionAdded);

            MFramework.Instance.Animation.OnTransitionRemoved += new
                MAnimation.__Delegate_OnTransitionRemoved(
                OnTransitionRemoved);

            MFramework.Instance.Animation.OnSequenceGroupAdded += new
                MAnimation.__Delegate_OnSequenceGroupAdded(
                OnSequenceGroupAdded);

            MFramework.Instance.Animation.OnSequenceGroupRemoved += new
                MAnimation.__Delegate_OnSequenceGroupRemoved(
                OnSequenceGroupRemoved);
        }

        private void UpdateSequenceIDProp(MSequence kSequence)
        {
            // SequenceID changed. We need to make sure
            // that our internal hash is aware of this.

            // 1. Find Hash entry with matching MSequence
            // 2. Remove it
            // 3. Re-add it with new ID entry.

            foreach (DictionaryEntry e in m_hashGlobalInput)
            {
                SeqInputInfo info = (SeqInputInfo) e.Value;
                if (info.m_Seq == kSequence)
                {
                    // remove this entry...
                    m_hashGlobalInput.Remove(e.Key);

                    // add back
                    m_hashGlobalInput.Add(kSequence.SequenceID, info);
                    ProjectData.Instance.SetNeedToSave(true);

                    // We are done.
                    break;
                }
            }   
        
            foreach (DictionaryEntry e in m_hashGroup)
            {
                GrpSeqInputInfo info = (GrpSeqInputInfo) e.Value;
                Hashtable SeqInputInfoHash = info.GrpSeqInputInfoHash;

                if (SeqInputInfoHash == null)
                    continue;

                foreach (DictionaryEntry eSeqInfo in SeqInputInfoHash)
                {
                    SeqInputInfo kValue = (SeqInputInfo) eSeqInfo.Value;
                    if (kValue == null)
                        continue;

                    if (kValue.m_Seq == kSequence)
                    {
                        SeqInputInfoHash.Remove(eSeqInfo.Key);
                        SeqInputInfoHash.Add(kSequence.SequenceID, kValue);
                        ProjectData.Instance.SetNeedToSave(true);
                        break;
                    }
                }
            }
        }

        private void UpdateSequenceGrpIDProp(MSequenceGroup kSeqGrp)
        {
            // SequenceGroup ID changed. We need to make sure
            // that our internal hash is aware of this.

            // 1. Find Hash entry with matching MSequenceGroup
            // 2. Remove it
            // 3. Re-add it with new ID entry.

            foreach (DictionaryEntry e in m_hashGroup)
            {
                GrpSeqInputInfo info = (GrpSeqInputInfo) e.Value;
                if (info.m_SeqGrp == kSeqGrp)
                {
                    // remove this entry...
                    m_hashGroup.Remove(e.Key);

                    // add back
                    m_hashGroup.Add(kSeqGrp.GroupID, info);
                    SetNeedToSave(true);

                    // We are done.
                    break;
                }
            }
        }

        private void UpdateSequenceGrpSeqInfoProp(MSequenceGroup kSeqGrp)
        {
            Hashtable kHash = GetSeqInputInfoForGroup(kSeqGrp.GroupID);

            if (kHash != null)
            {
                int iLength;
                
                if (kSeqGrp.SequenceInfo == null)
                    iLength = 0;
                else
                    iLength = kSeqGrp.SequenceInfo.Length;

                if (kHash.Count == iLength) // Sequence count did not change
                {
                    return;
                }
                else if (kHash.Count < iLength) // Added sequences
                {
                    foreach (MSequenceGroup.MSequenceInfo kInfo in
                        kSeqGrp.SequenceInfo)
                    {
                        if (!kHash.ContainsKey(kInfo.SequenceID))
                        {
                            MSequence kSeq = MFramework.Instance.Animation
                                .GetSequence(kInfo.SequenceID);
                            SeqInputInfo kEntry = new SeqInputInfo(kSeq);
                            kHash.Add(kInfo.SequenceID, kEntry);
                            SetNeedToSave(true);
                        }
                    }
                }
                else if (kHash.Count > iLength) // Removed sequences
                {
                    while (kHash.Count > iLength)
                    {
                        foreach (DictionaryEntry e in kHash)
                        {
                            SeqInputInfo kEntry = (SeqInputInfo) e.Value;
                            uint uiID = (uint) e.Key;
                            if (kSeqGrp.GetSequenceInfoIndex(uiID) == -1)
                            {
                                kHash.Remove(uiID);
                                SetNeedToSave(true);
                                break;
                            }
                        }
                    }
                }

                AppEvents.RaiseKeystrokeBindingsChanged();
            }
        }

        private void OnSequenceAdded(MSequence pkNewSequence)
        {
            try 
            {
                ProjectTreeNode kRoot = GetRootSequenceNode();
                ProjectTreeNode kNode = kRoot.FindNodeByID(
                    pkNewSequence.SequenceID);
                if (kNode == null)
                {
                    if (m_ptnCurrentFolder != null)
                    {
                        kRoot = m_ptnCurrentFolder;
                    }

                    kRoot.Nodes.Add(new ProjectTreeNode(pkNewSequence.Name,
                        NodeType.Sequence, pkNewSequence));
                    kRoot.Expand();
                }

                AddSequenceToHash(pkNewSequence, 
                    new SeqInputInfo(pkNewSequence));

                AppEvents.RaiseFrameworkDataChanged();
                AppEvents.RaiseKeystrokeBindingsChanged();

                SetNeedToSave(true);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void OnSequenceRemoved(uint uiSequenceID)
        {
            try 
            {
                ProjectTreeNode kNode = GetRootSequenceNode().FindNodeByID(
                    uiSequenceID);
                if (kNode != null)
                {
                    kNode.Delete();
                }

                RemoveSequenceFromHash(uiSequenceID);

                AppEvents.RaiseFrameworkDataChanged();
                SetNeedToSave(true);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void OnTransitionAdded(MTransition pkNewTransition)
        {
            SetNeedToSave(true);
        }

        private void OnTransitionRemoved(MTransition pkTransitionToBeRemoved)
        {
            SetNeedToSave(true);
        }

        private void OnSequenceGroupAdded(MSequenceGroup pkNewGroup)
        {
            try 
            {
                Hashtable ht = GetGroupHash();
                if (ht != null && ht[pkNewGroup.GroupID] == null)
                {
                    AddSequenceGroup(pkNewGroup, GetRootSequenceGroupNode());
                    AddSequenceGroupToHash(pkNewGroup, new GrpSeqInputInfo(
                        pkNewGroup));
                    
                    SetNeedToSave(true);
                    AppEvents.RaiseFrameworkDataChanged();
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void OnSequenceGroupRemoved(MSequenceGroup pkGroupToBeRemoved)
        {
            try
            {
                Hashtable ht = GetGroupHash();
                if (ht != null)
                {
                    ht.Remove(pkGroupToBeRemoved.GroupID);
                    SetNeedToSave(true);
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }
    
        private void OnSequenceGroupChanged(MSequenceGroupEventArgs Args,
            MSequenceGroup kSequenceGroup)
        {
            try
            {
                // Hey! Something changed about the Sequence...
                // Find out what it is and notify others...
                switch(Args.Type)
                {
                    case MSequenceGroup.PropertyType.Prop_GroupID:
                        UpdateSequenceGrpIDProp(kSequenceGroup);
                        SetNeedToSave(true);
                        break;
                    case MSequenceGroup.PropertyType.Prop_SequenceInfo:
                        UpdateSequenceGrpSeqInfoProp(kSequenceGroup);
                        SetNeedToSave(true);
                        break;
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void OnSequenceChanged(MSequence.PropertyType ePropType,
            MSequence kSequence)
        {
            try
            {
                // Hey! Something changed about the Sequence...
                // Find out what it is and notify others...
                switch(ePropType)
                {
                    case MSequence.PropertyType.Prop_AnimIndex:
                    case MSequence.PropertyType.Prop_Filename:
                    case MSequence.PropertyType.Prop_Name:
                        break;
                    case MSequence.PropertyType.Prop_SequenceID:
                        UpdateSequenceIDProp(kSequence);
                        SetNeedToSave(true);
                        break;
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void InitRoots()
        {
            m_akProjectRoot[(int) Position.CharacterRoot] =  
                new ProjectTreeNode("Character Model",
                NodeType.CharacterModelRoot);

            m_akProjectRoot[(int) Position.SequenceRoot] = 
                new ProjectTreeNode("Sequences",
                NodeType.SequenceFolderRoot);

            m_akProjectRoot[(int) Position.SequenceGroupRoot] = 
                new ProjectTreeNode("Sequence Groups",
                NodeType.SequenceGroupFolderRoot);

            m_akProjectRoot[(int) Position.SequenceIDOverviewRoot] =  
                new ProjectTreeNode("Sequence ID Overview",
                NodeType.SequenceIDOverviewRoot);

            m_akProjectRoot[(int) Position.GroupIDOverviewRoot] =
                new ProjectTreeNode("Group ID Overview",
                NodeType.GroupIDOverviewRoot);

            m_akProjectRoot[(int) Position.InteractivePreviewRoot] =
                new ProjectTreeNode("Interactive Preview",
                NodeType.InteractivePreviewRoot);

            m_akProjectRoot[(int) Position.TransitionsPreviewRoot] =
                new ProjectTreeNode("Transitions Preview",
                NodeType.TransitionsPreviewRoot);
        }

        static public void PopulateTreeView(System.Windows.Forms.TreeView tv)
        {
            // Create Project data if it doesn't exist.
            // Moved to post render creation: ProjectData.Create();

            ProjectData pd = ProjectData.Instance;
            Debug.Assert(pd != null && tv != null  && tv.Nodes != null);
            Debug.Assert(tv.Nodes.IsReadOnly == false);
            TreeNode kSelectedNode = tv.SelectedNode;
            try
            {
                tv.Nodes.Clear();
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        
            ProjectTreeNode[] akRoots = pd.GetRootFolders();
            Debug.Assert(akRoots != null && akRoots.Length > 0);
            
            foreach (ProjectTreeNode kPTN in akRoots)
                tv.Nodes.Add(kPTN);     
            Debug.Assert(pd.GetRootCharacterNode() != null);
            pd.GetRootCharacterNode().Expand();
            Debug.Assert(pd.GetRootSequenceNode() != null);
            pd.GetRootSequenceNode().Expand();
            Debug.Assert(pd.GetRootSequenceGroupNode() != null);
            pd.GetRootSequenceGroupNode().Expand();
            if (kSelectedNode != null)
                tv.SelectedNode = kSelectedNode;
            else
                tv.SelectedNode = pd.GetRootCharacterNode();
        }

        public void SortTree()
        {
            SortNode(m_akProjectRoot[(int) Position.SequenceRoot], true);
            SortNode(m_akProjectRoot[(int) Position.SequenceGroupRoot], true);
        }

        private void SortNode(ProjectTreeNode node, bool bRecursive)
        {
            ArrayList aFolders = new ArrayList();
            ArrayList aNodes = new ArrayList();

            foreach (ProjectTreeNode n in node.Nodes)
            {
                switch (n.NodeType)
                {
                    case NodeType.SequenceFolder:
                    case NodeType.SequenceGroupFolder:
                        SortedAdd(aFolders, n);
                        break;
                    default:
                        SortedAdd(aNodes, n);
                        break;
                }
            }

            node.Nodes.Clear();
            foreach (ProjectTreeNode n in aFolders)
            {
                node.Nodes.Add(n);
            }
            foreach (ProjectTreeNode n in aNodes)
            {
                node.Nodes.Add(n);
            }

            if (bRecursive)
            {
                foreach (ProjectTreeNode n in node.Nodes)
                {
                    SortNode(n, bRecursive);
                }
            }
        }

        private void SortedAdd(ArrayList array, ProjectTreeNode newnode)
        {
            int iIndex;
            for (iIndex = 0; iIndex < array.Count; iIndex++)
            {
                ProjectTreeNode oldnode = (ProjectTreeNode) array[iIndex];
                if (newnode.Text.CompareTo(oldnode.Text) < 0)
                {
                    break;
                }
            }

            array.Insert(iIndex, newnode);
        }

        #endregion

        #region Root Accessors

        public ProjectTreeNode[] GetRootFolders()
        {
            return m_akProjectRoot;
        }

        public ProjectTreeNode GetRootCharacterNode()
        {
            return m_akProjectRoot[(int) Position.CharacterRoot];
        }

        public ProjectTreeNode GetRootSequenceNode()
        {
            return m_akProjectRoot[(int) Position.SequenceRoot];
        }

        public ProjectTreeNode GetRootSequenceGroupNode()
        {
            return m_akProjectRoot[(int) Position.SequenceGroupRoot];
        }

        public ProjectTreeNode GetRootSequenceIDOverviewNode()
        {
            return m_akProjectRoot[(int) Position.SequenceIDOverviewRoot];
        }

        public ProjectTreeNode GetRootGroupIDOverviewNode()
        {
            return m_akProjectRoot[(int) Position.GroupIDOverviewRoot];
        }

        public ProjectTreeNode GetInteractivePreviewNode()
        {
            return m_akProjectRoot[(int) Position.InteractivePreviewRoot];
        }

        internal ProjectTreeNode GetTransitionsPreviewNode()
        {
            return m_akProjectRoot[(int) Position.TransitionsPreviewRoot];
        }

        #endregion

        #region Save Functions

        static public bool Save(string strFilePath)
        {
            return ProjectData.Instance._Save(strFilePath);
        }

        private bool _SaveSeqInputInfoHashTable(ref BinaryWriter w,
            ref Hashtable kHash)
        {
            uint uiCount = 0;
            w.Write(kHash.Count);
            if (kHash.Count == 0)
                return true;

            ArrayList kSequences = MFramework.Instance.Animation.Sequences;
            if (kSequences != null)
            {
                foreach (MSequence sequence in kSequences)
                {
                    if (kHash.ContainsKey(sequence.SequenceID))
                    {
                        uiCount++;
                        SeqInputInfo kEntry = (ProjectData.SeqInputInfo)kHash[
                            sequence.SequenceID];
                        w.Write((int)sequence.SequenceID);
                        w.Write(kEntry.m_bSync);
                        System.Int64 iKeyStroke = (System.Int64)
                            kEntry.m_eKeyStroke;
                        w.Write(iKeyStroke);
                        if (kEntry.m_strKeyStroke != null)
                            w.Write(kEntry.m_strKeyStroke);
                        else
                            w.Write("");

                        if (kEntry.m_strModifier != null)
                            w.Write(kEntry.m_strModifier);   
                        else
                            w.Write("");
                    }
                }
            }

            Debug.Assert(uiCount == kHash.Count, "Saved keycode count does " +
                "not equal hash table size");
            return true;
        }

        private bool _SaveGrpSeqInputInfoHashTable(ref BinaryWriter w,
            ref Hashtable kHash)
        {
            uint uiCount = 0;
            w.Write(kHash.Count);
            if (kHash.Count == 0)
                return true;

            ArrayList kSequenceGroups = MFramework.Instance.Animation
                .SequenceGroups;
            if (kSequenceGroups != null)
            {
                foreach (MSequenceGroup seqGroup in kSequenceGroups)
                {
                    if (kHash.ContainsKey(seqGroup.GroupID))
                    {
                        uiCount++;
                        GrpSeqInputInfo kEntry = (GrpSeqInputInfo)kHash[
                            seqGroup.GroupID];
                        w.Write((int)seqGroup.GroupID);
                        Hashtable kSubHash = GetSeqInputInfoForGroup(
                            seqGroup.GroupID);
                        if (!_SaveSeqInputInfoHashTable(ref w, ref kSubHash))
                            throw new IOException();
                    }
                }
            }

            Debug.Assert(uiCount == kHash.Count, "Saved keycode count does " +
                "not equal hash table size");
            return true;
        }
        
        private bool _SavePhysics(ref BinaryWriter w)
        {
            w.Write(PhysicsDebugEnabled);
            w.Write((double)PhysicsDebugScale);
            w.Write((double)PhysicsTimestep);
            w.Write((double)PhysicsGravityX);
            w.Write((double)PhysicsGravityY);
            w.Write((double)PhysicsGravityZ);
            w.Write((double)PhysicsGroundFriction);
            w.Write((double)PhysicsGroundBounce);
            w.Write((double)PhysicsForcePower);
        
            ArrayList kPhysicalSequences =
                MFramework.Instance.Physics.PhysicalSequences;
            w.Write(kPhysicalSequences.Count);
            foreach (MSequence seq in kPhysicalSequences)
            {
                w.Write(seq.SequenceID);
            }
            w.Write((double)MFramework.Instance.Physics.ForceExponent);

            MFramework.Instance.Physics.NeedToSave = false;

            return true;
        }
        
        private bool _Save(string strFilePath)
        {
            // Save out Root Sequence FOlder 
            // Save out Number of Sequences
            // save out SequenceIDs for each Sequence.
            // save out Number of folders
            // save out folders (recursive).
            //
            // Same sort of thing for SequenceGroups

            try 
            {
                if (File.Exists(strFilePath) &&
                    (File.GetAttributes(strFilePath) & FileAttributes.ReadOnly)
                        == FileAttributes.ReadOnly)
                {
                    // If the file is read-only, change its attributes to be
                    // writable.
                    try 
                    {
                        File.SetAttributes(strFilePath, FileAttributes.Normal);
                    }
                    catch(Exception e)
                    {
                        Debug.Assert(false, e.Message);
                        return false;
                    }
                }

                FileStream fs = File.OpenWrite(strFilePath);
                BinaryWriter w = new BinaryWriter(fs);
                w.Write(ms_strProjectDataVersion);
                w.Write((int) UpAxis);
                if (ms_strBackgroundFilename != null &&
                    !AppUtils.EqualsNoCase(ms_strBackgroundFilename, 
                    ms_strOriginalBackgroundFilename))
                {
                    w.Write(ms_strBackgroundFilename);
                }
                else
                {
                    w.Write("");
                }
                
                w.Write((double)BackgroundHeight);
                w.Write(this.IdleSequenceID);
                
                if (ImageSubFolder != null)
                    w.Write(ImageSubFolder);
                else
                    w.Write("");
                

                foreach (ProjectTreeNode node in m_akProjectRoot)
                {
                    switch (node.NodeType)
                    {
                        case NodeType.SequenceFolderRoot:
                        case NodeType.SequenceGroupFolderRoot:
                            node.Write(w);
                            break;
                    }
                }

                Hashtable kHash = GetGlobalInputHash();
                if (!_SaveSeqInputInfoHashTable(ref w, ref kHash))
                    throw new IOException();

                if (!_SaveGrpSeqInputInfoHashTable(ref w, ref m_hashGroup))
                    throw new IOException();

                // Save Physics at the end, so if we are loading without
                // physics then it will still work.
                if (MFramework.Instance.Physics.Present)
                {
                    if (!_SavePhysics(ref w))
                    {
                        throw new IOException();
                    }
                }

                w.Flush();
                w.Close();
                fs.Close();
                ProjectData.Instance.SetNeedToSave(false);

                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }


        public bool SaveTransitionTableRegistrySettings()
        {
            string registryPath = RegistryPath + "\\Settings";
            RegistryKey BaseKey;
            
            try 
            {
                BaseKey = Registry.CurrentUser.CreateSubKey(registryPath);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return false;
            }
        
            for (int i = 0; i < ms_iNumTransitionTypes; i++)
            {
                BaseKey.SetValue("TransitionTypeNames[" + i.ToString() + "]",
                                 m_astrTransitionTypeNames[i]);
                BaseKey.SetValue("TransitionTypeBackColors[" + i.ToString() +
                    "].r", m_akTransitionTypeBackColors[i].R);
                BaseKey.SetValue("TransitionTypeBackColors[" + i.ToString() +
                    "].g", m_akTransitionTypeBackColors[i].G);
                BaseKey.SetValue("TransitionTypeBackColors[" + i.ToString() +
                    "].b", m_akTransitionTypeBackColors[i].B);
                BaseKey.SetValue("TransitionTypeForeColors[" + i.ToString() +
                    "].r", m_akTransitionTypeForeColors[i].R);
                BaseKey.SetValue("TransitionTypeForeColors[" + i.ToString() +
                    "].g", m_akTransitionTypeForeColors[i].G);
                BaseKey.SetValue("TransitionTypeForeColors[" + i.ToString() +
                    "].b", m_akTransitionTypeForeColors[i].B);
            }
 
            BaseKey.Close();
            return true;
        }

        public bool SaveRendererRegistrySettings()
        {
            string registryPath = RegistryPath + "\\Settings";
            RegistryKey BaseKey;
            
            try 
            {
                BaseKey = Registry.CurrentUser.CreateSubKey(registryPath);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return false;
            }
        
            BaseKey.SetValue("RendererClearColor.R", RendererClearColor.r);
            BaseKey.SetValue("RendererClearColor.G", RendererClearColor.g);
            BaseKey.SetValue("RendererClearColor.B", RendererClearColor.b);
            
            BaseKey.Close();
            return true;
        }

        public bool SaveProjectRegistrySettings(bool bSaveUp)
        {
            string registryPath = RegistryPath + "\\Settings";
            RegistryKey BaseKey;
            
            try 
            {
                BaseKey = Registry.CurrentUser.CreateSubKey(registryPath);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return false;
            }
        
            if (bSaveUp)
            {
                BaseKey.SetValue("UpAxis", (int)UpAxis);
                DefaultUpAxis = UpAxis;
            }

            BaseKey.SetValue("InvertPan", InvertPan);
            DefaultInvertPan = InvertPan;
            
            BaseKey.Close();
            return true;
        }

        public bool SavePhysicsRegistrySettings()
        {
            // Can't change PhysX settings if it isn't present.
            if (!MFramework.Instance.Physics.Present)
                return true;

            string registryPath = RegistryPath + "\\Settings";
            RegistryKey BaseKey;
            
            try 
            {
                BaseKey = Registry.CurrentUser.CreateSubKey(registryPath);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return false;
            }

            BaseKey.SetValue("PhysicsDebugEnabled", PhysicsDebugEnabled);
            DefaultPhysicsDebugEnabled = PhysicsDebugEnabled;

            BaseKey.SetValue("PhysicsDebugScale", PhysicsDebugScale);
            DefaultPhysicsDebugScale = PhysicsDebugScale;

            BaseKey.SetValue("PhysicsTimestep", PhysicsTimestep);
            DefaultPhysicsTimestep = PhysicsTimestep;

            BaseKey.SetValue("PhysicsGravityX", PhysicsGravityX);
            DefaultPhysicsGravityX = PhysicsGravityX;

            BaseKey.SetValue("PhysicsGravityY", PhysicsGravityY);
            DefaultPhysicsGravityY = PhysicsGravityY;

            BaseKey.SetValue("PhysicsGravityZ", PhysicsGravityZ);
            DefaultPhysicsGravityZ = PhysicsGravityZ;

            BaseKey.SetValue("PhysicsGroundFriction", PhysicsGroundFriction);
            DefaultPhysicsGroundFriction = PhysicsGroundFriction;

            BaseKey.SetValue("PhysicsGroundBounce", PhysicsGroundBounce);
            DefaultPhysicsGroundBounce = PhysicsGroundBounce;

            BaseKey.SetValue("PhysicsForcePower", PhysicsForcePower);
            DefaultPhysicsForcePower = PhysicsForcePower;

            BaseKey.Close();
            return true;
        }

        public bool WriteHeaderFile(string strFilename)
        {
            if (strFilename == null || strFilename == string.Empty)
            {
                return false;
            }

            try
            {
                if (File.Exists(strFilename) &&
                    (File.GetAttributes(strFilename) & FileAttributes.ReadOnly)
                        == FileAttributes.ReadOnly)
                {
                    // If the file is read-only, change its attributes to be
                    // writable.
                    try 
                    {
                        File.SetAttributes(strFilename, FileAttributes.Normal);
                    }
                    catch(Exception e)
                    {
                        Debug.Assert(false, e.Message);
                        return false;
                    }
                }

                using (StreamWriter sw = new StreamWriter(strFilename))
                {
                    // Write comment header.
                    sw.Write("// This file was automatically generated. It " +
                        "contains definitions for all the\n// animations " +
                        "stored in the associated KFM file. Include this " +
                        "file in your\n// final application to easily " +
                        "refer to animation sequences.\n\n");

                    char[] acSeparators = new char[] {'\\', '.'};
                    string[] aSections = strFilename.Split(acSeparators);
                    string strName = aSections[aSections.Length - 2].Replace(
                        ' ', '_');
                    string strUpperName = strName.ToUpper();
                    sw.Write("#ifndef " + strUpperName + "_H__\n#define " +
                        strUpperName + "_H__\n\n");

                    sw.Write("namespace " + strName + "\n{\n");

                    string strTab = "    ";
                    sw.Write(strTab + "enum\n" + strTab + "{\n");

                    for (int i = 0;
                        i < MFramework.Instance.Animation.Sequences.Count;
                        i++)
                    {
                        MSequence seq = (MSequence) MFramework.Instance
                            .Animation.Sequences[i];

                        string strSeqName = seq.Name.Replace(' ', '_')
                            .ToUpper();
                        if (strSeqName.Length >= 24)
                        {
                            strSeqName += " ";
                        }
                        else
                        {
                            while (strSeqName.Length < 24)
                            {
                                strSeqName += " ";
                            }
                        }

                        string strLine = strTab + strTab + strSeqName +
                            "= " + seq.SequenceID.ToString();
                        if (i < MFramework.Instance.Animation.Sequences
                            .Count - 1)
                        {
                            strLine += ",";
                        }
                        sw.WriteLine(strLine);
                    }

                    sw.Write(strTab + "};\n}\n\n#endif  // #ifndef " +
                        strUpperName + "_H__\n");
                }

            }
            catch (Exception e)
            {
                MLogger.LogWarning("Error saving out header file: " +
                    e.ToString());
                return false;
            }

            return true;
        }

        #endregion

        #region Load Functions

        static public bool Load(string strFilePath)
        {
            ProjectData.Instance.LoadRegistrySettings();
            ProjectData.Instance.ImageSubFolder = "";
            ProjectData.Instance.IdleSequenceID = uint.MaxValue;
            return ProjectData.Instance._Load(strFilePath);
        }

        private bool _LoadSeqInputInfoHashTable(ref BinaryReader br,
            ref Hashtable kHash)
        {
            int iCount = br.ReadInt32();
                
            for (int i = 0; i < iCount; i++)
            {
                SeqInputInfo kEntry = null;
                uint uiID = (uint)br.ReadInt32();
                if (kHash.ContainsKey(uiID) == false)
                {
                    MSequence kSeq = MFramework.Instance.Animation.
                        GetSequence(uiID);
                    if (kSeq != null)
                    {
                        kEntry = new SeqInputInfo(kSeq);
                        kHash.Add(uiID, kEntry);
                    }
                }
                else
                {
                    kEntry = (ProjectData.SeqInputInfo)kHash[uiID];
                }

                bool bSync = br.ReadBoolean();
                System.Int64 iKeyStroke = br.ReadInt64();
                string strKeyStroke = br.ReadString();
                string strModifier = br.ReadString(); 
 
                if (kEntry != null)
                {
                    kEntry.m_bSync = bSync;
                    kEntry.m_eKeyStroke = (Keys) iKeyStroke;

                    if (strKeyStroke != "")
                        kEntry.m_strKeyStroke = strKeyStroke;
                    else 
                        kEntry.m_strKeyStroke = null;

                    if (strModifier != "")
                        kEntry.m_strModifier = strModifier;
                    else
                        kEntry.m_strModifier = null;
                }
            }
            return true;
        }

        private bool _LoadGrpSeqInputInfoHashTable(ref BinaryReader br,
            ref Hashtable kHash)
        {
            int iCount = br.ReadInt32();
                
            for (int i = 0; i < iCount; i++)
            {
                GrpSeqInputInfo kEntry;
                uint uiID = (uint)br.ReadInt32();
                if (kHash.ContainsKey(uiID) == false)
                {
                    MSequenceGroup seqgrp =
                        MFramework.Instance.Animation.GetSequenceGroup(uiID);
                    kEntry = new GrpSeqInputInfo(seqgrp);
                    kHash.Add(uiID, kEntry);
                }
                else
                {
                    kEntry = (GrpSeqInputInfo)kHash[uiID];
                }

                Hashtable kSubHash = kEntry.GrpSeqInputInfoHash;

                if (!_LoadSeqInputInfoHashTable(ref br, ref kSubHash))
                    throw new IOException();
            }
            return true;
        }

        private bool _LoadPhysics(ref BinaryReader br)
        {
            if (br.PeekChar() == -1)
            {
                // No physics in this file.
                return true;
            }

            PhysicsDebugEnabled = br.ReadBoolean();
            PhysicsDebugScale = (float)br.ReadDouble();
            PhysicsTimestep = (float)br.ReadDouble();
            PhysicsGravityX = (float)br.ReadDouble();
            PhysicsGravityY = (float)br.ReadDouble();
            PhysicsGravityZ = (float)br.ReadDouble();
            PhysicsGroundFriction = (float)br.ReadDouble();
            PhysicsGroundBounce = (float)br.ReadDouble();
            PhysicsForcePower = (float)br.ReadDouble();

            int iCount = br.ReadInt32();
            for (int i = 0; i < iCount; i++)
            {
                uint uiSeqID = br.ReadUInt32();
                MSequence kSeq =
                    MFramework.Instance.Animation.GetSequence(uiSeqID);
                MFramework.Instance.Physics.AddSequence(kSeq);
            }
            MFramework.Instance.Physics.ForceExponent = (float)br.ReadDouble();

            MFramework.Instance.Physics.NeedToSave = false;
            
            return true;
        }
        
        private bool _Load(string strFilePath)
        {
            // Load in Root Sequence Folder
            // Load in Number of Sequences
            // Load in Number of SequenceIDs
            // .... Should pass in MSequences into this function
            // .... Match SequenceID to MSequence
            // .... Add MSequence as Extra data
            // Load in Number of Folders
            // Load in Folders (recursive)
            //
            // Same sort of thing for SequenceGroups
        
            try 
            {
                if (!File.Exists(strFilePath))
                {
                    ProjectData.Instance.SetNeedToSave(false);
                    return false;
                }

                FileStream fs = File.OpenRead(strFilePath);
                BinaryReader br = new BinaryReader(fs);
                string strFileVersion = br.ReadString();
                UpAxis = (MCamera.UpAxisType) br.ReadInt32();
                string strFilename = br.ReadString();
                if (strFilename != null && strFilename.Length > 0)
                    BackgroundFilename = strFilename;
                else
                    BackgroundFilename = ms_strOriginalBackgroundFilename;
                
                BackgroundHeight = (float)br.ReadDouble();
                if (MUtility.GetVersionFromString(strFileVersion) < 
                    MUtility.GetVersionFromString("2.2.1"))
                {
                    // read in anim rate
                    br.ReadDouble();
                    // read in background color
                    br.ReadDouble();
                    br.ReadDouble();
                    br.ReadDouble();
                }

                if (MUtility.GetVersionFromString(strFileVersion) >= 
                    MUtility.GetVersionFromString("2.2.2"))
                {
                    IdleSequenceID = (uint)br.ReadInt32();
                    ImageSubFolder = br.ReadString();
                }

                if (MUtility.GetVersionFromString(strFileVersion) == 
                    MUtility.GetVersionFromString("2.2"))
                {
                    for (int i = 0; i < GetNumTransitionTypes(); i++)
                    {
                        // Used to stream transition types
                        int alpha = br.ReadInt32();
                        int red = br.ReadInt32();
                        int green = br.ReadInt32();
                        int blue = br.ReadInt32();
                        
                        alpha = br.ReadInt32();
                        red = br.ReadInt32();
                        green = br.ReadInt32();
                        blue = br.ReadInt32();
                       
                    }
                }
                

                InitRoots();
                foreach (ProjectTreeNode node in m_akProjectRoot)
                {
                    switch (node.NodeType)
                    {
                        case NodeType.SequenceFolderRoot:
                        case NodeType.SequenceGroupFolderRoot:
                            node.Read(br);
                            break;
                    }
                }

                Hashtable kHash = GetGlobalInputHash();
                if (!_LoadSeqInputInfoHashTable(ref br, ref kHash))
                    throw new IOException();
                
                if (MUtility.GetVersionFromString(strFileVersion) <= 
                    MUtility.GetVersionFromString("2.0"))
                {
                    // do nothing
                }
                else
                {
                    if (!_LoadGrpSeqInputInfoHashTable(ref br,
                        ref m_hashGroup))
                    {
                        throw new IOException();
                    }
                }

                // Load Physics if present
                if (MFramework.Instance.Physics.Present)
                {
                    if (!_LoadPhysics(ref br))
                    {
                        throw new IOException();
                    }
                }

                br.Close();
                fs.Close();

                ProjectData.Instance.SetNeedToSave(false);
                AppEvents.RaiseFrameworkDataChanged();
                return true;
            }
            catch(Exception)
            {
            
                return false;
            }
        }

        private static string strtok(ref string strBuffer, string strSeps)
        {
            int iStart = -1;
            int iEnd = -1;
            string pcToken = null;
            for (int i = 0; i < strBuffer.Length; i++)
            {
                bool bFoundSep = false;
                for (int j = 0; j < strSeps.Length; j++)
                {
                    if (strBuffer[i] == strSeps[j])
                    {
                        bFoundSep = true;
                        break;
                    }
                }
                
                if (bFoundSep && iStart != -1)
                {
                    iEnd = i;
                    break;
                }
                else if (bFoundSep)
                {
                    continue;
                }
                else if (iStart == -1)
                {
                    iStart = i;
                }
            }

            if (iStart == -1)
                return null;
            if (iEnd == -1)
                iEnd = strBuffer.Length;

            pcToken = "";
                        
            for (int k = iStart; k < iEnd; k++)
            {
                pcToken += strBuffer[k];
            }
            strBuffer = strBuffer.Remove(0, iEnd);
            return pcToken;
        }

        static public bool LoadOldSetFile(string strSetPath,
            string strMapPath)
        {
            if (Instance._LoadOldSetFile(strSetPath) == false)
                return false;
            if (Instance._LoadOldMapFile(strMapPath) == false)
                return false;
            AppEvents.RaiseFrameworkDataChanged();
            return true;
        }

        private bool _LoadOldSetFile(string strSetPath)
        {
            try 
            {
                if (File.Exists(strSetPath))
                {
                    StreamReader sr = new StreamReader(strSetPath);
                    string strBuf;
                    string strSeps = "#\n";
                    string strCurFolder;
                    string strCurGroupFolder;
                    ProjectTreeNode kCurrentSequenceFolder =
                        GetRootSequenceNode();
                    ProjectTreeNode kCurrentSequenceGroupFolder =
                        GetRootSequenceGroupNode();

                    string strVersion;
                    bool bOldFile = false;

                    strBuf = sr.ReadLine();
                    string strSetHeader = ";Gamebryo SET File Version ";
                    string strOldSetHeader = ";AnImmerse Version ";
                    if (strBuf.StartsWith(strSetHeader))
                    {
                        strBuf = strBuf.Remove(0, strSetHeader.Length);
                        strVersion = strBuf.Substring(0, 3);
                        strBuf = strBuf.Remove(0, 3);
                    }
                    else if (strBuf.StartsWith(strOldSetHeader))
                    {
                        strBuf = strBuf.Remove(0, strOldSetHeader.Length);
                        strVersion = strBuf.Substring(0, 3);
                        strBuf = strBuf.Remove(0, 3);
                        bOldFile = true;
                    }
                    else
                    {
                        return false;
                    }

                    int iVersion = MUtility.GetVersionFromString(strVersion);
                    if (bOldFile && (iVersion < MUtility.GetVersionFromString(
                        "1.1") ||
                        iVersion > MUtility.GetVersionFromString("5.0")))
                    {
                        return false;
                    }
                    else if (!bOldFile && iVersion > 
                        MUtility.GetVersionFromString("1.0"))
                    {
                        return false;
                    }

                    while ((strBuf = sr.ReadLine()) != null)
                    {
                        if (strBuf.Length == 0)
                            continue;

                        string strToken = strtok(ref strBuf, strSeps);


                        if (strToken == null)
                            continue; 
                        
                        if (strToken == "END_SET_FILE")
                        {
                            sr.Close();
                            ProjectData.Instance.SetNeedToSave(false);
                            return true;
                        }

                        int iIndex = strToken.IndexOf(';');
                        if (iIndex == 0)
                            continue;

                        switch (strToken)
                        {
                            case "SCENEMODEL":
                            {
                                string strScenePath  = strtok(ref strBuf,
                                    strSeps);
                                if (strScenePath == null)
                                    return false;
                                
                                BackgroundFilename =
                                    ms_strOriginalBackgroundFilename;
                                break;
                            }
                            case "SCENEHEIGHT":
                            {
                                string strSceneHeight = strtok(ref strBuf,
                                    strSeps);
                                if (strSceneHeight == null)
                                    return false;
                                if (strSceneHeight == "NULL")
                                {
                                    BackgroundHeight = 0.0f;
                                }
                                else
                                {
                                    BackgroundHeight = Convert.ToSingle(
                                        strSceneHeight);
                                }
                                break;
                            }
                            case "ANIMSPEED":
                            {
                                string strAnimSpeed = strtok(ref strBuf,
                                    strSeps);
                                if (strAnimSpeed == null)
                                    return false;
                                
                                break;
                            }
                            case "UPAXIS":
                            {
                                string strUpAxis = strtok(ref strBuf,
                                    strSeps);
                                if (strUpAxis == null)
                                    return false;
                                
                                UpAxis = (MCamera.UpAxisType) Convert.ToInt32(
                                    strUpAxis);
                                break;
                            }
                            case "FOLDER":
                            {
                                strToken = strtok(ref strBuf, strSeps);
                                if (strToken == null)
                                    return false;
                                
                                strCurFolder = strToken;
                                if (strCurFolder == "<None>")
                                {
                                    kCurrentSequenceFolder =
                                        GetRootSequenceNode();
                                }
                                else
                                {
                                    kCurrentSequenceFolder = CreateNewFolder(
                                        strCurFolder, GetRootSequenceNode());
                                }
                                //bAnimFolder = true;
                                break;
                            }
                            case "\tANIMATION":
                            {
                                strToken = strtok(ref strBuf, strSeps);
                                if (strToken == null)
                                    return false;
                                
                                ProjectTreeNode kSequenceTree =
                                    GetRootSequenceNode();
                                foreach (ProjectTreeNode kNode in
                                    kSequenceTree.Nodes)
                                {
                                    if (kNode.NodeType == NodeType.Sequence)
                                    {
                                        if (((MSequence)(kNode.Tag)).Name ==
                                            strToken)
                                        {
                                            kSequenceTree.Nodes.Remove(kNode);
                                            kCurrentSequenceFolder.Nodes.Add(
                                                kNode);
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                            case "GROUPFOLDER":
                            {
                                strToken = strtok(ref strBuf, strSeps);
                                if (strToken == null)
                                    return false;

                                strCurGroupFolder = strToken;
                                if (strCurGroupFolder == "<None>")
                                {
                                    kCurrentSequenceGroupFolder =
                                        GetRootSequenceGroupNode();
                                }
                                else
                                {
                                    kCurrentSequenceGroupFolder =
                                        CreateNewFolder(strCurGroupFolder,
                                        GetRootSequenceGroupNode());
                                }
                                    
                                break;
                            }
                            case "\tLAYERGROUP":
                            {
                                strToken = strtok(ref strBuf, strSeps);
                                if (strToken == null)
                                    return false;

                                ProjectTreeNode kSequenceGroupTree =
                                    GetRootSequenceGroupNode();
                                foreach (ProjectTreeNode kNode in
                                    kSequenceGroupTree.Nodes)
                                {
                                    if (kNode.NodeType == NodeType
                                        .SequenceGroup)
                                    {
                                        if (((MSequenceGroup)(kNode.Tag)).Name
                                            == strToken)
                                        {
                                            kSequenceGroupTree.Nodes.Remove(
                                                kNode);
                                            kCurrentSequenceGroupFolder.Nodes
                                                .Add(kNode);
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    }
                    return false;
                }
                else
                {
                    return false;
                }
            }
            catch(Exception)
            {
                return false;
            }            
        }

        private static object StringToEnum( Type t, string Value )
        {
            foreach ( FieldInfo fi in t.GetFields() )
            {
                if ( fi.Name == Value )
                    return fi.GetValue( null );    // We use null because
            }
            // enumeration values
            // are static

            throw new Exception(
                string.Format("Can't convert {0} to {1}", Value,
                t.ToString()) );
        }

        private bool _IsValidOldMapCode(uint uiInputCode)
        {
            if (uiInputCode == 0)
                return false;
            else if (uiInputCode == 0x06)
                return false;
            else if (uiInputCode == 0x05)
                return false;
            return true;
        }

        private bool _LoadOldMapFile(string strMapPath)
        {
            try 
            {
                if (File.Exists(strMapPath))
                {
                    StreamReader sr = new StreamReader(strMapPath);
                    string strBuf;
                    string strSeps = " #\n";
                    string strVersion;
                    bool bOldFile = false;

                    strBuf = sr.ReadLine();
                    string strMapHeader = ";Gamebryo MAP File Version ";
                    string strOldMapHeader = ";AnImmerse Version ";
                    if (strBuf.StartsWith(strMapHeader))
                    {
                        strBuf = strBuf.Remove(0, strMapHeader.Length);
                        strVersion = strBuf.Substring(0, 3);
                        strBuf = strBuf.Remove(0, 3);
                    }
                    else if (strBuf.StartsWith(strOldMapHeader))
                    {
                        strBuf = strBuf.Remove(0, strOldMapHeader.Length);
                        strVersion = strBuf.Substring(0, 3);
                        strBuf = strBuf.Remove(0, 3);
                        bOldFile = true;
                    }
                    else
                    {
                        return false;
                    }

                    int iVersion = MUtility.GetVersionFromString(strVersion);
                    if (bOldFile && (iVersion < MUtility.GetVersionFromString(
                        "1.1") || iVersion > MUtility.GetVersionFromString(
                        "5.0")))
                    {
                        return false;
                    }
                    else if (!bOldFile && iVersion > 
                        MUtility.GetVersionFromString("1.0"))
                    {
                        return false;
                    }

                    while ((strBuf = sr.ReadLine()) != null)
                    {
                        if (strBuf.Length == 0)
                            continue;

                        string strToken = strtok(ref strBuf, strSeps);

                        switch (strToken)
                        {
                            case "END_MAP_FILE":
                                sr.Close();
                                ProjectData.Instance.SetNeedToSave(false);
                                return true;
                            case "EVENTCODE":
                            {
                                string strEventCode = strtok(ref strBuf,
                                    strSeps);
                                if (strEventCode == null)
                                    return false;
                                uint uiEventCode = (uint)System.Convert
                                    .ToInt32(strEventCode);
                                MSequence kSequence = 
                                    MFramework.Instance.Animation.GetSequence(
                                    uiEventCode);
                                
                                if (kSequence == null)
                                    continue;
                                
                                strToken = strtok(ref strBuf, strSeps);
                                if (strToken == null ||
                                    strToken != "INPUTCODE")
                                {
                                    return false;
                                }
                                
                                string strInputCode = strtok(ref strBuf,
                                    strSeps);
                                if (strInputCode == null)
                                    return false;
                                uint uiInputCode = (uint)System.Convert
                                    .ToInt32(strInputCode);
                                
                                strToken = strtok(ref strBuf, strSeps);
                                if (strToken == null ||
                                    strToken != "MODIFIER")
                                {
                                    return false;
                                }
                                
                                string strModifier = strtok(ref strBuf,
                                    strSeps);
                                if (strModifier == null)
                                    return false;
                                uint uiModifier = (uint)System.Convert
                                    .ToInt32(strModifier);
                                
                                if (!_IsValidOldMapCode(uiInputCode))
                                    continue;

                                SeqInputInfo kInputInfo = new SeqInputInfo(
                                    kSequence);
                                Keys eKeyData;
                                eKeyData = (Keys)uiInputCode;
                                kInputInfo.m_strKeyStroke = eKeyData
                                    .ToString();

                                if (_IsValidOldMapCode(uiModifier))
                                {
                                    Keys eModifierData = Keys.None;
                                    switch((Keys)uiModifier)
                                    {
                                        case Keys.Menu:
                                            eModifierData = Keys.Alt; 
                                            break;
                                        case Keys.ControlKey: 
                                            eModifierData = Keys.Control;
                                            break;
                                        case Keys.ShiftKey: 
                                            eModifierData = Keys.Shift;
                                            break; 
                                        default:
                                        {
                                            string strWarning =
                                            "Unable to convert old key code " +
                                            "modifier for sequence \"" +
                                            kSequence.Name + "\". This " + 
                                            "sequence will not be assigned " +
                                            "a keycode.";
                                            MLogger.LogWarning(strWarning);
                                            continue;
                                        }
                                    }
                                    if (eModifierData != Keys.None)
                                    {
                                        eKeyData |= eModifierData;
                                        string strMungedKeyCode = eKeyData
                                            .ToString();
                                        kInputInfo.m_strModifier =
                                            eModifierData.ToString();
                                    }
                                }
                                

                                kInputInfo.m_eKeyStroke = eKeyData;

                                Hashtable kHash = GetGlobalInputHash();
                                if (kHash.ContainsKey(uiEventCode) == false)
                                {
                                    kHash.Add(uiEventCode, kInputInfo);
                                }
                                else
                                {
                                    SeqInputInfo kEntry = (ProjectData
                                        .SeqInputInfo) kHash[uiEventCode];
                                    kEntry.m_bSync = kInputInfo.m_bSync;
                                    kEntry.m_eKeyStroke =
                                        kInputInfo.m_eKeyStroke;
                                    kEntry.m_strKeyStroke =
                                        kInputInfo.m_strKeyStroke;
                                    kEntry.m_strModifier =
                                        kInputInfo.m_strModifier;
                                }
                                break;                           
                            }
                        }

                    }
                    return false;
                }
                else
                {
                    return false;
                }
            }
            catch(Exception)
            {
                return false;
            }            
        }

        public bool LoadRegistrySettings()
        {
            string registryPath = RegistryPath + "\\Settings";
            RegistryKey BaseKey;
            
            try 
            {
                BaseKey = Registry.CurrentUser.OpenSubKey(registryPath);
            }
            catch (Exception)
            {
                return false;
            }
        
            if (BaseKey == null)
                return false;

            object kValue = BaseKey.GetValue("UpAxis", (int)UpAxis);
            UpAxis = (MCamera.UpAxisType) Convert.ToInt32(kValue);
            DefaultUpAxis = UpAxis;
            kValue = BaseKey.GetValue("InvertPan", InvertPan);
            InvertPan = Convert.ToBoolean(kValue);
            DefaultInvertPan = InvertPan;
            kValue = BaseKey.GetValue("RendererClearColor.R",
                RendererClearColor.r);
            MColor kColor = new MColor(0.0f, 0.0f, 0.0f);
            kColor.r =  Convert.ToSingle(kValue);
            kValue = BaseKey.GetValue("RendererClearColor.G",
                RendererClearColor.g);
            kColor.g =  Convert.ToSingle(kValue);
            kValue = BaseKey.GetValue("RendererClearColor.B",
                RendererClearColor.b);
            kColor.b =  Convert.ToSingle(kValue);
            RendererClearColor = kColor;
            BackgroundHeight = Convert.ToSingle(
                BaseKey.GetValue("BackgroundHeight", BackgroundHeight));
            BackgroundFilename = (string)
                BaseKey.GetValue("BackgroundFilename", BackgroundFilename);

            kValue = BaseKey.GetValue("PhysicsDebugEnabled",
                PhysicsDebugEnabled);
            PhysicsDebugEnabled = Convert.ToBoolean(kValue);
            DefaultPhysicsDebugEnabled = PhysicsDebugEnabled;
            kValue = BaseKey.GetValue("PhysicsDebugScale",
                PhysicsDebugScale);
            PhysicsDebugScale = Convert.ToSingle(kValue);
            DefaultPhysicsDebugScale = PhysicsDebugScale;
            kValue = BaseKey.GetValue("PhysicsTimestep",
                PhysicsTimestep);
            PhysicsTimestep = Convert.ToSingle(kValue);
            DefaultPhysicsTimestep = PhysicsTimestep;
            kValue = BaseKey.GetValue("PhysicsGravityX",
                PhysicsGravityX);
            PhysicsGravityX = Convert.ToSingle(kValue);
            DefaultPhysicsGravityX = PhysicsGravityX;
            kValue = BaseKey.GetValue("PhysicsGravityY",
                PhysicsGravityY);
            PhysicsGravityY = Convert.ToSingle(kValue);
            DefaultPhysicsGravityY = PhysicsGravityY;
            kValue = BaseKey.GetValue("PhysicsGravityZ",
                PhysicsGravityZ);
            PhysicsGravityZ = Convert.ToSingle(kValue);
            DefaultPhysicsGravityZ = PhysicsGravityZ;
            kValue = BaseKey.GetValue("PhysicsGroundFriction",
                PhysicsGroundFriction);
            PhysicsGroundFriction = Convert.ToSingle(kValue);
            DefaultPhysicsGroundFriction = PhysicsGroundFriction;
            kValue = BaseKey.GetValue("PhysicsGroundBounce",
                PhysicsGroundBounce);
            PhysicsGroundBounce = Convert.ToSingle(kValue);
            DefaultPhysicsGroundBounce = PhysicsGroundBounce;
            kValue = BaseKey.GetValue("PhysicsForcePower",
                PhysicsForcePower);
            PhysicsForcePower = Convert.ToSingle(kValue);
            DefaultPhysicsForcePower = PhysicsForcePower;
            
            for (int i = 0; i < ms_iNumTransitionTypes; i++)
            {
                m_astrTransitionTypeNames[i] = (string) BaseKey.GetValue(
                    "TransitionTypeNames[" + i.ToString() + "]",
                    m_astrTransitionTypeNames[i]);
                int R =  Convert.ToInt32(BaseKey.GetValue(
                    "TransitionTypeBackColors[" + i.ToString() + "].r",
                    m_akTransitionTypeBackColors[i].R));
                int G  = Convert.ToInt32(BaseKey.GetValue(
                    "TransitionTypeBackColors[" + i.ToString() + "].g",
                    m_akTransitionTypeBackColors[i].G));
                int B  = Convert.ToInt32(BaseKey.GetValue(
                    "TransitionTypeBackColors[" + i.ToString() + "].b",
                    m_akTransitionTypeBackColors[i].B));
                System.Drawing.Color kBackColor = 
                    System.Drawing.Color.FromArgb(R,G,B);
                ProjectData.SetColorForTransitionType(i, kBackColor);

                R = Convert.ToInt32(BaseKey.GetValue(
                    "TransitionTypeForeColors[" + i.ToString() + "].r",
                    m_akTransitionTypeForeColors[i].R));
                G = Convert.ToInt32(BaseKey.GetValue(
                    "TransitionTypeForeColors[" + i.ToString() + "].g",
                    m_akTransitionTypeForeColors[i].G));
                B = Convert.ToInt32(BaseKey.GetValue(
                    "TransitionTypeForeColors[" + i.ToString() + "].b",
                    m_akTransitionTypeForeColors[i].B));
                System.Drawing.Color kForeColor = 
                    System.Drawing.Color.FromArgb(R,G,B);
                ProjectData.SetTextColorForTransitionType(i, kForeColor);
            }
 
            BaseKey.Close();
            return true;
        }

        #endregion

        #region Folder Manipulation

        private static bool AddSequenceToSequenceGroup(
            ProjectTreeNode SequenceNode, ProjectTreeNode SequenceGroupNode)
        {
            if (SequenceGroupNode.NodeType != NodeType.SequenceGroup)
                return false;

            if (SequenceNode.NodeType != NodeType.Sequence)
                return false;

            MSequenceGroup SequenceGroup = ((MSequenceGroup) SequenceGroupNode
                .Tag);
            MSequence Sequence = ((MSequence)SequenceNode.Tag);

            return SequenceGroup.AddSequence(Sequence.SequenceID);
        }

        private static bool AddSequenceFolderToSequenceGroup(
            ProjectTreeNode SequenceFolderNode,
            ProjectTreeNode SequenceGroupNode)
        {
            if (SequenceGroupNode.NodeType != NodeType.SequenceGroup)
                return false;

            if (SequenceFolderNode.NodeType != NodeType.SequenceFolder &&
                SequenceFolderNode.NodeType != NodeType.SequenceFolderRoot)
                return false;

            foreach (ProjectTreeNode child in SequenceFolderNode.Nodes)
            {
                if (child.NodeType == NodeType.Sequence)
                {
                    AddSequenceToSequenceGroup(child, SequenceGroupNode);
                }
                else if (child.NodeType == NodeType.SequenceFolder)
                {
                    AddSequenceFolderToSequenceGroup(child,
                        SequenceGroupNode);
                }
            }

            return true;
        }

        public static bool MoveToFolder(ProjectTreeNode Target, 
            ProjectTreeNode Moving)
        {
            // Ensure that the moving object is of a compatable type
            // with the ToFolder

            NodeType ToFolderNodeType = Target.NodeType; 
            NodeType MovingNodeType = Moving.NodeType; 

            if ((MovingNodeType == NodeType.Sequence || 
                MovingNodeType == NodeType.SequenceFolder) &&
                (ToFolderNodeType == NodeType.SequenceFolder || 
                ToFolderNodeType == NodeType.SequenceFolderRoot))
            {
                // Move Sequences and SequenceFolders into either
                // existing SequenceFolders or the SequenceFolderRoot
                Moving.Remove();
                Target.Nodes.Add(Moving);
                ProjectData.Instance.SetNeedToSave(true);
                return true;
            }
            else if ((MovingNodeType == NodeType.SequenceGroup || 
                MovingNodeType == NodeType.SequenceGroupFolder) &&
                (ToFolderNodeType == NodeType.SequenceGroupFolder || 
                ToFolderNodeType == NodeType.SequenceGroupFolderRoot))
            {
                // Move SequenceGroups and SequenceGroupFolders into either
                // existing SequenceGroupFolders or the
                // SequenceGroupFolderRoot
                Moving.Remove();
                Target.Nodes.Add(Moving);
                ProjectData.Instance.SetNeedToSave(true);
                return true;
            }
            else if ((MovingNodeType == NodeType.Sequence || 
                MovingNodeType == NodeType.SequenceFolder ||
                MovingNodeType == NodeType.SequenceFolderRoot) &&
                (ToFolderNodeType == NodeType.SequenceGroup))
            {
                // Add a sequence or all sequences in a sequence folder
                // to the target sequence group

                if (MovingNodeType == NodeType.Sequence)
                {
                    return AddSequenceToSequenceGroup(Moving,
                        Target);
                }
                else if (MovingNodeType == NodeType.SequenceFolder)
                {
                    return AddSequenceFolderToSequenceGroup(Moving,
                        Target);
                }
                else if (MovingNodeType == NodeType.SequenceFolderRoot)
                {
                    return AddSequenceFolderToSequenceGroup(Moving,
                        Target);
                }
            }

            return false;
        }

        static public ProjectTreeNode CreateNewFolder(string strFolderName,
            ProjectTreeNode kPTN)
        {
            // Folders can only be created inside other folders and only of
            // the same type.
            if (kPTN.IsFolder() == false)
                return null;

            NodeType folderType; 

            switch(kPTN.NodeType)
            {
                case NodeType.SequenceFolderRoot:
                    folderType = NodeType.SequenceFolder; 
                    break;
                case NodeType.SequenceGroupFolderRoot:
                    folderType = NodeType.SequenceGroupFolder;
                    break;
                default:
                    folderType = kPTN.NodeType;
                    break;
            }

            ProjectTreeNode kNewPTN = new ProjectTreeNode(strFolderName,
                folderType);
            kPTN.Nodes.Add(kNewPTN);
            ProjectData.Instance.SetNeedToSave(true);
            return kNewPTN;
        }
        #endregion

        #region Framework interaction
        
        public void AddSequences(ArrayList aMSeq, ProjectTreeNode kPTN)
        {
            if (aMSeq == null || aMSeq.Count == 0)
                return; 

            // add a new ProjectTreeNode with the correct type
            foreach(MSequence kSeq in aMSeq)
            {
                // From the properties of the MSequence
                // Create a ProjectTreeNode for each into an array.
                // When we create the kNewPTN, we'll pass in this
                // child array.
                
                AddSequence(kSeq, kPTN);
            }
        }

        public void AddSequence(MSequence kSeq, ProjectTreeNode kPTN)
        {
            ProjectTreeNode kNewPTN = new ProjectTreeNode(kSeq.Name, 
                NodeType.Sequence, kSeq); // Reference to the Sequence

            kPTN.Nodes.Add(kNewPTN);
        }

        public void AddSequenceGroups(ArrayList aMSeqGrp,
            ProjectTreeNode kPTN)
        {
            if (aMSeqGrp == null || aMSeqGrp.Count == 0)
                return;

            foreach(MSequenceGroup kSeqGrp in aMSeqGrp)
            {
                // From the properties of each MSequenceGroup
                // Create a ProjectTreeNode for each into an array.
                // When we create the kNewPTN, we'll pass in this
                // child array.
                
                AddSequenceGroup(kSeqGrp, kPTN);
            }
        }

        public void AddSequenceGroup(MSequenceGroup kSeqGroup,
            ProjectTreeNode kPTN)
        {
            ProjectTreeNode kNewPTN =
                new ProjectTreeNode(kSeqGroup.Name, NodeType.SequenceGroup,
                kSeqGroup); // Reference to the SequenceGroup.

            kPTN.Nodes.Add(kNewPTN);
        }
        

        static public bool DoesSeqIDExist(uint uiID)
        {
            Hashtable hash = GetGlobalInputHash();           
            return (hash.ContainsKey(uiID));
        }

        static public bool DoesGroupIDExist(uint uiID)
        {
            Hashtable hash = GetGroupHash();
            return (hash.ContainsKey(uiID));
        }

        public class SeqInputInfo
        {
            public SeqInputInfo(MSequence kSequence)
            {
                m_Seq = kSequence;
                m_strKeyStroke = null;
                m_strModifier = null;
                m_bSync = false;
                //m_eKeyStroke = Keys.None;
            }

            public MSequence m_Seq; // The sequence we point to.
            public string m_strKeyStroke;
            public string m_strModifier;
            public bool m_bSync; // for use in groups
            public Keys m_eKeyStroke;
        }

        public class GrpSeqInputInfo
        {
            public MSequenceGroup m_SeqGrp;

            public MSequence m_seqSynchronize;           
            private Hashtable m_hashLocalInput; 
            
            // Accessor property
            public Hashtable GrpSeqInputInfoHash
            {
                get
                {
                    return m_hashLocalInput;
                }
                set
                {
                    m_hashLocalInput = value;
                }
            }

            public GrpSeqInputInfo(MSequenceGroup seqGrp)
            {
                m_SeqGrp = seqGrp;

                m_seqSynchronize = null;
                m_hashLocalInput = new Hashtable();

                MSequenceGroup.MSequenceInfo[] aSeqInfo = seqGrp.SequenceInfo;

                if (aSeqInfo == null)
                    return;

                foreach (MSequenceGroup.MSequenceInfo SeqInfo in aSeqInfo)
                {
                    uint uiCurrSeqID = SeqInfo.SequenceID;
                    MSequence seq = MFramework.Instance.Animation.GetSequence(
                        uiCurrSeqID);

                    SeqInputInfo kEntry = new SeqInputInfo(seq);
                    m_hashLocalInput.Add(uiCurrSeqID, kEntry);
                }
            }
        }

        static public Hashtable GetSeqInputInfoForGroup(uint uiGroupID)
        {
            Hashtable ht = ProjectData.GetGroupHash();
            
            ProjectData.GrpSeqInputInfo grpInfo = 
                (ProjectData.GrpSeqInputInfo)ht[uiGroupID];
        
            return grpInfo.GrpSeqInputInfoHash;
        }

        #endregion
    }
}
