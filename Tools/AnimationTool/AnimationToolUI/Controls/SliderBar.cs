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

namespace AnimationToolUI
{
    public delegate void ValueChangedEventHandler(
        object sender, decimal Value);

    /// <summary>
    /// Summary description for SliderBar.
    /// </summary>
    /// 
    public class SliderBar : System.Windows.Forms.UserControl
    {
        private decimal m_Value;
        private decimal m_Min;
        private decimal m_Max;
        private int m_DecimalPlaces;
        private string m_strDecimalFormat;

        private decimal m_StepSize;
        private decimal m_Percentage;
        private bool m_TrackingMouse;

        private System.Drawing.Point m_LeftPanelLocation;
        private int m_LeftPanelHeight;
        private int m_LeftPanelWidth;
        private System.Drawing.Color m_LeftPanelColor;

        private System.Drawing.Point m_RightPanelLocation;
        private int m_RightPanelHeight;
        private int m_RightPanelWidth;
        private System.Drawing.Color m_RightPanelColor;

        private System.Drawing.Point m_CursorPanelLocation;
        private int m_CursorPanelHeight;
        private int m_CursorPanelWidth;

        private int m_BorderSize;
        private Point m_MousePos;

        public event ValueChangedEventHandler ValueChanged;
        public event ValueChangedEventHandler BeginValueDrag;
        public event ValueChangedEventHandler EndValueDrag;

        protected virtual void OnValueChanged(decimal Value)
        {
            if (ValueChanged != null) 
            {
                // Invokes the delegates. 
                ValueChanged(this, Value);
            }
        }

        protected virtual void OnBeginValueDrag(decimal Value)
        {
            if (BeginValueDrag != null) 
            {
                // Invokes the delegates. 
                BeginValueDrag(this, Value);
            }
        }

        protected virtual void OnEndValueDrag(decimal Value)
        {
            if (EndValueDrag != null) 
            {
                // Invokes the delegates. 
                EndValueDrag(this, Value);
            }
        }

        public bool AllowFocus;
        public decimal Minimum
        {
            get {return GetMin();}
            set {SetMin(value);}
        }

        public decimal Maximum
        {
            get {return GetMax();}
            set {SetMax(value);}
        }

        public decimal Value
        {
            get {return GetValue();}
            set {SetValue(value);}
        }     

        public decimal Increment
        {
            get {return m_StepSize;}
            set {m_StepSize = value;}
        }

        public decimal Percentage
        {
            get {return m_Percentage;}
        }

        public int BorderPadding
        {
            get {return m_BorderSize;}
            set {m_BorderSize = value; Invalidate(true);}
        }

        public Color LeftColor
        {
            get {return this.m_LeftPanelColor;}
            set {m_LeftPanelColor = value; Invalidate(true);}
        }

        public Color RightColor
        {
            get {return this.m_RightPanelColor;}
            set {m_RightPanelColor = value; Invalidate(true);}
        }

        public int DecimalPlaces
        {
            get {return this.m_DecimalPlaces;}
            set {
                    m_DecimalPlaces = value;
                    m_strDecimalFormat = "f" + value.ToString();
                    Invalidate(true);
                }
        }

        public int GetValueToMouseXOffset()
        {
            decimal dWidth = Convert.ToDecimal(Width);
            decimal dBorderPadding = Convert.ToDecimal(BorderPadding);
            decimal dPercentLocation = Percentage * 
                (dWidth - 2*dBorderPadding) + 
                dBorderPadding;
            return Convert.ToInt32(dPercentLocation);
        }

        public int GetValueToMouseYOffset()
        {
            return Height / 2;
        }

        public bool AllowCaptureOnVisChange;

        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public SliderBar()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            m_CursorPanelWidth = 8;
            m_Min = 0M;
            m_Max = 1M;
            m_Value = 0.5M;
            m_DecimalPlaces = 3;
            m_Percentage = 0.5M;
            m_StepSize = 0.01M;
            m_TrackingMouse = false;
            AllowCaptureOnVisChange = false;
            m_MousePos = new Point(0,0);
            
            m_LeftPanelLocation = new Point(0,0);
            m_LeftPanelColor =  System.Drawing.Color.FromKnownColor(
                System.Drawing.KnownColor.ActiveCaption);
            m_LeftPanelHeight = 0;
            m_LeftPanelWidth = 0;

            m_RightPanelLocation = new Point(0,0);
            m_RightPanelColor = System.Drawing.Color.FromKnownColor(
                System.Drawing.KnownColor.Control);
            m_RightPanelHeight = 0;
            m_RightPanelWidth = 0;

            m_CursorPanelLocation = new Point(0,0);
            m_CursorPanelHeight = 0;
            m_CursorPanelWidth = 0;

            m_BorderSize = 3;
            AllowFocus = true;
            CalculateLayout();
        }

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
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
            // 
            // SliderBar
            // 
            this.Name = "SliderBar";
            this.Size = new System.Drawing.Size(150, 16);
            this.VisibleChanged += new System.EventHandler(this.SliderBar_VisibleChanged);
            this.Load += new System.EventHandler(this.SliderBar_Load);

        }
        #endregion

        public decimal GetValue()
        {
            return m_Value;
        }

        public void SetValue(decimal dValue)
        {
            dValue = Decimal.Round(dValue, m_DecimalPlaces);
            string strValue = dValue.ToString(this.m_strDecimalFormat);
            dValue = Convert.ToDecimal(strValue);

            if (!IsWithinTolerance(dValue))
                return;
            if (Decimal.Compare(dValue, m_Max) > 0)
                dValue = m_Max;
            if (Decimal.Compare(dValue, m_Min) < 0)
                dValue = m_Min;
            if (Decimal.Compare(dValue, m_Value) == 0)
                return;

            m_Value = dValue;
            m_Percentage = (m_Value - m_Min)/(m_Max - m_Min);
            CalculateLayout();
            
            Invalidate(true);
            OnValueChanged(dValue);
        }

        public decimal GetMin()
        {
            return m_Min;
        }

        public void SetMin(decimal dMin)
        {
            m_Min = dMin;
            CalculateLayout();
        }

        public decimal GetMax()
        {
            return m_Max;
        }

        public void SetMax(decimal dMax)
        {
            m_Max = dMax;
            CalculateLayout();
        }

        protected void CalculateLayout()
        {
            decimal tempWidth = 
                Convert.ToDecimal(Width - 2*m_BorderSize) * m_Percentage;
            m_LeftPanelLocation.X = m_BorderSize;
            m_LeftPanelLocation.Y = m_BorderSize;
            m_LeftPanelHeight = Height - 2*m_BorderSize;
            m_LeftPanelWidth = Convert.ToInt32(tempWidth);

            m_RightPanelLocation.X = 
                m_LeftPanelLocation.X + m_LeftPanelWidth + m_BorderSize;
            m_RightPanelLocation.Y = m_BorderSize;
            m_RightPanelHeight = Height - 2*m_BorderSize;
            m_RightPanelWidth = 
                Width - 2*m_BorderSize - Convert.ToInt32(tempWidth) - 5;

            m_CursorPanelLocation.X = 
                m_LeftPanelLocation.X + m_LeftPanelWidth - 
                m_CursorPanelWidth/2;
            m_CursorPanelLocation.Y = m_BorderSize;
            m_CursorPanelHeight = Height - 2*m_BorderSize;
            m_CursorPanelWidth = 8;
        }

        protected override void OnLayout(LayoutEventArgs levent)
        {
            CalculateLayout();
            base.OnLayout (levent);
        }

        protected override bool IsInputKey(Keys key)
        {
            switch(key)
            {
                case Keys.Up:
                case Keys.Down:
                case Keys.Right:
                case Keys.Left:
                    return true;
            }
            return base.IsInputKey(key);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Left)
                Value -= Increment;
            else if (e.KeyCode == Keys.Right)
                Value += Increment;
            base.OnKeyDown (e);
            
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown (e);
            if (AllowFocus)
                Focus();
            m_TrackingMouse = true;
            OnBeginValueDrag(Value);
        }

        protected override void OnClick(EventArgs e)
        {
            if (AllowFocus)
                Focus();
            PositionCursor();
            base.OnClick (e);
        }

        //Check to see if the new value is within a half pixel of the old value
        //If not, do not update the value.
        protected bool IsWithinTolerance(decimal dValue)
        {
            decimal dWidth = Convert.ToDecimal(Width - 2*m_BorderSize);
            decimal dRange = m_Max - m_Min;
            decimal dTolerance = Convert.ToDecimal(0.5f)*(dRange / dWidth);

            decimal dDelta = dValue - Value;

            if (dDelta < decimal.Zero)
                dDelta = -dDelta;

            return dDelta > dTolerance;
        }

        protected void PositionCursor()
        {
            decimal dX = Convert.ToDecimal(m_MousePos.X-m_BorderSize);
            dX  = dX / Convert.ToDecimal(Width - 2*m_BorderSize);

            Value = m_Min + dX * (m_Max - m_Min);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            m_MousePos.X = e.X;
            m_MousePos.Y = e.Y;

            if (m_TrackingMouse)
            {
                if (m_MousePos.X >= Width)
                    Value = Maximum;
                else
                   PositionCursor();
            }
            base.OnMouseMove (e);
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            bool bIssueDrag = m_TrackingMouse;
            m_TrackingMouse = false;
            if (bIssueDrag)
                OnEndValueDrag(Value);
            base.OnMouseLeave (e);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            m_TrackingMouse = false;
            OnEndValueDrag(Value);
            base.OnMouseUp(e);
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
            //base.OnPaintBackground (pevent);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            //base.OnPaint (e);
            //CalculateLayout();
            Brush kBrush;
            
            System.Windows.Forms.ControlPaint.DrawBorder3D(e.Graphics,
                0, 0,Width, Height,
                System.Windows.Forms.Border3DStyle.Sunken,
                System.Windows.Forms.Border3DSide.All);

            kBrush = new System.Drawing.SolidBrush(m_LeftPanelColor);
            e.Graphics.FillRectangle(kBrush, m_LeftPanelLocation.X, 
                m_LeftPanelLocation.Y, m_LeftPanelWidth, m_LeftPanelHeight);
            kBrush.Dispose();

            kBrush = new System.Drawing.SolidBrush(m_RightPanelColor);
            e.Graphics.FillRectangle(kBrush, m_RightPanelLocation.X, 
                m_RightPanelLocation.Y, m_RightPanelWidth, m_RightPanelHeight);
            kBrush.Dispose();
            
            System.Windows.Forms.ControlPaint.DrawBorder3D(e.Graphics,
                m_CursorPanelLocation.X, m_CursorPanelLocation.Y,
                m_CursorPanelWidth, Height - 2*m_BorderSize,
                System.Windows.Forms.Border3DStyle.Raised,
                System.Windows.Forms.Border3DSide.All);
        }

        private void SliderBar_Load(object sender, System.EventArgs e)
        {
            
        }

        private void SliderBar_VisibleChanged(
            object sender, System.EventArgs e)
        {
            if (AllowCaptureOnVisChange && Control.MouseButtons != 
                                           MouseButtons.None && Visible)
            {
                this.m_TrackingMouse = true;
                if (AllowFocus)
                    Focus();
                Capture = true;
                OnBeginValueDrag(Value);
            }
        }

    }
}
