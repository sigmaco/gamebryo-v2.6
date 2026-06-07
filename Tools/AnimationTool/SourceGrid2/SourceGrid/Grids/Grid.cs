using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using SourceGrid2.Cells;

namespace SourceGrid2
{
	/// <summary>
	/// The mai grid control with static data.
	/// </summary>
#if !MINI
	[System.ComponentModel.ToolboxItem(true)]
#endif
	public class Grid : GridVirtual
	{
		#region Constructor
		/// <summary>
		/// Constructor
		/// </summary>
		public Grid()
		{
#if !MINI
			this.Name = "Grid";
#endif
			this.Size = new Size(100,100);

			Rows.RowsAdded += new IndexRangeEventHandler(m_Rows_RowsAdded);
			Rows.RowsRemoved += new IndexRangeEventHandler(m_Rows_RowsRemoved);

			Columns.ColumnsAdded += new IndexRangeEventHandler(m_Columns_ColumnsAdded);
			Columns.ColumnsRemoved += new IndexRangeEventHandler(m_Columns_ColumnsRemoved);

			Selection.SelectionChange += new SelectionChangeEventHandler(Selection_SelectionChange);
		}

		#endregion

		#region Cells
		/// <summary>
		/// Return the Cell at the specified Row and Col position.
		/// </summary>
		/// <param name="p_iRow"></param>
		/// <param name="p_iCol"></param>
		/// <returns></returns>
		public override Cells.ICellVirtual GetCell(int p_iRow, int p_iCol)
		{
			return this[p_iRow, p_iCol];
		}

		/// <summary>
		/// Set the specified cell int he specified position. Abstract method of the GridVirtual control
		/// </summary>
		/// <param name="p_iRow"></param>
		/// <param name="p_iCol"></param>
		/// <param name="p_Cell"></param>
		public override void SetCell(int p_iRow, int p_iCol, Cells.ICellVirtual p_Cell)
		{
			if (p_Cell is ICell)
				InsertCell(p_iRow, p_iCol, (ICell)p_Cell);
			else if (p_Cell == null)
				InsertCell(p_iRow, p_iCol, null);
			else
				throw new SourceGridException("Expected ICell class");
		}

		/// <summary>
		/// Array of cells
		/// </summary>
		private ICell[,] m_Cells = null;
		private int CellsRows
		{
			get
			{
				if (m_Cells==null)
					return 0;
				else
					return m_Cells.GetLength(0);
			}
		}
		private int CellsCols
		{
			get
			{
				if (m_Cells==null)
					return 0;
				else
					return m_Cells.GetLength(1);
			}
		}

		/// <summary>
		/// Returns or set a cell at the specified row and col. If you get a ICell position occupied by a row/col span cell, and EnableRowColSpan is true, this method returns the cell with Row/Col span.
		/// </summary>
		public ICell this[int row, int col]
		{
			get
			{
				if (EnableRowColSpan==false)
					return m_Cells[row,col];
				else //enable Row Col Span search
				{
					#region Search Row Col Span

					if (m_Cells[row,col] != null)
						return m_Cells[row,col];

					//this alghorithm search the spanned cell with a sqare search:
					//	4	4	4	4	4
					//	4	3	3	3	3
					//	4	3	2	2	2
					//	4	3	2	1	1
					//	4	3	2	1	X
					// the X represents the requestPos, the number represents the searchIteration loop.
					// search first on the row and then on the columns

					Position requestPos = new Position(row, col);
					int startPosRow = requestPos.Row;
					int startPosCol = requestPos.Column;
					bool endRow = false;
					bool endCol = false;
					ICell testCell;
					for (int searchIteration = 0; searchIteration < (m_MaxSpanSearch+1); searchIteration++)
					{
						if (endCol == false)
						{
							for (int r = startPosRow; r <= requestPos.Row; r++)
							{
								//if the cell contains the requested cell
								testCell = m_Cells[r,startPosCol];
								if (testCell != null && testCell.ContainsPosition(requestPos))
									return testCell;
							}
						}

						if (endRow == false)
						{
							for (int c = startPosCol; c <= requestPos.Column; c++)
							{
								//if the cell contains the requested cell
								testCell = m_Cells[startPosRow,c];
								if (testCell != null && testCell.ContainsPosition(requestPos))
									return testCell;
							}
						}

						if (startPosRow == 0 && startPosCol == 0)
							return null; //not found
						
						if (startPosRow == 0)
							endRow = true;
						else
							startPosRow--;

						if (startPosCol == 0)
							endCol = true;
						else
							startPosCol--;
					}

					return null;
					#endregion
				}
			}
			set
			{					
				InsertCell(row,col,value);
			}
		}

		/// <summary>
		/// Remove the specified cell
		/// </summary>
		/// <param name="row"></param>
		/// <param name="col"></param>
		public virtual void RemoveCell(int row, int col)
		{
			ICell tmp = m_Cells[row,col];

			if (tmp!= null)
			{
#if !MINI
				//se per caso la cella era quella correntemente con il mouse metta quest'ultima a null
				if (tmp == MouseCell)
					ChangeMouseCell(Position.Empty);
#endif

				tmp.Select = false; //deseleziono la cella (per evitare che venga rimossa senza essere stata aggiunta alla lista di selection
				tmp.LeaveFocus(); //tolgo l'eventuale focus dalla cella

				tmp.UnBindToGrid();

				m_Cells[row,col] = null;
			}
		}

		/// <summary>
		/// Insert the specified cell (for best performance set Redraw property to false)
		/// </summary>
		/// <param name="row"></param>
		/// <param name="col"></param>
		/// <param name="p_cell"></param>
		public virtual void InsertCell(int row, int col, ICell p_cell)
		{
			RemoveCell(row,col);
			m_Cells[row,col] = p_cell;

			if (p_cell!=null)
			{
				if (p_cell.Grid!=null)
					throw new ArgumentException("This cell already have a linked grid","p_cell");

				p_cell.BindToGrid(this,new Position(row, col));

				if (Redraw)
					p_cell.Invalidate();
			}
		}

		/// <summary>
		/// Returns all the cells at specified column position
		/// </summary>
		/// <param name="p_ColumnIndex"></param>
		/// <returns></returns>
		public override ICellVirtual[] GetCellsAtColumn(int p_ColumnIndex)
		{
			Cells.ICellVirtual[] l_Cells = new Cells.ICellVirtual[Rows.Count];

			for (int r = 0; r < Rows.Count;)
			{
				ICell l_Cell = this[r, p_ColumnIndex];
				if (l_Cell != null &&
					l_Cell.Range.Start == new Position(r, p_ColumnIndex))
				{
					l_Cells[r] = l_Cell;
					r += l_Cell.RowSpan;
				}
				else
				{
					l_Cells[r] = null;
					r++;
				}
			}

			return l_Cells;
		}

		/// <summary>
		/// Returns all the cells at specified row position
		/// </summary>
		/// <param name="p_RowIndex"></param>
		/// <returns></returns>
		public override ICellVirtual[] GetCellsAtRow(int p_RowIndex)
		{
			Cells.ICellVirtual[] l_Cells = new Cells.ICellVirtual[Columns.Count];

			for (int c = 0; c < Columns.Count;)
			{
				ICell l_Cell = this[p_RowIndex, c];
				if (l_Cell != null &&
					l_Cell.Range.Start == new Position(p_RowIndex, c))
				{
					l_Cells[c] = l_Cell;
					c += l_Cell.ColumnSpan;
				}
				else
				{
					l_Cells[c] = null;
					c++;
				}
			}

			return l_Cells;
		}


		#endregion

		#region AddRow/Col, RemoveRow/Col

		private void m_Rows_RowsAdded(object sender, IndexRangeEventArgs e)
		{
			//N.B. Uso m_Cells.GetLength(0) anziche' RowsCount e
			// m_Cells.GetLength(1) anziche' ColumnsCount per essere sicuro di lavorare sulle righe effetivamente allocate

			RedimCellsMatrix(CellsRows + e.Count, CellsCols);

			//dopo aver ridimensionato la matrice sposto le celle in modo da fare spazio alla nuove righe
			for (int r = CellsRows-1; r > (e.StartIndex + e.Count-1); r--)
			{
				for (int c = 0; c < CellsCols; c++)
				{
					ICell tmp = m_Cells[r - e.Count, c];
					RemoveCell(r - e.Count, c);
					InsertCell(r,c,tmp);
				}
			}

			if (Redraw)
				RefreshGridLayout();
		}

		private void m_Rows_RowsRemoved(object sender, IndexRangeEventArgs e)
		{
			//N.B. Uso m_Cells.GetLength(0) anziche' RowsCount e
			// m_Cells.GetLength(1) anziche' ColumnsCount per essere sicuro di lavorare sulle righe effetivamente allocate

			for (int r = (e.StartIndex + e.Count); r < CellsRows; r++)
			{
				for (int c = 0; c < CellsCols; c++)
				{
					ICell tmp = m_Cells[r,c];
					RemoveCell(r,c);
					InsertCell(r - e.Count, c, tmp);
				}
			}

			RedimCellsMatrix(CellsRows-e.Count, CellsCols);

			if (Redraw)
				RefreshGridLayout();
		}

		private void m_Columns_ColumnsAdded(object sender, IndexRangeEventArgs e)
		{
			//N.B. Uso m_Cells.GetLength(0) anziche' RowsCount e
			// m_Cells.GetLength(1) anziche' ColumnsCount per essere sicuro di lavorare sulle righe effetivamente allocate

			RedimCellsMatrix(CellsRows, CellsCols+e.Count);

			//dopo aver ridimensionato la matrice sposto le celle in modo da fare spazio alla nuove righe
			for (int c = CellsCols-1; c > (e.StartIndex + e.Count - 1); c--)
			{
				for (int r = 0; r < CellsRows; r++)
				{
					ICell tmp = m_Cells[r, c - e.Count];
					RemoveCell(r, c - e.Count);
					InsertCell(r,c,tmp);
				}
			}

			if (Redraw)
				RefreshGridLayout();
		}

		private void m_Columns_ColumnsRemoved(object sender, IndexRangeEventArgs e)
		{
			//N.B. Uso m_Cells.GetLength(0) anziche' RowsCount e
			// m_Cells.GetLength(1) anziche' ColumnsCount per essere sicuro di lavorare sulle righe effetivamente allocate

			for (int c = (e.StartIndex + e.Count); c < CellsCols; c++)
			{
				for (int r = 0; r < CellsRows; r++)
				{
					ICell tmp = m_Cells[r,c];
					RemoveCell(r,c);
					InsertCell(r, c - e.Count, tmp);
				}
			}

			RedimCellsMatrix(CellsRows, CellsCols-e.Count);

			if (Redraw)
				RefreshGridLayout();
		}

	
		/// <summary>
		/// Ridimensiona la matrice di celle e copia le eventuali vecchie celle presenti nella nuova matrice
		/// </summary>
		/// <param name="rows"></param>
		/// <param name="cols"></param>
		private void RedimCellsMatrix(int rows, int cols)
		{
			if (m_Cells == null)
			{
				m_Cells = new ICell[rows,cols];
			}
			else
			{
				if (rows != m_Cells.GetLength(0) || cols != m_Cells.GetLength(1))
				{
					ICell[,] l_tmp = m_Cells;
					int l_minRows = Math.Min(l_tmp.GetLength(0),rows);
					int l_minCols = Math.Min(l_tmp.GetLength(1),cols);

					//cancello le celle non più utilizzate
					for (int i = l_minRows; i <l_tmp.GetLength(0); i++)
						for (int j = 0; j < l_tmp.GetLength(1); j++)
							RemoveCell(i,j);
					for (int i = 0; i <l_minRows; i++)
						for (int j = l_minCols; j < l_tmp.GetLength(1); j++)
							RemoveCell(i,j);

					m_Cells = new ICell[rows,cols];

					//copio le vecchie celle
					for (int i = 0; i <l_minRows; i++)
						for (int j = 0; j < l_minCols; j++)
							m_Cells[i,j] = l_tmp[i,j];
				}
			}

			//			m_iRows = m_Cells.GetLength(0);
			//			m_iCols = m_Cells.GetLength(1);
		}

		#endregion

		#region Row/Col Span

		/// <summary>
		/// Get if Row/Col Span is enabled. This value is automatically calculated based on the current cells.
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public bool EnableRowColSpan
		{
			get{return (m_MaxSpanSearch > 0);}
		}

		private int m_MaxSpanSearch = 0;
		/// <summary>
		/// Gets the maximum rows or columns number to search when using Row/Col Span. This value is automatically calculated based on the current cells. Do not change this value manually.
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public int MaxSpanSearch
		{
			get{return m_MaxSpanSearch;}
		}

		/// <summary>
		/// Loads the MaxSpanSearch property.
		/// </summary>
		/// <param name="p_MaxSpanSearch"></param>
		/// <param name="p_Reset"></param>
		public void SetMaxSpanSearch(int p_MaxSpanSearch, bool p_Reset)
		{
			if (p_MaxSpanSearch > m_MaxSpanSearch || p_Reset)
				m_MaxSpanSearch = p_MaxSpanSearch;
		}

		/// <summary>
		/// This method convert a Position to the real start position of cell. This is usefull when RowSpan or ColumnSPan is grater than 1.
		/// For example suppose to have at grid[0,0] a cell with ColumnSpan equal to 2. If you call this method with the position 0,0 returns 0,0 and if you call this method with 0,1 return again 0,0.
		/// Get the real position for the specified position. For example when p_Position is a merged cell this method returns the starting position of the merged cells.
		/// Usually this method returns the same cell specified as parameter. This method is used for processing arrow keys, to find a valid cell when the focus is in a merged cell.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public override Position PositionToStartPosition(Position p_Position)
		{
			if (p_Position.IsEmpty())
				return p_Position;

			if (EnableRowColSpan == false)
				return p_Position;
			else
			{
				ICell l_Cell = this[p_Position.Row, p_Position.Column];
				if (l_Cell == null)
					return p_Position;
				else
					return l_Cell.Range.Start;
			}
		}

		#endregion

		#region Cell Rectangle
		/// <summary>
		/// Get the Rectangle of the cell respect all the scrollable area. Using the Cell Row/Col Span.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public override Rectangle PositionToAbsoluteRect(Position p_Position)
		{
			ICell l_Cell = this[p_Position.Row, p_Position.Column];
			if (l_Cell!=null)
			{
				return base.RangeToAbsoluteRect( l_Cell.Range );
			}
			else
				return base.PositionToAbsoluteRect(p_Position);
		}

		/// <summary>
		/// Returns the absolute rectangle relative to the total scrollable area of the specified Range. Returns a 0 rectangle if the Range is not valid
		/// </summary>
		/// <param name="p_Range"></param>
		/// <returns></returns>
		public override Rectangle RangeToAbsoluteRect(Range p_Range)
		{
			if (EnableRowColSpan)
			{
				//cerco il range anche tra le celle in rowspan o colspan
				Range l_RealRange = p_Range;
				for (int r = p_Range.Start.Row; r <= p_Range.End.Row; r++)
					for (int c = p_Range.Start.Column; c <= p_Range.End.Column; c++)
					{
						ICell l_Cell = this[r, c];
						if (l_Cell != null && (l_Cell.RowSpan > 1 || l_Cell.ColumnSpan > 1))
							l_RealRange = Range.Union(l_RealRange, l_Cell.Range);
					}

				return base.RangeToAbsoluteRect(l_RealRange);
			}
			else
				return base.RangeToAbsoluteRect(p_Range);
		}
		#endregion

		#region Cell Visible
		/// <summary>
		/// Returns true if the specified cell is visible otherwise false
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <returns></returns>
		public bool IsCellVisible(ICell p_Cell)
		{
			if (p_Cell!=null)
				return base.IsCellVisible(p_Cell.Range.Start);
			else
				return true;
		}
		/// <summary>
		/// Scroll the view to show the specified cell
		/// </summary>
		/// <param name="p_CellToShow"></param>
		/// <returns></returns>
		public bool ShowCell(ICell p_CellToShow)
		{
			if (p_CellToShow!=null)
				return base.ShowCell(p_CellToShow.Range.Start);
			else
				return true;
		}
		
		#endregion

		#region InvalidateCell
		/// <summary>
		/// Force a redraw of the specified cell
		/// </summary>
		/// <param name="p_Cell"></param>
		public virtual void InvalidateCell(ICell p_Cell)
		{
			if (p_Cell!=null)
				base.InvalidateRange(p_Cell.Range);
		}
		
		/// <summary>
		/// Force a cell to redraw. If Redraw is set to false this function has no effects. If ColSpan or RowSpan is greater than 0 this function invalidate the complete range with InvalidateRange
		/// </summary>
		/// <param name="p_Position"></param>
		public override void InvalidateCell(Position p_Position)
		{
			ICell l_Cell = this[p_Position.Row, p_Position.Column];
			if (l_Cell==null)
				base.InvalidateCell(p_Position);
			else
				InvalidateRange(l_Cell.Range);
		}

		#endregion

		#region PaintCell
		/// <summary>
		/// Draw the specified Cell
		/// </summary>
		/// <param name="p_Panel"></param>
		/// <param name="e"></param>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="p_PanelDrawRectangle"></param>
		protected override void PaintCell(GridSubPanel p_Panel,PaintEventArgs e, Cells.ICellVirtual p_Cell, Position p_CellPosition, Rectangle p_PanelDrawRectangle)
		{
			ICell l_Cell = (ICell)p_Cell;
			Range l_CellRange = l_Cell.Range;
			if ( l_CellRange.RowsCount == 1 && l_CellRange.ColumnsCount == 1 )
				base.PaintCell(p_Panel, e, p_Cell, p_CellPosition, p_PanelDrawRectangle);
			else //Row/Col Span > 1
			{
				Rectangle l_Rect = p_Panel.RectangleGridToPanel( PositionToDisplayRect(l_CellRange.Start) );
				base.PaintCell(p_Panel, e, p_Cell, l_CellRange.Start, l_Rect);//uso come rettangolo di disegno il rettanglo calcolato dalla posizione originale
			}
		}
		#endregion

		#region FocusCell
		/// <summary>
		/// Returns the active cell. Null if no cell are active
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public ICell FocusCell
		{
			get
			{
				if (Selection.FocusPosition.IsEmpty())
					return null;
				else
					return this[Selection.FocusPosition.Row, Selection.FocusPosition.Column];
			}
		}
		
		/// <summary>
		/// Set the focus to the specified cell (the specified cell became the active cell, FocusCell property).
		/// </summary>
		/// <param name="p_CellToSetFocus"></param>
		/// <returns></returns>
		public bool SetFocusCell(ICell p_CellToSetFocus)
		{
			if (p_CellToSetFocus==null)
				return Selection.Focus(Position.Empty);
			else
				return Selection.Focus(p_CellToSetFocus.Range.Start);
		}
		#endregion

		#region MouseCell
#if !MINI
		/// <summary>
		/// The cell currently under the mouse cursor. Null if no cell are under the mouse cursor.
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public ICell MouseCell
		{
			get
			{
				if (base.MouseCellPosition.IsEmpty())
					return null;
				else
					return this[base.MouseCellPosition.Row, base.MouseCellPosition.Column];
			}
		}
#endif

		/// <summary>
		/// Change the cell currently under the mouse
		/// </summary>
		/// <param name="p_MouseDownCell"></param>
		/// <param name="p_MouseCell"></param>
		protected override void ChangeMouseDownCell(Position p_MouseDownCell, Position p_MouseCell)
		{
			base.ChangeMouseDownCell(PositionToStartPosition(p_MouseDownCell), p_MouseCell);
		}

#if !MINI
		/// <summary>
		/// Fired when the cell under the mouse change. For internal use only.
		/// </summary>
		/// <param name="p_Cell"></param>
		protected override void ChangeMouseCell(Position p_Cell)
		{
			base.ChangeMouseCell(PositionToStartPosition(p_Cell));
		}
#endif

		#endregion

		#region Sort

		private bool m_CustomSort = false;

		/// <summary>
		/// Gets or sets if when calling SortRangeRows method use a custom sort or an automatic sort. Default = false (automatic)
		/// </summary>
		public bool CustomSort
		{
			get{return m_CustomSort;}
			set{m_CustomSort = value;}
		}

		/// <summary>
		/// Fired when calling SortRangeRows method. If the range contains all the columns this method move directly the row object otherwise move each cell.
		/// </summary>
		/// <param name="e"></param>
		protected override void OnSortingRangeRows(SortRangeRowsEventArgs e)
		{
			base.OnSortingRangeRows(e);

			if (CustomSort)
				return;

			if (e.AbsoluteColKeys > e.Range.End.Column && e.AbsoluteColKeys < e.Range.Start.Column)
				throw new ArgumentException("Invalid range","e.AbsoluteColKeys");

			IComparer cellComparer = e.CellComparer;
			if (cellComparer == null)
				cellComparer = new ValueCellComparer();

			//Sort all the columns (in this case I move directly the row object)
			if (e.Range.ColumnsCount == ColumnsCount)
			{
				RowInfo[] rowInfoToSort = new RowInfo[e.Range.End.Row-e.Range.Start.Row+1];
				ICell[] cellKeys = new ICell[e.Range.End.Row-e.Range.Start.Row+1];

				int zeroIndex = 0;
				for (int r = e.Range.Start.Row; r <= e.Range.End.Row;r++)
				{
					cellKeys[zeroIndex] = this[r, e.AbsoluteColKeys];

					rowInfoToSort[zeroIndex] = Rows[r];
					zeroIndex++;
				}

				Array.Sort(cellKeys, rowInfoToSort, 0, cellKeys.Length, cellComparer);

				//Apply sort
				if (e.Ascending)
				{
					for (zeroIndex = 0; zeroIndex < rowInfoToSort.Length; zeroIndex++)
					{
						Rows.Swap( rowInfoToSort[zeroIndex].Index, e.Range.Start.Row + zeroIndex);
					}
				}
				else //desc
				{
					for (zeroIndex = rowInfoToSort.Length-1; zeroIndex >= 0; zeroIndex--)
					{
						Rows.Swap( rowInfoToSort[zeroIndex].Index, e.Range.End.Row - zeroIndex);
					}
				}
			}
			else //sort only the specified range
			{
				ICell[][] l_RangeSort = new ICell[e.Range.End.Row-e.Range.Start.Row+1][];
				ICell[] l_CellsKeys = new ICell[e.Range.End.Row-e.Range.Start.Row+1];

				int zeroRowIndex = 0;
				for (int r = e.Range.Start.Row; r <= e.Range.End.Row;r++)
				{
					l_CellsKeys[zeroRowIndex] = this[r,e.AbsoluteColKeys];

					int zeroColIndex = 0;
					l_RangeSort[zeroRowIndex] = new ICell[e.Range.End.Column-e.Range.Start.Column+1];
					for (int c = e.Range.Start.Column; c <= e.Range.End.Column; c++)
					{
						l_RangeSort[zeroRowIndex][zeroColIndex] = this[r,c];
						zeroColIndex++;
					}
					zeroRowIndex++;
				}

				Array.Sort(l_CellsKeys, l_RangeSort, 0, l_CellsKeys.Length, cellComparer);

				//Apply sort
				zeroRowIndex = 0;
				if (e.Ascending)
				{
					for (int r = e.Range.Start.Row; r <= e.Range.End.Row;r++)
					{
						int zeroColIndex = 0;
						for (int c = e.Range.Start.Column; c <= e.Range.End.Column; c++)
						{
							RemoveCell(r,c);//rimuovo qualunque cella nella posizione corrente
							ICell tmp = l_RangeSort[zeroRowIndex][zeroColIndex];

							if (tmp!=null && tmp.Grid!=null && tmp.Range.Start.Row>=0 && tmp.Range.Start.Column>=0) //verifico che la cella sia valida
								RemoveCell(tmp.Range.Start.Row, tmp.Range.Start.Column);//la rimuovo dalla posizione precedente

							this[r,c] = tmp;
							zeroColIndex++;
						}
						zeroRowIndex++;
					}			
				}
				else //desc
				{
					for (int r = e.Range.End.Row; r >= e.Range.Start.Row;r--)
					{
						int zeroColIndex = 0;
						for (int c = e.Range.Start.Column; c <= e.Range.End.Column; c++)
						{
							RemoveCell(r,c);//rimuovo qualunque cella nella posizione corrente
							ICell tmp = l_RangeSort[zeroRowIndex][zeroColIndex];

							if (tmp!=null && tmp.Grid!=null && tmp.Range.Start.Row >= 0 && tmp.Range.Start.Column >= 0) //verifico che la cella sia valida
								RemoveCell(tmp.Range.Start.Row, tmp.Range.Start.Column);//la rimuovo dalla posizione precedente

							this[r,c] = tmp;
							zeroColIndex++;
						}
						zeroRowIndex++;
					}
				}
			}
		}

		#endregion

#if !MINI
		#region HTMLExport
		/// <summary>
		/// Export the specified cell to HTML
		/// </summary>
		/// <param name="p_CurrentPosition"></param>
		/// <param name="p_Cell"></param>
		/// <param name="p_Export"></param>
		/// <param name="p_Writer"></param>
		protected override void ExportHTMLCell(Position p_CurrentPosition, Cells.ICellVirtual p_Cell, IHTMLExport p_Export, System.Xml.XmlTextWriter p_Writer)
		{
			//export only real cell (not position occupied by rowspan or colspan
			if (p_Cell != null && ((ICell)p_Cell).Range.Start == p_CurrentPosition )
				p_Cell.VisualModel.ExportHTML(p_Cell, p_CurrentPosition, p_Export, p_Writer);
		}
		#endregion
#endif

		#region Selection
		private void Selection_SelectionChange(object sender, SelectionChangeEventArgs e)
		{
			//se è abilitato RowColSpan devo assicurarmi di selezionare la cella di origine e non quella che sfrutta il RowCol Span
			if (EnableRowColSpan)
			{
				if (e.EventType == SelectionChangeEventType.Add)
				{
					for (int r = e.Range.Start.Row; r <= e.Range.End.Row; r++)
						for (int c = e.Range.Start.Column; c <= e.Range.End.Column; c++)
						{
							ICell l_Cell = this[r,c];//N.B. questo metodo mi restituisce la cella reale (anche se questa posizione è occupata slo perchè in mee con Row/Col Span)
							if (l_Cell!=null)
							{
								Range l_Range = l_Cell.Range;

								if (l_Range != new Range(new Position(r,c)) ) //se la cella occupa più righe o colonne
									Selection.AddRange(l_Range); //la seleziono tutta
							}
						}
				}
				else if (e.EventType == SelectionChangeEventType.Remove)
				{
					for (int r = e.Range.Start.Row; r <= e.Range.End.Row; r++)
						for (int c = e.Range.Start.Column; c <= e.Range.End.Column; c++)
						{
							ICell l_Cell = this[r,c];//N.B. questo metodo mi restituisce la cella reale (anche se questa posizione è occupata slo perchè in mee con Row/Col Span)
							if (l_Cell!=null)
							{
								Range l_Range = l_Cell.Range;

								if (l_Range != new Range(new Position(r,c)) ) //se la cella occupa più righe o colonne
									Selection.RemoveRange(l_Range); //la seleziono tutta
							}
						}
				}
			}
		}
		#endregion
	}
}
