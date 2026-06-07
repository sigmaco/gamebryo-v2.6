using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace SourceGrid2.Cells.Real
{
	/// <summary>
	/// Represents a Cell in a grid, with Cell.Value support and row/col span. Support also ToolTipText, ContextMenu and Cursor
	/// </summary>
	public class Cell : Virtual.CellVirtual, ICell, ICellToolTipText, ICellCursor, ICellContextMenu
	{
		#region Constructors
		/// <summary>
		/// Cell constructor
		/// </summary>
		public Cell():this(null)
		{
		}

		/// <summary>
		/// Cell constructor, using VisualModels.Common.Default, BehaviorModels.ToolTipText.Default, BehaviorModels.Cursor.Default, BehaviorModels.ContextMenu.Default
		/// </summary>
		/// <param name="p_Value">The value of the cell</param>
		public Cell(object p_Value):
			this(p_Value, VisualModels.Common.Default, 
#if !MINI
			BehaviorModels.ToolTipText.Default, BehaviorModels.Cursor.Default,
#else
			null, null,
#endif
			BehaviorModels.ContextMenu.Default)
		{
		}

		/// <summary>
		/// Create a cell with an editor using the type specified. (using Utility.CreateCellModel).
		/// </summary>
		/// <param name="p_Value"></param>
		/// <param name="p_Type">Type of the cell</param>
		public Cell(object p_Value, Type p_Type):this(p_Value)
		{
			DataModel = Utility.CreateDataModel(p_Type);
		}

		/// <summary>
		/// Cell constructor
		/// </summary>
		/// <param name="p_Value">The value of the cell</param>
		/// <param name="p_Editor">The editor of this cell</param>
		public Cell(object p_Value, DataModels.IDataModel p_Editor):this(p_Value)
		{
			DataModel = p_Editor;
		}

		/// <summary>
		/// Create a new instance of the cell.
		/// </summary>
		/// <param name="p_Value">Initial value of the cell.</param>
		/// <param name="p_Editor">Formatters used for string conversion, if null is used a shared default formatter.</param>
		/// <param name="p_VisualModel">Visual properties of the current cell, if null is used a shared default properties.</param>
		public Cell(object p_Value, DataModels.IDataModel p_Editor, VisualModels.IVisualModel p_VisualModel):this(p_Value)
		{
			DataModel = p_Editor;

			if (p_VisualModel!=null)
				VisualModel = p_VisualModel;
		}

		/// <summary>
		/// Cell constructor
		/// </summary>
		/// <param name="p_Value">The value of the cell</param>
		/// <param name="p_VisualModel">Visual properties of the current cell, if null is used a shared default model VisualModels.Common.Default.</param>
		/// <param name="p_BehaviorToolTipText"></param>
		/// <param name="p_BehaviorCursor"></param>
		/// <param name="p_BehaviorContextMenu"></param>
		public Cell(object p_Value, 
					VisualModels.IVisualModel p_VisualModel, 
					BehaviorModels.IBehaviorModel p_BehaviorToolTipText, 
					BehaviorModels.IBehaviorModel p_BehaviorCursor, 
					BehaviorModels.IBehaviorModel p_BehaviorContextMenu):base(null, p_VisualModel)
		{
			Value = p_Value;
			Behaviors.Add(p_BehaviorToolTipText);
			Behaviors.Add(p_BehaviorCursor);
			Behaviors.Add(p_BehaviorContextMenu);
		}

		#endregion

		#region Cell Data (Value, DisplayText, Tag)

		//ATTENTION: is reccomanded that all the actions fired by the user interface does not modify this property
		// instead call the CellEditor.ChangeCellValue to preserve data consistence

		/// <summary>
		/// Get the value of the current cell
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public override object GetValue(Position p_Position)
		{
			return Value;
		}

		/// <summary>
		/// Set the value of the cell
		/// </summary>
		/// <param name="p_Position"></param>
		/// <param name="p_Value"></param>
		public override void SetValue(Position p_Position, object p_Value)
		{
			if (m_Value != p_Value)
			{
				m_Value = p_Value;

				OnValueChanged(new PositionEventArgs(p_Position, this));
			}
		}

		/// <summary>
		/// The string representation of the Cell.GetValue method (default Value.ToString()).
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public override string GetDisplayText(Position p_Position)
		{
			return DisplayText;
		}


		/// <summary>
		/// The string representation of the Cell.Value property (default Value.ToString())
		/// </summary>
		public virtual string DisplayText
		{
			get
			{
				return base.GetDisplayText(m_Range.Start);
			}
		}

		private object m_Value = null;
		/// <summary>
		/// Value of the cell 
		/// </summary>
		public virtual object Value
		{
			get{return m_Value;}
			set
			{
				SetValue(m_Range.Start, value);
			}
		}

		/// <summary>
		/// Object to put additional info for this cell
		/// </summary>
		private object m_Tag = null;
		/// <summary>
		/// Object to put additional info for this cell
		/// </summary>
		public virtual object Tag
		{
			get{return m_Tag;}
			set{m_Tag = value;}
		}

		/// <summary>
		/// ToString method
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			return DisplayText;
		}
		#endregion

		#region LinkToGrid

		/// <summary>
		/// Link the cell at the specified grid. 
		/// For Cell derived classes you must call BindToGrid(Grid p_grid, Position p_Position).
		/// </summary>
		/// <param name="p_grid"></param>
		public override void BindToGrid(GridVirtual p_grid)
		{
			throw new SourceGridException("Invalid method for this class, use BindToGrid(Grid p_grid, Position p_Position)");
		}


		/// <summary>
		/// Link the cell at the specified grid.
		/// REMARKS: To insert a cell in a grid use Grid.InsertCell, this methos is for internal use only
		/// </summary>
		/// <param name="p_grid"></param>
		/// <param name="p_Position"></param>
		public virtual void BindToGrid(Grid p_grid, Position p_Position)
		{
			m_Range.MoveTo(p_Position);
			base.BindToGrid(p_grid);
			RefreshSpanSearch();
		}
		/// <summary>
		/// Remove the link of the cell from the previous grid.
		/// REMARKS: To remove a cell from a grid use the grid.RemoveCell, this method is for internal use only
		/// </summary>
		public override void UnBindToGrid()
		{
			m_Range.MoveTo(Position.Empty);
			base.UnBindToGrid();
		}
		#endregion

		#region Range and Position
		private Range m_Range = Range.Empty; //la posizione è Range.Start

		/// <summary>
		/// Returns the current Row and Col position. If this cell is not attached to the grid returns Position.Empty. And the range occupied by the current cell.
		/// Returns the Range of the cells occupied by the current cell. If RowSpan and ColSpan = 1 then this method returns a single cell.
		/// </summary>
		public Range Range
		{
			get{return m_Range;}
		}

		/// <summary>
		/// Current Row
		/// </summary>
		public int Row
		{
			get{return m_Range.Start.Row;}
		}

		/// <summary>
		/// Current Column
		/// </summary>
		public int Column
		{
			get{return m_Range.Start.Column;}
		}


		/// <summary>
		/// Location absolute to the whole grid
		/// </summary>
		public Point LocationAbsolute
		{
			get
			{
				if (m_Range.IsEmpty() || Grid == null)
					return Point.Empty;
				return new Point(this.Grid.Columns[this.Column].Left, this.Grid.Rows[this.Row].Top);
			}
		}

		/// <summary>
		/// Location relative to the current scroll position
		/// </summary>
		public Point LocationRelative
		{
			get
			{
				if (m_Range.IsEmpty() || Grid == null)
					return Point.Empty;
				return Grid.PointAbsoluteToRelative(LocationAbsolute);
			}
		}
 
		/// <summary>
		/// Size occupied by the current cell
		/// </summary>
		public Size Size
		{
			get
			{
				if (m_Range.IsEmpty() || Grid == null)
					return Size.Empty;
				return Grid.RangeToAbsoluteRect(m_Range).Size;
			}
		}
		#endregion

		#region Row/Col Span

		private void RefreshSpanSearch()
		{
			if (Grid is SourceGrid2.Grid)
			{
				Grid l_grid = (SourceGrid2.Grid)Grid;
				l_grid.SetMaxSpanSearch(Math.Max(ColumnSpan, RowSpan)-1, false);
			}
		}

		/// <summary>
		/// ColSpan for merge operation
		/// </summary>
		public int ColumnSpan
		{
			get{return m_Range.ColumnsCount;}
			set
			{
				m_Range.ColumnsCount = value;
				RefreshSpanSearch();
				Invalidate();
			}
		}
		/// <summary>
		/// RowSpan for merge operation
		/// </summary>
		public int RowSpan
		{
			get{return m_Range.RowsCount;}
			set
			{
				m_Range.RowsCount = value;
				RefreshSpanSearch();
				Invalidate();
			}
		}

		/// <summary>
		/// Returns true if the position specified is inside the current cell range (use Range.Contains)
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public virtual bool ContainsPosition(Position p_Position)
		{
			return m_Range.Contains(p_Position);
		}
		#endregion

		#region CalculateRequiredSize
		/// <summary>
		/// If the cell is not linked to a grid the result is not accurate (Font can be null). Call InternalMeasure with RowSpan and ColSpan
		/// </summary>
		/// <param name="p_Position">Position of the current cell</param>
		/// <param name="g"></param>
		/// <param name="maxLayoutArea">SizeF structure that specifies the maximum layout area for the text. If width or height are zero the value is set to a default maximum value.</param>
		/// <returns></returns>
		public override Size Measure(Position p_Position, Graphics g, SizeF maxLayoutArea)
		{
			return InternalMeasure(p_Position, g, maxLayoutArea, RowSpan, ColumnSpan);
		}
		#endregion

		#region Selection

		/// <summary>
		/// Gets or Sets if the current cell is selected
		/// </summary>
		public bool Select
		{
			get
			{
				if (Grid!=null)
					return Grid.Selection.Contains( m_Range.Start );
				else
					return false;
			}
			set
			{
				if (Select != value && Grid != null)
				{
					if (value)
						Grid.Selection.Add( m_Range.Start );
					else
						Grid.Selection.Remove( m_Range.Start );
				}
			}
		}

		#endregion

		#region Focus
		/// <summary>
		/// True if the has the focus
		/// </summary>
		public bool Focused
		{
			get
			{
				if (Grid!=null)
					return Grid.Selection.FocusPosition == m_Range.Start;
				else
					return false;
			}
		}

		/// <summary>
		/// Give the focus at the cell
		/// </summary>
		/// <returns>Returns if the cell can receive the focus</returns>
		public bool Focus()
		{
			if (Grid!=null)
				return Grid.Selection.Focus(m_Range.Start);
			else
				return false;
		}
		
		/// <summary>
		/// Remove the focus from the cell
		/// </summary>
		/// <returns>Returns true if the cell can leave the focus otherwise false</returns>
		public bool LeaveFocus()
		{
			if (Grid!=null && Focused)
				return Grid.Selection.Focus(Position.Empty);
			else
				return true;
		}

		#endregion

		#region Editing
		/// <summary>
		/// True if this cell is currently in edit state, otherwise false.
		/// </summary>
		public bool IsEditing()
		{
			return IsEditing(m_Range.Start);
		}

		/// <summary>
		/// Start the edit operation with the current editor specified in the Model property. Using the current cell position.
		/// </summary>
		/// <param name="p_Position">Not used with this type of class.</param>
		/// <param name="p_NewStartEditValue">The value that the editor receive</param>
		public override void StartEdit(Position p_Position, object p_NewStartEditValue)
		{
			base.StartEdit (m_Range.Start, p_NewStartEditValue); //uso come posizione quella corrente
		}
		#endregion

		#region Invalidate
		/// <summary>
		/// Invalidate this cell
		/// </summary>
		public override void Invalidate()
		{
			if (Grid!=null)
				Grid.InvalidateRange(Range);
		}
		#endregion

		#region ToolTipText

		private string m_ToolTipText;
		/// <summary>
		/// ToolTipText for the current cell
		/// </summary>
		public virtual string ToolTipText
		{
			get{return m_ToolTipText;}
			set{m_ToolTipText = value;}
		}

		/// <summary>
		/// Returns the tooltip text for the current cell position. Returns the ToolTipText property.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public virtual string GetToolTipText(Position p_Position)
		{
			return m_ToolTipText;
		}
		#endregion

		#region ICellContextMenu
		/// <summary>
		/// To optimize creation of the cell I create only the collection when we need it
		/// </summary>
		private MenuCollection m_ContextMenuItems = null;

		/// <summary>
		/// Context Menu
		/// </summary>
		public MenuCollection ContextMenuItems
		{
			get
			{
				if (m_ContextMenuItems==null)
					m_ContextMenuItems = new MenuCollection();
				return m_ContextMenuItems;
			}
			set{m_ContextMenuItems = value;}
		}

		/// <summary>
		/// Get the contextmenu of the specified cell
		/// </summary>
		/// <param name="p_Position"></param>
		public MenuCollection GetContextMenu(Position p_Position)
		{
			return m_ContextMenuItems;
		}
		#endregion

		#region ICellCursor Members

		private Cursor m_Cursor;

		/// <summary>
		/// Cursor of the cell
		/// </summary>
		public Cursor Cursor
		{
			get{return m_Cursor;}
			set{m_Cursor = value;}
		}

		/// <summary>
		/// Get the Cursor property. This is required by the ICellCursor interface.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public virtual Cursor GetCursor(Position p_Position)
		{
			return m_Cursor;
		}
		#endregion
	}
}
