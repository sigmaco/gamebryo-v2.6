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
using NiManagedToolInterface;
using SourceGrid2;
using Cells = SourceGrid2.Cells.Real;
using System.Diagnostics;

namespace AnimationToolUI
{
    public delegate void IntEventHandler(object sender, int Value);

    /// <summary>
    /// Summary description for SequenceGroupMixer.
    /// </summary>
    public class SequenceGroupMixer : System.Windows.Forms.UserControl
    {
        public SourceGrid2.Grid Grid_SequenceGroup;
        private MSequenceGroup m_SeqGrp;
        private int SelectedIndex;
        private static uint NumInstances = 0;

        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;
        
        public SequenceGroupMixer()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;            
            m_SeqGrp = null;
        }

        public MSequenceGroup.MSequenceInfo SelectedSequenceInfo
        {
            get
            {
                if (SelectedIndex == -1)
                    return null;
                if (m_SeqGrp == null)
                    return null;
                if (m_SeqGrp.SequenceInfo == null)
                    return null;
                if (SelectedIndex == m_SeqGrp.SequenceInfo.Length) 
                    return null;
                return m_SeqGrp.SequenceInfo[SelectedIndex];
            }
        }

        public int SelectedSequenceInfoIndex
        {
            get
            {
                return SelectedIndex;
            }
        }

        public event EventHandler SelectionChanged;

        protected virtual void OnSelectionChanged()
        {
            if (SelectionChanged != null) 
            {
                // Invokes the delegates. 
                SelectionChanged(this, null);
            }
        }

        public event IntEventHandler CheckedChanged;

        protected virtual void OnCheckedChanged(int row)
        {
            if (CheckedChanged != null) 
            {
                // Invokes the delegates. 
                CheckedChanged(this, row);
            }
        }

        public event IntEventHandler WeightChanged;

        protected virtual void OnWeightChanged(int row)
        {
            if (WeightChanged != null) 
            {
                // Invokes the delegates. 
                WeightChanged(this, row);
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
                // This assertion was coming up in design mode in Visual
                // Studio and has thus been disabled.
//Debug.Assert(NumInstances == 0, "There should only ever be one un-disposed" +
//                    " version of this class in memory at a time.");
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
            this.Grid_SequenceGroup = new SourceGrid2.Grid();
            this.SuspendLayout();
            // 
            // Grid_SequenceGroup
            // 
            this.Grid_SequenceGroup.AutoSizeMinHeight = 20;
            this.Grid_SequenceGroup.AutoSizeMinWidth = 20;
            this.Grid_SequenceGroup.AutoStretchColumnsToFitWidth = false;
            this.Grid_SequenceGroup.AutoStretchRowsToFitHeight = false;
            this.Grid_SequenceGroup.BackColor = System.Drawing.Color.White;
            this.Grid_SequenceGroup.ContextMenuStyle =
                ((SourceGrid2.ContextMenuStyle)
                (((((SourceGrid2.ContextMenuStyle.ColumnResize
                | SourceGrid2.ContextMenuStyle.RowResize) 
                | SourceGrid2.ContextMenuStyle.AutoSize) 
                | SourceGrid2.ContextMenuStyle.ClearSelection) 
                | SourceGrid2.ContextMenuStyle.CopyPasteSelection)));
            this.Grid_SequenceGroup.CustomSort = false;
            this.Grid_SequenceGroup.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grid_SequenceGroup.GridToolTipActive = true;
            this.Grid_SequenceGroup.Location = new System.Drawing.Point(2, 2);
            this.Grid_SequenceGroup.Name = "Grid_SequenceGroup";
            this.Grid_SequenceGroup.Size = new System.Drawing.Size(188, 212);
            this.Grid_SequenceGroup.SpecialKeys =
                SourceGrid2.GridSpecialKeys.Default;
            this.Grid_SequenceGroup.TabIndex = 1;
            // 
            // SequenceGroupMixer
            // 
            this.AutoScroll = true;
            this.Controls.Add(this.Grid_SequenceGroup);
            this.DockPadding.All = 2;
            this.Name = "SequenceGroupMixer";
            this.Size = new System.Drawing.Size(192, 216);
            this.Load += new System.EventHandler(this.SequenceGroupMixer_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private SourceGrid2.DataModels.IDataModel m_CellEditor_Name;
        private EditorSliderBar m_CellEditor_Weight;

        private SourceGrid2.VisualModels.Common m_VisualPropertiesCheckBox;
        private SourceGrid2.VisualModels.Common m_VisualProperties;

        private SourceGrid2.BehaviorModels.CustomEvents m_CellEvents;
        private SourceGrid2.BehaviorModels.BehaviorModelGroup m_BehaviorGroup;

        private void SequenceGroupMixer_Load(object sender, System.EventArgs e)
        {
            Grid_SequenceGroup.RowsCount = 1;
            Grid_SequenceGroup.ColumnsCount = 3;
            Grid_SequenceGroup.FixedRows = 1;
            Grid_SequenceGroup.FixedColumns = 1;
            Grid_SequenceGroup.Selection.SelectionMode = 
                SourceGrid2.GridSelectionMode.Row;
            Grid_SequenceGroup.Selection.EnableMultiSelection = false;
            Grid_SequenceGroup.AutoStretchColumnsToFitWidth = true;
            Grid_SequenceGroup.Columns[0].AutoSizeMode = 
                SourceGrid2.AutoSizeMode.None;
            Grid_SequenceGroup.Columns[0].Width = 30;

            #region Create Header Row and Editor
            Grid_SequenceGroup[0,0] = new Cells.ColumnHeader("");
            Grid_SequenceGroup.Resize += 
                new EventHandler(Grid_SequenceGroup_Resize);
            ((Cells.ColumnHeader)Grid_SequenceGroup[0,0]).EnableSort = false;
            m_CellEditor_Name = 
                SourceGrid2.Utility.CreateDataModel(typeof(string));
            m_CellEditor_Name.EditableMode = SourceGrid2.EditableMode.None;

            m_CellEditor_Weight = 
                new EditorSliderBar(typeof(decimal), 1.0M, 0.0M, 0.01M, 3);
            m_CellEditor_Weight.EditableMode = SourceGrid2.EditableMode.Focus;

            m_CellEvents = new SourceGrid2.BehaviorModels.CustomEvents();
            m_BehaviorGroup = 
                new SourceGrid2.BehaviorModels.BehaviorModelGroup();

            SourceGrid2.BehaviorModels.CheckBox cbBehaviorModel = 
                new SourceGrid2.BehaviorModels.CheckBox();
            
            m_BehaviorGroup.SubModels.Add(cbBehaviorModel);
            m_BehaviorGroup.SubModels.Add(m_CellEvents);

            m_CellEvents.ValueChanged += 
                new PositionEventHandler(m_CellEvents_ValueChanged);
            Grid_SequenceGroup[0,1] = new Cells.ColumnHeader("Name");
            ((Cells.ColumnHeader)Grid_SequenceGroup[0,1]).EnableSort = false;
            Grid_SequenceGroup[0,2] = new Cells.ColumnHeader("Weight");
            ((Cells.ColumnHeader)Grid_SequenceGroup[0,2]).EnableSort = false;
            #endregion

            #region Visual Properties
            //set Cells style
            m_VisualProperties = new SourceGrid2.VisualModels.Common(false);
            m_VisualPropertiesCheckBox = 
                (SourceGrid2.VisualModels.CheckBox)SourceGrid2.VisualModels.
                CheckBox.Default.Clone(false);
            #endregion

            Grid_SequenceGroup.KeyDown += new KeyEventHandler(
                Grid_SequenceGroup_KeyDown);
            Grid_SequenceGroup.Selection.FocusBackColor = 
                Grid_SequenceGroup.Selection.BackColor;

            Grid_SequenceGroup.Selection.CellGotFocus +=
                new CellGotFocusEventHandler(Selection_CellGotFocus);
            Grid_SequenceGroup.Selection.SelectionChange +=
                new SelectionChangeEventHandler(Selection_SelectionChange);
            Grid_SequenceGroup.RowsCount = 1;

            
            this.Grid_SequenceGroup.SpecialKeys = 
                SourceGrid2.GridSpecialKeys.Arrows |
                SourceGrid2.GridSpecialKeys.Tab;
            Grid_SequenceGroup.AutoSize();
        }

        public void AddRow(MSequenceGroup.MSequenceInfo info)
        {
            int iRow = Grid_SequenceGroup.RowsCount;
            Grid_SequenceGroup.RowsCount++;

            bool bActivated = info.Activated;

            Cells.CheckBox cbActive = 
                new Cells.CheckBox("", bActivated, m_BehaviorGroup);
            cbActive.VisualModel = m_VisualPropertiesCheckBox;

            Grid_SequenceGroup[iRow,0] = cbActive;
            
            Grid_SequenceGroup[iRow,1] = new Cells.Cell(info.Name);
            Grid_SequenceGroup[iRow,1].DataModel = m_CellEditor_Name;
            Grid_SequenceGroup[iRow,1].VisualModel = m_VisualProperties;

            Grid_SequenceGroup[iRow,2] = 
                new Cells.Cell(Convert.ToDecimal(info.Weight));
            Grid_SequenceGroup[iRow,2].DataModel = m_CellEditor_Weight;
            Grid_SequenceGroup[iRow,2].VisualModel = m_VisualProperties;
            Grid_SequenceGroup[iRow,2].Behaviors.Add(this.m_CellEvents);
            SetWeight(iRow-1, Convert.ToDecimal(info.Weight));
          
            ((Cells.Cell)Grid_SequenceGroup[iRow, 0]).DataModel.Validating +=
                new ValidatingCellEventHandler(DataModel_Validating);
        }

        public void DataModel_Validating(
            object o, SourceGrid2.ValidatingCellEventArgs e)
        {
            SourceGrid2.Cells.Real.Cell cell = 
                (SourceGrid2.Cells.Real.Cell)e.Cell;
            MSequenceGroup.MSequenceInfo seqInfo =
                m_SeqGrp.SequenceInfo[cell.Row-1];

            if ((bool)e.NewValue)
            {
                bool bResult = seqInfo.Activate();

                if (bResult == false)
                {
                    e.Cancel = true;
                    string strMsg = seqInfo.Name + 
                        " can not be activated at this time.\n     " +
                        "It may be in the process of a ease-in or ease-out.";
                    MLogger.LogWarning(strMsg);
                }
            }
            else
            {
                // Detect whether the sequence being deactivated is used 
                // to synchronize other sequences. If so, prompt the user 
                // as to whether they want to continue with the deactivation. 
                // If the user decides to continue, all dependent sequences 
                // will be deactivated as well as the selected sync sequence.
                ArrayList kDependentSequences = new ArrayList();
                foreach (MSequenceGroup.MSequenceInfo kOtherInfo in 
                    m_SeqGrp.SequenceInfo)
                {
                    if (kOtherInfo == seqInfo)
                        continue;
                    if (kOtherInfo.Synchronized && 
                        kOtherInfo.SynchronizeToSequenceID == 
                        seqInfo.SequenceID)
                        kDependentSequences.Add(kOtherInfo.Name);

                }

                if (kDependentSequences.Count != 0)
                {
                    string[] strVariables = new string[2];
                    strVariables[0] = seqInfo.Name;
                    strVariables[1] = MessageBoxManager.CreateBulletedList(
                        kDependentSequences);
                    if (MessageBoxManager.DoMessageBox(
                                "DependentSequenceInSequenceGroupFound.rtf",
                        "Dependent Sequences Found", MessageBoxButtons.YesNo,
                        MessageBoxIcon.Warning, strVariables) == 
                        DialogResult.No)
                    {
                        e.Cancel = true;
                        return;
                    }
                }

                bool bResult = seqInfo.Deactivate();             

                if (bResult == false)
                {
                    e.Cancel = true;
                    string strMsg = seqInfo.Name +
                        " can not be deactivated at this time.\n     " +
                        "It may be in the process of a ease-in or ease-out.";

                    MLogger.LogWarning(strMsg);
                }

                int iRow = 0;
                foreach (MSequenceGroup.MSequenceInfo kOtherInfo in 
                    m_SeqGrp.SequenceInfo)
                {
                    iRow++;
                    if (kOtherInfo == seqInfo)
                        continue;
                    if (kOtherInfo.Synchronized && 
                        kOtherInfo.SynchronizeToSequenceID == 
                        seqInfo.SequenceID)
                    {
                        SetCheck(iRow - 1, false);
                    }
                }
            }
        }

        public void RemoveRow(MSequenceGroup.MSequenceInfo info)
        {
            int i = 0;
            foreach (MSequenceGroup.MSequenceInfo kMatchInfo 
                in m_SeqGrp.SequenceInfo)
            {
                if (kMatchInfo == info)
                    break;
                i++;
            }
            
            Grid_SequenceGroup.RemoveCell(i, 0);
            Grid_SequenceGroup.RemoveCell(i, 1);
            Grid_SequenceGroup.RemoveCell(i, 2);

            if (i == this.SelectedIndex)
            {
                if (i == m_SeqGrp.SequenceInfo.Length)
                    i = i - 1;

                Grid_SequenceGroup.Selection.Add(
                    new SourceGrid2.Position(i-1, 0));
            }
        }

        public void SetSequenceGroup(MSequenceGroup kGroup)
        {
            int iPrevSelectedIndex = SelectedIndex;
            this.m_SeqGrp = kGroup;
            Grid_SequenceGroup.Redim(1, 3);
            if (m_SeqGrp != null && m_SeqGrp.SequenceInfo != null)
            {
                Grid_SequenceGroup.RowsCount = 1;
                
                foreach (MSequenceGroup.MSequenceInfo info 
                    in m_SeqGrp.SequenceInfo)
                {
                    AddRow(info);
                }
                if (iPrevSelectedIndex == -1)
                    iPrevSelectedIndex = 0;
                else if (iPrevSelectedIndex >= Grid_SequenceGroup.RowsCount -1)
                    iPrevSelectedIndex = Grid_SequenceGroup.RowsCount -2 ;

                Grid_SequenceGroup.Selection.Add(
                    new SourceGrid2.Position(iPrevSelectedIndex+1, 0));
                this.SelectedIndex = iPrevSelectedIndex;
            }
            else
            {
                Grid_SequenceGroup.RowsCount = 1;
                SelectedIndex = -1;
            }

            Grid_SequenceGroup.AutoSize();
        }

        private void Selection_SelectionChange(
            object sender, SelectionChangeEventArgs e)
        {
            if (e.EventType == SourceGrid2.SelectionChangeEventType.Add)
            {
                if (Grid_SequenceGroup.Selection.FocusRow != null)
                {
                    if (SelectedIndex != 
                        Grid_SequenceGroup.Selection.FocusRow.Index - 1)
                    {
                        SelectedIndex = 
                            Grid_SequenceGroup.Selection.FocusRow.Index - 1;
                        OnSelectionChanged();
                    }
                }
                else
                {
                    SelectedIndex = -1;
                }
            }
        }

        private void Selection_CellGotFocus(
            object sender, CellGotFocusEventArgs e)
        {
            if (Grid_SequenceGroup.Selection.FocusRow != null)
            {
                SelectedIndex = 
                    Grid_SequenceGroup.Selection.FocusRow.Index - 1;
                OnSelectionChanged();
            }
            else
            {
                SelectedIndex = -1;
            }
        }

        public void SetCheck(int iRow, bool bCheck)
        {
            ((Cells.CheckBox)Grid_SequenceGroup[iRow + 1, 0]).Checked = bCheck;
        }

        public bool GetCheck(int iRow)
        {
            return ((Cells.CheckBox)Grid_SequenceGroup[iRow + 1, 0]).Checked;
        }

        public void SetWeight(int iRow, decimal dWeight)
        {
            dWeight = decimal.Round(dWeight, 3);
            string strValue = dWeight.ToString("f3");
            dWeight = Convert.ToDecimal(strValue);
            ((Cells.Cell)Grid_SequenceGroup[iRow + 1, 2]).Value = dWeight;
        }

        public decimal GetWeight(int iRow)
        {
            return 
                (decimal)((Cells.Cell)Grid_SequenceGroup[iRow + 1, 2]).Value;
        }

        private void m_CellEvents_ValueChanged(
            object sender, PositionEventArgs e)
        {
            if (e.Position.Column == 0)
            {
               
                OnCheckedChanged(e.Position.Row - 1);
            }
            else if (e.Position.Column == 2)
            {
                OnWeightChanged(e.Position.Row - 1);
            }
        }

        private void Grid_SequenceGroup_KeyDown(object sender, KeyEventArgs e)
        {
            /*if (e.KeyCode == Keys.Delete)
            {
                if (m_SeqGrp.SequenceInfo == null)
                    return;

                MSequenceGroup.MSequenceInfo seqInfo = 
                    SelectedSequenceInfo;
            
                if (SelectedIndex == Grid_SequenceGroup.RowsCount - 1)
                    SelectedIndex -= 1;

                m_SeqGrp.RemoveSequence(seqInfo.SequenceID);
                SetSequenceGroup(m_SeqGrp);
            }*/
            OnKeyDown(e);
        }

        private void Grid_SequenceGroup_Resize(object sender, EventArgs e)
        {
            Grid_SequenceGroup.AutoSize();
        }
    }
}
