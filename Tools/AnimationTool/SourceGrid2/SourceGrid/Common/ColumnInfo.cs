using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.ComponentModel;

namespace SourceGrid2
{
	/// <summary>
	/// Column Information
	/// </summary>
	public class ColumnInfo
	{
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Grid"></param>
		private ColumnInfo(GridVirtual p_Grid)
		{
			m_Grid = p_Grid;
		}

		private int m_Width = Utility.DefaultCellWidth;
		/// <summary>
		/// Width of the current Column
		/// </summary>
		public int Width
		{
			get{return m_Width;}
			set
			{
				if (value < 0)
					value=0;

				if (m_Width != value)
				{
					m_Width = value;
					if (m_Grid!=null)
						m_Grid.Columns.InvokeColumnWidthChanged(new ColumnInfoEventArgs(this));
				}
			}
		}

		private int m_Left;
		/// <summary>
		/// Left absolute position of the current Column
		/// </summary>
		[Browsable(false)]
		public int Left
		{
			get{return m_Left;}
		}

		/// <summary>
		/// Right of the column (Left+Width)
		/// </summary>
		[Browsable(false)]
		public int Right
		{
			get{return Left+Width;}
		}
		//private int m_Index;
		/// <summary>
		/// Index of the current Column
		/// </summary>
		public int Index
		{
			get{return m_Grid.Columns.IndexOf(this);}
		}

		private GridVirtual m_Grid;
		/// <summary>
		/// Attached Grid
		/// </summary>
		[Browsable(false)]
		public GridVirtual Grid
		{
			get{return m_Grid;}
		}


		/// <summary>
		/// Gets or sets the cells at the specified column
		/// </summary>
		[Browsable(false)]
		public Cells.ICellVirtual[] Cells
		{
			get
			{
				if (m_Grid == null)
					throw new SourceGridException("Invalid Grid object");
			
				return m_Grid.GetCellsAtColumn(Index);
			}
			set
			{
				if (m_Grid == null)
					throw new SourceGridException("Invalid Grid object");

				m_Grid.SetCellsAtColumn(Index, value);
			}
		}


		/// <summary>
		/// Move the Focus to the first cell that can receive the focus of the current column otherwise put the focus to null.
		/// </summary>
		/// <returns></returns>
		public bool Focus()
		{
			if (Grid.ColumnsCount > Index)
			{
				for (int r = 0; r < Grid.RowsCount; r++)
				{
					Position l_NewFocus = new Position(r, Index);
					Cells.ICellVirtual l_CellVirtual = Grid.GetCell(l_NewFocus);
					if (l_CellVirtual != null && l_CellVirtual.CanReceiveFocus)
						return Grid.Selection.Focus(l_NewFocus);
				}

				return Grid.Selection.Focus(Position.Empty);
			}
			else
				return Grid.Selection.Focus(Position.Empty);;
		}

		/// <summary>
		/// Gets or sets if the current row is selected. If only a column of the row is selected this property returns true.
		/// </summary>
		[Browsable(false)]
		public bool Select
		{
			get{return Grid.Selection.ContainsColumn(Index);}
			set
			{
				if (Grid.ColumnsCount > Index && Grid.RowsCount > 0)
				{
					if (value)
						Grid.Selection.AddRange(new Range(0, Index, Grid.RowsCount-1, Index));
					else
						Grid.Selection.RemoveRange(new Range(0, Index, Grid.RowsCount-1, Index));
				}
			}
		}


		private object m_Tag;
		/// <summary>
		/// A property that the user can use to insert custom informations associated to a specific column
		/// </summary>
		[Browsable(false)]
		public object Tag
		{
			get{return m_Tag;}
			set{m_Tag = value;}
		}

		private AutoSizeMode m_AutoSizeMode = AutoSizeMode.Default;
		/// <summary>
		/// Flags for autosize and stretch
		/// </summary>
		public AutoSizeMode AutoSizeMode
		{
			get{return m_AutoSizeMode;}
			set{m_AutoSizeMode = value;}
		}

		/// <summary>
		/// Auto size the current column with the max required width of all cells.
		/// </summary>
		/// <param name="useRowHeight">True to fix the row height when measure the column width.</param>
		/// <param name="StartRow">Start row to measure</param>
		/// <param name="EndRow">End row to measure</param>
		public void AutoSize(bool useRowHeight, int StartRow, int EndRow)
		{
			if ( (AutoSizeMode & AutoSizeMode.EnableAutoSize) == AutoSizeMode.EnableAutoSize)
			{
				Width = Measure(useRowHeight, StartRow, EndRow);
			}
		}
		/// <summary>
		/// Auto size the current column with the max required width of all cells.
		/// </summary>
		/// <param name="useRowHeight">True to fix the row height when measure the column width.</param>
		public void AutoSize(bool useRowHeight)
		{
			if ( (AutoSizeMode & AutoSizeMode.EnableAutoSize) == AutoSizeMode.EnableAutoSize)
			{
				Width = Measure(useRowHeight);
			}
		}

		/// <summary>
		/// Measures the current column when drawn with the specified cells.
		/// </summary>
		/// <param name="useRowHeight">True to fix the row height when measure the column width.</param>
		/// <returns>Returns the minimum width</returns>
		public int Measure(bool useRowHeight)
		{
			return Measure(useRowHeight,0, Grid.RowsCount-1);
		}

		/// <summary>
		/// Measures the current column when drawn with the specified cells.
		/// </summary>
		/// <param name="useRowHeight">True to fix the row height when measure the column width.</param>
		/// <param name="StartRow">Start row to measure</param>
		/// <param name="EndRow">End row to measure</param>
		/// <returns>Returns the minimum width</returns>
		public int Measure(bool useRowHeight, int StartRow, int EndRow)
		{
			int l_minWidth = Grid.AutoSizeMinWidth;

			if ( (AutoSizeMode & AutoSizeMode.MinimumSize) == AutoSizeMode.MinimumSize)
				return l_minWidth;

#if !MINI
			using (Graphics l_graphics = Grid.CreateGraphics())
#else
			//for a bug in the Compact Framework I should call the parent CreateGraphics because the CreateGraphics of the current control in the Load event fail
			using (Graphics l_graphics = Grid.Parent.CreateGraphics())
#endif
			{
				for (int r = StartRow; r <= EndRow; r++)
				{
					SourceGrid2.Cells.ICellVirtual cell = Grid.GetCell(r, Index);
					if (cell != null)
					{
						SizeF maxLayout = SizeF.Empty;
						if (useRowHeight)
							maxLayout.Height = Grid.Rows[r].Height;

						Size l_size = cell.Measure(new Position(r, Index), l_graphics, maxLayout);
						if (l_size.Width > l_minWidth)
							l_minWidth = l_size.Width;
					}
				}
			}
			return l_minWidth;
		}

		#region ColumnInfoCollection
		/// <summary>
		/// Collection of ColumnInfo
		/// </summary>
		public class ColumnInfoCollection : ICollection
		{
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="p_grid"></param>
			public ColumnInfoCollection(GridVirtual p_grid)
			{
				m_Grid = p_grid;
			}

			private GridVirtual m_Grid;
			/// <summary>
			/// Attached Grid
			/// </summary>
			public GridVirtual Grid
			{
				get{return m_Grid;}
			}

			private ArrayList m_List = new ArrayList();
			#region Comparer
			private ColumnInfoLeftComparer m_Comparer = new ColumnInfoLeftComparer();
			public class ColumnInfoLeftComparer : IComparer
			{
				public System.Int32 Compare ( System.Object x , System.Object y )
				{
					return ((ColumnInfo)x).Left.CompareTo( ((ColumnInfo)y).Left);
				}
			}
			#endregion

			/// <summary>
			/// Calculate the Column that have the Left value smaller or equal than the point p_X, or -1 if not found found. ExactMatch = false
			/// </summary>
			/// <param name="p_X">Absolute point to search</param>
			/// <returns></returns>
			public int ColumnAtPoint(int p_X)
			{
				return ColumnAtPoint(p_X, false);
			}
			/// <summary>
			/// Calculate the Column that have the Left value smaller or equal than the point p_X, or -1 if not found found.
			/// </summary>
			/// <param name="p_X">X Coordinate to search for a column</param>
			/// <param name="p_ExactMatch">True to returns only exact position. For example if you use a point outside the range and this value is true no column is returned otherwise the nearest column is returned.</param>
			/// <returns></returns>
			public int ColumnAtPoint(int p_X,bool p_ExactMatch)
			{
				//Restituisce la righa con il Left uguale a quello passato o la righa con il Left minore a quallo passato.
				// o -1 se tutte le righe hanno il Left maggiore
				int l_IndexFound;

				ColumnInfo l_Find = new ColumnInfo(null);
				l_Find.m_Left = p_X;
				int l_ObjFound = m_List.BinarySearch(0, m_List.Count, l_Find, m_Comparer);
				if (l_ObjFound>=0) //trovato il valore uguale
					l_IndexFound = l_ObjFound;
				else
				{
					l_ObjFound = ~l_ObjFound; //bitwise operator to return the nearest index
					if (l_ObjFound<=0)
						l_IndexFound = -1; //nessuna righa compatibile
					else if (l_ObjFound <= m_List.Count)
						l_IndexFound = l_ObjFound-1; //trovata una righa compatibile
					else
						l_IndexFound = -1; //non dovrebbe mai capitare
				}

				//se è stato richiesto un exactMatch verifico che il punto sia compreso tra il minimo e il massimo
				if (p_ExactMatch && l_IndexFound>=0)
				{
					if (p_X > Right ||
						p_X < Left)
						l_IndexFound = -1;
				}

				return l_IndexFound;
			}


			/// <summary>
			/// Returns true if the range passed is valid
			/// </summary>
			/// <param name="p_StartIndex"></param>
			/// <param name="p_Count"></param>
			/// <returns></returns>
			public bool IsValidRange(int p_StartIndex, int p_Count)
			{
				if (p_StartIndex < Count && p_StartIndex >= 0 &&
					p_Count > 0 && (p_StartIndex+p_Count) <= Count)
					return true;
				else
					return false;
			}

			/// <summary>
			/// Returns true if the range passed is valid for insert method
			/// </summary>
			/// <param name="p_StartIndex"></param>
			/// <param name="p_Count"></param>
			/// <returns></returns>
			public bool IsValidRangeForInsert(int p_StartIndex, int p_Count)
			{
				if (p_StartIndex <= Count && p_StartIndex >= 0 &&
					p_Count > 0)
					return true;
				else
					return false;
			}

			#region Insert/Remove Methods

			/// <summary>
			/// Insert a column at the specified position using the specified cells
			/// </summary>
			/// <param name="p_Index"></param>
			/// <param name="p_Cells">The new column values</param>
			public void Insert(int p_Index, params Cells.ICellVirtual[] p_Cells)
			{
				Insert(p_Index);

				this[p_Index].Cells = p_Cells;
			}

			/// <summary>
			/// Insert a column at the specified position
			/// </summary>
			/// <param name="p_Index"></param>
			public void Insert(int p_Index)
			{
				InsertRange(p_Index, 1);
			}

			/// <summary>
			/// Remove a column at the speicifed position
			/// </summary>
			/// <param name="p_Index"></param>
			public void Remove(int p_Index)
			{
				RemoveRange(p_Index, 1);
			}

			/// <summary>
			/// Insert the specified number of Columns at the specified position
			/// </summary>
			/// <param name="p_StartIndex"></param>
			/// <param name="p_Count"></param>
			public void InsertRange(int p_StartIndex, int p_Count)
			{
				if (IsValidRangeForInsert(p_StartIndex, p_Count)==false)
					throw new SourceGridException("Invalid index");

				//TODO si potrebbe ottimizzare aumentando la capacity
				for (int c = 0; c < p_Count; c++)
				{
					m_List.Insert(p_StartIndex+c,new ColumnInfo(m_Grid));
				}

				if (AutoCalculateLeft)
					CalculateLeft(p_StartIndex);

				OnColumnsAdded(new IndexRangeEventArgs(p_StartIndex, p_Count));
			}

			/// <summary>
			/// Remove the ColumnInfo at the specified positions
			/// </summary>
			/// <param name="p_StartIndex"></param>
			/// <param name="p_Count"></param>
			public void RemoveRange(int p_StartIndex, int p_Count)
			{
				if (IsValidRange(p_StartIndex, p_Count)==false)
					throw new SourceGridException("Invalid index");

				IndexRangeEventArgs eventArgs = new IndexRangeEventArgs(p_StartIndex, p_Count);
				OnColumnsRemoving(eventArgs);

				//azzero le informazioni legate alla griglia
				for (int c = p_StartIndex; c < p_StartIndex+p_Count; c++)
				{
					this[c].m_Grid = null;
					//this[c].m_Index = -1;
				}

				m_List.RemoveRange(p_StartIndex, p_Count);

				if (AutoCalculateLeft)
					CalculateLeft(p_StartIndex);

				OnColumnsRemoved(eventArgs);
			}


			#endregion

			/// <summary>
			/// Move a column from one position to another position
			/// </summary>
			/// <param name="p_CurrentColumnPosition"></param>
			/// <param name="p_NewColumnPosition"></param>
			public void Move(int p_CurrentColumnPosition, int p_NewColumnPosition)
			{
				if (p_CurrentColumnPosition == p_NewColumnPosition)
					return;

				if (p_CurrentColumnPosition < p_NewColumnPosition)
				{
					for (int r = p_CurrentColumnPosition; r < p_NewColumnPosition; r++)
					{
						Swap(r, r + 1);
					}				
				}
				else
				{
					for (int r = p_CurrentColumnPosition; r > p_NewColumnPosition; r--)
					{
						Swap(r, r - 1);
					}				
				}
			}

			/// <summary>
			/// Change the position of column 1 with column 2.
			/// </summary>
			/// <param name="p_ColumnIndex1"></param>
			/// <param name="p_ColumnIndex2"></param>
			public void Swap(int p_ColumnIndex1, int p_ColumnIndex2)
			{
				if (p_ColumnIndex1 == p_ColumnIndex2)
					return;

				ColumnInfo l_Column1 = this[p_ColumnIndex1];
				Cells.ICellVirtual[] l_Cells1 = l_Column1.Cells;
				ColumnInfo l_Column2 = this[p_ColumnIndex2];
				Cells.ICellVirtual[] l_Cells2 = l_Column2.Cells;

				m_List[p_ColumnIndex1] = l_Column2;
				m_List[p_ColumnIndex2] = l_Column1;

				l_Column1.Cells = new Cells.ICellVirtual[l_Cells1.Length];
				l_Column2.Cells = new Cells.ICellVirtual[l_Cells1.Length];
				l_Column1.Cells = l_Cells1;
				l_Column2.Cells = l_Cells2;

				if (AutoCalculateLeft)
					CalculateLeft(0);
			}

			/// <summary>
			/// Fired when the number of columns change
			/// </summary>
			public event IndexRangeEventHandler ColumnsAdded;

			/// <summary>
			/// Fired when the number of columns change
			/// </summary>
			/// <param name="e"></param>
			protected virtual void OnColumnsAdded(IndexRangeEventArgs e)
			{
				if (ColumnsAdded!=null)
					ColumnsAdded(this, e);
			}

			/// <summary>
			/// Fired when some columns are removed
			/// </summary>
			public event IndexRangeEventHandler ColumnsRemoved;

			/// <summary>
			/// Fired when some columns are removed
			/// </summary>
			/// <param name="e"></param>
			protected virtual void OnColumnsRemoved(IndexRangeEventArgs e)
			{
				if (ColumnsRemoved!=null)
					ColumnsRemoved(this, e);
			}

			/// <summary>
			/// Fired before some columns are removed
			/// </summary>
			public event IndexRangeEventHandler ColumnsRemoving;

			/// <summary>
			/// Fired before some columns are removed
			/// </summary>
			/// <param name="e"></param>
			protected virtual void OnColumnsRemoving(IndexRangeEventArgs e)
			{
				if (ColumnsRemoving!=null)
					ColumnsRemoving(this, e);
			}

			/// <summary>
			/// Indexer. Returns a ColumnInfo at the specified position
			/// </summary>
			public ColumnInfo this[int p]
			{
				get{return (ColumnInfo)m_List[p];}
			}

			/// <summary>
			/// Recalculate all the Left positions from the specified index
			/// </summary>
			/// <param name="p_StartIndex"></param>
			public void CalculateLeft(int p_StartIndex)
			{
				if (Count > 0)
				{
					int l_CurrentLeft = 0;
					if (p_StartIndex != 0)
						l_CurrentLeft = this[p_StartIndex-1].Left+this[p_StartIndex-1].Width;

					for (int c = p_StartIndex; c < Count; c++)
					{
						this[c].m_Left = l_CurrentLeft;
						l_CurrentLeft += this[c].m_Width;
					}
				}
			}

			/// <summary>
			/// Returns the maximum right value of the columns. Calculated with Columns[lastCol].Right or 0 if no columns are presents.
			/// </summary>
			public int Right
			{
				get
				{
					if (Count <= 0)
						return 0;
					else
						return this[Count-1].Right;
				}
			}
			/// <summary>
			/// Returns the minimum left value of the columns. Calculated with Columns[0].Left or 0 if no columns are presents.
			/// </summary>
			public int Left
			{
				get
				{
					if (Count <= 0)
						return 0;
					else
						return this[0].Left;
				}
			}
			/// <summary>
			/// Fired when the user change the Width property of one of the Column
			/// </summary>
			public event ColumnInfoEventHandler ColumnWidthChanged;

			/// <summary>
			/// Execute the RowHeightChanged event
			/// </summary>
			/// <param name="e"></param>
			public void InvokeColumnWidthChanged(ColumnInfoEventArgs e)
			{
				if (AutoCalculateLeft)
					CalculateLeft(e.Column.Index);

				if (ColumnWidthChanged!=null)
					ColumnWidthChanged(this, e);
			}

			private bool m_bAutoCalculateLeft = true;
			/// <summary>
			/// Indicates if auto recalculate left position when width value change. Default = true. Can be used when you need to change many Width value for example for an AutoSize operation to increase performance.
			/// </summary>
			public bool AutoCalculateLeft
			{
				get{return m_bAutoCalculateLeft;}
				set
				{
					m_bAutoCalculateLeft = value;
					if (m_bAutoCalculateLeft)
						CalculateLeft(0);
				}
			}

			public int IndexOf(ColumnInfo p_Info)
			{
				return m_List.IndexOf(p_Info);
			}

			/// <summary>
			/// Auto size all the columns with the max required width of all cells.
			/// </summary>
			/// <param name="useRowHeight">True to fix the row height when measure the column width.</param>
			/// <param name="StartRow">Start row to measure</param>
			/// <param name="EndRow">End row to measure</param>
			public void AutoSize(bool useRowHeight, int StartRow, int EndRow)
			{
				for (int i = 0; i < Count; i++)
				{
					ColumnInfo item = this[i];
					item.AutoSize(useRowHeight, StartRow, EndRow);
				}
			}
			/// <summary>
			/// Auto size all the columns with the max required width of all cells.
			/// </summary>
			/// <param name="useRowHeight">True to fix the row height when measure the column width.</param>
			public void AutoSize(bool useRowHeight)
			{
				for (int i = 0; i < Count; i++)
				{
					ColumnInfo item = this[i];
					item.AutoSize(useRowHeight);
				}
			}

			#region ICollection
			public virtual void CopyTo ( System.Array array , System.Int32 index )
			{
				m_List.CopyTo(array,index);
			}
			public int Count
			{
				get{return m_List.Count;}
			}
			public bool IsSynchronized
			{
				get{return m_List.IsSynchronized;}
			}
			public object SyncRoot
			{
				get{return m_List.SyncRoot;}
			}
			public virtual System.Collections.IEnumerator GetEnumerator (  )
			{
				return m_List.GetEnumerator();
			}
			#endregion
		}

		#endregion
	}
}
