using System;
using System.ComponentModel;
using System.Collections;
namespace SourceGrid2
{
	public delegate void ValidatingEventHandler(object sender, ValidatingEventArgs e);
	public delegate void ValidatingCellEventHandler(object sender, ValidatingCellEventArgs e);


	public delegate void CellEventHandler(object sender, CellEventArgs e);
//	public delegate void CellCancelEventHandler(object sender, CellCancelEventArgs e);
	
	public class ScrollPositionChangedEventArgs : EventArgs
	{
		private int m_NewValue;
		private int m_OldValue;
		public int NewValue
		{
			get{return m_NewValue;}
		}
		public int OldValue
		{
			get{return m_OldValue;}
		}
		public int Delta
		{
			get{return m_OldValue-m_NewValue;}
		}

		public ScrollPositionChangedEventArgs(int p_NewValue, int p_OldValue)
		{
			m_NewValue = p_NewValue;
			m_OldValue = p_OldValue;
		}
	}
	public delegate void ScrollPositionChangedEventHandler(object sender, ScrollPositionChangedEventArgs e);

	/// <summary>
	/// EventArgs used by the FocusRowEnter
	/// </summary>
	public class RowEventArgs : EventArgs
	{
		private RowInfo m_Row;
		/// <summary>
		/// Row
		/// </summary>
		public RowInfo Row
		{
			get{return m_Row;}
			set{m_Row = value;}
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pRow"></param>
		public RowEventArgs(RowInfo pRow)
		{
			m_Row = pRow;
		}
	}

	/// <summary>
	/// EventHandler used by the FocusRowEnter
	/// </summary>
	public delegate void RowEventHandler(object sender, RowEventArgs e);

	/// <summary>
	/// EventArgs used by the FocusRowLeaving
	/// </summary>
	public class RowCancelEventArgs : RowEventArgs
	{
		private bool m_Cancel = false;
		/// <summary>
		/// Row
		/// </summary>
		public bool Cancel
		{
			get{return m_Cancel;}
			set{m_Cancel = value;}
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pRow"></param>
		public RowCancelEventArgs(RowInfo pRow):base(pRow)
		{
		}
	}

	/// <summary>
	/// EventHandler used by the FocusRowLeaving
	/// </summary>
	public delegate void RowCancelEventHandler(object sender, RowCancelEventArgs e);

	/// <summary>
	/// EventArgs used by the FocusColumnEnter
	/// </summary>
	public class ColumnEventArgs : EventArgs
	{
		private ColumnInfo m_Column;
		/// <summary>
		/// Column
		/// </summary>
		public ColumnInfo Column
		{
			get{return m_Column;}
			set{m_Column = value;}
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pColumn"></param>
		public ColumnEventArgs(ColumnInfo pColumn)
		{
			m_Column = pColumn;
		}
	}

	/// <summary>
	/// EventHandled used by the FocusColumnEnter
	/// </summary>
	public delegate void ColumnEventHandler(object sender, ColumnEventArgs e);

	/// <summary>
	/// EventArgs used by the FocusColumnLeaving
	/// </summary>
	public class ColumnCancelEventArgs : ColumnEventArgs
	{
		private bool m_Cancel;
		/// <summary>
		/// Column
		/// </summary>
		public bool Cancel
		{
			get{return m_Cancel;}
			set{m_Cancel = value;}
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pColumn"></param>
		public ColumnCancelEventArgs(ColumnInfo pColumn):base(pColumn)
		{
		}
	}

	/// <summary>
	/// EventHandled used by the FocusColumnLeave and FocusColumnEnter
	/// </summary>
	public delegate void ColumnCancelEventHandler(object sender, ColumnCancelEventArgs e);

	public class ValidatingEventArgs : System.ComponentModel.CancelEventArgs
	{
		private object m_NewValue;
		public ValidatingEventArgs(object p_NewValue):base(false)
		{
			m_NewValue = p_NewValue;
		}
		public object NewValue
		{
			get{return m_NewValue;}
			set{m_NewValue = value;}
		}
	}

	public class ValidatingCellEventArgs : ValidatingEventArgs
	{
		private Cells.ICellVirtual m_Cell;
		public ValidatingCellEventArgs(Cells.ICellVirtual p_Cell, object p_NewValue):base(p_NewValue)
		{
			m_Cell = p_Cell;
		}

		public Cells.ICellVirtual Cell
		{
			get{return m_Cell;}
		}
	}

	public class CellEventArgs : EventArgs
	{
		private Cells.ICellVirtual m_Cell;
		public CellEventArgs(Cells.ICellVirtual p_Cell)
		{
			m_Cell = p_Cell;
		}

		public Cells.ICellVirtual Cell
		{
			get{return m_Cell;}
			set{m_Cell = value;}
		}

	}
	
//	public class CellCancelEventArgs : CellEventArgs
//	{
//		public CellCancelEventArgs(Cell p_Cell):base(p_Cell)
//		{
//		}
//
//		private bool m_bCancel = false;
//		public bool Cancel
//		{
//			get{return m_bCancel;}
//			set{m_bCancel = value;}
//		}
//	}
//
//	public class CellArrayEventArgs : EventArgs
//	{
//		private Cell[] m_Cell;
//		public CellArrayEventArgs(Cell[] p_Cell)
//		{
//			m_Cell = p_Cell;
//		}
//
//		public Cell[] Cells
//		{
//			get{return m_Cell;}
//			set{m_Cell = value;}
//		}
//
//	}

	public class RowInfoEventArgs : EventArgs
	{
		private RowInfo m_RowInfo;
		public RowInfoEventArgs(RowInfo p_RowInfo)
		{
			m_RowInfo = p_RowInfo;
		}

		public RowInfo Row
		{
			get{return m_RowInfo;}
		}
	}

	public delegate void RowInfoEventHandler(object sender, RowInfoEventArgs e);

	public class ColumnInfoEventArgs : EventArgs
	{
		private ColumnInfo m_ColumnInfo;
		public ColumnInfoEventArgs(ColumnInfo p_ColumnInfo)
		{
			m_ColumnInfo = p_ColumnInfo;
		}

		public ColumnInfo Column
		{
			get{return m_ColumnInfo;}
		}
	}

	public delegate void ColumnInfoEventHandler(object sender, ColumnInfoEventArgs e);

	public class IndexRangeEventArgs : EventArgs
	{
		private int m_iStartIndex;
		private int m_iCount;

		public IndexRangeEventArgs(int p_iStartIndex, int p_iCount)
		{
			m_iStartIndex = p_iStartIndex;
			m_iCount = p_iCount;
		}

		public int StartIndex
		{
			get{return m_iStartIndex;}
		}

		public int Count
		{
			get{return m_iCount;}
		}
	}

	public delegate void IndexRangeEventHandler(object sender, IndexRangeEventArgs e);

	public class PositionEventArgs : EventArgs
	{
		private Position m_Position;
		private Cells.ICellVirtual m_Cell;
		public PositionEventArgs(Position p_Position, Cells.ICellVirtual p_Cell)
		{
			m_Position = p_Position;
			m_Cell = p_Cell;
		}

		public GridVirtual Grid
		{
			get{return m_Cell.Grid;}
		}

		public Cells.ICellVirtual Cell
		{
			get{return m_Cell;}
			set{m_Cell = value;} //this set method is used for GettingCellEvent
		}

		public Position Position
		{
			get{return m_Position;}
		}
	}
	public delegate void PositionEventHandler(object sender, PositionEventArgs e);

	public class PositionMouseEventArgs : PositionEventArgs
	{
		private System.Windows.Forms.MouseEventArgs m_MouseArgs;
		public PositionMouseEventArgs(Position p_Position, Cells.ICellVirtual p_Cell, System.Windows.Forms.MouseEventArgs p_MouseArgs):base(p_Position, p_Cell)
		{
			m_MouseArgs = p_MouseArgs;
		}

		public System.Windows.Forms.MouseEventArgs MouseEventArgs
		{
			get{return m_MouseArgs;}
			set{m_MouseArgs = value;}
		}
	}

	public delegate void PositionMouseEventHandler(object sender, PositionMouseEventArgs e);


	public class PositionContextMenuEventArgs : PositionEventArgs
	{
		private MenuCollection m_ContextMenu;
		public PositionContextMenuEventArgs(Position p_Position, Cells.ICellVirtual p_Cell, MenuCollection p_ContextMenu):base(p_Position, p_Cell)
		{
			m_ContextMenu = p_ContextMenu;
		}

		public MenuCollection ContextMenu
		{
			get{return m_ContextMenu;}
			set{m_ContextMenu = value;}
		}
	}

	public delegate void PositionContextMenuEventHandler(object sender, PositionContextMenuEventArgs e);
	

	public class PositionKeyPressEventArgs : PositionEventArgs
	{
		private System.Windows.Forms.KeyPressEventArgs m_KeyPressArgs;
		public PositionKeyPressEventArgs(Position p_Position, Cells.ICellVirtual p_Cell, System.Windows.Forms.KeyPressEventArgs p_KeyPressArge):base(p_Position, p_Cell)
		{
			m_KeyPressArgs = p_KeyPressArge;
		}

		public System.Windows.Forms.KeyPressEventArgs KeyPressEventArgs
		{
			get{return m_KeyPressArgs;}
			set{m_KeyPressArgs = value;}
		}
	}

	public delegate void PositionKeyPressEventHandler(object sender, PositionKeyPressEventArgs e);

	public class PositionKeyEventArgs : PositionEventArgs
	{
		private System.Windows.Forms.KeyEventArgs m_KeyArgs;
		public PositionKeyEventArgs(Position p_Position, Cells.ICellVirtual p_Cell, System.Windows.Forms.KeyEventArgs p_KeyArge):base(p_Position, p_Cell)
		{
			m_KeyArgs = p_KeyArge;
		}

		public System.Windows.Forms.KeyEventArgs KeyEventArgs
		{
			get{return m_KeyArgs;}
			set{m_KeyArgs = value;}
		}
	}

	public delegate void PositionKeyEventHandler(object sender, PositionKeyEventArgs e);

	/// <summary>
	/// EventArgs that represents a cell position and a cancel parameters. Extends PositionEventArgs
	/// </summary>
	public class PositionCancelEventArgs : PositionEventArgs
	{
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Position"></param>
		/// <param name="p_Cell"></param>
		public PositionCancelEventArgs(Position p_Position, Cells.ICellVirtual p_Cell):base(p_Position, p_Cell)
		{
		}

		private bool m_bCancel = false;
		/// <summary>
		/// Cancel parameter: default is false
		/// </summary>
		public bool Cancel
		{
			get{return m_bCancel;}
			set{m_bCancel = value;}
		}
	}

	/// <summary>
	/// PositionCancel Event Handler.
	/// </summary>
	public delegate void PositionCancelEventHandler(object sender, PositionCancelEventArgs e);

	/// <summary>
	/// Cell Lost Focus event arguments with the old position and the new position. Extends PositionCancelEventArgs.
	/// </summary>
	public class CellLostFocusEventArgs : PositionCancelEventArgs
	{
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pPositionLostFocus"></param>
		/// <param name="pCellLostFocus"></param>
		/// <param name="pNewFocusPosition">If Empty there isn't a cell that will receive the focus.</param>
		public CellLostFocusEventArgs(Position pPositionLostFocus, 
									Cells.ICellVirtual pCellLostFocus,
									Position pNewFocusPosition):
													base(pPositionLostFocus, pCellLostFocus)
		{
			m_NewFocusPosition = pNewFocusPosition;
		}

		private Position m_NewFocusPosition;
		/// <summary>
		/// Position that will receive the focus. If Empty there isn't a cell that will receive the focus.
		/// </summary>
		public Position NewFocusPosition
		{
			get{return m_NewFocusPosition;}
		}
	}

	/// <summary>
	/// Cell Lost Focus event handler
	/// </summary>
	public delegate void CellLostFocusEventHandler(object sender, CellLostFocusEventArgs e);


	/// <summary>
	/// Cell Got Focus event arguments with the old position and the new position. Extends PositionCancelEventArgs.
	/// </summary>
	public class CellGotFocusEventArgs : PositionCancelEventArgs
	{
		public CellGotFocusEventArgs(Position pPositionGotFocus, 
			Cells.ICellVirtual pCellGotFocus,
			Position pOldFocusPosition):
			base(pPositionGotFocus, pCellGotFocus)
		{
			m_OldFocusPosition = pOldFocusPosition;
		}

		private Position m_OldFocusPosition;
		/// <summary>
		/// Position that previous has the focus. If Empty there is no cell with the focus..
		/// </summary>
		public Position OldFocusPosition
		{
			get{return m_OldFocusPosition;}
		}
	}

	/// <summary>
	/// Cell Lost Focus event handler
	/// </summary>
	public delegate void CellGotFocusEventHandler(object sender, CellGotFocusEventArgs e);


	public class RangeEventArgs : EventArgs
	{
		private Range m_GridRange;
		public RangeEventArgs(Range p_GridRange)
		{
			m_GridRange = p_GridRange;
		}

		public Range Range
		{
			get{return m_GridRange;}
		}
	}

	public delegate void RangeEventHandler(object sender, RangeEventArgs e);

	public class SelectionChangeEventArgs : EventArgs
	{
		public SelectionChangeEventArgs(SelectionChangeEventType p_Type, Range p_Range)
		{
			m_Type = p_Type;
			m_Range = p_Range;
		}

		private Range m_Range;
		private SelectionChangeEventType m_Type;

		public Range Range
		{
			get{return m_Range;}
		}

		public SelectionChangeEventType EventType
		{
			get{return m_Type;}
		}
	}

	public delegate void SelectionChangeEventHandler(object sender, SelectionChangeEventArgs e);

    public class FinishOccurredEventArgs : EventArgs
    {
        private GridRangeCollection m_RangeList;

        public FinishOccurredEventArgs(GridRangeCollection RangeList)
        {
            m_RangeList = RangeList;
        }

        public GridRangeCollection RangeList
        {
            get{return m_RangeList;}
        }
    }
    public delegate void FinishOccurredEventHandler(object sender,
        FinishOccurredEventArgs e);

	public class EditExceptionEventArgs : EventArgs
	{
		public EditExceptionEventArgs(Exception p_Exception)
		{
			m_Exception = p_Exception;
		}

		private Exception m_Exception;

		public Exception Exception
		{
			get{return m_Exception;}
		}
	}

	public delegate void EditExceptionEventHandler(object sender, EditExceptionEventArgs e);

	public class SortRangeRowsEventArgs : EventArgs
	{
		private Range m_Range;
		private int m_AbsoluteColKeys;
		private bool m_bAscending;
		private IComparer m_CellComparer;

		public SortRangeRowsEventArgs(Range p_Range,
			int p_AbsoluteColKeys, 
			bool p_bAscending,
			IComparer p_CellComparer)
		{
			m_Range = p_Range;
			m_AbsoluteColKeys = p_AbsoluteColKeys;
			m_bAscending = p_bAscending;
			m_CellComparer = p_CellComparer;
		}

		public Range Range
		{
			get{return m_Range;}
		}
		public int AbsoluteColKeys
		{
			get{return m_AbsoluteColKeys;}
		}
		public bool Ascending
		{
			get{return m_bAscending;}
		}
		public IComparer CellComparer
		{
			get{return m_CellComparer;}
		}
	}

	public delegate void SortRangeRowsEventHandler(object sender, SortRangeRowsEventArgs e);
}
