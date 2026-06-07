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
using System.Windows.Forms;
using SourceGrid2;
using SourceGrid2.DataModels;
using SourceGrid2.Cells.Virtual;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for SliderBarEditor.
    /// </summary>
    public class EditorSliderBar : EditorControlBase
    {
        private decimal m_Maximum = 1.0M;
        private decimal m_Minimum = 0.0M;
        private decimal m_Increment = 0.01M;
        private int m_DecimalPlaces = 3;

        /// <summary>
        /// Create a model of type Decimal
        /// </summary>
        public EditorSliderBar():base(typeof(decimal))
        {
        }
        
        public EditorSliderBar(Type p_CellType, decimal p_Maximum, 
            decimal p_Minimum, decimal p_Increment,
            int p_DecimalPlaces):base(p_CellType)
        {
            if (p_CellType==null || p_CellType == typeof(int) ||
                p_CellType == typeof(long) || p_CellType == typeof(decimal))
            {
                m_Maximum = p_Maximum;
                m_Minimum = p_Minimum;
                m_Increment = p_Increment;
                m_DecimalPlaces = p_DecimalPlaces;
            }
            else
                throw new SourceGridException(
                    "Invalid CellType expected long, int or decimal");
        }

        #region Edit Control
        public override Control CreateEditorControl()
        {
            SliderBar l_Control = new SliderBar();
            l_Control.AllowCaptureOnVisChange = true;
            l_Control.AllowFocus = true;
#if !MINI
            //l_Control.BorderStyle = System.Windows.Forms.BorderStyle.None;
#endif
            l_Control.ValueChanged +=new ValueChangedEventHandler(
                l_Control_ValueChanged);
            return l_Control;
        }

        [Obsolete("Use GetEditorSliderBar(GridSubPanel)")]
        public virtual SliderBar GetEditorSliderBar(GridVirtual p_Grid)
        {
            return (SliderBar)GetEditorControl(p_Grid);
        }

        public virtual SliderBar GetEditorSliderBar(
            GridSubPanel p_GridSubPanel)
        {
            return (SliderBar)GetEditorControl(p_GridSubPanel);
        }
        #endregion

        /// <summary>
        /// Start editing the cell passed
        /// </summary>
        /// <param name="p_Cell">Cell to start edit</param>
        /// <param name="p_Position">Editing position(Row/Col)</param>
        /// <param name="p_StartEditValue">
        /// Can be null(in this case use the p_cell.Value</param>
        public override void InternalStartEdit(
            SourceGrid2.Cells.ICellVirtual p_Cell, Position p_Position,
            object p_StartEditValue)
        {
            base.InternalStartEdit(p_Cell, p_Position, p_StartEditValue);

            if (EnableEdit==false)
                return;

            SliderBar l_Control = GetEditorSliderBar(
                p_Cell.Grid.PanelAtPosition(p_Position));

            if (p_StartEditValue != null)
            {
                if ( IsValidObject(p_StartEditValue) )
                    SetValueToControl( ObjectToValue(p_StartEditValue) );
            }
            else
            {
                SetValueToControl(p_Cell.GetValue(p_Position));
            }

            int iPercentLocation = Convert.ToInt32(l_Control.Percentage * 
                Convert.ToDecimal(l_Control.Width));

            AnimationToolUI.User32.POINT kPt;
            kPt.x = 0;
            kPt.y = 0;
            User32.ClientToScreen(l_Control.Handle, ref kPt);

            User32.SetCursorPos(kPt.x + iPercentLocation,
                kPt.y + l_Control.Height/2);

        }

        /// <summary>
        /// Fired when the control is attached to a panel. 
        /// Use this method to customize the control.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnEditorAttached(ControlEventArgs e)
        {
            base.OnEditorAttached(e);

            SliderBar l_Control = (SliderBar)e.Control;

            l_Control.Maximum = m_Maximum;
            l_Control.Minimum = m_Minimum;
            l_Control.Increment = m_Increment;
            l_Control.DecimalPlaces = m_DecimalPlaces;
        }

        public decimal Maximum
        {
            get{return m_Maximum;}
            set{m_Maximum = value;}
        }
        public decimal Minimum
        {
            get{return m_Minimum;}
            set{m_Minimum = value;}
        }
        public decimal Increment
        {
            get{return m_Increment;}
            set{m_Increment = value;}
        }
        public int DecimalPlaces
        {
            get{return m_DecimalPlaces;}
            set{m_DecimalPlaces = value;}
        }

        /// <summary>
        /// Returns the value inserted with the current editor control
        /// </summary>
        /// <returns></returns>
        public override object GetEditedValue()
        {
            return GetValueFromControl();
        }

        private object GetValueFromControl()
        {
            if (ValueType == null)
                return GetEditorSliderBar(
                    EditCell.Grid.PanelAtPosition(EditPosition)).Value;
            if (ValueType == typeof(decimal))
                return GetEditorSliderBar(
                    EditCell.Grid.PanelAtPosition(EditPosition)).Value;
            if (ValueType == typeof(int))
                return (int)(GetEditorSliderBar(
                    EditCell.Grid.PanelAtPosition(EditPosition)).Value);
            if (ValueType == typeof(long))
                return (long)(GetEditorSliderBar(
                    EditCell.Grid.PanelAtPosition(EditPosition)).Value);

            throw new SourceGridException(
                "Invalid type of the cell expected decimal, long or int");
        }

        private void SetValueToControl(object p_Value)
        {
            if (p_Value is decimal)
                GetEditorSliderBar(
                    EditCell.Grid.PanelAtPosition(EditPosition)).Value = 
                    (decimal)p_Value;
            else if (p_Value is long)
                GetEditorSliderBar(
                    EditCell.Grid.PanelAtPosition(EditPosition)).Value = 
                    (decimal)((long)p_Value);
            else if (p_Value is int)
                GetEditorSliderBar(
                    EditCell.Grid.PanelAtPosition(EditPosition)).Value = 
                    (decimal)((int)p_Value);
            else if (p_Value == null)
                GetEditorSliderBar(
                    EditCell.Grid.PanelAtPosition(EditPosition)).Value = 
                    GetEditorSliderBar(
                        EditCell.Grid.PanelAtPosition(EditPosition)).Minimum;
            else
                throw new SourceGridException(
                    "Invalid value, expected Decimal, Int or Long");
        }

        private void l_Control_ValueChanged(object sender, decimal Value)
        {
            this.InternalApplyEdit();
        }
    }
}
