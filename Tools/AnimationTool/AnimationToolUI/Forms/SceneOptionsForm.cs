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
using System.IO;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for SceneOptionsForm.
    /// </summary>
    public class SceneOptionsForm : System.Windows.Forms.Form
    {
        private System.Windows.Forms.Button Button_OK;
        private System.Windows.Forms.Button Button_Cancel;
        private MCamera.UpAxisType kOriginalAxis;
        private bool m_bOriginalInvertPan;
        private System.Windows.Forms.CheckBox cbUpSaveDefault;
        private System.Windows.Forms.Label Label_UpAxis;
        private System.Windows.Forms.RadioButton RadioButton_ZAxis;
        private System.Windows.Forms.RadioButton RadioButton_YAxis;
        private AnimationToolUI.NumericSlider nsPlaybackRate;
        private System.Windows.Forms.Label label1;
        private float m_fOriginalRate;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtImageSubFolder;
        private System.Windows.Forms.Button btnSelectImageSubFolder;
        private 
            System.Windows.Forms.FolderBrowserDialog dlgImageSubFolderBrowser;
        private System.Windows.Forms.CheckBox cbInvertPan;
        private string strOriginalImageSubFolder;
        private System.Windows.Forms.Button bnSameAsNIF;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public SceneOptionsForm()
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
            this.Button_OK = new System.Windows.Forms.Button();
            this.Button_Cancel = new System.Windows.Forms.Button();
            this.cbUpSaveDefault = new System.Windows.Forms.CheckBox();
            this.Label_UpAxis = new System.Windows.Forms.Label();
            this.RadioButton_ZAxis = new System.Windows.Forms.RadioButton();
            this.RadioButton_YAxis = new System.Windows.Forms.RadioButton();
            this.nsPlaybackRate = new AnimationToolUI.NumericSlider();
            this.label1 = new System.Windows.Forms.Label();
            this.dlgImageSubFolderBrowser = new System.Windows.Forms.FolderBrowserDialog();
            this.label2 = new System.Windows.Forms.Label();
            this.txtImageSubFolder = new System.Windows.Forms.TextBox();
            this.btnSelectImageSubFolder = new System.Windows.Forms.Button();
            this.cbInvertPan = new System.Windows.Forms.CheckBox();
            this.bnSameAsNIF = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // Button_OK
            // 
            this.Button_OK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.Button_OK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_OK.Location = new System.Drawing.Point(104, 168);
            this.Button_OK.Name = "Button_OK";
            this.Button_OK.Size = new System.Drawing.Size(64, 32);
            this.Button_OK.TabIndex = 2;
            this.Button_OK.Text = "OK";
            this.Button_OK.Click += new System.EventHandler(this.Button_OK_Click);
            // 
            // Button_Cancel
            // 
            this.Button_Cancel.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.Button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Button_Cancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_Cancel.Location = new System.Drawing.Point(200, 168);
            this.Button_Cancel.Name = "Button_Cancel";
            this.Button_Cancel.Size = new System.Drawing.Size(64, 32);
            this.Button_Cancel.TabIndex = 3;
            this.Button_Cancel.Text = "Cancel";
            this.Button_Cancel.Click += new System.EventHandler(this.Button_Cancel_Click);
            // 
            // cbUpSaveDefault
            // 
            this.cbUpSaveDefault.Location = new System.Drawing.Point(232, 16);
            this.cbUpSaveDefault.Name = "cbUpSaveDefault";
            this.cbUpSaveDefault.Size = new System.Drawing.Size(104, 16);
            this.cbUpSaveDefault.TabIndex = 13;
            this.cbUpSaveDefault.Text = "Save as default";
            // 
            // Label_UpAxis
            // 
            this.Label_UpAxis.Location = new System.Drawing.Point(16, 16);
            this.Label_UpAxis.Name = "Label_UpAxis";
            this.Label_UpAxis.Size = new System.Drawing.Size(48, 16);
            this.Label_UpAxis.TabIndex = 10;
            this.Label_UpAxis.Text = "Up Axis:";
            // 
            // RadioButton_ZAxis
            // 
            this.RadioButton_ZAxis.Location = new System.Drawing.Point(152, 16);
            this.RadioButton_ZAxis.Name = "RadioButton_ZAxis";
            this.RadioButton_ZAxis.Size = new System.Drawing.Size(56, 24);
            this.RadioButton_ZAxis.TabIndex = 12;
            this.RadioButton_ZAxis.Text = "Z-Axis";
            this.RadioButton_ZAxis.CheckedChanged += new System.EventHandler(this.RadioButton_ZAxis_CheckedChanged);
            // 
            // RadioButton_YAxis
            // 
            this.RadioButton_YAxis.Location = new System.Drawing.Point(80, 16);
            this.RadioButton_YAxis.Name = "RadioButton_YAxis";
            this.RadioButton_YAxis.Size = new System.Drawing.Size(56, 24);
            this.RadioButton_YAxis.TabIndex = 11;
            this.RadioButton_YAxis.Text = "Y-Axis";
            this.RadioButton_YAxis.CheckedChanged += new System.EventHandler(this.RadioButton_YAxis_CheckedChanged);
            // 
            // nsPlaybackRate
            // 
            this.nsPlaybackRate.DecimalPlaces = 3;
            this.nsPlaybackRate.EnableMeter = true;
            this.nsPlaybackRate.EnablePopup = true;
            this.nsPlaybackRate.Increment = new System.Decimal(new int[] {
                                                                             1,
                                                                             0,
                                                                             0,
                                                                             131072});
            this.nsPlaybackRate.Location = new System.Drawing.Point(104, 56);
            this.nsPlaybackRate.Maximum = new System.Decimal(new int[] {
                                                                           2,
                                                                           0,
                                                                           0,
                                                                           0});
            this.nsPlaybackRate.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsPlaybackRate.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsPlaybackRate.Minimum = new System.Decimal(new int[] {
                                                                           1,
                                                                           0,
                                                                           0,
                                                                           131072});
            this.nsPlaybackRate.Name = "nsPlaybackRate";
            this.nsPlaybackRate.PopupHeight = 16;
            this.nsPlaybackRate.ReadOnly = false;
            this.nsPlaybackRate.Size = new System.Drawing.Size(136, 28);
            this.nsPlaybackRate.TabIndex = 14;
            this.nsPlaybackRate.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.nsPlaybackRate.Value = new System.Decimal(new int[] {
                                                                         1000,
                                                                         0,
                                                                         0,
                                                                         196608});
            this.nsPlaybackRate.ValueChanged += new AnimationToolUI.ValueChangedEventHandler(this.nsPlaybackRate_ValueChanged);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(16, 64);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(80, 16);
            this.label1.TabIndex = 15;
            this.label1.Text = "Playback Rate:";
            // 
            // dlgImageSubFolderBrowser
            // 
            this.dlgImageSubFolderBrowser.Description = "Set the sub-directory that you wish to use for locating textures.";
            this.dlgImageSubFolderBrowser.ShowNewFolderButton = false;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(16, 96);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 28);
            this.label2.TabIndex = 16;
            this.label2.Text = "Image Subfolder:";
            // 
            // txtImageSubFolder
            // 
            this.txtImageSubFolder.Location = new System.Drawing.Point(104, 96);
            this.txtImageSubFolder.Name = "txtImageSubFolder";
            this.txtImageSubFolder.ReadOnly = true;
            this.txtImageSubFolder.Size = new System.Drawing.Size(144, 20);
            this.txtImageSubFolder.TabIndex = 1;
            this.txtImageSubFolder.Text = "";
            // 
            // btnSelectImageSubFolder
            // 
            this.btnSelectImageSubFolder.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnSelectImageSubFolder.Location = new System.Drawing.Point(248, 96);
            this.btnSelectImageSubFolder.Name = "btnSelectImageSubFolder";
            this.btnSelectImageSubFolder.Size = new System.Drawing.Size(24, 23);
            this.btnSelectImageSubFolder.TabIndex = 0;
            this.btnSelectImageSubFolder.Text = "...";
            this.btnSelectImageSubFolder.Click += new System.EventHandler(this.btnSelectImageSubFolder_Click);
            // 
            // cbInvertPan
            // 
            this.cbInvertPan.Location = new System.Drawing.Point(18, 138);
            this.cbInvertPan.Name = "cbInvertPan";
            this.cbInvertPan.Size = new System.Drawing.Size(272, 16);
            this.cbInvertPan.TabIndex = 17;
            this.cbInvertPan.Text = "Invert Pan Mode";
            this.cbInvertPan.CheckedChanged += new System.EventHandler(this.cbInvertPan_CheckedChanged);
            // 
            // bnSameAsNIF
            // 
            this.bnSameAsNIF.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.bnSameAsNIF.Location = new System.Drawing.Point(277, 96);
            this.bnSameAsNIF.Name = "bnSameAsNIF";
            this.bnSameAsNIF.Size = new System.Drawing.Size(80, 23);
            this.bnSameAsNIF.TabIndex = 18;
            this.bnSameAsNIF.Text = "Same as NIF";
            this.bnSameAsNIF.Click += new System.EventHandler(this.bnSameAsNIF_Click);
            // 
            // SceneOptionsForm
            // 
            this.AcceptButton = this.Button_OK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.Button_Cancel;
            this.ClientSize = new System.Drawing.Size(362, 216);
            this.ControlBox = false;
            this.Controls.Add(this.bnSameAsNIF);
            this.Controls.Add(this.cbInvertPan);
            this.Controls.Add(this.btnSelectImageSubFolder);
            this.Controls.Add(this.txtImageSubFolder);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.nsPlaybackRate);
            this.Controls.Add(this.cbUpSaveDefault);
            this.Controls.Add(this.Label_UpAxis);
            this.Controls.Add(this.RadioButton_ZAxis);
            this.Controls.Add(this.RadioButton_YAxis);
            this.Controls.Add(this.Button_Cancel);
            this.Controls.Add(this.Button_OK);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "SceneOptionsForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Scene Options";
            this.Load += new System.EventHandler(this.SceneOptionsForm_Load);
            this.ResumeLayout(false);

        }
        #endregion
       

        private void Button_Cancel_Click(object sender, System.EventArgs e)
        {
            UndoAll();
        }

        private void Button_OK_Click(object sender, System.EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            ProjectData.Instance.SaveProjectRegistrySettings(
                cbUpSaveDefault.Checked);
        }

        private void SceneOptionsForm_Load(object sender, System.EventArgs e)
        {
            ProjectData kData = ProjectData.Instance;
            strOriginalImageSubFolder = ProjectData.Instance.ImageSubFolder;
            
            kOriginalAxis = kData.UpAxis;
            switch(kOriginalAxis)
            {
                case MCamera.UpAxisType.Z_AXIS:
                    RadioButton_YAxis.Checked = false;
                    RadioButton_ZAxis.Checked = true;
                    break;
                case MCamera.UpAxisType.Y_AXIS:
                    RadioButton_YAxis.Checked = true;
                    RadioButton_ZAxis.Checked = false;
                    break;
            }

            m_bOriginalInvertPan = kData.InvertPan;
            cbInvertPan.Checked = m_bOriginalInvertPan;

            nsPlaybackRate.Value = Convert.ToDecimal(
                MFramework.Instance.Clock.ScaleFactor);
            m_fOriginalRate = MFramework.Instance.Clock.ScaleFactor;

            SetImageFolderText();
        }

        private void SetImageFolderText()
        {
            if (ProjectData.Instance.ImageSubFolder == "")
                txtImageSubFolder.Text = "(Same as NIF)";
            else
                txtImageSubFolder.Text = ProjectData.Instance.ImageSubFolder;
        
        }

        public void UndoAll()
        {
            if (ProjectData.Instance.UpAxis != kOriginalAxis)
            {
                ProjectData.Instance.UpAxis = kOriginalAxis;
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.ZOOM_EXTENTS);

                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.RESET_BACKGROUND_HEIGHT);
            }

            ProjectData.Instance.InvertPan = m_bOriginalInvertPan;
            MFramework.Instance.Clock.ScaleFactor = m_fOriginalRate;
            ProjectData.Instance.ImageSubFolder = strOriginalImageSubFolder;
        }

        private void UpdateRadioButtons()
        {
            MCamera.UpAxisType kAxis = ProjectData.Instance.UpAxis;
            if (RadioButton_YAxis.Checked)
                kAxis = MCamera.UpAxisType.Y_AXIS;
            else if (RadioButton_ZAxis.Checked)
                kAxis = MCamera.UpAxisType.Z_AXIS;

            if (ProjectData.Instance.UpAxis != kAxis)
            {
                ProjectData.Instance.UpAxis = kAxis;
                
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.ZOOM_EXTENTS);

                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.RESET_BACKGROUND_HEIGHT);
            }
        }

        private void RadioButton_YAxis_CheckedChanged(
            object sender, System.EventArgs e)
        {
            UpdateRadioButtons();
        }

        private void RadioButton_ZAxis_CheckedChanged(
            object sender, System.EventArgs e)
        {
            UpdateRadioButtons();
        }

        private void nsPlaybackRate_ValueChanged(object sender, decimal Value)
        {
            if (!MFramework.InstanceIsValid())
                return;
            MFramework.Instance.Clock.ScaleFactor = Convert.ToSingle(Value);
        }

        private void btnSelectImageSubFolder_Click(
            object sender, System.EventArgs e)
        {
            MAnimation pmAnimation = MFramework.Instance.Animation;
            if (ProjectData.Instance.ImageSubFolder == "" && 
                pmAnimation.FullModelPath != "")
                dlgImageSubFolderBrowser.SelectedPath = 
                    Path.GetDirectoryName(pmAnimation.FullModelPath);
            else
                dlgImageSubFolderBrowser.SelectedPath = 
                    ProjectData.Instance.ImageSubFolder;
            
            
            if (dlgImageSubFolderBrowser.ShowDialog(this) == DialogResult.OK)
            {
                string strSelPath = 
                    dlgImageSubFolderBrowser.SelectedPath + "\\";
                if (ProjectData.Instance.ImageSubFolder == "" && 
                    pmAnimation.FullModelPath != "" &&
                    strSelPath == 
                    Path.GetDirectoryName(pmAnimation.FullModelPath))
                {
                    return;
                }
                else if (pmAnimation.FullModelPath != "" && strSelPath == 
                    Path.GetDirectoryName(pmAnimation.FullModelPath))
                {
                    ProjectData.Instance.ImageSubFolder = "";
                }
                else if (strSelPath != ProjectData.Instance.ImageSubFolder)
                {
                    ProjectData.Instance.ImageSubFolder = strSelPath;
                }
                
                SetImageFolderText();
            }
        }

        private void cbInvertPan_CheckedChanged(
            object sender, System.EventArgs e)
        {
            ProjectData.Instance.InvertPan = 
                cbInvertPan.Checked;
        }

        private void bnSameAsNIF_Click(object sender, System.EventArgs e)
        {
            ProjectData.Instance.ImageSubFolder = "";
            SetImageFolderText();
        }
    }
}
