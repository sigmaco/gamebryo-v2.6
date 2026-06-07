using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// A behavior that support sort and resize. Once created cannot be modified. When calculated automatically the range to sort is all the grid range without the rows minor of the current row and the range header is all the grid range with the rows minor or equal of the current row
	/// </summary>
	public class ColumnHeader : BehaviorModelGroup
	{
		/// <summary>
		/// Column header behavior with sort and resize support, same as SortResizeHeader.
		/// </summary>
		public readonly static ColumnHeader Default;
		/// <summary>
		/// Column header behavior with sort support
		/// </summary>
		public readonly static ColumnHeader SortHeader;
		/// <summary>
		/// Column header behavior with resize support
		/// </summary>
		public readonly static ColumnHeader ResizeHeader;
		/// <summary>
		/// Column header behavior with sort and resize support
		/// </summary>
		public readonly static ColumnHeader SortResizeHeader;
		/// <summary>
		/// Column header behavior without sort and without resize support.
		/// </summary>
		public readonly static ColumnHeader NoSortNoResizeHeader;

		private Resize m_Resize;

		#region Constructor

		/// <summary>
		/// Static constructor
		/// </summary>
		static ColumnHeader()
		{
			SortHeader = new ColumnHeader(true, null, null, null, Button.Default);
			ResizeHeader = new ColumnHeader(false, null, null, Resize.ResizeWidth, Button.Default);
			SortResizeHeader = new ColumnHeader(true);
			NoSortNoResizeHeader = new ColumnHeader(false, null, null, null, Button.Default);
			Default = SortResizeHeader;
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_EnableSort"></param>
		public ColumnHeader(bool p_EnableSort):this(p_EnableSort, null, null, Resize.ResizeWidth, Button.Default)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_EnableSort">True to enable sort, otherwise false.</param>
		/// <param name="p_RangeToSort">If null and p_EnableSort is true then the range is automatically calculated.</param>
		/// <param name="p_HeaderRange">If null and p_EnableSort is true then the range is automatically calculated.</param>
		/// <param name="p_BehaviorResize"></param>
		/// <param name="p_BehaviorButton"></param>
		public ColumnHeader(bool p_EnableSort,IRangeLoader p_RangeToSort, IRangeLoader p_HeaderRange, Resize p_BehaviorResize, IBehaviorModel p_BehaviorButton)
		{
			m_bEnableSort = p_EnableSort;

			if (p_EnableSort)
			{
				m_HeaderRange = p_HeaderRange;
				m_RangeToSort = p_RangeToSort;
			}
			else
			{
				m_HeaderRange = null;
				m_RangeToSort = null;
			}

			m_Resize = p_BehaviorResize;

			SubModels.Add(m_Resize);
			SubModels.Add(p_BehaviorButton);
		}
		#endregion

		#region IBehaviorModel Members
		public override void OnFocusEntering(PositionCancelEventArgs e)
		{
			//check if the user is in a resize region
			if (m_Resize!=null &&
				m_Resize.IsHeightResizing == false && 
				m_Resize.IsWidthResizing == false &&
				IsSortEnable(e) == false &&
				e.Grid.Selection.SelectionMode != GridSelectionMode.Row)
			{
				//if the sort is disable I use the header as a column selector
				e.Grid.Columns[e.Position.Column].Focus();
				if (e.Grid.Selection.EnableMultiSelection)
					e.Grid.Columns[e.Position.Column].Select = true;
			}

			base.OnFocusEntering (e);
		}

		public override void OnClick(PositionEventArgs e)
		{
			base.OnClick(e);

			//eseguo il sort solo se non sono attualmente in resize
			if ( IsSortEnable(e) && 
				(m_Resize == null || (m_Resize.IsHeightResizing == false && m_Resize.IsWidthResizing == false) ) )
			{
				ICellSortableHeader l_tmp = (ICellSortableHeader)e.Cell;
				SortStatus l_Status = l_tmp.GetSortStatus(e.Position);
				if (l_Status.EnableSort)
				{
					if (l_Status.Mode == GridSortMode.Ascending)
						SortColumn(e, false, l_Status.Comparer);
					else
						SortColumn(e, true, l_Status.Comparer);
				}
			}
		}

		#endregion

		#region Sort Methods
		#region Status Properties
		/// <summary>
		/// Range to sort
		/// </summary>
		private IRangeLoader m_RangeToSort;
		/// <summary>
		/// Header range (can be null).
		/// </summary>
		private IRangeLoader m_HeaderRange;

		/// <summary>
		/// Range to sort. If null and EnableSort is true the range is automatically calculated.
		/// </summary>
		public IRangeLoader RangeToSort
		{
			get{return m_RangeToSort;}
		}

		/// <summary>
		/// Header range. If null and EnableSort is true the range is automatically calculated.
		/// </summary>
		public IRangeLoader RangeHeader
		{
			get{return m_HeaderRange;}
		}

		private bool m_bEnableSort = true;
		/// <summary>
		/// True to enable sort otherwise false. Default is true.
		/// </summary>
		public bool EnableSort
		{
			get{return m_bEnableSort;}
		}
		#endregion

		#region Support Function
		/// <summary>
		/// Indicates if for the specified cell the sort is enabled.
		/// </summary>
		/// <param name="e"></param>
		/// <returns></returns>
		public bool IsSortEnable(PositionEventArgs e)
		{
			if (e.Cell is ICellSortableHeader && m_bEnableSort)
			{
				ICellSortableHeader l_tmp = (ICellSortableHeader)e.Cell;
				SortStatus l_Status = l_tmp.GetSortStatus(e.Position);
				return l_Status.EnableSort;
			}
			else
				return false;
		}

		/// <summary>
		/// Sort the current column. If the range contains all the columns this method move directly the row object otherwise move each cell.
		/// </summary>
		/// <param name="e"></param>
		/// <param name="p_bAscending"></param>
		/// <param name="p_Comparer"></param>
		public void SortColumn(PositionEventArgs e, bool p_bAscending, System.Collections.IComparer p_Comparer )
		{
			//verifico che il sort sia abilitato e che ci sia almeno una riga da ordinare oltra a quella corrente
			if (IsSortEnable(e) && e.Position.Row < (e.Grid.RowsCount) && e.Grid.ColumnsCount > 0)
			{
				Range l_RangeToSort;
				Range l_RangeHeader;
				if (m_RangeToSort != null)
					l_RangeToSort = m_RangeToSort.GetRange(e.Grid);
				else
					//the range to sort is all the grid range without the rows < of the current row
					l_RangeToSort = new Range(e.Position.Row+1, 0, e.Grid.RowsCount-1, e.Grid.ColumnsCount-1);

				if (m_HeaderRange != null)
					l_RangeHeader = m_HeaderRange.GetRange(e.Grid);
				else
					//the range header is all the grid range with the rows <= of the current row
					l_RangeHeader = new Range(0, 0, e.Position.Row, e.Grid.ColumnsCount-1);

				ICellSortableHeader l_CellSortable = (ICellSortableHeader)e.Cell;
				if (e.Grid.RowsCount > (e.Position.Row+1) && e.Grid.ColumnsCount > e.Grid.FixedColumns)
				{
					e.Grid.SortRangeRows(l_RangeToSort, e.Position.Column, p_bAscending, p_Comparer);
					if (p_bAscending)
						l_CellSortable.SetSortMode(e.Position,GridSortMode.Ascending);
					else
						l_CellSortable.SetSortMode(e.Position, GridSortMode.Descending);
		
					//Remove the image from others ColHeaderSort
					for (int r  = l_RangeHeader.Start.Row; r <= l_RangeHeader.End.Row; r++)
					{
						for (int c  = l_RangeHeader.Start.Column; c <= l_RangeHeader.End.Column; c++)
						{
							Cells.ICellVirtual l_tmp = e.Grid.GetCell(r,c);
							if (l_tmp != l_CellSortable && 
								l_tmp != null && 
								l_tmp is ICellSortableHeader)
								((ICellSortableHeader)l_tmp).SetSortMode(new Position(r, c) ,GridSortMode.None);
						}
					}
				}
			}
		}
		#endregion
		#endregion
	}
}
