using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace SourceGrid2
{
	internal enum CellSizeMode
	{
		Row,
		Col
	}

	/// <summary>
	/// Summary description for frmCellSize.
	/// </summary>
	internal class frmCellSize : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label lblWidthHeight;
		private System.Windows.Forms.Label lblColRow;
		private System.Windows.Forms.ComboBox cbColRow;
		private System.Windows.Forms.Button btClose;
		private SourceLibrary.Windows.Forms.NumericUpDownEx txtWidthHeight;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public frmCellSize()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
		}

		private CellSizeMode m_Mode = CellSizeMode.Col;
		public CellSizeMode CellSizeMode
		{
			get{return m_Mode;}
			set{m_Mode = value;RefreshSetting();}
		}

		private int m_Row = 0;
		public int Row
		{
			get{return m_Row;}
			set{m_Row = value;RefreshSetting();}
		}
		private int m_Col = 0;
		public int Col
		{
			get{return m_Col;}
			set{m_Col = value;RefreshSetting();}
		}
		private GridVirtual m_Grid = null;
		public GridVirtual Grid
		{
			get{return m_Grid;}
			set{m_Grid = value;RefreshSetting();}
		}

		public void LoadSetting(GridVirtual p_Grid, int p_Col, int p_Row, CellSizeMode p_Mode)
		{
			m_Grid = p_Grid;
			m_Col = p_Col;
			m_Mode = p_Mode;
			m_Row = p_Row;

			RefreshSetting();
		}

		public void RefreshSetting()
		{
			if (m_Grid != null)
			{
				if (m_Mode == CellSizeMode.Col)
				{
					lblColRow.Text = "Column :";
					lblWidthHeight.Text = "Width :";
					cbColRow.Items.Clear();
					for (int c = 0; c < m_Grid.ColumnsCount; c++)
					{
						cbColRow.Items.Add("Col "+(c+1).ToString());
					}
					cbColRow.SelectedIndex = m_Col;
				}
				else if (m_Mode == CellSizeMode.Row)
				{
					lblColRow.Text = "Row :";
					lblWidthHeight.Text = "Height :";
					cbColRow.Items.Clear();
					for (int r = 0; r < m_Grid.RowsCount; r++)
					{
						cbColRow.Items.Add("Row "+(r+1).ToString());
					}
					cbColRow.SelectedIndex = m_Row;
				}
			}
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
			this.txtWidthHeight = new SourceLibrary.Windows.Forms.NumericUpDownEx();
			this.lblWidthHeight = new System.Windows.Forms.Label();
			this.lblColRow = new System.Windows.Forms.Label();
			this.cbColRow = new System.Windows.Forms.ComboBox();
			this.btClose = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.txtWidthHeight)).BeginInit();
			this.SuspendLayout();
			// 
			// txtWidthHeight
			// 
			this.txtWidthHeight.Increment = new System.Decimal(new int[] {
																			 2,
																			 0,
																			 0,
																			 0});
			this.txtWidthHeight.Location = new System.Drawing.Point(80, 40);
			this.txtWidthHeight.Maximum = new System.Decimal(new int[] {
																		   10000,
																		   0,
																		   0,
																		   0});
			this.txtWidthHeight.Name = "txtWidthHeight";
			this.txtWidthHeight.Size = new System.Drawing.Size(104, 20);
			this.txtWidthHeight.TabIndex = 0;
			this.txtWidthHeight.ValueChanged += new System.EventHandler(this.txtWidthHeight_ValueChanged);
			// 
			// lblWidthHeight
			// 
			this.lblWidthHeight.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblWidthHeight.Location = new System.Drawing.Point(8, 40);
			this.lblWidthHeight.Name = "lblWidthHeight";
			this.lblWidthHeight.Size = new System.Drawing.Size(72, 16);
			this.lblWidthHeight.TabIndex = 1;
			this.lblWidthHeight.Text = "label1";
			// 
			// lblColRow
			// 
			this.lblColRow.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblColRow.Location = new System.Drawing.Point(8, 16);
			this.lblColRow.Name = "lblColRow";
			this.lblColRow.Size = new System.Drawing.Size(64, 16);
			this.lblColRow.TabIndex = 2;
			this.lblColRow.Text = "label1";
			// 
			// cbColRow
			// 
			this.cbColRow.Location = new System.Drawing.Point(80, 8);
			this.cbColRow.Name = "cbColRow";
			this.cbColRow.Size = new System.Drawing.Size(104, 21);
			this.cbColRow.TabIndex = 3;
			this.cbColRow.Text = "comboBox1";
			this.cbColRow.SelectedIndexChanged += new System.EventHandler(this.cbColRow_SelectedIndexChanged);
			// 
			// btClose
			// 
			this.btClose.BackColor = System.Drawing.SystemColors.Control;
			this.btClose.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btClose.Location = new System.Drawing.Point(112, 72);
			this.btClose.Name = "btClose";
			this.btClose.TabIndex = 4;
			this.btClose.Text = "&Close";
			this.btClose.Click += new System.EventHandler(this.btClose_Click);
			// 
			// frmCellSize
			// 
			this.AcceptButton = this.btClose;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.SystemColors.Control;
			this.CancelButton = this.btClose;
			this.ClientSize = new System.Drawing.Size(194, 104);
			this.Controls.Add(this.btClose);
			this.Controls.Add(this.cbColRow);
			this.Controls.Add(this.lblColRow);
			this.Controls.Add(this.lblWidthHeight);
			this.Controls.Add(this.txtWidthHeight);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "frmCellSize";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Grid properties";
			((System.ComponentModel.ISupportInitialize)(this.txtWidthHeight)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void cbColRow_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (m_Grid != null && cbColRow.SelectedIndex >= 0)
			{
				if (m_Mode == CellSizeMode.Col)
				{
					m_Col = cbColRow.SelectedIndex;
					txtWidthHeight.Value = m_Grid.Columns[m_Col].Width;
				}
				else if (m_Mode == CellSizeMode.Row)
				{
					m_Row = cbColRow.SelectedIndex;
					txtWidthHeight.Value = m_Grid.Rows[m_Row].Height;
				}
			}
		}

		private void txtWidthHeight_ValueChanged(object sender, System.EventArgs e)
		{
			if (m_Grid != null)
			{
				if (m_Mode == CellSizeMode.Col)
				{
					m_Grid.Columns[m_Col].Width = (int)txtWidthHeight.Value;
				}
				else if (m_Mode == CellSizeMode.Row)
				{
					m_Grid.Rows[m_Row].Height = (int)txtWidthHeight.Value;
				}
			}		
		}

		private void btClose_Click(object sender, System.EventArgs e)
		{
			Close();
		}
	}
}
