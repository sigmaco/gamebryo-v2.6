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

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for NoEditableProperties.
    /// </summary>
    public class NoEditableProperties : System.Windows.Forms.UserControl
    {
        private System.Windows.Forms.Label Label_NoEditableProperties;
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;
        private static uint NumInstances = 0;

        public NoEditableProperties()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
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
            this.Label_NoEditableProperties = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // Label_NoEditableProperties
            // 
            this.Label_NoEditableProperties.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.Label_NoEditableProperties.ForeColor = System.Drawing.SystemColors.ActiveCaption;
            this.Label_NoEditableProperties.Location = new System.Drawing.Point(32, 24);
            this.Label_NoEditableProperties.Name = "Label_NoEditableProperties";
            this.Label_NoEditableProperties.Size = new System.Drawing.Size(200, 32);
            this.Label_NoEditableProperties.TabIndex = 0;
            this.Label_NoEditableProperties.Text = "Selection has no editable properties";
            this.Label_NoEditableProperties.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // NoEditableProperties
            // 
            this.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.Controls.Add(this.Label_NoEditableProperties);
            this.Name = "NoEditableProperties";
            this.Size = new System.Drawing.Size(264, 80);
            this.Load += new System.EventHandler(this.NoEditableProperties_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void NoEditableProperties_Load(
            object sender, System.EventArgs e)
        {
            
        }
    }
}
