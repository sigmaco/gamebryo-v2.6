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
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;

using NiManagedToolInterface;

using SourceGrid2;
using SourceGrid2.Cells;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for WorkSheet.
    /// </summary>
    public class WorkSheet : System.Windows.Forms.UserControl
    {
        static bool sbFrameWorkInit = false;

        public class TransitionCellData 
        {
            public MTransitionData TransitionData;
            public string m_strText;
        }

        
        public class TransitionGrid : SourceGrid2.GridVirtual
        {
            public TransitionGrid() : base()
            {
            }

            public void Finish()
            {
                Selection.Finish();
            }

            protected override void PaintCell(
                GridSubPanel p_Panel,PaintEventArgs e, ICellVirtual p_Cell,
                Position p_CellPosition, Rectangle p_PanelDrawRectangle)
            {
                if (this.DesignMode)
                    return;

                p_Cell.VisualModel.MakeReadWrite();
               
                if (p_CellPosition.Column > 0 && p_CellPosition.Row > 0)
                {         
                    {
                        WorkSheet ws = WorkSheet.Get();

                        if (ws == null) 
                            return;

                        TransitionCellData data = 
                            ws.m_Data[p_CellPosition.Row-1, 
                                      p_CellPosition.Column-1];

                        if (data == null)
                            return;

                        MTransition kTran = data.TransitionData.Transition;

                        System.Drawing.Color kColor = 
                            WorkSheet.Instance.grid.BackColor;
                        System.Drawing.Color kTextColor = 
                            System.Drawing.Color.Black;
                        if (kTran != null)
                        {
                            kColor = ProjectData.GetColorForTransition(kTran);
                            kTextColor = 
                                ProjectData.GetTextColorForTransition(kTran);
                        }
                        else
                        {
                            kColor = ProjectData.GetColorForTransitionType(
                                MTransition.TransitionType.Trans_None);
                            kTextColor = 
                                ProjectData.GetTextColorForTransitionType(
                                MTransition.TransitionType.Trans_None);
                        }
                    
                        p_Cell.VisualModel.BackColor = kColor;
                        p_Cell.VisualModel.ForeColor = kTextColor;
                    }
                }
                
                p_Cell.VisualModel.DrawCell(p_Cell, p_CellPosition, e, 
                    p_PanelDrawRectangle);
                if (p_CellPosition.Column == p_CellPosition.Row && 
                    p_CellPosition.Column > 0 && p_CellPosition.Row > 0)
                {     
                    System.Drawing.Color kNewColor = 
                        Color.FromArgb(207,207,207); 
                   
                    using (System.Drawing.Pen pen = new Pen(kNewColor))
                    {
                        e.Graphics.DrawLine(pen, 
                            new System.Drawing.Point(
                                p_PanelDrawRectangle.Left, 
                                p_PanelDrawRectangle.Top),
                            new System.Drawing.Point(
                                p_PanelDrawRectangle.Right, 
                                p_PanelDrawRectangle.Bottom));
                        e.Graphics.DrawLine(pen, 
                            new System.Drawing.Point(p_PanelDrawRectangle.Left,
                            p_PanelDrawRectangle.Bottom),
                            new System.Drawing.Point(
                                p_PanelDrawRectangle.Right, 
                                p_PanelDrawRectangle.Top));
                    }
                }
                else if (p_CellPosition.Column == 0 && p_CellPosition.Row == 0)
                {
                    System.Drawing.Image image = 
                        WorkSheet.Instance.ilTableCellImages.Images[0];
                    int x = (p_PanelDrawRectangle.Width - image.Width)/2;
                    int y = (p_PanelDrawRectangle.Height - image.Height)/2;
                    if (x < 0)
                        x = 0;
                    if (y < 0)
                        y = 0;

                    e.Graphics.DrawImageUnscaled(image, 
                        p_PanelDrawRectangle.Left + x, 
                        p_PanelDrawRectangle.Top + y);
                }
            }
        }

        //private SourceGrid2.GridVirtual grid;
        private TransitionGrid grid;
        private System.ComponentModel.IContainer components;
        
        //private string[,] m_Data;
        private TransitionCellData[,] m_Data;
        private ArrayList TransitionList;
        private WorkSheetCell m_Cell;
        private WorkSheetColumn m_Column;
        private WorkSheetRow m_Row;
        private WorkSheetHeader m_Header;
        private System.Windows.Forms.ContextMenu ContextMenu_TableSettings;
        private System.Windows.Forms.MenuItem MenuItem_TableSettings;
        private System.Windows.Forms.MenuItem menuItem2;
        private System.Windows.Forms.MenuItem MenuItem_Delete;
        private System.Windows.Forms.MenuItem MenuItem_Immediate;
        private System.Windows.Forms.MenuItem MenuItem_Delayed;
        private System.Windows.Forms.MenuItem MenuItem_Morph;
        private System.Windows.Forms.MenuItem MenuItem_CrossFade;
        private System.Windows.Forms.MenuItem MenuItem_Chain;
        private System.Windows.Forms.MenuItem MenuItem_Sync;
        private System.Windows.Forms.MenuItem MenuItem_NonSync;
        private System.Windows.Forms.MenuItem menuItem3;
        private System.Windows.Forms.MenuItem miHelpRoot;
        private System.Windows.Forms.MenuItem miTransitionTypeOverview;
        private System.Windows.Forms.MenuItem miTransitionTableOverview;
        private SourceGrid2.Position CurrentMouseCell;
        private System.Windows.Forms.MenuItem miSelectAll;
        private static uint NumInstances = 0;
        private System.Windows.Forms.ImageList ilTableCellImages;
        private System.Windows.Forms.MenuItem miDefSync;
        private System.Windows.Forms.MenuItem miDefNonSync;

        static private WorkSheet ms_kThis = null;

        static public WorkSheet Create()
        {
            ms_kThis = new WorkSheet();
            return ms_kThis;
        }

        static public WorkSheet Get()
        {
            return ms_kThis;
        }

        static public WorkSheet Instance
        {
            get {return ms_kThis;}
        }

        public WorkSheet()
        {
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            if (this.DesignMode)
                return;

            CurrentMouseCell = SourceGrid2.Position.Empty;
            sbFrameWorkInit = true;

            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            grid.FixedRows = 1;
            grid.FixedColumns = 1;
            grid.Redim(30,30);
            grid.GridToolTipActive = true;
            grid.GridToolTipText =
                "Click on a cell to preview the transition.\n" +
                "Double-click on a cell to edit the transition type.";
            //m_Data = new string[30, 30];
            m_Data = new TransitionCellData[30,30];

            m_Cell = new WorkSheetCell(this);
            m_Cell.BindToGrid(grid);
            m_Column = new WorkSheetColumn();
            m_Column.BindToGrid(grid);
            m_Row = new WorkSheetRow();
            m_Row.BindToGrid(grid);
            m_Header = new WorkSheetHeader();
                       
            m_Header.BindToGrid(grid);
            TransitionList = null;

            grid.Selection.BorderMode = SelectionBorderMode.Auto;
            System.Drawing.Color kColor = 
                System.Drawing.Color.FromArgb(128, System.Drawing.Color.Gray);
            grid.Selection.FocusBackColor = kColor;
            grid.Selection.BackColor = kColor;
            grid.Selection.SelectionChange +=
                new SelectionChangeEventHandler(Selection_SelectionChange);
            grid.Selection.FinishOccurred +=
                new FinishOccurredEventHandler(Selection_FinishOccurred);
            AppEvents.FrameworkDataChanged +=
                new AppEvents.EventHandler_FrameworkDataChanged(
                    OnFrameworkDataChangedHandler);

            AppEvents.HandleTypeConversion +=
                new AnimationToolUI.
                AppEvents.EventHandler_HandleTypeConversion(
                OnHandleTypeConversion);

            MFramework.Instance.Animation.OnTransitionModified +=
                new MAnimation.__Delegate_OnTransitionModified(
                OnTransitionModified);
            MFramework.Instance.Animation.OnTransitionRemoved +=
                new MAnimation.__Delegate_OnTransitionRemoved(
                OnTransitionRemoved);
            MFramework.Instance.Animation.OnTransitionAdded +=
                new MAnimation.__Delegate_OnTransitionAdded(OnTransitionAdded);
            AppEvents.TransitionTableSettingsChanged += 
                new AppEvents.EventHandler_TransitionTableSettingsChanged(
                OnTransitionTableSettingsChanged);
            AppEvents.TransitionsPreviewSelected +=
                new AppEvents.EventHandler_TransitionsPreviewSelected(
                this.OnTransitionsPreviewSelected);
        }

        public void SetCurrentMouseCell(SourceGrid2.Position kPos)
        {

            SourceGrid2.Position kOldMousePos = CurrentMouseCell;
            CurrentMouseCell = kPos;

            if (m_Data != null && kPos != SourceGrid2.Position.Empty)
            {
                TransitionCellData kData = m_Data[kPos.Row-1, kPos.Column-1];
                SourceGrid2.Position kColPos = 
                    new SourceGrid2.Position(0, kPos.Column);
                SourceGrid2.Position kRowPos = 
                    new SourceGrid2.Position(kPos.Row, 0);

                ICellVirtual kColCell = grid.GetCell(kColPos);
                ICellVirtual kRowCell = grid.GetCell(kRowPos);

                kColCell.Invalidate(kColPos);
                kRowCell.Invalidate(kRowPos);
            }

            if (kOldMousePos != SourceGrid2.Position.Empty)
            {
                SourceGrid2.Position kOldColPos = 
                    new SourceGrid2.Position(0, kOldMousePos.Column);
                SourceGrid2.Position kOldRowPos = 
                    new SourceGrid2.Position(kOldMousePos.Row, 0);

                ICellVirtual kOldColCell = grid.GetCell(kOldColPos);
                ICellVirtual kOldRowCell = grid.GetCell(kOldRowPos);

                kOldColCell.Invalidate(kOldColPos);
                kOldRowCell.Invalidate(kOldRowPos);
            }
        }

        public SourceGrid2.Position GetCurrentMouseCell()
        {
            return CurrentMouseCell;
        }

        private void OnTransitionTableSettingsChanged()
        {
            Invalidate();
        }

        private void OnTransitionsPreviewSelected()
        {
            if (TransitionList == null || TransitionList.Count == 0)
            {
                grid.Selection.Add(new SourceGrid2.Position(1, 1));
                Selection_FinishOccurred(this, 
                                         new FinishOccurredEventArgs(null));
            }
            else
            {
                AppEvents.RaiseTransitionViewSelectionCountChanged(
                    TransitionList.Count);
                AppEvents.RaiseTransitionViewSelectionChanged(
                    TransitionList);
            }
        }

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                NumInstances--;
                Debug.Assert(NumInstances == 0, 
                    "There should only ever be one un-disposed" + 
                    " version of this class in memory at a time.");
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(WorkSheet));
            this.grid = new AnimationToolUI.WorkSheet.TransitionGrid();
            this.ContextMenu_TableSettings = new System.Windows.Forms.ContextMenu();
            this.MenuItem_Delete = new System.Windows.Forms.MenuItem();
            this.MenuItem_Immediate = new System.Windows.Forms.MenuItem();
            this.MenuItem_Delayed = new System.Windows.Forms.MenuItem();
            this.MenuItem_Morph = new System.Windows.Forms.MenuItem();
            this.MenuItem_CrossFade = new System.Windows.Forms.MenuItem();
            this.MenuItem_Chain = new System.Windows.Forms.MenuItem();
            this.MenuItem_Sync = new System.Windows.Forms.MenuItem();
            this.MenuItem_NonSync = new System.Windows.Forms.MenuItem();
            this.menuItem2 = new System.Windows.Forms.MenuItem();
            this.miSelectAll = new System.Windows.Forms.MenuItem();
            this.MenuItem_TableSettings = new System.Windows.Forms.MenuItem();
            this.miDefSync = new System.Windows.Forms.MenuItem();
            this.miDefNonSync = new System.Windows.Forms.MenuItem();
            this.menuItem3 = new System.Windows.Forms.MenuItem();
            this.miHelpRoot = new System.Windows.Forms.MenuItem();
            this.miTransitionTypeOverview = new System.Windows.Forms.MenuItem();
            this.miTransitionTableOverview = new System.Windows.Forms.MenuItem();
            this.ilTableCellImages = new System.Windows.Forms.ImageList(this.components);
            this.SuspendLayout();
            // 
            // grid
            // 
            this.grid.AutoSizeMinHeight = 10;
            this.grid.AutoSizeMinWidth = 10;
            this.grid.AutoStretchColumnsToFitWidth = false;
            this.grid.AutoStretchRowsToFitHeight = false;
            this.grid.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.grid.ContextMenuStyle = SourceGrid2.ContextMenuStyle.None;
            this.grid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grid.GridToolTipActive = true;
            this.grid.Location = new System.Drawing.Point(0, 0);
            this.grid.Name = "grid";
            this.grid.Size = new System.Drawing.Size(384, 352);
            this.grid.SpecialKeys = SourceGrid2.GridSpecialKeys.Default;
            this.grid.TabIndex = 0;
            this.grid.GettingCell += new SourceGrid2.PositionEventHandler(this.grid_GettingCell);
            this.grid.Paint += new System.Windows.Forms.PaintEventHandler(this.grid_Paint);
            // 
            // ContextMenu_TableSettings
            // 
            this.ContextMenu_TableSettings.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                                      this.MenuItem_Delete,
                                                                                                      this.MenuItem_Immediate,
                                                                                                      this.MenuItem_Delayed,
                                                                                                      this.MenuItem_Morph,
                                                                                                      this.MenuItem_CrossFade,
                                                                                                      this.MenuItem_Chain,
                                                                                                      this.MenuItem_Sync,
                                                                                                      this.MenuItem_NonSync,
                                                                                                      this.menuItem2,
                                                                                                      this.miSelectAll,
                                                                                                      this.MenuItem_TableSettings,
                                                                                                      this.miDefSync,
                                                                                                      this.miDefNonSync,
                                                                                                      this.menuItem3,
                                                                                                      this.miHelpRoot});
            this.ContextMenu_TableSettings.Popup += new System.EventHandler(this.ContextMenu_TableSettings_Popup);
            // 
            // MenuItem_Delete
            // 
            this.MenuItem_Delete.Index = 0;
            this.MenuItem_Delete.Text = "&Delete Transition\tDel";
            this.MenuItem_Delete.Click += new System.EventHandler(this.MenuItem_Delete_Click);
            // 
            // MenuItem_Immediate
            // 
            this.MenuItem_Immediate.Index = 1;
            this.MenuItem_Immediate.Text = "Convert to &Immediate Blend \tI";
            this.MenuItem_Immediate.Click += new System.EventHandler(this.MenuItem_Immediate_Click);
            // 
            // MenuItem_Delayed
            // 
            this.MenuItem_Delayed.Index = 2;
            this.MenuItem_Delayed.Text = "Convert to &Delayed Blend\tD";
            this.MenuItem_Delayed.Click += new System.EventHandler(this.MenuItem_Delayed_Click);
            // 
            // MenuItem_Morph
            // 
            this.MenuItem_Morph.Index = 3;
            this.MenuItem_Morph.Text = "Convert to &Morph\tM";
            this.MenuItem_Morph.Click += new System.EventHandler(this.MenuItem_Morph_Click);
            // 
            // MenuItem_CrossFade
            // 
            this.MenuItem_CrossFade.Index = 4;
            this.MenuItem_CrossFade.Text = "Convert to Cross &Fade\tF";
            this.MenuItem_CrossFade.Click += new System.EventHandler(this.MenuItem_CrossFade_Click);
            // 
            // MenuItem_Chain
            // 
            this.MenuItem_Chain.Index = 5;
            this.MenuItem_Chain.Text = "Convert to &Chain\tC";
            this.MenuItem_Chain.Click += new System.EventHandler(this.MenuItem_Chain_Click);
            // 
            // MenuItem_Sync
            // 
            this.MenuItem_Sync.Index = 6;
            this.MenuItem_Sync.Text = "Convert to Default &Synchronized\tS";
            this.MenuItem_Sync.Click += new System.EventHandler(this.MenuItem_Sync_Click);
            // 
            // MenuItem_NonSync
            // 
            this.MenuItem_NonSync.Index = 7;
            this.MenuItem_NonSync.Text = "Convert to Default &Non-Synchronized\tN";
            this.MenuItem_NonSync.Click += new System.EventHandler(this.MenuItem_NonSync_Click);
            // 
            // menuItem2
            // 
            this.menuItem2.Index = 8;
            this.menuItem2.Text = "-";
            // 
            // miSelectAll
            // 
            this.miSelectAll.Index = 9;
            this.miSelectAll.Text = "Select All";
            this.miSelectAll.Click += new System.EventHandler(this.miSelectAll_Click);
            // 
            // MenuItem_TableSettings
            // 
            this.MenuItem_TableSettings.Index = 10;
            this.MenuItem_TableSettings.Text = "&Edit Table Settings";
            this.MenuItem_TableSettings.Click += new System.EventHandler(this.MenuItem_TableSettings_Click);
            // 
            // miDefSync
            // 
            this.miDefSync.Index = 11;
            this.miDefSync.Text = "Edit Default Synchronized Settings";
            this.miDefSync.Click += new System.EventHandler(this.miDefSync_Click);
            // 
            // miDefNonSync
            // 
            this.miDefNonSync.Index = 12;
            this.miDefNonSync.Text = "Edit Default Non-Synchronized Settings";
            this.miDefNonSync.Click += new System.EventHandler(this.miDefNonSync_Click);
            // 
            // menuItem3
            // 
            this.menuItem3.Index = 13;
            this.menuItem3.Text = "-";
            // 
            // miHelpRoot
            // 
            this.miHelpRoot.Index = 14;
            this.miHelpRoot.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                       this.miTransitionTypeOverview,
                                                                                       this.miTransitionTableOverview});
            this.miHelpRoot.Text = "Help...";
            // 
            // miTransitionTypeOverview
            // 
            this.miTransitionTypeOverview.Index = 0;
            this.miTransitionTypeOverview.Text = "Transition Type Overview";
            this.miTransitionTypeOverview.Click += new System.EventHandler(this.miTransitionTypeOverview_Click);
            // 
            // miTransitionTableOverview
            // 
            this.miTransitionTableOverview.Index = 1;
            this.miTransitionTableOverview.Text = "Transition Table Overview";
            this.miTransitionTableOverview.Click += new System.EventHandler(this.miTransitionTableOverview_Click);
            // 
            // ilTableCellImages
            // 
            this.ilTableCellImages.ColorDepth = System.Windows.Forms.ColorDepth.Depth24Bit;
            this.ilTableCellImages.ImageSize = new System.Drawing.Size(106, 16);
            this.ilTableCellImages.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilTableCellImages.ImageStream")));
            this.ilTableCellImages.TransparentColor = System.Drawing.Color.Fuchsia;
            // 
            // WorkSheet
            // 
            this.ContextMenu = this.ContextMenu_TableSettings;
            this.Controls.Add(this.grid);
            this.Name = "WorkSheet";
            this.Size = new System.Drawing.Size(384, 352);
            this.Load += new System.EventHandler(this.WorkSheet_Load);
            this.Enter += new System.EventHandler(this.WorkSheet_Enter);
            this.ResumeLayout(false);

        }
        #endregion

        private void OnFrameworkDataChangedHandler()
        {
            if (this.DesignMode)
                return;

            // We have to reallocate arrays.
            ArrayList akSeq = MFramework.Instance.Animation.Sequences;
            akSeq.Sort();

            TransitionList = null;
            CurrentMouseCell = SourceGrid2.Position.Empty;
            
            if (akSeq == null || akSeq.Count == 0)
            {
                grid.Redim(0,0);
                m_Data = null;
                return;
            }

            int iSize = akSeq.Count;

            grid.Redim(iSize + 1 , iSize + 1);
            m_Data = new TransitionCellData[iSize, iSize];
            //m_Data = new string[iSize, iSize];

            string strNoneName = ProjectData.GetTypeNameForTransitionType(
                MTransition.TransitionType.Trans_None);
            // Type...
            for (int i=0; i<iSize; i++)
            {
                MTransition[] akTran = ((MSequence)akSeq[i]).Transitions;

                for (int j=0; j<iSize; j++)
                {                   
                    m_Data[i,j] = new TransitionCellData();

                    m_Data[i,j].m_strText = "";
                    m_Data[i,j].TransitionData = new MTransitionData(null, 
                        (MSequence)akSeq[i], (MSequence)akSeq[j]);

                    MTransition kT = ((MSequence)akSeq[i]).GetTransition(
                        ((MSequence)akSeq[j]).SequenceID);
                    if (kT != null)
                    {
                        string strType = 
                            ProjectData.GetTypeNameForTransition(kT);
                        m_Data[i,j].m_strText = strType;
                        m_Data[i,j].TransitionData.Transition = kT;
                    }
                }
            }

            m_Header.SuppressText = false;
            grid.AutoSize();
            m_Header.SuppressText = true;
            grid.Selection.AutoClear = true;
            grid.Selection.Clear();
            grid.Selection.Focus(SourceGrid2.Position.Empty);
        }

        private int FindIndex(uint SequenceID)
        {
            ArrayList akSeq = MFramework.Instance.Animation.Sequences;

            if (akSeq == null || akSeq.Count == 0)
                return -1;

            int iSize = akSeq.Count;

            for (int i=0; i<iSize; i++)
            {
                if (((MSequence)akSeq[i]).SequenceID == SequenceID)
                    return i;
            }

            return -1;
        }

        private void OnTransitionModified(
            MTransition.PropertyType ePropChanged, MTransition kTransition)
        {
            if (ePropChanged == MTransition.PropertyType.Prop_Type)
            {
                int iSrcIdx = FindIndex(kTransition.SrcID);
                int iDestIdx = FindIndex(kTransition.DesID);

                m_Data[iSrcIdx, iDestIdx].m_strText = 
                    ProjectData.GetTypeNameForTransitionType(
                        kTransition.StoredType);
                Invalidate(true);
            }
        }

        private void OnTransitionAdded(MTransition kTransition)
        {
            int iSrcIdx = FindIndex(kTransition.SrcID);
            int iDestIdx = FindIndex(kTransition.DesID);

            m_Data[iSrcIdx, iDestIdx].TransitionData.Transition = kTransition;
            m_Data[iSrcIdx, iDestIdx].m_strText = 
                ProjectData.GetTypeNameForTransitionType(
                    kTransition.StoredType);
            Invalidate(true);

        }

        private void OnTransitionRemoved(MTransition kTransition)
        {
            int iSrcIdx = FindIndex(kTransition.SrcID);
            int iDestIdx = FindIndex(kTransition.DesID);

            m_Data[iSrcIdx, iDestIdx].TransitionData.Transition = null;
            m_Data[iSrcIdx, iDestIdx].m_strText ="";
            Invalidate(true);
        }

        
        public void OnHandleTypeConversion(
            MTransition.TransitionType kTransitionType)
        {
            HandleTypeConversion(kTransitionType);
        }

        public static bool DoChangeTransitionTypeDialog(
            ref ArrayList Transitions)
        {
            SelectTransitionTypeForm kForm = 
                new SelectTransitionTypeForm(Transitions);
            DialogResult kResult = kForm.ShowDialog(null);
            bool bChanged = false;
            if (kResult == DialogResult.OK)
            {
                AppEvents.RaiseHandleTypeConversion(kForm.m_eFinalType);
                bChanged = true;
            }

            return bChanged;
        }

        private void grid_GettingCell(object sender,
            SourceGrid2.PositionEventArgs e)
        {
            if (e.Position.Row < grid.FixedRows &&
                e.Position.Column < grid.FixedColumns)
            {
                e.Cell = m_Header;
            }
            else if (e.Position.Row < grid.FixedRows)
            {
                e.Cell = m_Column;
            }
            else if (e.Position.Column < grid.FixedColumns)
            {
                e.Cell = m_Row;
            }
            else
            {
                e.Cell = m_Cell;
            }
        }

        private void grid_Paint(object sender,
            System.Windows.Forms.PaintEventArgs e)
        {
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {            
        }

        private void WorkSheet_Load(object sender, System.EventArgs e)
        {
            this.SetStyle( ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);
            
            if (!HelpManager.HelpExists())
                miHelpRoot.Enabled = false;

            TransitionList = null;
        }

        private class WorkSheetCell : SourceGrid2.Cells.Virtual.CellVirtual
        {
            private WorkSheet m_Worksheet;

            public WorkSheetCell(WorkSheet p_WorkSheet)
            {
                m_Worksheet = p_WorkSheet;
                DataModel = SourceGrid2.Utility.CreateDataModel(
                    typeof(string));
                VisualModel.MakeReadWrite();
            }

            public override void OnClick(PositionEventArgs e)
            {
                //base.OnClick (e);
            }

            public override void OnMouseEnter(PositionEventArgs e)
            {
                base.OnMouseEnter (e);
                m_Worksheet.SetCurrentMouseCell(Grid.MouseCellPosition);
            }

            public override void OnMouseLeave(PositionEventArgs e)
            {
                base.OnMouseLeave (e);
                m_Worksheet.SetCurrentMouseCell(SourceGrid2.Position.Empty);

            }


            public override void OnDoubleClick(PositionEventArgs e)
            {
                if (WorkSheet.Instance.DesignMode)
                    return;

                WorkSheet.Instance.HandleDoubleClick(e);
            }

            public override void OnKeyUp(PositionKeyEventArgs e)
            {
                base.OnKeyUp (e);
                if (WorkSheet.Instance.DesignMode)
                    return;

                WorkSheet.Instance.HandleKeyUp(e);
            }


            public override void OnEditStarting(PositionCancelEventArgs e)
            {
                //base.OnEditStarting (e);
                e.Cancel = true;
            }


            public override object GetValue(SourceGrid2.Position p_Position)
            {
                if (WorkSheet.sbFrameWorkInit == false)
                    return null;

                if (m_Worksheet.m_Data[p_Position.Row-1, p_Position.Column-1]
                    != null)
                    return m_Worksheet.m_Data[p_Position.Row-1,
                        p_Position.Column-1].m_strText;
                else 
                    return null;
            }

            public override void SetValue(
                SourceGrid2.Position p_Position, object p_Value)
            {
                if (WorkSheet.sbFrameWorkInit == false)
                    return;

                if (p_Value is String)
                {
                    m_Worksheet.m_Data[p_Position.Row-1, p_Position.Column-1].
                        m_strText = (string)p_Value;
                    OnValueChanged(
                        new SourceGrid2.PositionEventArgs(p_Position, this));
                }
                else if (p_Value == null)
                {
                    m_Worksheet.m_Data[p_Position.Row-1, p_Position.Row-1].
                        m_strText = null;
                    OnValueChanged(
                        new SourceGrid2.PositionEventArgs(p_Position, this));
                }
            }
        }
         
        private class WorkSheetColumnVisualModel : 
            SourceGrid2.VisualModels.Header
        {
            System.Drawing.Color HighlightColor = 
                System.Drawing.Color.DarkGray;

            public WorkSheetColumnVisualModel() : base(true)
            {
            }
           
            protected override void DrawCell_Background(
                SourceGrid2.Cells.ICellVirtual p_Cell,
                Position p_CellPosition,
                PaintEventArgs e, 
                Rectangle p_ClientRectangle)
            {
                if (p_CellPosition.Column == 
                    WorkSheet.Instance.GetCurrentMouseCell().Column)
                {
                    using (SolidBrush br = new SolidBrush(HighlightColor))
                    {
                        e.Graphics.FillRectangle(br,p_ClientRectangle);
                    }
                }
                else
                {
                    using (SolidBrush br = new SolidBrush(BackColor))
                    {
                        e.Graphics.FillRectangle(br,p_ClientRectangle);
                    }
                }
            }

            protected override void DrawCell_Border(
                SourceGrid2.Cells.ICellVirtual p_Cell,
                Position p_CellPosition, PaintEventArgs e, 
                Rectangle p_ClientRectangle)
            {
                if (p_CellPosition.Column == 
                    WorkSheet.Instance.GetCurrentMouseCell().Column)
                    SourceLibrary.Drawing.ControlPaint.DrawGradient3DBorder(
                        e.Graphics, p_ClientRectangle, HighlightColor, 
                        HeaderShadowColor, HeaderLightColor, 
                        HeaderShadowBorderWidth, HeaderLightBorderWidth,
                        SourceLibrary.Drawing.Gradient3DBorderStyle.Sunken);
                else
                    SourceLibrary.Drawing.ControlPaint.DrawGradient3DBorder(
                        e.Graphics, p_ClientRectangle, BackColor,
                        HeaderShadowColor, HeaderLightColor,
                        HeaderShadowBorderWidth, HeaderLightBorderWidth,
                        SourceLibrary.Drawing.Gradient3DBorderStyle.Raised);
            }

        }
        private class WorkSheetColumn : SourceGrid2.Cells.Virtual.ColumnHeader
        {
            public WorkSheetColumn()
            {
                TextAlignment = 
                    SourceLibrary.Drawing.ContentAlignment.MiddleCenter;
                VisualModel = new WorkSheetColumnVisualModel();
            }

            public override object GetValue(SourceGrid2.Position p_Position)
            {
                if (WorkSheet.sbFrameWorkInit == false)
                    return "Animation";

                //ProjectData kPD = ProjectData.Get();
                ArrayList akSeq = MFramework.Instance.Animation.Sequences;

                if (akSeq != null || akSeq.Count == 0)
                {
                    string strSeqName = 
                        ((MSequence)akSeq[p_Position.Column - 1]).Name;

                    if (strSeqName != null)
                        return strSeqName;
                }

                return "Animation " + (p_Position.Column - 1);
            }

            public override void SetValue(SourceGrid2.Position p_Position,
                                          object p_Value)
            {
            
            }
            public override SourceGrid2.SortStatus GetSortStatus(
                SourceGrid2.Position p_Position)
            {
                return new SourceGrid2.SortStatus (
                    SourceGrid2.GridSortMode.None, false);
            }
            public override void SetSortMode(
                SourceGrid2.Position p_Position,
                SourceGrid2.GridSortMode p_Mode)
            {
            }
        }

        private class WorkSheetRowVisualModel : SourceGrid2.VisualModels.Header
        {
            System.Drawing.Color HighlightColor = 
                System.Drawing.Color.DarkGray;

            public WorkSheetRowVisualModel() : base(false)
            {
                TextAlignment = 
                    SourceLibrary.Drawing.ContentAlignment.MiddleCenter;
                MakeReadOnly();
            }

            protected override void DrawCell_Background(
                SourceGrid2.Cells.ICellVirtual p_Cell,
                Position p_CellPosition,
                PaintEventArgs e, 
                Rectangle p_ClientRectangle)
            {
                if (p_CellPosition.Row == 
                    WorkSheet.Instance.GetCurrentMouseCell().Row)
                {
                    using (SolidBrush br = new SolidBrush(HighlightColor))
                    {
                        e.Graphics.FillRectangle(br,p_ClientRectangle);
                    }
                }
                else
                {
                    using (SolidBrush br = new SolidBrush(BackColor))
                    {
                        e.Graphics.FillRectangle(br,p_ClientRectangle);
                    }
                }
            }

            protected override void DrawCell_Border(
                SourceGrid2.Cells.ICellVirtual p_Cell,
                Position p_CellPosition, PaintEventArgs e,
                Rectangle p_ClientRectangle)
            {
                if (p_CellPosition.Row == 
                    WorkSheet.Instance.GetCurrentMouseCell().Row)
                    SourceLibrary.Drawing.ControlPaint.DrawGradient3DBorder(
                        e.Graphics, p_ClientRectangle, HighlightColor,
                        HeaderShadowColor, HeaderLightColor,
                        HeaderShadowBorderWidth, HeaderLightBorderWidth,
                        SourceLibrary.Drawing.Gradient3DBorderStyle.Sunken);
                else
                    SourceLibrary.Drawing.ControlPaint.DrawGradient3DBorder(
                        e.Graphics, p_ClientRectangle, BackColor,
                        HeaderShadowColor, HeaderLightColor,
                        HeaderShadowBorderWidth, HeaderLightBorderWidth,
                        SourceLibrary.Drawing.Gradient3DBorderStyle.Raised);
            }

        }
        private class WorkSheetRow : SourceGrid2.Cells.Virtual.RowHeader
        {
            public WorkSheetRow() : base()
            {
                VisualModel = new WorkSheetRowVisualModel();
            }

            public override object GetValue(SourceGrid2.Position p_Position)
            {
                if (WorkSheet.sbFrameWorkInit == false)
                    return "Animation";

                //ProjectData kPD = ProjectData.Get();
                ArrayList akSeq = MFramework.Instance.Animation.Sequences;

                if (akSeq != null || akSeq.Count == 0)
                {
                    string strSeqName = 
                        ((MSequence)akSeq[p_Position.Row - 1]).Name;

                    if (strSeqName != null)
                        return strSeqName;
                }

                return "Animation " + (p_Position.Row - 1);;
            }        

            public override void SetValue(
                SourceGrid2.Position p_Position, object p_Value)
            {
            
            }
        }

        

        private class WorkSheetHeader : SourceGrid2.Cells.Virtual.Header
        {
            public bool SuppressText;

            public WorkSheetHeader() : base()
            {
                SuppressText = false;
            }

            public override object GetValue(SourceGrid2.Position p_Position)
            {
                if (SuppressText)
                    return null;
                else
                    return "Source |   Dest ->";
            }        
            public override void SetValue(
                SourceGrid2.Position p_Position, object p_Value)
            {        
            }
        }

        public void HandleDoubleClick(PositionEventArgs e)
        {
            Debug.Assert(TransitionList != null);
            Debug.Assert(TransitionList.Count > 0);

            if (TransitionList.Count == 1)
            {
                MTransitionData data = (MTransitionData) TransitionList[0];
                if (data.Source == data.Destination)
                {
                    return;
                }
            }
            if (DoChangeTransitionTypeDialog(ref TransitionList))
            {
                AppEvents.RaiseTransitionViewSelectionCountChanged(
                    TransitionList.Count);
                AppEvents.RaiseTransitionViewSelectionChanged(TransitionList);
            }
        }

        private void HandleTypeConversion(MTransition.TransitionType eType)
        {
            Debug.Assert(TransitionList != null);
            Debug.Assert(TransitionList.Count > 0);

            bool bConverted = false;
            bool bCancelled = false;
                
            ArrayList PossibleTypes = 
                AnimationHelpers.GetPossibleTransitionTypes(TransitionList);
            if (PossibleTypes != null && PossibleTypes.Contains(eType))
            {
                ArrayList ChainSequences = null;
                bool bOkayToConvert = true;
                if (eType == MTransition.TransitionType.Trans_Chain)
                {
                    ArrayList aaSequences = 
                        new ArrayList(TransitionList.Count);
                    
                    foreach (MTransitionData data in TransitionList)
                    {
                        ArrayList Sequences = new ArrayList(2);

                        MTransition Tran = data.Transition;
                        MSequence Src = data.Source;
                        MSequence Dest = data.Destination;

                        Sequences.Add(Src);
                        Sequences.Add(Dest);

                        aaSequences.Add(Sequences);
                    }
            
                    TransitionChainEditForm kForm = 
                        new TransitionChainEditForm(aaSequences);
                    DialogResult kRes = kForm.ShowDialog();
                    if (kRes == DialogResult.OK)
                    {
                        bOkayToConvert = true;

                        // For multiple chain selections, 
                        // it will be enough to grab the sequences
                        // form the first set as we are setting all the 
                        // chains to have the same intermediates.
                        ChainSequences = 
                            ((TransitionChainEditForm.SeqTuple)kForm.
                            m_aSeqTuple[0]).m_aSequences;
                    }
                    else
                    {
                        bOkayToConvert = false;
                        bCancelled = true;
                    }
                }

                if (bOkayToConvert)
                {
                    AnimationHelpers.ConvertTransition(
                        ref TransitionList, eType, ChainSequences);
                    bConverted = true;
                }
            }

            if (bConverted)
            {
                AppEvents.RaiseTransitionViewSelectionCountChanged(
                    TransitionList.Count);
                AppEvents.RaiseTransitionViewSelectionChanged(TransitionList);
            }
            else if (!bCancelled)
            {
                MTransition.TransitionType eOldType = 
                    AnimationHelpers.GetCommonStoredType(TransitionList);
                string oldName = 
                    ProjectData.GetTypeNameForTransitionType(eOldType);
                string newName = 
                    ProjectData.GetTypeNameForTransitionType(eType);

                string [] strVariables = new string[3];
                strVariables[0] = oldName;
                strVariables[1] = newName;

                if (eType == MTransition.TransitionType.Trans_Chain && 
                    TransitionList.Count > 1)
                    strVariables[2] = 
                        "Multiple animations are selected. "+
                        "Only one animation can be converted to "+
                        "a chain transition at a time";
                else if (eType == MTransition.TransitionType.Trans_Morph || 
                         eType == MTransition.TransitionType.Trans_DefaultSync)
                    strVariables[2] = "The source and destination do "+
                        "not have matching morph tags.";
                else if (TransitionList.Count == 1 && 
                         ((MTransitionData) TransitionList[0]).Source == 
                         ((MTransitionData) TransitionList[0]).Destination)
                    strVariables[2] = 
                        "The source and destination are the same sequences.";
                else 
                    strVariables[2] = " ";

                MessageBoxManager.DoErrorBox(
                    "UnableToChangeTransitionWarning.rtf",
                    "Unable to Change Transition Type", strVariables);
            }
        }

        public void HandleKeyUp(PositionKeyEventArgs e)
        {
            MTransition.TransitionType eType = 
                MTransition.TransitionType.Trans_MultipleSelected;

            switch(e.KeyEventArgs.KeyCode)
            {
                case Keys.Up:
                case Keys.Down:
                case Keys.Left:
                case Keys.Right:
                    Selection_FinishOccurred(this, null);
                    break;
                case Keys.Delete:
                    eType = MTransition.TransitionType.Trans_None;
                    break;
                case Keys.I:
                    eType = MTransition.TransitionType.Trans_ImmediateBlend;
                    break;
                case Keys.D:
                    eType = MTransition.TransitionType.Trans_DelayedBlend;
                    break;
                case Keys.M:
                    eType = MTransition.TransitionType.Trans_Morph;
                    break;
                case Keys.F:
                    eType = MTransition.TransitionType.Trans_CrossFade ;
                    break;
                case Keys.C:
                    eType = MTransition.TransitionType.Trans_Chain;
                    break;
                case Keys.S:
                    eType = MTransition.TransitionType.Trans_DefaultSync;
                    break;
                case Keys.N:
                    eType = MTransition.TransitionType.Trans_DefaultNonSync;
                    break;
                case Keys.F1:
                    if (HelpManager.HelpExists())
                        miTransitionTableOverview_Click(this, null);
                    return;
                case Keys.A:
                    if (e.KeyEventArgs.Modifiers == Keys.Control)
                        SelectAll();
                    return;
            }

            if (eType == MTransition.TransitionType.Trans_MultipleSelected)
                return;

            Debug.Assert(TransitionList != null);
            Debug.Assert(TransitionList.Count > 0);

            if (eType == AnimationHelpers.GetCommonStoredType(TransitionList))
                return;

            HandleTypeConversion(eType);
        }

        private void SelectAll()
        {
            grid.Selection.AddRange(
                new SourceGrid2.Range(1,1, grid.RowsCount-1, 
                grid.ColumnsCount-1));
            grid.Finish();
            grid.Invalidate(true);
        }

        private void Selection_FinishOccurred(
            object sender, FinishOccurredEventArgs e)
        {
            PositionCollection kPosGrid = grid.Selection.GetCellsPositions();

            if (kPosGrid.Count == 0)
                return;

            ArrayList kTransitionList = new ArrayList(0);
    
            bool bMatches = true;
            foreach (Position pos in kPosGrid)
            {
                if (pos.Row < 1 || pos.Column < 1)
                    continue;

                if (m_Data[pos.Row-1, pos.Column-1] == null)
                    continue;

                MTransitionData kTranData = 
                    m_Data[pos.Row-1, pos.Column-1].TransitionData;
                kTransitionList.Add(kTranData);  
             
                if (TransitionList != null && 
                    !TransitionList.Contains(kTranData))
                    bMatches = false;
            }

            if (TransitionList != null && 
                TransitionList.Count != kTransitionList.Count)
                bMatches = false;
            
            if (bMatches == true && TransitionList != null)
                return;
            else
                TransitionList = kTransitionList;
          
            AppEvents.RaiseTransitionViewSelectionCountChanged(
                TransitionList.Count);
            AppEvents.RaiseTransitionViewSelectionChanged(
                TransitionList);
        }

        private void Selection_SelectionChange(
            object sender, SelectionChangeEventArgs e)
        {
            // Final selection is detected in Finish
            return;
/*
            if (this.DesignMode)
                return;

            if (m_Data == null)
                return;

            if (e.EventType == SourceGrid2.SelectionChangeEventType.Clear)
                return;

            if (e.EventType == SourceGrid2.SelectionChangeEventType.Remove)
                return;
*/
        }

        private void MenuItem_TableSettings_Click(
            object sender, System.EventArgs e)
        {
            TransitionTableSettingsForm kForm = 
                new TransitionTableSettingsForm();
            kForm.ShowDialog(this);
        }

        private void MenuItem_Delete_Click(
            object sender, System.EventArgs e)
        {
            HandleTypeConversion(MTransition.TransitionType.Trans_None);
        }

        private void MenuItem_Immediate_Click(
            object sender, System.EventArgs e)
        {
            HandleTypeConversion(
                MTransition.TransitionType.Trans_ImmediateBlend);
        }

        private void MenuItem_Delayed_Click(
            object sender, System.EventArgs e)
        {
            HandleTypeConversion(
                MTransition.TransitionType.Trans_DelayedBlend);      
        }

        private void MenuItem_CrossFade_Click(
            object sender, System.EventArgs e)
        {
            HandleTypeConversion(MTransition.TransitionType.Trans_CrossFade);
        }

        private void MenuItem_Morph_Click(object sender, System.EventArgs e)
        {
            HandleTypeConversion(MTransition.TransitionType.Trans_Morph);
        }

        private void MenuItem_Chain_Click(object sender, System.EventArgs e)
        {
            HandleTypeConversion(MTransition.TransitionType.Trans_Chain);
        }

        private void MenuItem_Sync_Click(object sender, System.EventArgs e)
        {
            HandleTypeConversion(MTransition.TransitionType.Trans_DefaultSync);
        }

        private void MenuItem_NonSync_Click(object sender, System.EventArgs e)
        {
            HandleTypeConversion(
                MTransition.TransitionType.Trans_DefaultNonSync);
        }

        private void ContextMenu_TableSettings_Popup(
            object sender, System.EventArgs e)
        {
            ArrayList PossibleList = null;
            if (TransitionList != null)
                PossibleList = 
                    AnimationHelpers.GetPossibleTransitionTypes(
                    TransitionList);
           
            if (PossibleList == null)
            {
                MenuItem_Delete.Enabled = false;
                MenuItem_Immediate.Enabled = false;
                MenuItem_Delayed.Enabled = false;
                MenuItem_Morph.Enabled = false;
                MenuItem_CrossFade.Enabled = false;
                MenuItem_Chain.Enabled = false;
                MenuItem_Sync.Enabled = false;
                MenuItem_NonSync.Enabled = false;
                return;
            }

            MTransition.TransitionType eType = 
                AnimationHelpers.GetCommonStoredType(TransitionList);

            if (PossibleList.Contains(eType))
                PossibleList.Remove(eType);

            if (PossibleList.Contains(MTransition.TransitionType.Trans_None))
                MenuItem_Delete.Enabled = true;
            else
                MenuItem_Delete.Enabled = false;

            if (PossibleList.Contains(MTransition.TransitionType.Trans_Morph))
                MenuItem_Morph.Enabled = true;
            else
                MenuItem_Morph.Enabled = false;

            if (PossibleList.Contains(
                MTransition.TransitionType.Trans_ImmediateBlend))
                MenuItem_Immediate.Enabled = true;
            else
                MenuItem_Immediate.Enabled = false;

            if (PossibleList.Contains(
                MTransition.TransitionType.Trans_DelayedBlend))
                MenuItem_Delayed.Enabled = true;
            else
                MenuItem_Delayed.Enabled = false;

            if (PossibleList.Contains(
                MTransition.TransitionType.Trans_DefaultSync))
                MenuItem_Sync.Enabled = true;
            else
                MenuItem_Sync.Enabled = false;

            if (PossibleList.Contains(
                MTransition.TransitionType.Trans_DefaultNonSync))
                MenuItem_NonSync.Enabled = true;
            else
                MenuItem_NonSync.Enabled = false;

            if (PossibleList.Contains(
                MTransition.TransitionType.Trans_CrossFade))
                MenuItem_CrossFade.Enabled = true;
            else
                MenuItem_CrossFade.Enabled = false;

            if (PossibleList.Contains(MTransition.TransitionType.Trans_Chain))
                MenuItem_Chain.Enabled = true;
            else
                MenuItem_Chain.Enabled = false;

            if (!HelpManager.HelpExists())
                miHelpRoot.Enabled = false;
            else
                miHelpRoot.Enabled = true;
        }

        private void WorkSheet_Enter(object sender, System.EventArgs e)
        {
            if (MFramework.Instance.Animation.Mode !=
                MAnimation.PlaybackMode.Transition &&
                AnimationHelpers.DoTransitionsExist() == true &&
                CurrentMouseCell != SourceGrid2.Position.Empty)
            {
                OnTransitionsPreviewSelected();
            }
        }

        private void miTransitionTypeOverview_Click(
            object sender, System.EventArgs e)
        {
            HelpManager.DoHelp(this, @"Basics/Transition_Types.htm");
        }

        private void miTransitionTableOverview_Click(
            object sender, System.EventArgs e)
        {
            HelpManager.DoHelp(this,
                @"User_Interface_Description/Transition_View.htm"); 
        }

        private void miSelectAll_Click(object sender, System.EventArgs e)
        {
            SelectAll();
        }

        private void miDefSync_Click(object sender, System.EventArgs e)
        {
            DefaultTransitionSettingsForm dlg = new
                DefaultTransitionSettingsForm(
                MTransition.TransitionType.Trans_DefaultSync);
            dlg.ShowDialog();
        }

        private void miDefNonSync_Click(object sender, System.EventArgs e)
        {
            DefaultTransitionSettingsForm dlg = new
                DefaultTransitionSettingsForm(
                MTransition.TransitionType.Trans_DefaultNonSync);
            dlg.ShowDialog();       
        }
    }
}
