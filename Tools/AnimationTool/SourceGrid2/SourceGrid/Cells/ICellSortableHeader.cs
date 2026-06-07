using System;

namespace SourceGrid2.Cells
{
	/// <summary>
	/// Summary description for ICellSortableHeader.
	/// </summary>
	public interface ICellSortableHeader
	{
		/// <summary>
		/// Returns the current sort status
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		SortStatus GetSortStatus(Position p_Position);

		/// <summary>
		/// Set the current sort mode
		/// </summary>
		/// <param name="p_Position"></param>
		/// <param name="p_Mode"></param>
		void SetSortMode(Position p_Position, GridSortMode p_Mode);
	}
}

namespace SourceGrid2
{
	public struct SortStatus
	{
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Mode">Status of current sort.</param>
		/// <param name="p_EnableSort">True to enable sort otherwise false</param>
		public SortStatus(GridSortMode p_Mode, bool p_EnableSort)
		{
			Mode = p_Mode;
			EnableSort = p_EnableSort;
			Comparer = null;
		}
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Mode">Status of current sort.</param>
		/// <param name="p_EnableSort">True to enable sort otherwise false</param>
		/// <param name="p_Comparer">Comparer used to sort the column. The comparer will take 2 Cell. If null the default ValueCellComparer is used.</param>
		public SortStatus(GridSortMode p_Mode, bool p_EnableSort, System.Collections.IComparer p_Comparer):this(p_Mode, p_EnableSort)
		{
			Comparer = p_Comparer;
		}
		public GridSortMode Mode;

		public bool EnableSort;

		public System.Collections.IComparer Comparer;
	}
}
