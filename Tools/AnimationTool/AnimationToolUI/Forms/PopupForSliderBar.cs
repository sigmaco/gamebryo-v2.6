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

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for PopupForSliderBar.
    /// </summary>
    public class PopupForSliderBar : System.Windows.Forms.Form
    {
        public AnimationToolUI.SliderBar SliderBar;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public PopupForSliderBar()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
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

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SliderBar = new AnimationToolUI.SliderBar();
            this.SuspendLayout();
            // 
            // SliderBar
            // 
            this.SliderBar.BorderPadding = 3;
            this.SliderBar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.SliderBar.Increment = new System.Decimal(new int[] {
                                                                          1,
                                                                          0,
                                                                          0,
                                                                          131072});
            this.SliderBar.LeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.SliderBar.Location = new System.Drawing.Point(2, 2);
            this.SliderBar.Maximum = new System.Decimal(new int[] {
                                                                  1,
                                                                  0,
                                                                  0,
                                                                  0});
            this.SliderBar.Minimum = new System.Decimal(new int[] {
                                                                  0,
                                                                  0,
                                                                  0,
                                                                  0});
            this.SliderBar.Name = "SliderBar";
            this.SliderBar.RightColor = System.Drawing.SystemColors.Control;
            this.SliderBar.Size = new System.Drawing.Size(288, 12);
            this.SliderBar.TabIndex = 0;
            this.SliderBar.Value = new System.Decimal(new int[] {
                                                                    5,
                                                                    0,
                                                                    0,
                                                                    65536});
            this.SliderBar.Load += new System.EventHandler(this.SliderBar_Load);
            this.SliderBar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.SliderBar_MouseUp);
            this.SliderBar.MouseLeave += new System.EventHandler(this.SliderBar_MouseLeave);
            // 
            // PopupForSliderBar
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 16);
            this.ControlBox = false;
            this.Controls.Add(this.SliderBar);
            this.DockPadding.All = 2;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "PopupForSliderBar";
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.TopMost = true;
            this.ResumeLayout(false);

        }
        #endregion

        private void SliderBar_MouseLeave(object sender, System.EventArgs e)
        {
            Close();
        }

        private void SliderBar_Load(object sender, System.EventArgs e)
        {
        }

        public int GetValueToMouseXOffset()
        {
            int iPercentLocation = SliderBar.Location.X + 
                SliderBar.GetValueToMouseXOffset();
            return iPercentLocation;
        }

        public int GetValueToMouseYOffset()
        {
            return SliderBar.Location.Y + SliderBar.GetValueToMouseYOffset();
        }

        private void SliderBar_MouseUp(
            object sender, System.Windows.Forms.MouseEventArgs e)
        {
            Close();
        }
    }
}
