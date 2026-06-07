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
    /// <summary>
    /// Summary description for NumericSlider.
    /// </summary>
    public class NumericSlider : System.Windows.Forms.UserControl
    {
        private decimal m_dValue;
        private decimal m_dMinimum;
        private decimal m_dMaximum;
        private int m_iDecimalPlaces;
        private decimal m_iStepSize;
        private decimal m_Percentage;
        private bool m_bEnableMeter;
        private int m_iMeterWidth;
        private int m_iMeterLeftWidth;
        private int m_iMeterRightWidth;
        private int m_iMeterHeight;
        private Color m_MeterLeftColor;
        private Color m_MeterRightColor;
        private System.Windows.Forms.HorizontalAlignment m_TextAlign;
        
        private bool m_bPopupActive;
        private bool m_bEnablePopup;
        private int m_iPopupButtonWidth;
        private int m_iPopupHeight;
        private bool m_bReadOnly;
        private bool m_bLoaded;
        
        private bool m_TrackingMouse;
        private Point m_MousePos;
        public System.Windows.Forms.NumericUpDown NumericSpinner;
        private AnimationToolUI.PopupForSliderBar PopupWindow;

        private CancelEventHandler m_PopUpCloseHandler;
        private ValueChangedEventHandler m_PopUpValueChangeHandler;
        private EventHandler m_PopUpSizeChangeHandler;
        
        public decimal Increment
        {
            get {return m_iStepSize;}
            set {m_iStepSize = value;}
        }

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

        public bool EnableMeter
        {
            get {return m_bEnableMeter;}
            set {m_bEnableMeter = value;CalculateLayout();Invalidate(true);}
        }

        public bool EnablePopup
        {
            get {return m_bEnablePopup;}
            set {m_bEnablePopup = value;CalculateLayout();Invalidate(true);}
        }

        public int PopupHeight
        {
            get {return m_iPopupHeight;}
            set {m_iPopupHeight = value;}
        }
        

        public bool ReadOnly
        {
            get {return m_bReadOnly;}
            set {SetReadOnly(value);}
        }

        public Color MeterLeftColor
        {
            get {return this.m_MeterLeftColor;}
            set {m_MeterLeftColor = value; Invalidate(true);}
        }

        public Color MeterRightColor
        {
            get {return this.m_MeterRightColor;}
            set {m_MeterRightColor = value; Invalidate(true);}
        }

        public int DecimalPlaces
        {
            get {return this.m_iDecimalPlaces;}
            set {m_iDecimalPlaces = value;SyncValues(); Invalidate(true);}
        }

        public System.Windows.Forms.HorizontalAlignment TextAlign
        {
            get {return m_TextAlign;}
            set {m_TextAlign = value;SyncValues();}
        }

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


        public NumericSlider()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            m_dMinimum = 0M;
            m_dMaximum = 1M;
            m_dValue = 0.5M;
            m_iDecimalPlaces = 3;
            m_Percentage = 0.5M;
            m_iStepSize = 0.01M;
            m_iMeterWidth = 0;
            m_iMeterHeight = 8;
            m_MeterLeftColor =  System.Drawing.Color.FromKnownColor(
                System.Drawing.KnownColor.ActiveCaption);
            m_MeterRightColor = System.Drawing.Color.FromKnownColor(
                System.Drawing.KnownColor.Control);
            m_MousePos = new Point(0,0);
            m_bReadOnly = false;
            m_bPopupActive = false;
            m_bEnableMeter = true;
            m_bEnablePopup = true;
            m_bLoaded = false;
            m_iPopupHeight = 16;
            m_iPopupButtonWidth = 16;
            m_TextAlign = HorizontalAlignment.Center;
          }

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            base.Dispose( disposing );
        }


        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.NumericSpinner = new System.Windows.Forms.NumericUpDown();
            ((System.ComponentModel.ISupportInitialize)(this.NumericSpinner)).BeginInit();
            this.SuspendLayout();
            // 
            // NumericSpinner
            // 
            this.NumericSpinner.DecimalPlaces = 3;
            this.NumericSpinner.Location = new System.Drawing.Point(0, 0);
            this.NumericSpinner.Name = "NumericSpinner";
            this.NumericSpinner.Size = new System.Drawing.Size(64, 20);
            this.NumericSpinner.TabIndex = 0;
            this.NumericSpinner.ValueChanged += new System.EventHandler(this.NumericSpinner_ValueChanged);
            this.NumericSpinner.Leave += new System.EventHandler(this.NumericSpinner_Leave);
            // 
            // NumericSlider
            // 
            this.Controls.Add(this.NumericSpinner);
            this.Name = "NumericSlider";
            this.Size = new System.Drawing.Size(80, 24);
            this.Load += new System.EventHandler(this.NumericSlider_Load);
            ((System.ComponentModel.ISupportInitialize)(this.NumericSpinner)).EndInit();
            this.ResumeLayout(false);

        }
        #endregion

        public decimal GetValue()
        {
            return m_dValue;
        }

        public void SetValue(decimal dValue)
        {
            dValue = Decimal.Round(dValue, m_iDecimalPlaces);
            if (Decimal.Compare(dValue, m_dMaximum) > 0)
                dValue = m_dMaximum;
            if (Decimal.Compare(dValue, m_dMinimum) < 0)
                dValue = m_dMinimum;
            if (Decimal.Compare(dValue, m_dValue) == 0)
                return;

            m_dValue = dValue;
            m_Percentage = (m_dValue - m_dMinimum)/(m_dMaximum - m_dMinimum);
            CalculateLayout();
            SyncValues();
            Invalidate(true);
            OnValueChanged(dValue);
        }

        private void SyncValues()
        {
            if (m_bLoaded)
            {
                if (NumericSpinner.DecimalPlaces != m_iDecimalPlaces)
                    NumericSpinner.DecimalPlaces = m_iDecimalPlaces;
                if (NumericSpinner.Increment != m_iStepSize)
                    NumericSpinner.Increment = m_iStepSize;
                if (NumericSpinner.Maximum != m_dMaximum)
                    NumericSpinner.Maximum = m_dMaximum;
                if (NumericSpinner.Minimum != m_dMinimum)
                    NumericSpinner.Minimum = m_dMinimum;
                if (NumericSpinner.Value != m_dValue)
                    NumericSpinner.Value = m_dValue;
                NumericSpinner.TextAlign = m_TextAlign;
            }
        }

        public decimal GetMin()
        {
            return m_dMinimum;
        }

        public void SetMin(decimal dMin)
        {
            m_dMinimum = dMin;
            if (m_dValue < dMin)
                m_dValue = dMin;
            SyncValues();
            CalculateLayout();
        }

        public decimal GetMax()
        {
            return m_dMaximum;
        }

        public void SetMax(decimal dMax)
        {
            m_dMaximum = dMax;
            if (m_dValue > dMax)
                m_dValue = dMax;
            SyncValues();
            CalculateLayout();
        }

        public void SetReadOnly(bool bReadOnly)
        {
            if (bReadOnly)
            {
                m_bEnablePopup = false;
            }
            m_bReadOnly = bReadOnly;
            NumericSpinner.ReadOnly = bReadOnly;
            CalculateLayout();
        }

        protected void ShowPopup()
        {
            if (!m_bReadOnly && m_bEnablePopup && !m_bPopupActive)
            {
                if (m_MousePos.X < NumericSpinner.Width)
                    return;

                PopupWindow = new AnimationToolUI.PopupForSliderBar();
                PopupWindow.Size = new Size(
                    NumericSpinner.Width + 
                    m_iPopupButtonWidth, m_iPopupHeight);
                
                AnimationToolUI.User32.POINT kPt;
                kPt.x = Location.X;
                kPt.y = Location.Y + Height;
                User32.ClientToScreen(Parent.Handle, ref kPt);
                
                PopupWindow.Location = new Point(kPt.x, kPt.y);
                PopupWindow.SliderBar.Maximum = this.m_dMaximum;
                PopupWindow.SliderBar.Minimum = this.m_dMinimum;
                PopupWindow.SliderBar.Value = this.m_dValue;
                PopupWindow.SliderBar.LeftColor = this.m_MeterLeftColor;
                PopupWindow.SliderBar.RightColor = this.m_MeterRightColor;
                m_bPopupActive = true;
                
                m_PopUpValueChangeHandler = new ValueChangedEventHandler(
                    PopupWindow_SliderBar_ValueChanged);
                m_PopUpCloseHandler = new CancelEventHandler(
                    PopupWindow_Closing);
                m_PopUpSizeChangeHandler = new EventHandler(
                    PopupWindow_SizeChanged);
                PopupWindow.Closing += m_PopUpCloseHandler;
                PopupWindow.SliderBar.ValueChanged += 
                    m_PopUpValueChangeHandler;
                PopupWindow.SizeChanged += m_PopUpSizeChangeHandler;
                User32.SetCursorPos(kPt.x + 
                                    PopupWindow.GetValueToMouseXOffset(),
                    kPt.y + PopupWindow.GetValueToMouseYOffset());

                PopupWindow.SliderBar.AllowFocus = false;
                PopupWindow.SliderBar.AllowCaptureOnVisChange = true;
                OnBeginValueDrag(Value);
                PopupWindow.Show();
                Invalidate(true);
            }
        }

        protected void CalculateLayout()
        {
            if (m_bEnablePopup)
                NumericSpinner.Width = Width - m_iPopupButtonWidth;
            else
                NumericSpinner.Width = Width;

            m_iMeterWidth = Width;

            decimal tempWidth = 
                Convert.ToDecimal(m_iMeterWidth) * m_Percentage;
            
            m_iMeterLeftWidth = Convert.ToInt32(tempWidth);
            m_iMeterRightWidth = Convert.ToInt32(m_iMeterWidth - tempWidth);
        }

        protected override void OnLayout(LayoutEventArgs levent)
        {
            CalculateLayout();
            base.OnLayout (levent);
        }

        protected override void OnResize(System.EventArgs e)
        {
            if (!m_bEnableMeter)
                Height = 20;
            else
                Height = 20 + m_iMeterHeight;
            base.OnResize(e);
            CalculateLayout();
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            if (!m_bEnableMeter)
                Height = 20;
            else
                Height = 20 + m_iMeterHeight;
            base.OnSizeChanged (e);
            CalculateLayout();
        }

        private void NumericSlider_Load(object sender, System.EventArgs e)
        {
            m_bLoaded = true;
            NumericSpinner.Value = Value;
            SyncValues();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint (e);

            if (m_bEnablePopup && !Enabled)
            {
                System.Windows.Forms.ControlPaint.DrawComboButton(e.Graphics,
                    Width - m_iPopupButtonWidth, 0, m_iPopupButtonWidth,
                    NumericSpinner.Height,
                    System.Windows.Forms.ButtonState.Inactive);
            }
            else if (m_bPopupActive && m_bEnablePopup)
            {
                System.Windows.Forms.ControlPaint.DrawComboButton(e.Graphics,
                    Width - m_iPopupButtonWidth, 0,
                    m_iPopupButtonWidth, NumericSpinner.Height,
                    System.Windows.Forms.ButtonState.Pushed);
            }
            else if (m_bEnablePopup)
            {
                System.Windows.Forms.ControlPaint.DrawComboButton(e.Graphics,
                    Width - m_iPopupButtonWidth, 0, 
                    m_iPopupButtonWidth, NumericSpinner.Height,
                    System.Windows.Forms.ButtonState.Normal);
            }

            if (m_bEnableMeter)
            {
                e.Graphics.FillRectangle(new SolidBrush(this.m_MeterLeftColor),
                    0, NumericSpinner.Height,
                    this.m_iMeterLeftWidth, this.m_iMeterHeight);
                e.Graphics.FillRectangle(new SolidBrush(this.m_MeterRightColor)
                    , m_iMeterLeftWidth, NumericSpinner.Height,
                    this.m_iMeterRightWidth, this.m_iMeterHeight);
            }

        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
            
        }

        private void NumericSpinner_ValueChanged(
            object sender, System.EventArgs e)
        {
            if (NumericSpinner.Value != Value)
                Value = NumericSpinner.Value;
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            Focus();
            m_MousePos.X = e.X;
            m_MousePos.Y = e.Y;

            if (this.m_bEnableMeter && 
                m_MousePos.Y > NumericSpinner.Height && !m_bReadOnly)
            {
                m_TrackingMouse = true;
                OnBeginValueDrag(Value);
            }
            else
            {
                ShowPopup();
            }
            base.OnMouseDown (e);
            
        }

        protected override void OnClick(EventArgs e)
        {
            Focus();
            if (this.m_bEnableMeter && m_MousePos.X < this.m_iMeterWidth &&
                m_MousePos.Y > NumericSpinner.Height && !m_bReadOnly)
            {
                PositionCursor();
            }
            base.OnClick (e);
        }

        protected void PositionCursor()
        {
            decimal dX = Convert.ToDecimal(m_MousePos.X);
            dX  = dX / Convert.ToDecimal(m_iMeterWidth);
            Value = (m_dMinimum + dX * (m_dMaximum - m_dMinimum));

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
            AnimationToolUI.User32.POINT kPt;
            kPt.x = Location.X + Width;
            kPt.y = Location.Y + Height;
            User32.ClientToScreen(Parent.Handle, ref kPt);

            if (m_TrackingMouse == true &&
                Control.MousePosition.X >= kPt.x)
            {
                Value = Maximum;
            }

            bool bTracking = m_TrackingMouse;
            m_TrackingMouse = false;
            if (bTracking && !m_bPopupActive)
                OnEndValueDrag(Value);

            base.OnMouseLeave (e);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            m_TrackingMouse = false;
            if (!m_bPopupActive)
                OnEndValueDrag(Value);
            base.OnMouseUp(e);
        }

        private void PopupWindow_Closing(object sender, CancelEventArgs e)
        {
            m_bPopupActive = false;
            PopupWindow.Closing -= this.m_PopUpCloseHandler;
            PopupWindow.SliderBar.ValueChanged -= 
                this.m_PopUpValueChangeHandler;
            PopupWindow.SizeChanged -= this.m_PopUpSizeChangeHandler;
            OnEndValueDrag(Value);
            Invalidate(true);
        }

        private void PopupWindow_SliderBar_ValueChanged(
            object sender, decimal dValue)
        {
            if (dValue != Value)
                Value = dValue;
        }

        private void NumericSpinner_Leave(object sender, System.EventArgs e)
        {
            string strText = NumericSpinner.Text;
            if (strText == "") 
                return;

            try
            {
                decimal newValue = Convert.ToDecimal(strText);
                if (newValue > NumericSpinner.Maximum)
                    newValue = NumericSpinner.Maximum;
                if (newValue < NumericSpinner.Minimum)
                    newValue = NumericSpinner.Minimum;
                NumericSpinner.Value = newValue;
                NumericSpinner.Text = newValue.ToString(
                    "f" + DecimalPlaces.ToString());
            }
            catch (Exception)
            {
                NumericSpinner.Value = Value;
                NumericSpinner.Text = Value.ToString(
                    "f"  + DecimalPlaces.ToString());
            }
        }

        private void PopupWindow_SizeChanged(object sender, EventArgs e)
        {
            System.Drawing.Size kSize = PopupWindow.Size;
            if (kSize.Width != Width)
                PopupWindow.Width = Width;
        }
    }
}
