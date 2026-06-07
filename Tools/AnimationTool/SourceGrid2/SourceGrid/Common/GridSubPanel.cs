using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace SourceGrid2
{
	/// <summary>
	/// Summary description for GridSubPanel.
	/// </summary>
#if !MINI
	[System.ComponentModel.ToolboxItem(false)]
#endif
	public class GridSubPanel : System.Windows.Forms.UserControl
	{
		#region Constructor
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_GridContainer"></param>
		/// <param name="p_CustomDraw">True to use custom draw function.</param>
		public GridSubPanel(GridVirtual p_GridContainer, bool p_CustomDraw)
		{
#if !MINI
			toolTip = new System.Windows.Forms.ToolTip();

			ToolTipText = "";


			if (p_CustomDraw)
			{
				//to remove flicker and use custom draw
				SetStyle(ControlStyles.UserPaint, true);
				SetStyle(ControlStyles.AllPaintingInWmPaint, true);
				SetStyle(ControlStyles.DoubleBuffer, true);
//				SetStyle(ControlStyles.Opaque, true); //questo non permetteva la trasparenza
				SetStyle(ControlStyles.SupportsTransparentBackColor, true);
			}
#endif

			m_GridContainer = p_GridContainer;

			m_ControlsRepository = new ControlsRepository(this);
		}

		#endregion

		#region Grid
		private GridVirtual m_GridContainer;
		/// <summary>
		/// Grid
		/// </summary>
		public GridVirtual Grid
		{
			get{return m_GridContainer;}
		}

		/// <summary>
		/// Returns a Range that represents the complete cells of the panel
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public Range CompleteRange
		{
			get
			{
				if (Grid.RowsCount == 0 || Grid.ColumnsCount == 0)
					return Range.Empty;

				if (ContainerType == GridSubPanelType.Left)
				{
					if (Grid.FixedColumns > 0)
						return new Range(Grid.FixedRows, 0, Grid.RowsCount-1, Grid.FixedColumns-1);
					else
						return Range.Empty;
				}
				else if (ContainerType == GridSubPanelType.Top)
				{
					if (Grid.FixedRows > 0)
						return new Range(0, Grid.FixedColumns, Grid.FixedRows-1, Grid.ColumnsCount-1);
					else
						return Range.Empty;
				}
				else if (ContainerType == GridSubPanelType.TopLeft)
				{
					if (Grid.FixedColumns > 0 && Grid.FixedRows > 0)
						return new Range(0, 0, Grid.FixedRows-1, Grid.FixedColumns-1);
					else
						return Range.Empty;
				}
				else
				{
					return new Range(Grid.FixedRows, Grid.FixedColumns, Grid.RowsCount-1, Grid.ColumnsCount-1);
				}
			}
		}
		#endregion

#if !MINI
		#region InputKeys
		/// <summary>
		/// Allow the grid to handle specials keys like Arrows and Tab. See also Grid.SpecialKeys
		/// </summary>
		/// <param name="keyData"></param>
		/// <returns></returns>
		protected override bool IsInputKey(Keys keyData)
		{
			//serve per poter gestire le freccie e tab

			if ( (Grid.SpecialKeys & GridSpecialKeys.Arrows) == GridSpecialKeys.Arrows)
			{
				switch (keyData)
				{
					case Keys.Up:
					case Keys.Down:
					case Keys.Left:
					case Keys.Right:
						//altrimenti venivano (per qualche strana ragione) selezionate le scrollbars
					case (Keys.Up|Keys.Shift):
					case (Keys.Down|Keys.Shift):
					case (Keys.Left|Keys.Shift):
					case (Keys.Right|Keys.Shift):
						return true;
				}
			}

			if ( (Grid.SpecialKeys & GridSpecialKeys.Tab) == GridSpecialKeys.Tab)
			{
				switch (keyData)
				{
					case Keys.Tab:
					case (Keys.Tab|Keys.Shift):
						return true;
				}
			}

			return base.IsInputKey(keyData);
		}
		#endregion
#endif

#if !MINI
		#region ToolTip
		private System.Windows.Forms.ToolTip toolTip;
		/// <summary>
		/// ToolTip text
		/// </summary>
		public string ToolTipText
		{
			get{return toolTip.GetToolTip(this);}
			set{toolTip.SetToolTip(this,value);}
		}
		/// <summary>
		/// ToolTip active
		/// </summary>
		public bool ToolTipActive
		{
			get{return toolTip.Active;}
			set{toolTip.Active = value;}
		}

		#endregion
#endif

		#region Cells Search

		/// <summary>
		/// Returns the cell at the specified grid view relative point (the point must be relative to the grid display region not to the panel display region)
		/// </summary>
		/// <param name="p_RelativeViewPoint">Point</param>
		/// <returns></returns>
		public virtual Position PositionAtPointGrid(Point p_RelativeViewPoint)
		{
			int l_Row, l_Col;
			GridSubPanelType l_Type = ContainerType;

			if (l_Type == GridSubPanelType.TopLeft)
			{
				l_Row = m_GridContainer.Rows.RowAtPoint(p_RelativeViewPoint.Y, true);
				l_Col = m_GridContainer.Columns.ColumnAtPoint(p_RelativeViewPoint.X, true);
				if (l_Row>=m_GridContainer.FixedRows)
					l_Row = Position.c_EmptyIndex;
				if (l_Col>=m_GridContainer.FixedColumns)
					l_Col = Position.c_EmptyIndex;
			}
			else if (l_Type == GridSubPanelType.Top)
			{
				Point l_AbsPoint = m_GridContainer.PointRelativeToAbsolute(p_RelativeViewPoint);
				l_Row = m_GridContainer.Rows.RowAtPoint(p_RelativeViewPoint.Y, true);
				l_Col = m_GridContainer.Columns.ColumnAtPoint(l_AbsPoint.X, true);
				if (l_Row>=m_GridContainer.FixedRows)
					l_Row = Position.c_EmptyIndex;

				if (l_Col<m_GridContainer.FixedColumns)
					l_Col=Position.c_EmptyIndex;
			}
			else if (l_Type == GridSubPanelType.Left)
			{
				Point l_AbsPoint = m_GridContainer.PointRelativeToAbsolute(p_RelativeViewPoint);
				l_Row = m_GridContainer.Rows.RowAtPoint(l_AbsPoint.Y, true);
				l_Col = m_GridContainer.Columns.ColumnAtPoint(p_RelativeViewPoint.X, true);
				if (l_Col>=m_GridContainer.FixedColumns)
					l_Col = Position.c_EmptyIndex;

				if (l_Row<m_GridContainer.FixedRows)
					l_Row=Position.c_EmptyIndex;
			}
			else //scrollable
			{
				Point l_AbsPoint = m_GridContainer.PointRelativeToAbsolute(p_RelativeViewPoint);
				l_Row = m_GridContainer.Rows.RowAtPoint(l_AbsPoint.Y, true);
				l_Col = m_GridContainer.Columns.ColumnAtPoint(l_AbsPoint.X, true);
				
				//TODO devo controllare che non cada nelle celle fisse?
			}

			if (l_Row==Position.c_EmptyIndex || l_Col==Position.c_EmptyIndex)
				return Position.Empty;

			return new Position(l_Row, l_Col);
		}

		/// <summary>
		/// Returns a range of cells from the specified rectangle, relative to the current grid container, using scrolling information.
		/// </summary>
		/// <param name="p_RelativeRect">A grid relative rectangle (not panel relative)</param>
		/// <returns></returns>
		public Range RangeAtDisplayRect(Rectangle p_RelativeRect)
		{
			Rectangle l_Absolute = m_GridContainer.RectangleRelativeToAbsolute(p_RelativeRect);
			GridSubPanelType l_Type = ContainerType;

			if (l_Type == GridSubPanelType.TopLeft)
				return RangeAtAbsRect(new Rectangle(p_RelativeRect.X, p_RelativeRect.Y, l_Absolute.Width, l_Absolute.Height));
			else if (l_Type == GridSubPanelType.Top)
				return RangeAtAbsRect(new Rectangle(l_Absolute.X, p_RelativeRect.Y, l_Absolute.Width, l_Absolute.Height));
			else if (l_Type == GridSubPanelType.Left)
				return RangeAtAbsRect(new Rectangle(p_RelativeRect.X, l_Absolute.Y, l_Absolute.Width, l_Absolute.Height));
			else//scrollable
				return RangeAtAbsRect(l_Absolute);
		}

		/// <summary>
		/// Returns a range of cells from the specified absolute rectangle. Returns Empty if no valid cells are found.
		/// </summary>
		/// <param name="p_AbsoluteRect"></param>
		/// <returns></returns>
		public Range RangeAtAbsRect(Rectangle p_AbsoluteRect)
		{
			int l_Start_R, l_Start_C, l_End_R, l_End_C;
			GridSubPanelType l_Type = ContainerType;

			Range l_tmp = m_GridContainer.RangeAtAbsRect(p_AbsoluteRect);
			if (l_tmp.IsEmpty())
				return l_tmp;
			l_Start_R = l_tmp.Start.Row;
			l_Start_C = l_tmp.Start.Column;

			l_End_R = l_tmp.End.Row;
			l_End_C = l_tmp.End.Column;

			if (l_Type == GridSubPanelType.TopLeft)
			{
				if (l_Start_R>=m_GridContainer.FixedRows)
					l_Start_R = Position.c_EmptyIndex;

				if (l_Start_C>=m_GridContainer.FixedColumns)
					l_Start_C = Position.c_EmptyIndex;

				if (l_End_R>=m_GridContainer.FixedRows)
					l_End_R = m_GridContainer.FixedRows-1;

				if (l_End_C>=m_GridContainer.FixedColumns)
					l_End_C = m_GridContainer.FixedColumns-1;
			}
			else if (l_Type == GridSubPanelType.Top)
			{
				if (l_Start_R>=m_GridContainer.FixedRows)
					l_Start_R = Position.c_EmptyIndex;

				if (l_End_R>=m_GridContainer.FixedRows)
					l_End_R = m_GridContainer.FixedRows-1;

				if (l_Start_C<m_GridContainer.FixedColumns)
				{
					l_Start_C=m_GridContainer.FixedColumns;
					if (l_Start_C>=m_GridContainer.ColumnsCount)
						l_Start_C=Position.c_EmptyIndex;
				}
				if (l_End_C<m_GridContainer.FixedColumns)
				{
					l_End_C=m_GridContainer.FixedColumns;
					if (l_End_C>=m_GridContainer.ColumnsCount)
						l_End_C=Position.c_EmptyIndex;
				}
			}
			else if (l_Type == GridSubPanelType.Left)
			{
				if (l_Start_C>=m_GridContainer.FixedColumns)
					l_Start_C = Position.c_EmptyIndex;

				if (l_End_C>=m_GridContainer.FixedColumns)
					l_End_C = m_GridContainer.FixedColumns-1;

				if (l_Start_R<m_GridContainer.FixedRows)
				{
					l_Start_R=m_GridContainer.FixedRows;
					if (l_Start_R>=m_GridContainer.RowsCount)
						l_Start_R=Position.c_EmptyIndex;
				}
				if (l_End_R<m_GridContainer.FixedRows)
				{
					l_End_R=m_GridContainer.FixedRows;
					if (l_End_R>=m_GridContainer.RowsCount)
						l_End_R=Position.c_EmptyIndex;
				}
			}

			//TODO nel caso di ScrollablePanel dovrei verificare di non restituire celle fisse?

			if (l_Start_R==Position.c_EmptyIndex || l_Start_C==Position.c_EmptyIndex 
				|| l_End_C==Position.c_EmptyIndex || l_End_R==Position.c_EmptyIndex)
				return Range.Empty;

			return new Range(l_Start_R, l_Start_C, l_End_R, l_End_C);
		}

		#endregion

		#region Point and Rectangle Conversion
		/// <summary>
		/// Convert a grid relative point to a panel relative point
		/// </summary>
		/// <param name="p_GridPoint"></param>
		/// <returns></returns>
		public Point PointGridToPanel(Point p_GridPoint)
		{
			return new Point(p_GridPoint.X-Left,p_GridPoint.Y-Top);
		}

		/// <summary>
		/// Convert a panel relative point to a grid relative point
		/// </summary>
		/// <param name="p_PanelPoint"></param>
		/// <returns></returns>
		public Point PointPanelToGrid(Point p_PanelPoint)
		{
			return new Point(p_PanelPoint.X+Left,p_PanelPoint.Y+Top);
		}

		/// <summary>
		/// Converts a panel coordinate rectangle to Grid coordinate rectangle
		/// </summary>
		/// <param name="p_PanelRectangle"></param>
		/// <returns></returns>
		public Rectangle RectanglePanelToGrid(Rectangle p_PanelRectangle)
		{
			Point l_Point = PointPanelToGrid(p_PanelRectangle.Location);

			return new Rectangle(l_Point.X, l_Point.Y, p_PanelRectangle.Size.Width, p_PanelRectangle.Size.Height);
		}

		/// <summary>
		/// Converts a grid coordinate rectangle to Panel coordinate rectangle
		/// </summary>
		/// <param name="p_GridRectangle"></param>
		/// <returns></returns>
		public Rectangle RectangleGridToPanel(Rectangle p_GridRectangle)
		{
			Point l_Point = PointGridToPanel(p_GridRectangle.Location);

			return new Rectangle(l_Point.X, l_Point.Y, p_GridRectangle.Size.Width, p_GridRectangle.Size.Height);
		}

		#endregion

		#region ContainerType
		private GridSubPanelType ContainerType
		{
			get
			{
				if (m_GridContainer.LeftPanel==this)
					return GridSubPanelType.Left;
				else if (m_GridContainer.TopPanel==this)
					return GridSubPanelType.Top;
				else if (m_GridContainer.TopLeftPanel==this)
					return GridSubPanelType.TopLeft;
				else
					return GridSubPanelType.Scrollable;
			}
		}
		#endregion

#if MINI
		//per evitare lo sfarfallio nel compact framework
		//per sostituire SetStyle UserDraw
		protected override void OnPaintBackground(PaintEventArgs e)
		{
			//base.OnPaintBackground (e);
		}
#endif

		#region Events Dispatcher
		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint (e);

//#warning temporaneo per testare i panels
//			GridSubPanelType l_Type = ContainerType;
//			if (l_Type == GridSubPanelType.TopLeft)
//				e.Graphics.FillRectangle(new SolidBrush(Color.Yellow),e.ClipRectangle);
//			else if (l_Type == GridSubPanelType.Left)
//				e.Graphics.FillRectangle(new SolidBrush(Color.Red),e.ClipRectangle);
//			else if (l_Type == GridSubPanelType.Top)
//				e.Graphics.FillRectangle(new SolidBrush(Color.Green),e.ClipRectangle);
//			else
//				e.Graphics.FillRectangle(new SolidBrush(Color.Black),e.ClipRectangle);
//			ControlPaint.DrawBorder(e.Graphics, DisplayRectangle,Color.Blue,ButtonBorderStyle.Solid);

			GridSubPanelType l_Type = ContainerType;
			if (l_Type == GridSubPanelType.TopLeft)
				m_GridContainer.OnTopLeftPanelPaint(e);
			else if (l_Type == GridSubPanelType.Left)
				m_GridContainer.OnLeftPanelPaint(e);
			else if (l_Type == GridSubPanelType.Top)
				m_GridContainer.OnTopPanelPaint(e);
			else //scrollable
				m_GridContainer.OnScrollablePanelPaint(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown (e);
			Point l_GridPoint = PointPanelToGrid(new Point(e.X, e.Y));
#if !MINI
			MouseEventArgs l_MouseArgs = new MouseEventArgs(e.Button, e.Clicks, l_GridPoint.X, l_GridPoint.Y, e.Delta);
#else
			MouseEventArgs l_MouseArgs = new MouseEventArgs(e.Button, 1, l_GridPoint.X, l_GridPoint.Y, 0);
#endif
			m_GridContainer.OnGridMouseDown(l_MouseArgs);
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			base.OnMouseUp (e);
			Point l_GridPoint = PointPanelToGrid(new Point(e.X, e.Y));
#if !MINI
			MouseEventArgs l_MouseArgs = new MouseEventArgs(e.Button, e.Clicks, l_GridPoint.X, l_GridPoint.Y, e.Delta);
#else
			MouseEventArgs l_MouseArgs = new MouseEventArgs(e.Button, 1, l_GridPoint.X, l_GridPoint.Y, 0);
#endif
			m_GridContainer.OnGridMouseUp(l_MouseArgs);
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove (e);
			Point l_GridPoint = PointPanelToGrid(new Point(e.X, e.Y));
#if !MINI
			MouseEventArgs l_MouseArgs = new MouseEventArgs(e.Button, e.Clicks, l_GridPoint.X, l_GridPoint.Y, e.Delta);
#else
			MouseEventArgs l_MouseArgs = new MouseEventArgs(e.Button, 1, l_GridPoint.X, l_GridPoint.Y, 0);
#endif
			m_GridContainer.OnGridMouseMove(l_MouseArgs);
		}

		protected override void OnClick(EventArgs e)
		{
			base.OnClick (e);
			m_GridContainer.OnGridClick(e);
		}

#if !MINI
		protected override void OnDoubleClick(EventArgs e)
		{
			base.OnDoubleClick (e);
			m_GridContainer.OnGridDoubleClick(e);
		}
#endif		
		protected override void OnKeyDown(KeyEventArgs e)
		{
			base.OnKeyDown (e);
			m_GridContainer.OnGridKeyDown(e);
		}

		protected override void OnKeyUp(KeyEventArgs e)
		{
			base.OnKeyUp (e);
			m_GridContainer.OnGridKeyUp(e);
		}

		protected override void OnKeyPress(KeyPressEventArgs e)
		{
			base.OnKeyPress (e);
			m_GridContainer.OnGridKeyPress(e);
		}
#if !MINI
		#region Drag Events
		protected override void OnDragDrop(DragEventArgs drgevent)
		{
			base.OnDragDrop (drgevent);
			m_GridContainer.OnPanelDragDrop(drgevent);
		}

		protected override void OnDragEnter(DragEventArgs drgevent)
		{
			base.OnDragEnter (drgevent);
			m_GridContainer.OnPanelDragEnter(drgevent);
		}

		protected override void OnDragLeave(EventArgs e)
		{
			base.OnDragLeave (e);
			m_GridContainer.OnPanelDragLeave(e);
		}

		protected override void OnDragOver(DragEventArgs drgevent)
		{
			base.OnDragOver (drgevent);
			m_GridContainer.OnPanelDragOver(drgevent);
		}
		#endregion
#endif

#if !MINI
		protected override void OnMouseEnter(EventArgs e)
		{
			base.OnMouseEnter (e);
			m_GridContainer.OnPanelMouseEnter(e);
		}

		protected override void OnMouseHover(EventArgs e)
		{
			base.OnMouseHover (e);
			m_GridContainer.OnGridMouseHover(e);
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			base.OnMouseLeave (e);
			m_GridContainer.OnPanelMouseLeave(e);
		}
#endif
		#endregion

		#region ControlsRepository
		private ControlsRepository m_ControlsRepository;

		/// <summary>
		/// A collection of controls used for editing operations
		/// </summary>
		public ControlsRepository ControlsRepository
		{
			get{return m_ControlsRepository;}
		}
		#endregion


		private enum GridSubPanelType
		{
			TopLeft,
			Top,
			Left,
			Scrollable
		}
	}
}
